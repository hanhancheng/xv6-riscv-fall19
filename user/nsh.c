#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
 
#define MAXARGS 10
#define MAXWORD 30
#define MAXLINE 100
 
int getcmd(char *buf, int nbuf)
{
    printf("@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if (buf[0] == 0) //Error
        return -1;
    return 0;
}

char whitespace[] = " \t\r\n\v";
char args[MAXARGS][MAXWORD]; //每一行存储一个输入参数

//解析划分cmd中的输入
void setargs(char *cmd, char* argv[],int* argc)
{
    for(int i=0; i<MAXARGS; i++)
    {
        argv[i]=&args[i][0];
    }
    int i = 0;
    int j = 0;
    for (; cmd[j] != '\n' && cmd[j] != '\0'; j++)
    {
        //跳过之前的空格
        while (strchr(whitespace,cmd[j])){
            j++;
        }
        //找到有用的参数存入argv
        argv[i++]=cmd+j;
        //继续往后找到下一个空格
        while (strchr(whitespace,cmd[j])==0){
            j++;
        }
        cmd[j]='\0';
    }
    argv[i]=0;
    *argc=i;
}

void execPipe(char*argv[],int argc);

void runcmd(char*argv[],int argc)
{
    for(int i=1; i<argc; i++){
        if(!strcmp(argv[i],"|")){
            //判断是否是管道命令
            execPipe(argv,argc);
        }
    }
    for(int i=1;i<argc;i++){
        //判断是否是输出重定向
        if(!strcmp(argv[i],">")){
            close(1); //关闭标准输出
            open(argv[i+1],O_CREATE|O_WRONLY);
            argv[i]=0;
        }
        //输入重定向
        if(!strcmp(argv[i],"<")){
            close(0); //关闭标准输入
            open(argv[i+1],O_RDONLY);
            argv[i]=0;
        }
    }
    exec(argv[0], argv);
}

void execPipe(char*argv[],int argc){
    int i=0;
    //找到命令中的"|",替换成'\0'
    for(; i<argc; i++){
        if(!strcmp(argv[i],"|")){
            argv[i]=0;
            break;
        }
    }
    int fd[2];
    pipe(fd);
    if(fork()==0){
        //子进程，关闭标准输出，执行左边命令
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv,i);
    }else{
        //父进程，关闭标准输入，执行右边命令
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv+i+1,argc-i-1);
    }
}
 

int main()
{
    char buf[MAXLINE];
    while (getcmd(buf, sizeof(buf)) >= 0)
    {
 
        if (fork() == 0)
        {
            char* argv[MAXARGS];
            int argc=-1;
            setargs(buf, argv,&argc);
            runcmd(argv,argc);
        }
        wait(0);
    }
    exit(0);
}