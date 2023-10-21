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
    
    int inputfd;
    int outputfd; 

    if(initialize_io(argc, argv, &inputfd, &outputfd) == -1) {
        return -1;
    }
    
    struct sigaction sig;
    union sigval val;
    int pipefd[2];


    assert(pipe(pipefd)==0);
    sigpipe = pipefd[1];
    fcntl(sigpipe,F_SETFL,O_NONBLOCK);
    sigemptyset(&sig.sa_mask);

    sig.sa_flags= SA_RESTART|SA_SIGINFO;
    sig.sa_sigaction = sig_usr;

    assert(sigaction(SIGRTMIN,&sig,NULL)==0);
   

    //Create child process B

    pid_t pid = fork();

    if (pid == 0) { 
        // Child process (B). Reads from inputfd
        FILE *logtx;
        logtx = fopen("Log_tx.txt", "w");
        
        int buf[6] = {0};
        char ch[1];
        int rx = SIGRTMIN;
    
        
        while(read(inputfd, ch, sizeof(ch))>0){
        
 
            if (code(ch[0], buf) !=0 ) return -1;
            for (int i = 0; i<6;i++){
                
                if (buf[i]==0){
                    val.sival_int = 20;
                    assert(sigqueue(getppid(), rx, val)==0);
                    fprintf(logtx, "Sent 20\n");
                    break;
                } else {
                    val.sival_int=buf[i];
                    assert(sigqueue(getppid(), rx, val)==0);
                    fprintf(logtx, "Sent %d\n", buf[i]);
                    } 
                    val.sival_int = 20;
                    if (i==5) {assert(sigqueue(getppid(), rx, val)==0); fprintf(logtx, "Sent 20\n");}
                    
                
            }
                
        }
        val.sival_int = 20;
        assert(sigqueue(getppid(), rx, val)==0);
        fprintf(logtx, "Sent 20\n");

        fclose(logtx);
    
           
    } else {
        // Parent process (A). Writes to outputfd
        FILE *logrx;
        logrx = fopen("Log_rx.txt", "w");
        int ind = 0;
        int buffer[6] = {0};
        char rxCh[1] = {0};
        int count = 0;
        char last =0;
        
        
        for ( ; ; ) {

            char mysignal;
            int res = read(pipefd[0],&mysignal,1);
            if (res<0) perror("Read failed");
            if (last==mysignal && mysignal==20) {
                break;
                }
            last = mysignal;
            
            // When read is interrupted by a signal, it will return -1 and errno is EINTR.
            
            if (res == 1) {
                    // printf("recieved: %d ", mysignal);
            
                    if (mysignal == 10){
                        if(ind<6){
                            buffer[ind] = mysignal;
                            fprintf(logrx, "Recieved 10\n");
                            ind++;
                        } else printf("Buffer full");
                        
                    }        
                    else if (mysignal == 12)  {
                        if(ind<6){
                            buffer[ind] = mysignal;
                            fprintf(logrx, "Recieved 12\n");
                            ind++;
                        } else {printf("Buffer full");}

                        }

                    else if (mysignal == 20) {
                      
                        fprintf(logrx, "Recieved 20\n");
                        count++;
                        if(decode(buffer, &(rxCh[0]))<0) return -1;
                        for (int a = 0;a<6;a++) buffer[a] = 0;
                        ind = 0;
                        if(write(outputfd, rxCh, sizeof(rxCh))==-1) return -1;
                    
                    }
                    
                }
                
            }
        fclose(logrx);
    }

    
    close(inputfd);
    close(outputfd);
    
    return 0;
}