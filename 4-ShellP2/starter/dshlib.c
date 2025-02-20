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



int alloc_cmd_buff(cmd_buff_t *cmd_buff){
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (!cmd_buff->_cmd_buffer){
        return ERR_MEMORY;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff){
    free(cmd_buff->_cmd_buffer);
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff){
    cmd_buff->argc=0;
    for (int i = 0; i < CMD_ARGV_MAX; i++){
        cmd_buff->argv[i] = NULL;
    }
    memset(cmd_buff->_cmd_buffer, 0, ARG_MAX);
    return OK;
}



Built_In_Cmds match_command(const char *input){
if(strcmp(input, EXIT_CMD) == 0){
        return BI_CMD_EXIT;
    }else if (strcmp(input, CD_CMD)==0){
        return BI_CMD_CD;
    }else{
        return BI_NOT_BI;
    }
 }

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd){
    int command = match_command(cmd->argv[0]);

    switch(command){
        case BI_CMD_EXIT:
            exit(0);
        case BI_CMD_CD:
            if(cmd->argc == 1){
                //do nothing
                return BI_EXECUTED;
            }else if (cmd->argc==2){
                int rc = chdir(cmd->argv[1]);
                if(rc==0){
                    return BI_EXECUTED;
                }else{
                    return BI_NOT_BI;
                }
            }
            return BI_EXECUTED;
            break;
        case BI_NOT_BI:
            return BI_NOT_BI;
    }

    return BI_NOT_BI;
    
}


int exec_cmd(cmd_buff_t *cmd){
    Built_In_Cmds biExecRC = exec_built_in_cmd(cmd);

    if(biExecRC==BI_NOT_BI){
        //this means that it is not a built in command, so need to fork/exec
        int f_result, c_result;

        f_result = fork();
        if (f_result < 0){
            perror("fork error");
            exit(1);
        }

        if (f_result==0){
            //now in child
            int rc;            
            rc = execvp(cmd->argv[0], cmd->argv);

            if (rc < 0){
                //error with the fork
                perror("Fork error");
                exit(42);
            }
        } else{
            //now in parent
            wait(&c_result);
        }

    } 
    return OK;
}






int parse_args(char *arg_string, cmd_buff_t *cmd){

    //leading spaces
    while(isspace(*arg_string)){
        arg_string++;
    }

    int quote_flag = 0;
    char *tmp = arg_string;
    while(*arg_string){

        //check for quotes
        if(quote_flag==1){
            if(*arg_string == QUOTE_CHAR){
                quote_flag = 0;
                *arg_string = '\0';
           } 
            arg_string++;
        }else{
            if(*arg_string == SPACE_CHAR){ //if space
                *arg_string = '\0'; //set space to null char
                cmd->argv[cmd->argc] = tmp; //set index of argv 
                
                cmd->argc++; //increment argc
                arg_string++; //increment the arg string
                
                tmp=arg_string; //set tmp to the next arg
                
            } else if(*arg_string == QUOTE_CHAR){
                
                quote_flag = 1;
                arg_string++;
                tmp=arg_string;
            }
            else{
                arg_string++;
            }
        }

    }

    if (*tmp != '\0') { 
        cmd->argv[cmd->argc] = tmp;
        cmd->argc++;
    }
    cmd->argv[cmd->argc] = NULL;

    return OK;

}





 int build_cmd_buff(char *cmd_string, cmd_buff_t *cmd)
 {
 
     char *trail;
     trail= cmd_string;
 
 //  handle trailing spaces first! prevents error later on if it were done after parsing
     while(*trail){
         trail++;
     }
     trail--;
     while(*trail == SPACE_CHAR){
         trail--;
     }
     //add null term
     *(trail+1) = '\0';


     //need to handle leading spaces

     while(isspace(*cmd_string)){
         cmd_string++;
     }
 
    
     strcpy(cmd->_cmd_buffer, cmd_string);

     char *params = strchr(cmd_string, SPACE_CHAR);
 
    cmd->argv[cmd->argc] = cmd_string; 
    cmd->argc++;
     //if there are args:
     if (params){
 
 
         *params = '\0'; //split
         params++;
 
         //need to handle leading space for args
         while(isspace(*params)){
             params++;
             
         }
         
         
         //check length of executable
        if (strlen(cmd_string) > EXE_MAX){
             return ERR_CMD_OR_ARGS_TOO_BIG;
         }
 
         //check length of args string
         if (strlen(params) > ARG_MAX){
             return ERR_CMD_OR_ARGS_TOO_BIG;
         }
         int rc = parse_args(params, cmd);
 
     }else{
        if (strlen(cmd_string) > EXE_MAX){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        return OK;
     }
     return OK;
 }
 


int exec_local_cmd_loop()
{

    // TODO IMPLEMENT MAIN LOOP
    char *cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    int rc = 0;
    cmd_buff_t cmd; 

    alloc_cmd_buff(&cmd);
    
    if (!cmd_buff){
        return ERR_MEMORY;
    }

    
    while (1)
    {
        clear_cmd_buff(&cmd);
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        // remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strlen(cmd_buff)==0){
            printf(CMD_WARN_NO_CMD);
            continue;
        }


        // IMPLEMENT THE REST OF THE REQUIREMENTS

        rc = build_cmd_buff(cmd_buff, &cmd);
        if (rc==OK){
            exec_cmd(&cmd);
        }
        else if (rc == ERR_CMD_OR_ARGS_TOO_BIG){
            printf(CMD_EXE_TOO_LONG);
        }else if(rc == ERR_TOO_MANY_COMMANDS){
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        }else if (rc==WARN_NO_CMDS){
            printf(CMD_WARN_NO_CMD);
        }
    
    }

    free(cmd_buff);
    free_cmd_buff(&cmd);

    return OK;
}