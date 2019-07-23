#include"work_thread.h"
#define ARGC 10
#include<fcntl.h>
#include<unistd.h>

void thread_start(int c)
{
    pthread_t id;
    pthread_create(&id,NULL,work_thread,(void*)c);

}


void get_argv(char buff[],char *myargv[])//字符串分割
{
    //strtok_r();
    char *p=NULL;
    char *s=strtok_r(buff," ",&p);
    int i=0;
    while(s!=NULL)
    {
        myargv[i++]=s;
        s=strtok_r(NULL," ",&p);

    }
}


        


int recv_file(int c,char*name)
{
    send(c,"sd",2,0);
    char buff[128]={0};
    if(recv(c,buff,127,0)<=0)
    {
        return -1;
    }
 
    if(strncmp(buff,"ok#",3)!=0)
    {
        printf("%s\n",buff);
        return; 
    }
   // printf("%s\n",buff);
    int size=0;

    sscanf(buff+3,"%d",&size);
    int fd=open(name,O_WRONLY|O_CREAT,0600);

   // printf("%s\n",buff);
    if(fd==-1)
    {
        send(c,"err",3,0);
        return;

    }
    
    send(c,"ok",2,0);    

    int num=0;
    int cur_size=0;
    char data[256]={0};

  //  printf("%s\n",buff);
    while(1)
    {
       // printf("num\n");
        num=recv(c,data,256,0);
        if(num<=0)
        {
         //   printf("num<=0\n");
            return -1;
        }
        write(fd,data,num);
        cur_size=cur_size+num;
        if(cur_size>=size)
        {
            break;
        }

    }
    close(fd);
    return 0;

}
void send_file(int c,char *myargv[])
{
    if(myargv[1]==NULL)
    {
        send(c,"no file name",17,0);
        return;
    }
    int fd=open(myargv[1],O_RDONLY);
    if(fd==-1)
    {
        send(c,"not found!",10,0);
        return;
    }
    int size=lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);

    char status[32]={0};
    sprintf(status,"ok#%d",size);
    send(c,status,strlen(status),0);
    
    char cli_status[32]={0};
    if(recv(c,cli_status,31,0)<=0)
    {
        return;
    }
    if(strcmp(cli_status,"ok")!=0)
    {
        return;
    }

    char data[256]={0};
    int num=0;
    while((num=read(fd,data,256))>0)
    {
        
        send(c,data,num,0);
        

    }
    close(fd);
    return;

}


//工作线程
void* work_thread(void *arg)
{
    

    int c=(int)arg;

    //测试
    while(1)
    
    {
        char buff[128]={0};
        int n=recv(c,buff,127,0);
        if(n<=0)
        
        {
            close(c);
            printf("one client over\n");
            break;
        }
        
        printf("recv=%s\n",buff);
          
        
        char *myargv[ARGC]={0};
        get_argv(buff,myargv);//把收到的put aa  进行分割
        //再进行字符串比较
        if(strcmp(myargv[0],"get")==0)
        {
            send_file(c,myargv);
        }
        else if(strcmp(myargv[0],"put")==0)
        {
             recv_file(c,myargv[1]);
        }
        else
        {
             int pipefd[2];
             pipe(pipefd);//创建管道

             pid_t pid=fork();
             if(pid==0)
             {
                 dup2(pipefd[1],1);//fd[0]读   fd[1]写 1标准输出
                 dup2(pipefd[1],2);// 标准错误输出 2

                 execvp(myargv[0],myargv);
                 perror("exec error\n");
                 exit(0);
             }

             close(pipefd[1]);//guanbixieduan
             wait(NULL);
             char read_buff[1024]={0};
             strcpy(read_buff,"ok#");
             read(pipefd[0],read_buff+strlen(read_buff),1000);       //kenengdubudaoshujv
        
             send(c,read_buff,strlen(read_buff),0);
         }
    
    }
}
