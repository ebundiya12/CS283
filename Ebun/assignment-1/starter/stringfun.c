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
void reverse_string(char *buff, int user_str_len);
void print_words(char *buff, int user_str_len);
int replace_substring(char *buff, int user_str_len, char *find, char *replace);
void trim_trailing_spaces(char *buff, int *user_str_len);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    char *src = user_str;
    char *dest = buff;
    int count = 0;
    int space_flag = 0;

    while (*src != '\0' && count < len) {
        if (*src == ' ' || *src == '\t') {
            if (!space_flag) {
                *dest++ = ' ';
                count++;
                space_flag = 1;
            }
        } else {
            *dest++ = *src;
            count++;
            space_flag = 0;
        }
        src++;
    }

    if (*src != '\0') return -1;


    while (count < len) {
        *dest++ = '.';
        count++;
    }

    return count;
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
    //YOU MUST IMPLEMENT
    if (str_len > len) {
        return -1;
    }

    int count = 0;
    char *ptr = buff;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (*ptr != ' ') {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
        ptr++;
    }

    // printf("%d",count);

    return count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
void reverse_string(char *buff, int user_str_len) {
    char *start = buff;
    char *end = buff + user_str_len - 1;

    while (start < end) {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }
}

void trim_trailing_spaces(char *buff, int *user_str_len) {
    int len = *user_str_len - 1;
    while (len >= 0 && (buff[len] == '.' || buff[len] == ' ')) {
        len--;
    }
    *user_str_len = len + 1;
}

void print_words(char *buff, int user_str_len) {
    trim_trailing_spaces(buff, &user_str_len);

    printf("Word Print\n----------\n");
    char *start = buff;
    int word_count = 0;
    // printf("%d",user_str_len);

    for (int i = 0; i < user_str_len; i++) {
        if (*(buff + i) == ' ' || i == user_str_len - 1) {
            if (start < buff + i) {
                printf("%d. ", ++word_count);
                for (char *p = start; p <= buff + i; p++) {
                    if (*p != '.') {
                        putchar(*p);
                    }
                }
                printf(" (%ld)\n", buff + i - start + 1);
            }
            start = buff + i + 1;
        }
    }
}

int replace_substring(char *buff, int user_str_len, char *find, char *replace) {
    char *pos = buff;
    int find_len = 0;
    int replace_len = 0;
    char *end = buff + user_str_len;

    while (find[find_len] != '\0') find_len++;
    while (replace[replace_len] != '\0') replace_len++;

    while (pos + find_len <= end) {
        if (strncmp(pos, find, find_len) == 0) {
            if (pos - buff + replace_len + (end - (pos + find_len)) >= BUFFER_SZ) {
                return -1;
            }

            memmove(pos + replace_len, pos + find_len, end - (pos + find_len));
            memcpy(pos, replace, replace_len);

            return 0;
        }
        pos++;
    }

    return -2;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //
    // This is safe because argv[1] is only accessed after verifying that
    // argc is greater than or equal to 2. If not, the program
    // will print the usage message and exit. This ensures no out-of-bounds access
    // to the array.
    //
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
    // This is to ensure that the required input is provided. 
    // If argc is less than 3, it means an input string was not provided, 
    // so the program will print the usage message and exit
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    buff = (char *)malloc(BUFFER_SZ);
    if (!buff) {
        printf("Error: Memory allocation failed\n");
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
            reverse_string(buff, user_str_len);
            break;

        case 'w':
            print_words(buff, user_str_len);
            printf("\nNumber of words returned: %d\n", count_words(buff, BUFFER_SZ, user_str_len));
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

// Passing both the buffer pointer and its length ensures safety by preventing buffer overflows, 
// improves flexibility, and makes the code more reusable and easier to maintain, 
// even if the buffer size changes.
