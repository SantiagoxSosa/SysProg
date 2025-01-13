#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);

//add additional prototypes here
void reverse_word(char *, int);
void word_print(char *, int, int);

int setup_buff(char *buff, char *user_str, int len){
    if(!user_str)
        return 0;

    int charcnt = 0;

    while(*user_str==' '||*user_str=='\t')
    {
        user_str++;
    }

    while(*user_str != '\0')
    {
        if(charcnt == len)
        {
            return -1;
        }
        if(*user_str == ' ' || *user_str=='\t')
        {
            while(*user_str == ' ' || *user_str=='\t')
            {
                user_str++;
            }

            if(*user_str!='\0')
            {
                if(charcnt == len)
                    return -1;
                
                *buff= ' '; 
                buff++;
                charcnt++;
            }
        }
        else {
            *buff=*user_str;
            buff++;
            user_str++;            
            charcnt++;
        }
    }
    for(int j = charcnt; j < len; j++)
    {
        *buff = '.';
        buff++;
    }
    return charcnt;
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

int count_words(char *buff, int len, int str_len){
    
    int wrdcnt=0;
    int start = 1; // 1 if start of word, 0 if not

    if(str_len > len)
    {
        exit(99);
    }

    if(str_len==0)
    {
        return 0;
    }

    while(*buff!='.')
    {
        if(start==1)
        {
            wrdcnt++;
            start=0;
        }

        if(*buff==' ')
        {
            //printf("uhh SPACE");
            start=1;
        }
        buff++;
    }
    return wrdcnt;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

void reverse_word(char *buff, int str_len)
{
    char *ptr = buff+(str_len-1);

    //printf("Reversing: %s, length: %d", buff, str_len);

    char *out = (char *)malloc(sizeof(char)*str_len);
    char *start = out;
    
    for (int i = 0; i < str_len; i++) {
        *out = *ptr;
        out++;
        ptr--;
    }

    printf("Reversed string: %s\n", start);
    free(start);  
    return;
}

void word_print(char *buff, int str_len, int words)
{
    int start = 1; // 1 if start of word, 0 if not
    int cnt = 0;
    int n=1;
    printf("Word Print\n----------\n");

    if(str_len==0)
    {
        exit(99);
    }

    while(*buff!='.')
    {
        if(n>words)
        {
            exit(98);
        }

        char* word = (char *)malloc(sizeof(char)*str_len);
        char *startWord = word;

        if(start==1)
        {
            while(*buff!=' ')
            {
                if(*buff=='.')
                {
                    break;
                }
                *word = *buff;
                start=0;
                word++;
                buff++;
                cnt++;
            }
            printf("%d. %s (%d)\n", n, startWord, cnt);
            if(n==words)
                free(startWord);
        }
        buff++;
        start=1;
        cnt=0;
        n++;    
    }            
    return;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    /*      
            This is safe because the segment "argc < 2" executes before the second half. 
            This guarentees that argv[1] exists before trying to access it 
    */
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
    /*      The statement checks if there are at least 3 arguments on the input. 
    If not, then it prints what executable inputted, and then exits with error code 1. */

    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char*)malloc(BUFFER_SZ*sizeof(char)); 
    if(buff==NULL)
    {
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_word(buff, user_str_len);
            break;
        case 'w':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            word_print(buff, user_str_len, rc);
            break;
        case 'x':
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
/*         
    This is better practice because that way we only need to access memory we know we need. The buff has a ,ax of 50, but if we know the length is only 17 (for example), then we only need to allocate what is needed, and have a safety net to double check for sanity if the code we are writing makes sense.
*/