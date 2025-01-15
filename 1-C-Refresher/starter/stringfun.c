#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);
void reverse_string(char *, int);
void word_print(char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int);
//add additional prototypes here
int lengthOfString(char *);



int lengthOfString(char *str){
    //the string has to contain a null character
    //ptr[x] = *(ptr + x), so not using array notation
    int count = 0;
    char cur = str[count]; //current character

    while (cur != '\0'){
        count++;
        cur = str[count]; 
    }
    return count;
}



int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    bool isWhiteSpace = false;
    int buffSize = 0; 
    int userStringLength = lengthOfString(user_str);
    int i;
    //Go through userstring, and check for consecutive whitespace, if not swap into buff
    

    //Have to check the length of the user string, if more than 50 bytes.

    if (userStringLength > 50){
        printf("Invalid String Length. String must be < 50 chars.\n");
        exit(3); //user supplied string is too large

    }

    for (i = 0; i < userStringLength; i++){
        //buff[i] = user_str[i];
        if ((*(user_str + i) == ' ') || *(user_str + i) == '\t'){ //check if on whitespace or tab
            if (isWhiteSpace){
                continue;
            }
            else{
                *(buff + i) = ' ';
                buffSize++; 
                isWhiteSpace = true;
            }
        } else{
            isWhiteSpace = false;
            *(buff + i) = *(user_str + i); 
            buffSize++; 
        }
        
    }

    //fill rest of buffer with .
    for (; i < len; i++){
        *(buff + i) = '.';
    }
    
    return buffSize; 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int str_len){
    int wc = 0;
    bool word_start = false;

    for (int i=0; i < str_len; i++){
        char c = *(buff + i);
        if (word_start == false){
            if (c == ' '){
                continue;
            } else {
                wc++;
                word_start = true;
            }
        } else {
            if (c == ' '){
                word_start = false;
            }
        }
    }

    return wc;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

void reverse_string(char *buff, int str_len){
    //local variables
    int end_idx = (str_len - 1);        //should be length of string - 1
    int start_idx = 0;
    char tmp_char;

    //reverse function, using tmp variable
    while (end_idx > start_idx){
        tmp_char = *(buff + start_idx);
        *(buff + start_idx) = *(buff + end_idx);
        *(buff + end_idx) = tmp_char;
        start_idx++;
        end_idx--;
    }
    
    printf("Reversed String: "); //moved from main for formatting
    for (int i = 0; i < str_len; i++){

        printf("%c", *(buff + i));
    }
    printf("\n");

    return;

}

void word_print(char *buff, int str_len){ //adapted from part 1 of this assignment
    // local variables
    int wc = 0;         //counts words
    int wlen = 0;       //length of current word
    bool word_start = false;    //am I at the start of a new word

    

    for (int i=0; i < str_len; i++){
        char c = *(buff + i); //getting each character
        if (word_start == false){ //if not at the start of a word
            if (c == ' '){
                printf(" (%d)\n", wlen);
                word_start = false;
                wlen = 0;
                continue;
            } else {
                wc++;
                word_start = true;
                wlen = 1;
                printf("%d. %c", wc, c);
               }
        } else { //at the start of a word
            if (c == ' '){
                word_start = false;
                printf(" (%d)\n", wlen);
            }
            else{
                printf("%c", c);
                wlen++;
            }
        }
    }
    printf(" (%d)\n", wlen);
    word_start = false;
    wlen = 0;
}


int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      If arv[1] does not exist, the program prints the usage of the command, and then exits, so no action is performed and the program doesn't crash.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      For this program to function, there needs to be atleast 3 arguments, the exe, the option flag, and the string to operate on. If it is less than 3
    //then the program just prints out how to use the command and exits. No crashing happens.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL){ //checking for null pointer if malloc fails
        exit(99); //as per the directions, shouldn't this be 2?
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(3);
            }
            printf("Word Count: %d\n", rc);
            break;
            
        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        
        case 'r':
            
            reverse_string(buff, user_str_len);
            // printf("Reversed string: %s\n", buff); moved into reverse string
            break;

        case 'w':
            printf("Word Print\n----------\n");

            word_print(buff, user_str_len);
            break;

        
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          It is your responsiblity of the user to know where the memory bounds are. When passing a pointer all that does is give the address
// to the start of the memory, but system has no idea how much memory you actually have.