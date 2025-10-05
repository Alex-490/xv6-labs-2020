#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

int main(int argv,char const* argc[]){
    char buf1[4]="ping";
    char buf2[4]="pong";
    char buf3[4]="pppp";
    
    int fd_c2p[2];
    int fd_p2c[2];

    pipe(fd_c2p);
    pipe(fd_p2c);

    int pid=fork();
    int exit_status=0;

    if(pid<0){
        close(fd_c2p[RD]);
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        close(fd_p2c[WR]);

        fprintf(2,"fork() error!\n");
        exit(1);
    }else if(pid==0){
        close(fd_c2p[RD]);
        close(fd_p2c[WR]);

        if(read(fd_p2c[RD],&buf3,sizeof(buf3))!=sizeof(buf3)){
            fprintf(2,"child read() error!\n");
            exit_status=1;
        }else{
            fprintf(1,"%d: receive %s\n",getpid(),buf3);
        }

        if(write(fd_c2p[WR],&buf2,sizeof(buf2))!=sizeof(buf2)){
            fprintf(2,"child write() error!\n");
            exit_status=1;
        }

        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        
        exit(exit_status);
    }else{
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);

        if(write(fd_p2c[WR],&buf1,sizeof(buf1))!=sizeof(buf1)){
            fprintf(2,"parent write() error!\n");
            exit_status=1;
        }

        if(read(fd_c2p[RD],&buf1,sizeof(buf1))!=sizeof(buf1)){
            fprintf(2,"parent read() error!\n");
            exit_status=1;
        }else{
            fprintf(1,"%d: receive %s\n",getpid(),buf1);
        }

        close(fd_c2p[RD]);
        close(fd_p2c[WR]);
        
        exit(exit_status);
    }
}

