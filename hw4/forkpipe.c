#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t ok = 0;

void sig(int num){
        ok = 1;
}

int main(int argc, char *argv[]) {
    int pipefd[2];
    pid_t cpid1, cpid2;

     if (pipe(pipefd) == -1){
        return 2;
    }

    
    //starting with gen
    if ((cpid1 = fork()) == 0) {
    
        if (close(pipefd[0]) < 0){
            return 2;
        }
        if(dup2(pipefd[1], STDOUT_FILENO) == -1){
            return 2;
        }
        if (close(pipefd[1]) < 0){
            return 2;
        }

        if (signal(SIGTERM, sig) == SIG_ERR){
            fprintf(stdout, "error\n");
            return 2;
        }

        while (ok == 0) {
            printf("%d %d\n", rand(), rand());
            fflush(stdout);
            sleep(1);
        }
    fprintf(stderr, "GEN TERMINATED\n");

    } else if (cpid1 < 0) {
        fprintf(stdout, "ERROR");
        return 2;

    } else {
        //doing nsd child
        cpid2 = fork();

        if (cpid2 ==  0) { 
            if (close(pipefd[1]) < 0){
                return 2;
            }
            if (dup2(pipefd[0],STDIN_FILENO) == -1){
                return 2;
            }
            if (close(pipefd[0]) < 0){
                return 2;
            }
            if (execl("nsd", "nsd", NULL) < 0){
                return 2;
            }
        } else if (cpid2 < 0) {
            fprintf(stdout, "ERROR");
            return 2;
        } else {
            //parrent
            close(pipefd[0]);
            close(pipefd[1]);
            sleep(5);
            
            int gens;
            int ndss;
            kill(cpid1, SIGTERM);
            waitpid(cpid1, &gens, 0);
            waitpid(cpid2, &ndss, 0);
            if(gens == 0 && ndss == 0){
                fprintf(stdout, "OK\n");
                return 0;
            } else{
                fprintf(stdout, "ERROR\n");
                return 1;
            }
        }
    }

    return 0;
}





