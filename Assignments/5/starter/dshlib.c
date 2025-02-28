#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

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
        // Initialize current command buffer.
        cmd_buff_t *cmd = &clist->commands[clist->num];
        cmd->argc = 0;
        cmd->_cmd_buffer = token;
        char *arg_saveptr;
        char *arg = strtok_r(token, " ", &arg_saveptr);
        while (arg != NULL && cmd->argc < CMD_ARGV_MAX - 1) {
            cmd->argv[cmd->argc++] = arg;
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

/*
 * exec_local_cmd_loop:
 * This function implements the main shell loop. It reads a command line from
 * the user, parses it into individual commands (possibly piped), and then
 * executes each command in a separate process. Pipes are used to connect the
 * standard output of one command to the standard input of the next.
 */
int exec_local_cmd_loop() {
    char *cmd_buff = malloc(ARG_MAX);
    if (!cmd_buff) {
        perror("malloc");
        return ERR_MEMORY;
    }
    int rc = OK;

    while (1) {
        // Display shell prompt and read input.
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // Trim leading whitespace.
        char *trimmed = cmd_buff;
        while (*trimmed && isspace((unsigned char)*trimmed))
            trimmed++;

        if (*trimmed == '\0') {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // Parse the command line into individual commands.
        command_list_t clist;
        rc = build_cmd_list(trimmed, &clist);
        if (rc != OK) {
            fprintf(stderr, "Error parsing command line\n");
            continue;
        }

        // Handle built-in commands if there is only one command.
        if (clist.num == 1 && clist.commands[0].argc > 0) {
            if (strcmp(clist.commands[0].argv[0], EXIT_CMD) == 0) {
                free(cmd_buff);
                return OK_EXIT;
            }
            if (strcmp(clist.commands[0].argv[0], "cd") == 0) {
                if (clist.commands[0].argc > 1) {
                    if (chdir(clist.commands[0].argv[1]) != 0) {
                        perror("cd");
                    }
                }
                continue;
            }
        }

        int num_cmds = clist.num;
        int num_pipes = num_cmds - 1;
        // Create the necessary pipes (each pipe gives 2 file descriptors).
        int pipefds[2 * num_pipes];
        for (int i = 0; i < num_pipes; i++) {
            if (pipe(pipefds + i * 2) < 0) {
                perror("pipe");
                rc = ERR_EXEC_CMD;
                break;
            }
        }
        if (rc != OK) {
            free(cmd_buff);
            return rc;
        }

        // Fork a child for each command in the pipeline.
        pid_t pids[num_cmds];
        int forked = 0;
        for (int i = 0; i < num_cmds; i++) {
            pids[i] = fork();
            if (pids[i] < 0) {
                perror("fork");
                rc = ERR_EXEC_CMD;
                break;
            }
            if (pids[i] == 0) {
                // Child process:
                // For non-first commands, redirect STDIN to the previous pipe's read end.
                if (i > 0) {
                    if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0) {
                        perror("dup2");
                        exit(ERR_EXEC_CMD);
                    }
                }
                // For non-last commands, redirect STDOUT to the current pipe's write end.
                if (i < num_cmds - 1) {
                    if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0) {
                        perror("dup2");
                        exit(ERR_EXEC_CMD);
                    }
                }
                // Close all pipe file descriptors in the child.
                for (int j = 0; j < 2 * num_pipes; j++) {
                    close(pipefds[j]);
                }
                // Execute the command using its parsed argv.
                execvp(clist.commands[i].argv[0], clist.commands[i].argv);
                perror("execvp");
                exit(ERR_EXEC_CMD);
            }
            forked++;
        }
        // Parent process: close all pipe file descriptors.
        for (int i = 0; i < 2 * num_pipes; i++) {
            close(pipefds[i]);
        }
        // Wait for all forked child processes.
        for (int i = 0; i < forked; i++) {
            int status;
            waitpid(pids[i], &status, 0);
        }
    }
    free(cmd_buff);
    return rc;
}