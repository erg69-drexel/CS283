
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>

//INCLUDES for extra credit
//#include <signal.h>
//#include <pthread.h>
//-------------------------

#include "dshlib.h"
#include "rshlib.h"


/*
 * start_server(ifaces, port, is_threaded)
 *      ifaces:  a string in ip address format, indicating the interface
 *              where the server will bind.  In almost all cases it will
 *              be the default "0.0.0.0" which binds to all interfaces.
 *              note the constant RDSH_DEF_SVR_INTFACE in rshlib.h
 * 
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -s implemented in dsh_cli.c
 *              For example ./dsh -s 0.0.0.0:5678 where 5678 is the new port  
 * 
 *      is_threded:  Used for extra credit to indicate the server should implement
 *                   per thread connections for clients  
 * 
 *      This function basically runs the server by: 
 *          1. Booting up the server
 *          2. Processing client requests until the client requests the
 *             server to stop by running the `stop-server` command
 *          3. Stopping the server. 
 * 
 *      This function is fully implemented for you and should not require
 *      any changes for basic functionality.  
 * 
 *      IF YOU IMPLEMENT THE MULTI-THREADED SERVER FOR EXTRA CREDIT YOU NEED
 *      TO DO SOMETHING WITH THE is_threaded ARGUMENT HOWEVER.  
 */
int start_server(char *ifaces, int port, int is_threaded){
    int svr_socket;
    int rc;

    svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0){
        int err_code = svr_socket;  //server socket will carry error code
        return err_code;
    }

    rc = process_cli_requests(svr_socket);

    stop_server(svr_socket);


    return rc;
}

/*
 * stop_server(svr_socket)
 *      svr_socket: The socket that was created in the boot_server()
 *                  function. 
 * 
 *      This function simply returns the value of close() when closing
 *      the socket.  
 */
int stop_server(int svr_socket){
    return close(svr_socket);
}

/*
 * boot_server(ifaces, port)
 *      ifaces & port:  see start_server for description.  They are passed
 *                      as is to this function.   
 * 
 *      This function "boots" the rsh server.  It is responsible for all
 *      socket operations prior to accepting client connections.  Specifically: 
 * 
 *      1. Create the server socket using the socket() function. 
 *      2. Calling bind to "bind" the server to the interface and port
 *      3. Calling listen to get the server ready to listen for connections.
 * 
 *      after creating the socket and prior to calling bind you might want to 
 *      include the following code:
 * 
 *      int enable=1;
 *      setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
 * 
 *      when doing development you often run into issues where you hold onto
 *      the port and then need to wait for linux to detect this issue and free
 *      the port up.  The code above tells linux to force allowing this process
 *      to use the specified port making your life a lot easier.
 * 
 *  Returns:
 * 
 *      server_socket:  Sockets are just file descriptors, if this function is
 *                      successful, it returns the server socket descriptor, 
 *                      which is just an integer.
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code is returned if the socket(),
 *                               bind(), or listen() call fails. 
 * 
 */
int boot_server(char *ifaces, int port){
    int svr_socket;
    int ret;
    
    struct sockaddr_in addr;

    // TODO set up the socket - this is very similar to the demo code

    svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket == -1){
        perror("socket");
        printf(CMD_ERR_RDSH_COMM);
        return ERR_RDSH_COMMUNICATION;
    }


    int enable=1;
    setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ifaces);
    addr.sin_port = htons(port);


    ret = bind(svr_socket, (const struct sockaddr *) &addr,
               sizeof(struct sockaddr_in));
    if (ret == -1) {
        perror("accept");
        printf("%s\n", CMD_ERR_RDSH_COMM);
        return ERR_RDSH_COMMUNICATION;
    }

    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */
    ret = listen(svr_socket, 20);
    if (ret == -1) {
        perror("listen");
        printf("%s\n", CMD_ERR_RDSH_COMM);
        return ERR_RDSH_COMMUNICATION;
    }

    return svr_socket;
}

/*
 * process_cli_requests(svr_socket)
 *      svr_socket:  The server socket that was obtained from boot_server()
 *   
 *  This function handles managing client connections.  It does this using
 *  the following logic
 * 
 *      1.  Starts a while(1) loop:
 *  
 *          a. Calls accept() to wait for a client connection. Recall that 
 *             the accept() function returns another socket specifically
 *             bound to a client connection. 
 *          b. Calls exec_client_requests() to handle executing commands
 *             sent by the client. It will use the socket returned from
 *             accept().
 *          c. Loops back to the top (step 2) to accept connecting another
 *             client.  
 * 
 *          note that the exec_client_requests() return code should be
 *          negative if the client requested the server to stop by sending
 *          the `stop-server` command.  If this is the case step 2b breaks
 *          out of the while(1) loop. 
 * 
 *      2.  After we exit the loop, we need to cleanup.  Dont forget to 
 *          free the buffer you allocated in step #1.  Then call stop_server()
 *          to close the server socket. 
 * 
 *  Returns:
 * 
 *      OK_EXIT:  When the client sends the `stop-server` command this function
 *                should return OK_EXIT. 
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code terminates the loop and is
 *                returned from this function in the case of the accept() 
 *                function failing. 
 * 
 *      OTHERS:   See exec_client_requests() for return codes.  Note that positive
 *                values will keep the loop running to accept additional client
 *                connections, and negative values terminate the server. 
 * 
 */
int process_cli_requests(int svr_socket){
    int     cli_socket;
    int     rc = OK;    

    while(1){
         

        
        cli_socket = accept(svr_socket, NULL, NULL);
        if (cli_socket == -1){
            perror("accept");
            printf("%s\n", CMD_ERR_RDSH_COMM);
            return ERR_RDSH_COMMUNICATION;
        }


        rc = exec_client_requests(cli_socket); //returns OK, OK_EXIT, or ERR_RDSH_COMMUNICATION

        if (rc == OK_EXIT){
            break; // go to stop server
        } else if (rc == ERR_RDSH_COMMUNICATION){
            return rc;
        }
    }

    stop_server(cli_socket);
    return rc;
}

/*
 * exec_client_requests(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 *   
 *  This function handles accepting remote client commands. The function will
 *  loop and continue to accept and execute client commands.  There are 2 ways
 *  that this ongoing loop accepting client commands ends:
 * 
 *      1.  When the client executes the `exit` command, this function returns
 *          to process_cli_requests() so that we can accept another client
 *          connection. 
 *      2.  When the client executes the `stop-server` command this function
 *          returns to process_cli_requests() with a return code of OK_EXIT
 *          indicating that the server should stop. 
 * 
 *  Note that this function largely follows the implementation of the
 *  exec_local_cmd_loop() function that you implemented in the last 
 *  shell program deliverable. The main difference is that the command will
 *  arrive over the recv() socket call rather than reading a string from the
 *  keyboard. 
 * 
 *  This function also must send the EOF character after a command is
 *  successfully executed to let the client know that the output from the
 *  command it sent is finished.  Use the send_message_eof() to accomplish 
 *  this. 
 * 
 *  Of final note, this function must allocate a buffer for storage to 
 *  store the data received by the client. For example:
 *     io_buff = malloc(RDSH_COMM_BUFF_SZ);
 *  And since it is allocating storage, it must also properly clean it up
 *  prior to exiting.
 * 
 *  Returns:
 * 
 *      OK:       The client sent the `exit` command.  Get ready to connect
 *                another client. 
 *      OK_EXIT:  The client sent `stop-server` command to terminate the server
 * 
 *      ERR_RDSH_COMMUNICATION:  A catch all for any socket() related send
 *                or receive errors. 
 */
int exec_client_requests(int cli_socket) {
    int io_size;
    command_list_t cmd_list;
    int rc;
    int message_rc;
    int last_rc;
    char *io_buff;

    io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (io_buff == NULL){
        return ERR_RDSH_SERVER;
    }

    while(1) {
        // TODO use recv() syscall to get input
        rc = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0);
        if (rc == -1){
            perror("read error");
            free(io_buff);
            printf("%s\n", CMD_ERR_RDSH_COMM);
            return ERR_RDSH_COMMUNICATION;
        
        } 

        //null terminate:
        io_buff[rc] = '\0';

        printf(RCMD_MSG_SVR_EXEC_REQ, io_buff); //print the command

        //check for the built ins:
        Built_In_Cmds bi_cmd = rsh_match_command(io_buff);
        if (bi_cmd == BI_CMD_EXIT){
            printf(RCMD_MSG_CLIENT_EXITED);
            send_message_string(cli_socket, "Exiting the client...\n");
            free(io_buff);
            return OK;
        } else if (bi_cmd == BI_CMD_STOP_SVR){
            printf(RCMD_MSG_SVR_STOP_REQ);
            send_message_string(cli_socket, RCMD_SERVER_EXITED);
            free(io_buff);
            return OK_EXIT;
        }


        // TODO build up a cmd_list

        memset(&cmd_list, 0, sizeof(command_list_t)); 
        rc = build_cmd_list(io_buff, &cmd_list);
        if (rc != OK){
            if (rc == ERR_CMD_OR_ARGS_TOO_BIG) {
                send_message_string(cli_socket, CMD_EXE_TOO_LONG);
            }
            else if (rc == WARN_NO_CMDS) {
                send_message_string(cli_socket, CMD_WARN_NO_CMD);
            }
            
            continue;  // Loop for next command
        }

        
        // TODO rsh_execute_pipeline to run your cmd_list
        last_rc = rsh_execute_pipeline(cli_socket, &cmd_list);  //returns: exit code of last command executed in pipeline
        
        // need to check if successful execute
        if (last_rc != EXIT_SUCCESS){
            send_message_string(cli_socket, CMD_ERR_RDSH_EXEC);
        }
        

        // TODO send appropriate respones with send_message_string
        // - error constants for failures
        // - buffer contents from execute commands
        //  - etc.

        // TODO send_message_eof when done
        message_rc = send_message_eof(cli_socket);
        if (message_rc != OK){
            free(io_buff);
            return message_rc;
        }

        
    }

    free(io_buff);
    return OK;
    
}

/*
 * send_message_eof(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client

 *  Sends the EOF character to the client to indicate that the server is
 *  finished executing the command that it sent. 
 * 
 *  Returns:
 * 
 *      OK:  The EOF character was sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the EOF character. 
 */
int send_message_eof(int cli_socket){
    int send_len = (int)sizeof(RDSH_EOF_CHAR);
    int sent_len;
    sent_len = send(cli_socket, &RDSH_EOF_CHAR, send_len, 0);

    if (sent_len != send_len){
        printf("%s\n", CMD_ERR_RDSH_COMM);
        return ERR_RDSH_COMMUNICATION;
    }
    return OK;
}


/*
 * send_message_string(cli_socket, char *buff)
 *      cli_socket:  The server-side socket that is connected to the client
 *      buff:        A C string (aka null terminated) of a message we want
 *                   to send to the client. 
 *   
 *  Sends a message to the client.  Note this command executes both a send()
 *  to send the message and a send_message_eof() to send the EOF character to
 *  the client to indicate command execution terminated. 
 * 
 *  Returns:
 * 
 *      OK:  The message in buff followed by the EOF character was 
 *           sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the message followed by the EOF character. 
 */
int send_message_string(int cli_socket, char *buff){
    //TODO implement writing to cli_socket with send()
    int len = strlen(buff);
    int sentMessage = send(cli_socket, buff, len, 0);

    if (sentMessage == -1){
        printf("%s\n", CMD_ERR_RDSH_COMM);
        return ERR_RDSH_COMMUNICATION;
    }

    //need to send EOF
    int sentEOF = send_message_eof(cli_socket);

    //check for successful send
    if (sentEOF != OK){
        return sentEOF;
    }

    return OK;
}


/*
 * rsh_execute_pipeline(int cli_sock, command_list_t *clist)
 *      cli_sock:    The server-side socket that is connected to the client
 *      clist:       The command_list_t structure that we implemented in
 *                   the last shell. 
 *   
 *  This function executes the command pipeline.  It should basically be a
 *  replica of the execute_pipeline() function from the last deliverable. 
 *  The only thing different is that you will be using the cli_sock as the
 *  main file descriptor on the first executable in the pipeline for STDIN,
 *  and the cli_sock for the file descriptor for STDOUT, and STDERR for the
 *  last executable in the pipeline.  See picture below:  
 * 
 *      
 *┌───────────┐                                                    ┌───────────┐
 *│ cli_sock  │                                                    │ cli_sock  │
 *└─────┬─────┘                                                    └────▲──▲───┘
 *      │   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐  │  │    
 *      │   │   Process 1  │     │   Process 2  │     │   Process N  │  │  │    
 *      │   │              │     │              │     │              │  │  │    
 *      └───▶stdin   stdout├─┬──▶│stdin   stdout├─┬──▶│stdin   stdout├──┘  │    
 *          │              │ │   │              │ │   │              │     │    
 *          │        stderr├─┘   │        stderr├─┘   │        stderr├─────┘    
 *          └──────────────┘     └──────────────┘     └──────────────┘   
 *                                                      WEXITSTATUS()
 *                                                      of this last
 *                                                      process to get
 *                                                      the return code
 *                                                      for this function       
 * 
 *  Returns:
 * 
 *      EXIT_CODE:  This function returns the exit code of the last command
 *                  executed in the pipeline.  If only one command is executed
 *                  that value is returned.  Remember, use the WEXITSTATUS()
 *                  macro that we discussed during our fork/exec lecture to
 *                  get this value. 
 */
int rsh_execute_pipeline(int cli_sock, command_list_t *clist) {

    //first check for build in commands:
    int rc;
    rc = exec_built_in_cmd(&clist->commands[0]);


    if (rc==BI_NOT_BI){
        int pipes[clist->num - 1][2];  // Array of pipes
        pid_t pids[clist->num];
        int  pids_st[clist->num];         // Array to store process IDs
        Built_In_Cmds bi_cmd;
        int exit_code;
    
        // Create all necessary pipes
        for (int i = 0; i < clist->num - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }
    
        for (int i = 0; i < clist->num; i++) {
            // TODO this is basically the same as the piped fork/exec assignment, except for where you connect the begin and end of the pipeline (hint: cli_sock)
    
            // TODO HINT you can dup2(cli_sock with STDIN_FILENO, STDOUT_FILENO, etc.

            pids[i] = fork();
            if (pids[i] == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
    
            if (pids[i] == 0) {  // Child process
                // Set up input pipe for all except first process
                if (i==0){
                    dup2(cli_sock, STDIN_FILENO);
                } 

                if (i > 0) {
                    dup2(pipes[i-1][0], STDIN_FILENO);
                }

                if (i==clist->num - 1){
                    dup2(cli_sock, STDOUT_FILENO);
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
                rc = exec_cmd(&clist->commands[i]); //returns 
                if (rc == OK){
                    exit(EXIT_SUCCESS);
                } else{
                    exit(EXIT_FAILURE);
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
            waitpid(pids[i], &pids_st[i], 0);
        }
    
        //by default get exit code of last process
        //use this as the return value
        exit_code = WEXITSTATUS(pids_st[clist->num - 1]);
        for (int i = 0; i < clist->num; i++) {
            //if any commands in the pipeline are EXIT_SC
            //return that to enable the caller to react
            if (WEXITSTATUS(pids_st[i]) == EXIT_SC)
                exit_code = EXIT_SC;
        }
        return exit_code;
    } else if (rc == BI_EXECUTED){
        return EXIT_SUCCESS;
    }

}

/**************   OPTIONAL STUFF  ***************/
/****
 **** NOTE THAT THE FUNCTIONS BELOW ALIGN TO HOW WE CRAFTED THE SOLUTION
 **** TO SEE IF A COMMAND WAS BUILT IN OR NOT.  YOU CAN USE A DIFFERENT
 **** STRATEGY IF YOU WANT.  IF YOU CHOOSE TO DO SO PLEASE REMOVE THESE
 **** FUNCTIONS AND THE PROTOTYPES FROM rshlib.h
 **** 
 */

/*
 * rsh_match_command(const char *input)
 *      cli_socket:  The string command for a built-in command, e.g., dragon,
 *                   cd, exit-server
 *   
 *  This optional function accepts a command string as input and returns
 *  one of the enumerated values from the BuiltInCmds enum as output. For
 *  example:
 * 
 *      Input             Output
 *      exit              BI_CMD_EXIT
 *      dragon            BI_CMD_DRAGON
 * 
 *  This function is entirely optional to implement if you want to handle
 *  processing built-in commands differently in your implementation. 
 * 
 *  Returns:
 * 
 *      BI_CMD_*:   If the command is built-in returns one of the enumeration
 *                  options, for example "cd" returns BI_CMD_CD
 * 
 *      BI_NOT_BI:  If the command is not "built-in" the BI_NOT_BI value is
 *                  returned. 
 */
Built_In_Cmds rsh_match_command(const char *input)
{
    if (strcmp(input, "exit") == 0)
        return BI_CMD_EXIT;
    if (strcmp(input, "dragon") == 0)
        return BI_CMD_DRAGON;
    if (strcmp(input, "cd") == 0)
        return BI_CMD_CD;
    if (strcmp(input, "stop-server") == 0)
        return BI_CMD_STOP_SVR;
    if (strcmp(input, "rc") == 0)
        return BI_CMD_RC;
    return BI_NOT_BI;
}


