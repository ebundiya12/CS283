#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

extern void print_dragon(void);

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */


static int parse_line_to_cmd_buff(const char *line, cmd_buff_t *cmd)
{
    // skip leading whitespace
    while (*line && isspace((unsigned char)*line))
        line++;
    if (*line == '\0')
    {
        return WARN_NO_CMDS;
    }

    // allocate storage to copy line
    cmd->_cmd_buffer = malloc(strlen(line) + 1);
    if (!cmd->_cmd_buffer)
    {
        return ERR_MEMORY;
    }
    strcpy(cmd->_cmd_buffer, line);

    cmd->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++)
    {
        cmd->argv[i] = NULL;
    }

    char *p = cmd->_cmd_buffer;
    while (*p)
    {
        // skip whitespace
        while (*p && isspace((unsigned char)*p))
            p++;
        if (*p == '\0')
            break;

        if (cmd->argc >= (CMD_ARGV_MAX - 1))
            break;

        char *start = p;
        if (*p == '"')
        {
            // parse quoted substring
            p++;
            start = p;
            while (*p && *p != '"')
                p++;
            if (*p == '"')
            {
                *p = '\0';
                p++;
            }
        }
        else
        {
            // parse until next whitespace
            while (*p && !isspace((unsigned char)*p))
                p++;
        }

        if (*p)
        {
            *p = '\0';
            p++;
        }

        if (*start != '\0')
        {
            cmd->argv[cmd->argc] = start;
            cmd->argc++;
        }
    }
    cmd->argv[cmd->argc] = NULL;

    if (cmd->argc == 0)
    {
        free(cmd->_cmd_buffer);
        cmd->_cmd_buffer = NULL;
        return WARN_NO_CMDS;
    }

    return OK;
}


int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *token;
    char *rest = cmd_line;
    int count = 0;

    clist->num = 0;

    while ((token = strtok_r(rest, PIPE_STRING, &rest)) != NULL && count < CMD_MAX) {
        // Trim leading and trailing whitespace
        while (isspace((unsigned char)*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        if (strlen(token) == 0) continue;

        cmd_buff_t *cmd = &clist->commands[count];
        int rc = parse_line_to_cmd_buff(token, cmd);
        if (rc != OK) {
            free_cmd_list(clist);
            return rc;
        }
        count++;
    }

    if (token != NULL) {
        free_cmd_list(clist);
        return ERR_TOO_MANY_COMMANDS;
    }

    clist->num = count;
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for (int i = 0; i < cmd_lst->num; i++) {
        free(cmd_lst->commands[i]._cmd_buffer);
        cmd_lst->commands[i]._cmd_buffer = NULL;
        cmd_lst->commands[i].argc = 0;
        for (int j = 0; j < CMD_ARGV_MAX; j++) {
            cmd_lst->commands[i].argv[j] = NULL;
        }
    }
    cmd_lst->num = 0;
    return OK;
}


int execute_pipeline(command_list_t *clist) {
    int num_commands = clist->num;
    int prev_pipe_read = -1;
    pid_t pids[CMD_MAX];
    int status;

    for (int i = 0; i < num_commands; i++) {
        int pipe_fd[2];
        if (i < num_commands - 1) {
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                return ERR_EXEC_CMD;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return ERR_EXEC_CMD;
        } else if (pid == 0) {
            // Child process
            if (prev_pipe_read != -1) {
                dup2(prev_pipe_read, STDIN_FILENO);
                close(prev_pipe_read);
            }

            if (i < num_commands - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
                close(pipe_fd[0]);
            }

            // Execute the command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            fprintf(stderr, "error: could not execute command '%s'\n", clist->commands[i].argv[0]);
            _exit(EXIT_FAILURE);
        } else {
            // Parent process
            pids[i] = pid;

            if (prev_pipe_read != -1) {
                close(prev_pipe_read);
            }

            if (i < num_commands - 1) {
                prev_pipe_read = pipe_fd[0];
                close(pipe_fd[1]);
            }
        }
    }

    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], &status, 0);
    }

    return OK;
}

int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX + 1];
    command_list_t clist;
    int rc;
    static int last_rc = 0;

    if (isatty(STDIN_FILENO)) {
        printf("%s", SH_PROMPT);
        fflush(stdout);
    }

    while (fgets(cmd_buff, SH_CMD_MAX, stdin)) {
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        memset(&clist, 0, sizeof(clist));
        rc = build_cmd_list(cmd_buff, &clist);
        if (rc != OK) {
            if (rc == WARN_NO_CMDS)
                printf(CMD_WARN_NO_CMD);
            else if (rc == ERR_TOO_MANY_COMMANDS)
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            printf("%s", SH_PROMPT);
            fflush(stdout);
            continue;
        }
        if (clist.num == 0) {
            printf(CMD_WARN_NO_CMD);
            printf("%s", SH_PROMPT);
            fflush(stdout);
            continue;
        }
        if (clist.num == 1) {
            cmd_buff_t *cmd = &clist.commands[0];
            if (strcmp(cmd->argv[0], EXIT_CMD) == 0) {
                free_cmd_list(&clist);
                break;
            } else if (strcmp(cmd->argv[0], "cd") == 0) {
                if (cmd->argc > 1) {
                    if (chdir(cmd->argv[1]) != 0) {
                        perror("cd failed");
                        last_rc = 1;
                    } else {
                        char cwd[1024];
                        getcwd(cwd, sizeof(cwd));
                        printf("%s\n", cwd);
                        last_rc = 0;
                    }
                } else {
                    char cwd[1024];
                    getcwd(cwd, sizeof(cwd));
                    printf("%s\n", cwd);
                    last_rc = 0;
                }
                free_cmd_list(&clist);
                printf("%s", SH_PROMPT);
                fflush(stdout);
                continue;
            } else if (strcmp(cmd->argv[0], "dragon") == 0) {
                print_dragon();
                free_cmd_list(&clist);
                last_rc = 0;
                printf("%s", SH_PROMPT);
                fflush(stdout);
                continue;
            } else if (strcmp(cmd->argv[0], "rc") == 0) {
                printf("%d\n", last_rc);
                free_cmd_list(&clist);
                printf("%s", SH_PROMPT);
                fflush(stdout);
                continue;
            }
        }
        rc = execute_pipeline(&clist);
        free_cmd_list(&clist);
        printf("%s", SH_PROMPT);
        fflush(stdout);
    }

    if (!isatty(STDIN_FILENO))
        printf("%s", SH_PROMPT);
    return OK;
}