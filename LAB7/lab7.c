#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>


#define N_OUT_SIGNALS 101

#define N_PROCS	8

typedef struct{
	int nSendU1;
	int nSendU2;
	int nGetU1;
	int nGetU2;
} ProcInfo;

ProcInfo infos[N_PROCS + 1] = {0};

int flag = 1;

pid_t pid0 = 0;
pid_t pid1 = 0;
pid_t pid2 = 0;
pid_t pid3 = 0;
pid_t pid4 = 0;
pid_t pid5 = 0;
pid_t pid6 = 0;
pid_t pid7 = 0;
pid_t pid8 = 0;

int getProcessNum(int currPid){
	int res = 0;

	if(currPid == pid1)
		res = 1;
	else if(currPid == pid2)
		res = 2;
	else if(currPid == pid3)
		res = 3;
	else if(currPid == pid4)
		res = 4;
	else if(currPid == pid5)
		res = 5;
	else if(currPid == pid6)
		res = 6;
	else if(currPid == pid7)
		res = 7;
	else if(currPid == pid8)
		res = 8;
	
	return res;
}

void printEndInfo(){
	printf("n:%d; Pid:%d; PPid:%d is ended (sU1: %d; sU2: %d; gU1: %d; gU2: %d)\n", getProcessNum(getpid()), getpid(), getppid(), 
		infos[getProcessNum(getpid())].nSendU1, infos[getProcessNum(getpid())].nSendU2, infos[getProcessNum(getpid())].nGetU1, infos[getProcessNum(getpid())].nGetU2);
}

void sendSignals(pid_t to, int sig){
	if(sig == SIGUSR1){
		for(int i = 0; i < N_OUT_SIGNALS; i++){
			kill(to, sig);
			printf("(%d) C%d P%d send a U1 signal(time: %ld)\n", getProcessNum(getpid()), getpid(), getppid(),time (NULL));
			infos[getProcessNum(getpid())].nSendU1 += 1;
		}	
	}else if(sig == SIGUSR2){
		for(int i = 0; i < N_OUT_SIGNALS; i++){
			kill(to, sig);
			printf("(%d) C%d P%d send a U2 signal(time: %ld)\n", getProcessNum(getpid()), getpid(), getppid(),time (NULL));
			infos[getProcessNum(getpid())].nSendU2 += 1;
		}
	}
}


void signal_handler(int sig) {
    
	flag = 0;
	
	if(sig == SIGUSR1){
		printf("n:%d; Pid:%d; PPid:%d get SIGUSR1 (time: %ld)\n", getProcessNum(getpid()), getpid(), getppid(),time (NULL));
		infos[getProcessNum(getpid())].nGetU1 += 1;
	} else if(sig == SIGUSR2){
		printf("n:%d; Pid:%d; PPid:%d get SIGUSR2 (time: %ld)\n", getProcessNum(getpid()), getpid(), getppid(),time (NULL));
		infos[getProcessNum(getpid())].nGetU2 += 1;
	}
	signal(sig, signal_handler);
}

void term_handler(int sig){	
	printEndInfo();
	signal(sig, signal_handler);
	exit(0);
}


int main(){

    signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);
	signal(SIGTERM, term_handler);

    pid1 = fork();
    if(!pid1){
        //in proc 1

        pid1 = getpid();
        printf("1(%d) is started\n", getpid());

        pid2 = fork();
        if (!pid2){
            //in proc 2

            pid2 = getpid();
            printf("2(%d) is started\n", getpid());

            while(flag);
            sleep(0.5);
            printEndInfo();
            exit(0);
             
        }else{
            // in proc 1

            pid3 =fork();
            pid_t grp2 = getpgid(pid2);
            if(!pid3){
                //in proc 3
                setpgid(getpid(), grp2);

                pid3 = getpid();
                printf("3(%d) is started\n", getpid());

                pid4 = fork();
                if (!pid4){
                    //in proc 4

                    pid4 = getpid();
                    printf("4(%d) is started\n", getpid());
                    pid5 = fork();
                    if(!pid5){
                        //in proc 5

                        pid5 = getpid();
                        printf("5(%d) is started\n", getpid());
                        
                        while(flag);
                        sleep(0.5);
                        printEndInfo();
                        exit(0);
                        
                    }else{
                        //in proc 4

                        pid6 = fork();
                        pid_t grp5 = getpgid(pid5);
                        if(!pid6){
                            //in proc 6
                            setpgid(getpid(), grp5);
                            pid6 = getpid();
                            printf("6(%d) is started\n", getpid());
                            
                            while(flag);
                            sleep(0.5);
                            printEndInfo();
                            exit(0);

                        }else{
                            //in proc 4

                            pid7 = fork();
                            pid_t grp56 = getpgid(grp5);
                            if(!pid7){
                                //in proc 7

                                setpgid(getpid(), grp56);
                                pid7 = getpid();
                                printf("7(%d) is started\n", getpid());

                                pid8 = fork();
                                if(!pid8){
                                    //in proc 8

                                    pid8 = getpid();
	                                printf("8(%d) is started\n", getpid());
	                                kill(pid1, SIGUSR1);
	                                kill(pid2, SIGUSR1);
                                    kill(pid3, SIGUSR1);
                                    kill(pid4, SIGUSR1);
                                    kill(pid5, SIGUSR1);
                                    kill(pid6, SIGUSR1);
                                    kill(pid7, SIGUSR1);

                                    sendSignals(pid1, SIGUSR2);
                                    sleep(0.5);
                                    printEndInfo();
                                    
                                    exit(0);	
                                }else{
                                    //in proc 7

                                    while(flag);
                                    sendSignals(pid8, SIGUSR1);
                                    sleep(0.5);
                                    kill(pid8, SIGTERM);
                                    while(wait(NULL)>0);
                                    printEndInfo();
                                    
                                    exit(0);
                                }
                            }else{
                                //in proc 4

                                while(flag);
                                for(int i = 0; i < N_OUT_SIGNALS; i++){
                                    kill(grp56, SIGUSR1);
                                    printf("(%d) C%d P%d send a U2 signal(time: %ld)\n", getProcessNum(getpid()), getpid(), getppid(),time (NULL));
                                    infos[getProcessNum(getpid())].nSendU2 += 1;
                                }
                                sleep(0.5);
                                kill(grp56, SIGTERM);	
                                while(wait(NULL)>0);
                                printEndInfo();
                                exit(0);
                            }
                        }
                    }
                }else{
                    //in proc 3

                    while(flag);
                    sendSignals(pid4, SIGUSR1);
                    sleep(0.5);
                    kill(pid4, SIGTERM);
                    while(wait(NULL)>0);
                    printEndInfo();
                    exit(0);
                }

            }else{
                //in proc 1

                while(flag);
                for(int i = 0; i < N_OUT_SIGNALS; i++){
                    kill(grp2, SIGUSR1);
                    printf("(%d) C%d P%d send a U2 signal(time: %ld)\n", getProcessNum(getpid()), getpid(), getppid(),time (NULL));
                    infos[getProcessNum(getpid())].nSendU2 += 1;
                }
                sleep(0.5);
                kill(grp2, SIGTERM);	
                while(wait(NULL)>0);
                printEndInfo();
                exit(0);
            }
        }
        
    }
}