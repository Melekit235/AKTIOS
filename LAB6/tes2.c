#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<alloca.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<libgen.h>
#include<limits.h>
#include<unistd.h>
#include<dirent.h>
#include<pthread.h>

int maxProc;
int countProc = 0;
FILE *outfile;
pthread_t tid[500];



void * Process(void *curPath){
	DIR *dp;
	dp = opendir((char *)curPath);

	struct dirent *d;
	struct stat buf;

	char *file = alloca(strlen(curPath) + NAME_MAX + 2);

	long int sum = 0;
	int count = 0;
	int maxSize = -1;
	char *maxFile = alloca(NAME_MAX);
	maxFile[0] = 0;

	while(d = readdir(dp)){
		if(strcmp(".", d->d_name) && strcmp("..", d->d_name)){
			strcpy(file, curPath);
			strcat(file, "/");
 			strcat(file, d->d_name);
			if(!S_ISDIR(buf.st_mode)){
				if(buf.st_size > maxSize){
					maxSize = buf.st_size;
					strcpy(maxFile, basename(file));
				}
				sum+=buf.st_size;
				count++;
			}
			if(S_ISDIR(buf.st_mode)){
				if(countProc<maxProc){
					pthread_create(&tid[countProc],NULL,Process,file);
					countProc++;
					pthread_join(tid[countProc-1],NULL);
				}else{
					
					pthread_join(tid[countProc-1],NULL);
					countProc--;
					pthread_create(&tid[countProc],NULL,Process,file);
					countProc++;
					pthread_join(tid[countProc-1],NULL);
				}
			}
		}
	}

	printf("%s %d %ld %s %d %d\n", curPath, count, sum, maxFile,countProc,getpid());
	fprintf(outfile,"%s %d %ld %s %d %d\n", curPath, count, sum, maxFile,countProc,getpid());

	countProc--;

}





int main( int argc , char *argv[] ){

	outfile = fopen(argv[2],"w");
	maxProc = atol(argv[3]);
	pthread_t tid;
	pthread_create(&tid,NULL,Process,realpath(argv[1], NULL));
	countProc++;
	pthread_join(tid,NULL);						

	fclose(outfile);

	return 0;
}