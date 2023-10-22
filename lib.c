#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "morse.c"
//This is a library file for main.c


//Initializes the input and ouput file descriptors based on the arguments argv
//
//argc: Number of arguments
//argv: Arguments list
//inputfd: file descriptor of input
//outputfd: file descriptor of output
//return: 0 if succeeded -1 if failed
int initialize_io(int argc, char *argv[], int *inputfd, int *outputfd) {

    if (argc == 2) { // Only input file, output stdout
    *outputfd = STDOUT_FILENO;
    if (strcmp(argv[1],"-") == 0) {
        *inputfd = STDIN_FILENO;
    } else {
        *inputfd = open(argv[1],O_RDONLY);
        if (*inputfd < 0) {
            fprintf(stderr,"Opening input file (%s) failed\n",argv[1]);
            return -1;
        }
    }
    } else if (argc == 3) { // Both input and output file given
        if (strcmp(argv[1],"-") == 0) {
            *inputfd = STDIN_FILENO;
        } else {
            *inputfd = open(argv[1],O_RDONLY);
            if (*inputfd < 0) {
                fprintf(stderr,"Opening input file (%s) failed\n",argv[1]);
                return -1;
            }
        }
        if (strcmp(argv[2],"-") == 0) {
            *outputfd = STDOUT_FILENO;
        } else {
            *outputfd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
            if (*outputfd < 0) {
                fprintf(stderr,"Creating output file (%s) failed\n",argv[2]);
                return -1;
            }
        }
} else {
    fprintf(stderr,"Usage: %s [input|-] [output|-]\n",argv[0]);
    return -1;
}
    return 0;
}

//Encodes the character ch and saves the result into buf
//Used character set and morse table found in morse.c
//
//ch: Character to be encoded (Must be in charset found in morse.c)
//buf: Buffer of size 6 where the encoded character is saved
//return: 0 if succeeded -1 if failed
int code(char ch, int *buf) {
    char chUpper = toupper(ch); //Converting the character to upper case
    size_t i; //Index of charset
    int k=0; //Index of charset as an integer
    int found=0; //Boolean to tell if character is found

    //First loops through the charset to see if ch found in charset
    for (i=0;i<strlen(charset);i++) {
        //If found set found to 1 and break the loop
        if(chUpper ==charset[k]){
            found = 1;
            break;
        }
        k++;
    }
    //Write the signals of the found character to buffer using k. 
    //This is possible since charset and morseset has the characters in the same order
    for (int n=0;n<SYMBOL_SIZE;n++) {
        buf[n] = morseset[k][n]; 
    }
    //In case character was not found
    if (found !=1) {
        printf("Character not found - code\n");
        return -1;
    }
    return 0;
}

//Decodes the encoded character from a morseArr of size 6 and saves the result to ch
//
//morseArr: Buffer with signals
//ch: Where the decoded character is saved
//return: 0 if succeeded -1 if failed
int decode(int *morseArr, char *ch) {
    int i,j; //Variables for loops
    int f = 1; //Whether the charcter was found

    //First loops through each character of the morseset
    for (i = 0; i<CHARSET_SIZE;i++){
        f=1;
        //Loopts through each signal of every character
        for (j=0;j<SYMBOL_SIZE;j++) {
            //If the decoded character is not morseSet[i] then set f to 0 and break the loop and try with morseSet[i+1]
            if(morseset[i][j]!=*(morseArr+j)){
                f=0;
                break;
            }
        }
        //If this part is reached with f set to 1 character was found and the result is saved to ch
        if (f == 1){
            *ch = charset[i];   
            return 1;
        }
    }

    printf("Character not found - decode\n");
    return -1;

}