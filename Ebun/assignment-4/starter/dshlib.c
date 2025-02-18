#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

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



int exec_local_cmd_loop()
{
    char *cmd_buff;
    int rc = 0;
    cmd_buff_t cmd;

    
    static int last_rc = 0;

    // Allocate buffer for reading lines
    cmd_buff = malloc(SH_CMD_MAX + 1);
    if (!cmd_buff)
    {
        fprintf(stderr, "error: cannot allocate cmd_buff\n");
        return ERR_MEMORY;
    }

    while (1)
    {
        // Print the prompt
        printf("%s", SH_PROMPT);
        fflush(stdout);

        // Read a line
        if (!fgets(cmd_buff, SH_CMD_MAX, stdin))
        {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        
        if (cmd_buff[0] != '\0')
        {
            printf("%s\n", cmd_buff);
        }

        // Parse into cmd_buff_t
        memset(&cmd, 0, sizeof(cmd));
        rc = parse_line_to_cmd_buff(cmd_buff, &cmd);
        if (rc == WARN_NO_CMDS)
        {
            printf("warning: no commands provided\n");
            continue;
        }
        else if (rc == ERR_MEMORY)
        {
            fprintf(stderr, "error: memory allocation failed\n");
            continue;
        }

        // Built-in "exit"
        if (strcmp(cmd.argv[0], EXIT_CMD) == 0)
        {
            free(cmd._cmd_buffer);
            break;
        }

        // Built-in "cd"
        if (strcmp(cmd.argv[0], "cd") == 0)
        {
            if (cmd.argc > 1)
            {
                if (chdir(cmd.argv[1]) != 0)
                {
                    perror("cd failed");
                    last_rc = 1;
                }
                else
                {
                    char cwd[512];
                    if (getcwd(cwd, sizeof(cwd)))
                    {
                        printf("%s\n", cwd);
                    }
                    last_rc = 0;
                }
            }
            else
            {
                char cwd[512];
                if (getcwd(cwd, sizeof(cwd)))
                {
                    printf("%s\n", cwd);
                }
                last_rc = 0;
            }
            free(cmd._cmd_buffer);
            continue;
        }

        // Extra credit: "dragon"
        if (strcmp(cmd.argv[0], "dragon") == 0)
        {
            print_dragon();
            last_rc = 0;
            free(cmd._cmd_buffer);
            continue;
        }

        // Extra credit: "rc"
        if (strcmp(cmd.argv[0], "rc") == 0)
        {
            printf("%d\n", last_rc);
            free(cmd._cmd_buffer);
            continue;
        }

        // External command => fork/exec
        pid_t pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "error: failed to fork\n");
            free(cmd._cmd_buffer);
            continue;
        }
        if (pid == 0)
        {
            // Child
            execvp(cmd.argv[0], cmd.argv);
            // If we get here, execvp failed
            switch (errno)
            {
                case ENOENT:
                    fprintf(stderr, "Command not found in PATH\n");
                    break;
                case EACCES:
                    fprintf(stderr, "Permission denied\n");
                    break;
                default:
                    fprintf(stderr, "error: could not execute command\n");
                    break;
            }
            _exit(errno);
        }
        else
        {
            // Parent
            int status = 0;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status))
            {
                last_rc = WEXITSTATUS(status);
            }
            else
            {
                last_rc = 1;
            }
        }

        free(cmd._cmd_buffer);
    }

    free(cmd_buff);
    return OK;
}
