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
 *  that command by taking the first wrd as the executable name, and
 *  then the remaining wrds as the arguments.
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

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    clist->num = 0;
    char *saveptr;
    char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);
    while (token != NULL) {
        // Trim leading whitespace.
        while (*token && isspace((unsigned char)*token))
            token++;
        // Trim trailing whitespace.
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }
        // Skip empty tokens.
        if (strlen(token) == 0) {
            token = strtok_r(NULL, PIPE_STRING, &saveptr);
            continue;
        }
        if (clist->num >= CMD_MAX) {
            fprintf(stderr, CMD_ERR_PIPE_LIMIT, CMD_MAX);
            return ERR_TOO_MANY_COMMANDS;
        }

        // Initialize current command in the command list.
        cmd_buff_t *cmd = &clist->commands[clist->num];
        cmd->argc = 0;
        cmd->_cmd_buffer = token;

        // Tokenize the command using whitespace.
        char *arg_saveptr;
        char *arg = strtok_r(token, " ", &arg_saveptr);
        while (arg != NULL && cmd->argc < CMD_ARGV_MAX - 1) {
            cmd->argv[cmd->argc] = arg;
            cmd->argc++;
            arg = strtok_r(NULL, " ", &arg_saveptr);
        }
        cmd->argv[cmd->argc] = NULL;

        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }
    if (clist->num == 0)
        return WARN_NO_CMDS;
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    clist->num = 0;
    memset(clist->commands, 0, sizeof(clist->commands));
    char *start = cmd_line;

    while (*start != '\0') {
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        char *nextPipe = strchr(start, '|');
        char *subcmd = NULL;

        if (nextPipe != NULL) {
            *nextPipe = '\0';
            subcmd = start;
            start = nextPipe + 1;
        } 
        else {
            subcmd = start;
            start = subcmd + strlen(subcmd);
        }

        while (*subcmd == SPACE_CHAR) {
            subcmd++;
        }

        char *end = subcmd + strlen(subcmd) - 1;
        while (*end == SPACE_CHAR) {
            *end = '\0';
            end--;
        }

        if (strlen(subcmd) == 0) {
            continue;
        }

        char *dupcmd = strdup(subcmd);

        char *wrd = strtok(dupcmd, " ");
        if (wrd == NULL) {
            free(dupcmd);
            continue;
        }

        if (strlen(wrd) >= EXE_MAX) {
            free(dupcmd);
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strcpy(clist->commands[clist->num].exe, wrd);

        char args_buffer[ARG_MAX];
        args_buffer[0] = '\0';

        wrd = strtok(NULL, " ");
        while (wrd != NULL) {
            size_t arglen = strlen(args_buffer);
            if (arglen + strlen(wrd) >= ARG_MAX) {
                free(dupcmd);
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }

            if (arglen > 0) {
                strcat(args_buffer, " ");
            }
            strcat(args_buffer, wrd);
            wrd = strtok(NULL, " ");
        }

        strcpy(clist->commands[clist->num].args, args_buffer);
        clist->num++;
        free(dupcmd);
    }

    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }
    return OK;
}