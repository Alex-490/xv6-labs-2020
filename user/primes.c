#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

int getFirstData(int l[2],int* a){
    if(read(l[RD],a,sizeof(int))==sizeof(int)){
        printf("prime %d\n",*a);
        return 0;//success
    }

    return -1;//fail
}

void trans(int l[2],int r[2],int first){
    int a;
    
    while(read(l[RD],&a,sizeof(int))==sizeof(int)){
        if(a%first){
            write(r[WR],&a,sizeof(int));
        }
    }
}

void primes(int l[2]){
    int first;
    if(getFirstData(l,&first)==-1)  exit(0);

    int r[2];
    pipe(r);
    trans(l,r,first);
    close(l[RD]);
    close(r[WR]);//only left r's read.

    if(fork()==0){//child
        primes(r);
    }else{
        close(r[RD]);
        wait(0);
    }
}

int main(int argv,char const* argc[]){
    int p[2];
    pipe(p);

    for(int i=2;i<=35;i++){
        write(p[WR],&i,sizeof(int));
    }

    close(p[WR]);//only left p's rd

    if(fork()==0){
        primes(p);//son only can use p's rd
    }else{
        close(p[RD]);
        wait(0);
    }

    exit(0);
}