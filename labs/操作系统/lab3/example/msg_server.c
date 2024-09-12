#include <stdlib.h>
#include <stdio.h> 
#include <sys/msg.h>
#include <signal.h>

#define MSGKEY 75         
 
struct msgform
{
	long mtype;
	char mtext[256];
}msg;
int msgqid;

main()
{
	int i,pid,*pint;
	extern cleanup();
	for (i=0;i<20;i++)       /*���жϴ���*/
		signal(i,cleanup);

    if ((msgqid=msgget(MSGKEY,0777|IPC_CREAT|IPC_EXCL))==-1)     /*������Ϣ����ʧ��*/
	{
		printf("This message queue already exists. \n",*pint);
		return;
	}
	for(;;)
	{
		msgrcv(msgqid,&msg,256,1,0);        /*�������Կͻ����̵���Ϣ*/ 	
		pint=(int *)msg.mtext;
		pid=*pint;
		printf("server:receive from pid %d\n",pid);   
		msg.mtype=pid;
		*pint=getpid();
		msgsnd(msgqid,&msg,sizeof(int),0);       /*����Ӧ����Ϣmsg*/
	}	
}

cleanup()
{
	msgctl(msgqid,IPC_RMID,0);
	exit(0);
}
