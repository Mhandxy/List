#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "list.h"

void* Append(void* llist);
void* Insert(void* llist);
void* Delete(void* llist);
void* Print(void* llist);

void DeleteNumber(void* e)
{
    printf("删除:\t  %d\n",*(int*)e);
    free(e);
}
void PrintNumber(void* e,void* unused)
{
    if(e!=NULL)
        printf("%d - ",*(int*)e);
}

int main()
{
    pthread_t thread[4];
    LINKED_LIST list=LlCreate();

    pthread_create(&thread[0],NULL,&Append,(void*)list);
    pthread_create(&thread[1],NULL,&Insert,(void*)list);
    pthread_create(&thread[2],NULL,&Delete,(void*)list);
    pthread_create(&thread[3],NULL,&Print,(void*)list);

    for(int i=0;i<4;i++)
        pthread_join(thread[i],NULL);
    return 0;
}

void* Append(void* llist)
{
    static int i=10;
    while(i--)
    {
        sleep(3);
        printf("头部插入: %d\n",i);
        int* tmp=(int*)malloc(sizeof(int));
        *tmp=i;
        LlAppend(llist,tmp);
    }
}

void* Insert(void* llist)
{
    static int i=10;
    while(i--)
    {
        sleep(3);
        printf("尾部插入: %d\n",i);
        int* tmp=(int*)malloc(sizeof(int));
        *tmp=i;
        LlInsert(llist,tmp,0);
    }
}
//每两秒删除链表中部的数字
void* Delete(void* llist)
{
    static int i=15;
    while(i--)
    {
        sleep(2);
        unsigned int length=LlGetCount(llist);
        if(length==0) continue;
        length=length/2;
        LlDelete(llist,length,&DeleteNumber);
    }
}
//每五秒遍历输出一边链表
void* Print(void* llist)
{
    static int i=7;
    while(i--)
    {
        sleep(5);
        LlTraverse(llist,&PrintNumber,NULL);
        printf("NULL\n\n");
    }
}

