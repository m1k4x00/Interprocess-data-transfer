#define _POSIX_C_SOURCE 202009L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include "lib.c"
#include <assert.h>


static int sigpipe = 0;

//Signal handler
static void sig_usr(int signo, siginfo_t *info, void *contex) {
	char ret = info->si_value.sival_int;
	write(sigpipe,&ret,1);

}

int main(int argc, char *argv[]) {
    
    FILE *log; //Log file fd
    int inputfd; //Input file descriptor 
    int outputfd; //Output file descriptor
    log = fopen("./Logs/Log.txt", "w"); //Opening main log file

    if (log == NULL){
        printf("Error! Opening log file failed");
    }

    //Initializing the file descriptor
    //initialize_io function found in lib.c file

    fprintf(log, "Initializing io\n"); //Logging
    if(initialize_io(argc, argv, &inputfd, &outputfd) == -1) {
        return -1;
    }

    // Initializing signal handler and pipe
    fprintf(log, "Initializing signal handler\n"); //Logging
    struct sigaction sig;
    union sigval val;
    int pipefd[2];  //Pipe file descriptor

    fprintf(log, "Creating pipe\n"); //Logging pipe creation

    if(pipe(pipefd)!=0){
        fprintf(log, "Creating pipe failed\n");
        printf("Creating pipe failed\n");
        return -1;
    }

    sigpipe = pipefd[1];
    fcntl(sigpipe,F_SETFL,O_NONBLOCK);
    sigemptyset(&sig.sa_mask);

    sig.sa_flags= SA_RESTART|SA_SIGINFO; //SA_RESTART avoids interrupts by signals
    sig.sa_sigaction = sig_usr; //Setting the handler function for sigaction

    //Setting action for signal
    fprintf(log, "Setting action for signal\n");
    if (sigaction(SIGRTMIN,&sig,NULL)!=0) {
        fprintf(log, "Error! Setting action for signal failed\n");
        printf("Error! Setting action for signal failed\n");
        return -1;
    }
   

    //Creating child process
    fprintf(log, "Creating child process\n");
    pid_t pid = fork();

    if (pid == 0) { 
        // Child process (B). Reads from inputfd
        FILE *logtx;
        logtx = fopen("./Logs/Log_tx.txt", "w");
        
        int buf[6] = {0};   //Buffer to store symbols as morse code  
        char ch[1];         //Variable to store character read from input
        int tx = SIGRTMIN;  //Signal used for transmission
    
        //Main loop to read from input 1 char at a time

        while(read(inputfd, ch, sizeof(ch))>0) { //Reading until end of input
            if (ch[0] == '\n') break;
            //Encoding the read character ch with morse code using the code function from lib.c and saving the result to buf
            fprintf(logtx, "Encoding character: %c\n", ch[0]);
            if (code(ch[0], buf)!=0) { 
                fprintf(logtx, "Error! Encoding character: \"%c\" failed\n", ch[0]);
                return -1;
            }

            //Sending the symbol
            for (int i = 0; i<6;i++){
                
                if (buf[i]==0) { //If 0 then no more data to be sent
                    //e.g, {10,12,0,0,0,0} need to only send 10 and 12 so sending 20 to denote end of symbol
                    val.sival_int = 20;
                    if (sigqueue(getppid(), tx, val)!=0) {
                        fprintf(logtx, "Error when sending value: \"%d\"\n", val.sival_int);
                        return -1;
                    }
                    fprintf(logtx, "Sent 20\n");
                    break;  //No need to continue for loop when 0 is reached
                } 
                else {  //Sending either 10 or 12
                    val.sival_int=buf[i];
                    if (sigqueue(getppid(), tx, val)!=0){
                        fprintf(logtx, "Error when sending value: \"%d\"\n", val.sival_int);
                        return -1;
                    }
                    fprintf(logtx, "Sent %d\n", buf[i]);
                    } 
                    //If i is 5 then we know that we are sending a symbol that uses all 6 bytes of the buffer so no 0 values,
                    //therefore, sending 20 to denote end of symbol
                    val.sival_int = 20;
                    if (i==5) {
                        if (sigqueue(getppid(), tx, val)!=0){
                            fprintf(logtx, "Error when sending value: \"%d\"\n", val.sival_int);
                            return -1;
                        } 
                        fprintf(logtx, "Sent 20\n");
                        }
            }
                
        }
        //To denote end of transmission we send two times signal 20.
        val.sival_int = 20;
        if (sigqueue(getppid(), tx, val)!=0){
            fprintf(logtx, "Error when sending value: \"%d\"\n", val.sival_int);
        }
        fprintf(logtx, "Sent 20\n");

        fclose(logtx); //Closing log file
    
           
    } else {
        // Parent process (A). Writes to outputfd
        FILE *logrx; //Log file file descriptor
        logrx = fopen("./Logs/Log_rx.txt", "w"); //Opening log file
        if (logrx == NULL) {
            printf("Error! Opeding Log rx failed");
            return -1;
        }
        int ind = 0; //Keeps count of number of bits recieved to avoid buffer overflow
        int buffer[6] = {0}; //Buffer to save received signals
        char rxCh[1] = {0}; //Used to save the decoded character
        char last = 0;  //Used to save last received signal to know when end of transmission is reached
        
        //Main loop to receive signals
        for ( ; ; ) {

            char rsignal; //Signal recieved
            int res = read(pipefd[0],&rsignal,1); //Reading signal from pipe


             //When read is interrupted by a signal, it will return -1 and errno is EINTR.
            if (res<0) {
                fprintf(logrx, "Read failed\n");
                perror("Read failed");
                }
            //If end of transmission reached breaking the loop    
            if (last==rsignal && rsignal==20) {
                fprintf(logrx, "End of transmission reached! Shutting down\n");
                break;
                }
            last = rsignal; //Saving current signal as previous signal
            
           //If data read from pipe save to buffer or decode
            if (res == 1) {
                    //If received signal is 10 save 10 to ind position of buffer
                    if (rsignal == 10){
                        
                        if (ind<6) { //Preventing buffer overflow
                            buffer[ind] = rsignal;
                            fprintf(logrx, "Recieved 10\n");
                            ind++;
                        } else {
                            fprintf(logrx, "Error! Buffer full");
                            printf("Error! Buffer full");
                            return -1;
                         }
                        
                        }      
                    //If received signal is 12 save 12 to ind position of buffer  
                    else if (rsignal == 12) { 
                        if (ind<6) { //Preventing buffer overflow
                            buffer[ind] = rsignal;
                            fprintf(logrx, "Recieved 12\n");
                            ind++;
                        } else {
                            fprintf(logrx, "Error! Buffer full");
                            printf("Error! Buffer full");
                            return -1;
                            }

                        }
                    //If received signal is 20 end of signal is reached
                    else if (rsignal == 20) {
                      
                        fprintf(logrx, "Recieved 20. Decoding...\n");
                        //Decoding received character
                        if(decode(buffer, &(rxCh[0]))<0) { 
                            fprintf(logrx, "Error! Decoding symbol failed\n");    
                            return -1;
                        }
                        for (int a = 0;a<6;a++) buffer[a] = 0; //Resetting buffer to all zeros
                        ind = 0; //Resetting ind to 0 since we are counting number of signals in one recieved symbol

                        //Writing received character to output
                        fprintf(logrx, "Writing \"%c\" to output\n", rxCh[0]);
                        if(write(outputfd, rxCh, sizeof(rxCh))==-1) {
                            fprintf(logrx, "Error! Writing \"%c\" to output failed\n", rxCh[0]);
                            return -1;
                            }
                    
                    }
                    
                }
                
            }
        fclose(logrx);  //Closing log file
    }

    
    close(inputfd); //Closing input
    close(outputfd);    //Closing output
    
    return 0;
}