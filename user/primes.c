#define RD 0
#define WR 1

int getFirstData(int lpipe[2],int* dst){
    if(read(lpipe[RD],dst,sizeof(int))==sizeof(int)){
        printf("prime %d",*dst);
        return 0;
    }

    return -1;
}

void func(int lpipe[2],int rpipe[2],int first){
    int buf=0;
    
    while(read(lpipe[RD],&buf,sizeof(int))==sizeof(int)){
        if(buf%first){
            write(rpipe[WR],&buf,sizeof(int));
        }
    }

    close(lpipe[RD]);
    close(rpipe[WR]);
}

int main(int argv,char const* argc[]){
    int p[2];
    pipe(p);

    for(int i=2;i<=35;i++){
        write(p[WR],&i,sizeof(int));

    }

    if(fork()==0){

    }else{
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }
}