#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "morse.c"

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

int code(char ch, int *buf) {
    char chUpper = toupper(ch);
    size_t i; 
    int k=0; 
    int found=0;

    for (i=0;i<strlen(charset);i++) {
        if(chUpper ==charset[k]){
            found = 1;
            break;
        }
        k++;
    }
    for (int n=0;n<SYMBOL_SIZE;n++) {
        buf[n] = morseset[k][n]; 
    }
    if (found !=1) {
        printf("Character not found code");
        return -1;
    }
    return 0;
}

int decode(int *morseArr, char *ch) {
    int i,j;
    int f = 1;

    for (i = 0; i<CHARSET_SIZE;i++){
        f=1;
        for (j=0;j<SYMBOL_SIZE;j++){
            if(morseset[i][j]!=*(morseArr+j)){
                f=0;
                break;
            }
        }
        if (f == 1){
            *ch = charset[i];   
            return 1;
        }
    }

    printf("Character not found decode");
    return -1;

}