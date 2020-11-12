#include <kernel/types.h>
#include <user/user.h>

int main(){
    int p1[2],p2[2];
    //来回传输的字符数组：一个字节
    char buffer[] = {'X'};
    //传输字符数组的长度
    long length = sizeof(buffer);
    //父进程写，子进程读的pipe
    pipe(p1);
    //子进程写，父进程读的pipe
    pipe(p2);
    //子进程
    if(fork() == 0){
        //关掉不用的p1[1]、p2[0]
        close(p1[1]);
        close(p2[0]);
		//子进程从pipe1的读端，读取字符数组
		if(read(p1[0], buffer, length) != length){
			printf("a--->b read error!");
			exit();
		}
		printf("%d: received ping\n", getpid());
		//子进程向pipe2的写端，写入字符数组
		if(write(p2[1], buffer, length) != length){
			printf("a<---b write error!");
			exit();
		}
        exit();
    }
    close(p1[0]);
    close(p2[1]);
	//父进程向pipe1的写端，写入字符数组
	if(write(p1[1], buffer, length) != length){
		printf("a--->b write error!");
		exit();
	}
	//父进程从pipe2的读端，读取字符数组
	if(read(p2[0], buffer, length) != length){
		printf("a<---b read error!");
		exit();
	}
	printf("%d: received pong\n", getpid());
    //等待进程子退出
    wait();
	exit();
}
