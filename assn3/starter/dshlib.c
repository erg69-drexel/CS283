#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "dshlib.h"
// /*
//  *  build_cmd_list
//  *    cmd_line:     the command line from the user
//  *    clist *:      pointer to clist structure to be populated
//  *
//  *  This function builds the command_list_t structure passed by the caller
//  *  It does this by first splitting the cmd_line into commands by spltting
//  *  the string based on any pipe characters '|'.  It then traverses each
//  *  command.  For each command (a substring of cmd_line), it then parses
//  *  that command by taking the first token as the executable name, and
//  *  then the remaining tokens as the arguments.
//  *
//  *  NOTE your implementation should be able to handle properly removing
//  *  leading and trailing spaces!
//  *
//  *  errors returned:
//  *
//  *    OK:                      No Error
//  *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
//  *                             commands.
//  *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
//  *                             was larger than allowed, either the
//  *                             executable name, or the arg string.
//  *
//  *  Standard Library Functions You Might Want To Consider Using
//  *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
//  * 
//  *      cmda1 a1 a2 | cmda2 a3 a4 | cmd3 
//  * 
//  * 
//  * 
//  * 
//  * 
//  */





int parse_params(char *ptr, command_list_t *clist, int index)
{

    //need to handle leading spaces
    while(isspace(*ptr)){
        ptr++;
    }

    char *params = strchr(ptr, SPACE_CHAR);

    //if there are args:
    if (params){


        *params = '\0'; //split
        params++;

        //need to handle leading space for args
        while(isspace(*params)){
            params++;
        }
        
        //check length of executable
    if (strlen(ptr) > EXE_MAX){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        //check length of args string
        if (strlen(params) > ARG_MAX){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        //remove trailing whitespace
        char *trail;
        trail= params;

        //go to end of the string
        while(*trail){
            trail++;
        }
        trail--;
        while(*trail == SPACE_CHAR){
            trail--;
        }
        //add null term
        *(trail+1) = '\0';


        //set the exe and args
        strcpy(clist->commands[index].exe, ptr);
        strcpy(clist->commands[index].args, params);
    }
    else { //cmd is passed with no arguments, like just cmd1
        if (strlen(ptr) <= EXE_MAX){


            memset(clist->commands[index].args, 0, ARG_MAX);
            strcpy(clist->commands[index].exe, ptr);
        } else{
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        
    }

    return OK;
}


int build_cmd_list(char *cmd_line, command_list_t *clist)
{

    char *pt = strtok(cmd_line, PIPE_STRING);
    int counter = 0;
    int ret = 0;
    
    //check if empty
    if (strlen(cmd_line) == 0){
        return WARN_NO_CMDS;
    }

    while (pt != NULL) {


        counter++;
        //have to check if too many commands:
        if (counter > CMD_MAX){
            return ERR_TOO_MANY_COMMANDS;
        }

        //now split into the exe and args:
        ret = parse_params(pt, clist, counter);

        if (ret == OK){
            //increase to next command
            pt = strtok(NULL, PIPE_STRING);
        } else{
            return ret;
        }

    }

    //set the num of commands
    clist->num = counter;
    return OK;
}