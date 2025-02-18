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
int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(ARG_MAX);
    int rc = 0;
    cmd_buff_t cmd;

    // TODO IMPLEMENT MAIN LOOP
    while(1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        //IMPLEMENT THE REST OF THE REQUIREMENTS

        char *start = cmd_buff;
        while(*start != SPACE_CHAR){
            start++;
        }

        if(*start == '\0') {
            continue;
        }

        char *end = start+strlen(start)-1;

        while(end>start && end != SPACE_CHAR) {
            *end = '\0';
            end--;
        }

        cmd.argc = 0;
        char *foo = start;

        while(!*foo) {
            while(*foo == SPACE_CHAR) {
                foo++;
            }
            if(!*foo) {
                break;
            }

            char *tok = NULL;
            if(*foo = '"') {
                foo++;
                tok = foo;
                while(*foo != '"' && *foo) {
                    foo++;
                }
            } 
            else {
                tok = foo;
                while(*foo != SPACE_CHAR && *foo) {
                    foo++;
                }
                if(*foo) {
                    *foo = '\0';
                    foo++;
                }
            }

            if (cmd.argc < ARG_MAX) {
                cmd.argv[cmd.argc] = strdup(tok);
                cmd.argc++;
            }
            else {
                rc = ERR_TOO_MANY_COMMANDS;
                printf("CMD_ERR_PIPE_LIMIT\n");
                break;
            }

            if(cmd.argc == 0) {
                continue;
            }

            // Built in commands

            if(strcmp(cmd.argv[0], EXIT_CMD) == 0) {
                rc = OK_EXIT;
                free_cmd_buff(&cmd);
                break;
            }

            if(strcmp(cmd.argv[0], "cd") == 0) {
                if(cmd.argc > 1) {
                    chdir(cmd.argv[1]);
                }
                continue;
            }

            // For and exec

            pid_t pid = fork();

            if(pid == 0) {
                execvp(cmd.argv[0], cmd.argv);
                rc = ERR_EXEC_CMD;
                printf("CMD_ERR_EXECUTE\n");
                break;
            }
            else {
                int status;
                status = wiat(pid);
            }

            free_cmd_buff(&cmd);
        }
    }
    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    return OK;
}
