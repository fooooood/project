#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

//启动线程
void thread_start(int c);

//工作线程
void *work_thread(void *arg);

