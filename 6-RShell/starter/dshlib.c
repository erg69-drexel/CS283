#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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
// int exec_local_cmd_loop()
// {

//     // THIS CODE SHOULD BE THE SAME AS PRIOR ASSIGNMENTS
   
//     return OK;
// }


//Struct Management: //

int alloc_cmd_buff(cmd_buff_t *cmd_buff){
    cmd_buff->_cmd_buffer = NULL; 
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (!cmd_buff->_cmd_buffer){
        return ERR_MEMORY;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff){
    if (cmd_buff->_cmd_buffer) { 
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL; 
    }
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

int free_cmd_list(command_list_t *cmd_lst){ 

    for (int i = 0; i < cmd_lst->num; i++){
        
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    cmd_lst->num = 0;
    return OK;
}


// end struct management //







//command execution: 



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
        //printf("%s\n", cmd->argv[0]);
    
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
            

            int rc;            
            rc = execvp(cmd->argv[0], cmd->argv);

            if (rc < 0){
                //error with the fork 
                perror("Fork error");
                exit(42);
            }

    
        } 
        return OK;
    }



    int execute_pipeline(command_list_t *clist){

        //first check for built in:
        int rc;
        rc = exec_built_in_cmd(&clist->commands[0]);
        if (rc==BI_NOT_BI){
            int pipes[clist->num - 1][2];  // Array of pipes
            pid_t pids[clist->num];        // Array to store process IDs
        
            // Create all necessary pipes
            for (int i = 0; i < clist->num - 1; i++) {
                if (pipe(pipes[i]) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }
        
            //check for built in
    
            // Create processes for each command
            for (int i = 0; i < clist->num; i++) {
                pids[i] = fork();
                if (pids[i] == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
        
                if (pids[i] == 0) {  // Child process
                    // Set up input pipe for all except first process
                    if (i > 0) {
                        dup2(pipes[i-1][0], STDIN_FILENO);
                    }
        
                    // Set up output pipe for all except last process
                    if (i < clist->num - 1) {
                        dup2(pipes[i][1], STDOUT_FILENO);
                    }
        
                    // Close all pipe ends in child
                    for (int j = 0; j < clist->num - 1; j++) {
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
        
                    // Execute command
                    rc = exec_cmd(&clist->commands[i]);
                    if (rc == OK){
                        exit(EXIT_SUCCESS);
                    }
                }
            }
        
            // Parent process: close all pipe ends
            for (int i = 0; i < clist->num - 1; i++) {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
        
            // Wait for all children
            for (int i = 0; i < clist->num; i++) {
                waitpid(pids[i], NULL, 0);
            }

        }


        return OK;
    
    }

// end command execution



//Command parsing:



int build_cmd_list(char *cmd_line, command_list_t *clist)
{

    
    char *pt = strtok(cmd_line, PIPE_STRING);
    int counter = 0;
    int ret = 0;



    while (pt != NULL) {


        counter++;
        //have to check if too many commands:
        if (counter > CMD_MAX){
            return ERR_TOO_MANY_COMMANDS;
        }

        alloc_cmd_buff(&clist->commands[counter-1]);
        clear_cmd_buff(&clist->commands[counter-1]); 

        clist->num = counter;
        ret = check_arg_lengths(pt);

        if (ret != OK) {
            return ret;
        }

        //now split into the exe and args:
        ret = parse_args(pt, &clist->commands[counter-1]);
         
        if (ret == OK){
            //increase to next command
            pt = strtok(NULL, PIPE_STRING);
        } else{
            free_cmd_list(clist);
            return ret;
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
                if (!(strlen(tmp) == 0)){
                    cmd->argv[cmd->argc] = tmp; //set index of argv 
                    cmd->argc++; //increment argc
                }
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





 int check_arg_lengths(char *cmd_string)
 {
    char *copy = malloc(sizeof(char) * (strlen(cmd_string)+1));
    
    memcpy(copy, cmd_string, strlen(cmd_string) + 1);
     char *params = strchr(copy, SPACE_CHAR);

     //if there are args:
     
     if (params){
        
 
 
         *params = '\0'; //split
         params++;
 
         //need to handle leading space for args
         while(isspace(*params)){
             params++;
             
         }
         
         //check length of executable
        if (strlen(copy) > EXE_MAX){
            free(copy);
             return ERR_CMD_OR_ARGS_TOO_BIG;
         }
 
         //check length of args string
         if (strlen(params) > ARG_MAX){
            free(copy);
             return ERR_CMD_OR_ARGS_TOO_BIG;
         }
 
     }else{
        if (strlen(copy) > EXE_MAX){
            free(copy);
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        free(copy);
        return OK;
     }
     free(copy);
     return OK;
 }
 




int exec_local_cmd_loop()
{

    // TODO IMPLEMENT MAIN LOOP
    char *cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    int rc = 0;
    command_list_t clist; 


    
    if (!cmd_buff){ 
        return ERR_MEMORY;
    }

    
    while (1)
    {
        
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

        rc = build_cmd_list(cmd_buff,&clist);

        if (rc==OK){
            execute_pipeline(&clist);
        }
        else if (rc == ERR_CMD_OR_ARGS_TOO_BIG){
            printf(CMD_EXE_TOO_LONG);
        }else if(rc == ERR_TOO_MANY_COMMANDS){
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        }else if (rc==WARN_NO_CMDS){
            printf(CMD_WARN_NO_CMD);
        }
        free_cmd_list(&clist); 
    }


    free(cmd_buff);
    

    return OK;
}

