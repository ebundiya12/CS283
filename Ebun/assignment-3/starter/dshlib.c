#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *  
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    // 1) Check if the line is all whitespace or empty
    char *scan = cmd_line;
    while (*scan && isspace((unsigned char)*scan))
        scan++;
    if (*scan == '\0')
    {
        return WARN_NO_CMDS;
    }

    char *saveptr = NULL;
    int count = 0;

    // Use strtok_r to safely tokenize with '|'
    char *token = strtok_r(cmd_line, "|", &saveptr);
    while (token != NULL)
    {
        // If we already have CMD_MAX commands, error out
        if (count >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Trim leading spaces
        while (*token && isspace((unsigned char)*token))
            token++;

        // Trim trailing spaces
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end))
        {
            *end = '\0';
            end--;
        }

        char *exe = strtok(token, " \t");
        if (exe == NULL)
        {
            return WARN_NO_CMDS;
        }
        if (strlen(exe) >= EXE_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        strcpy(clist->commands[count].exe, exe);

        char *remain = strtok(NULL, "");
        if (remain != NULL)
        {
            while (*remain && isspace((unsigned char)*remain))
                remain++;
            if (strlen(remain) >= ARG_MAX)
            {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcpy(clist->commands[count].args, remain);
        }
        else
        {
            clist->commands[count].args[0] = '\0';
        }

        count++;
        token = strtok_r(NULL, "|", &saveptr);
    }

    clist->num = count;
    return OK;
}