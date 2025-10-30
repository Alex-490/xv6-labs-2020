#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXSZ 512

enum state{S_WAIT,S_ARG,S_ARG_END,S_ARG_LINE_END,S_LINE_END,S_END};

enum c_type{C_SPACE,C_CHAR,C_LINE_END};

enum c_type get_type(char c){
    if(c==' ')  return C_SPACE;
    else if(c=='\n') return C_LINE_END;
    else    return C_CHAR;
}

enum state next_state(enum state cur,enum c_type ct){
    switch(cur){
        case S_WAIT:
            if(ct==C_SPACE) return S_WAIT;
            if(ct==C_CHAR)  return S_ARG;
            if(ct==C_LINE_END)  return S_LINE_END;
            break;
        case S_ARG:
            if(ct==C_SPACE) return S_END;
            if(ct==C_CHAR)  return S_ARG;
            if(ct==C_LINE_END)  return S_ARG_LINE_END;
            break;
        case S_ARG_END:
        case S_ARG_LINE_END:
        case S_LINE_END:
            if(ct==C_SPACE) return S_WAIT;
            if(ct==C_CHAR)  return S_ARG;
            if(ct==C_LINE_END)  return S_LINE_END;
            break;
        default:
            break;
    }

    return S_END;
}

void clear_xargs(char* xargv[MAXARG],int beg){
    for(int i=beg;i<MAXARG;i++){
        xargv[i]=0;
    }
}

int main(int argc,char* argv[]){
    if(argc-1>=MAXARG){
        fprintf(2,"xargs: too many arguments\n");
        exit(1);
    }

    int beg=0,end=0;

    char* xargv[MAXARG]={0};
    char lines[MAXSZ];
    char* p=lines;//p is the store position of read()
    int xargv_cnt=argc-1;

    for(int i=1;i<argc;i++){
        xargv[i-1]=argv[i];
    }

    enum state st=S_WAIT;

    while(st!=S_END){
        if(read(0,p,sizeof(char))!=sizeof(char)){
            st=S_END;
        }else{
            st=next_state(st,get_type(*p));
        }

        if(++end>=MAXSZ){//end is always the latest character's next position or p+1
            fprintf(2,"xargs: argument too long\n");
            exit(1);
        }

        switch(st){
            case S_WAIT:
                ++beg;
                break;
            case S_ARG_END:
                xargv[xargv_cnt++]=&lines[beg];
                beg=end;
                *p=0;
                break;
            case S_ARG_LINE_END:
                xargv[xargv_cnt++]=&lines[beg];//exec when meet line end;
            case S_LINE_END:
                beg=end;
                *p=0;
                if(fork()==0){
                    exec(argv[1],xargv);
                }
                xargv_cnt=argc-1;
                clear_xargs(xargv,xargv_cnt);
                wait(0);
                break;
            default:
                break;
        }

        ++p;
    }

    exit(0);
}