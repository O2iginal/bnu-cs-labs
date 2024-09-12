#include <stdio.h> 
#include <sys/msg.h> 
#define MSGKEY 75         /*¶¨Òå¹Ø¼ü´Ê*/

struct msgform
{
	long mtype;
	char mtext[256]; 
};

main()
{
	struct msgform msg;
	int msgqid,pid,*pint;

	if ((msgqid=msgget(MSGKEY,0777))==-1)     /*½¨Á¢ÏûÏ¢¶ÓÁÐÊ§°Ü*/
	{
		printf("This message queue does not exist. \n",*pint);
		return;
	}
	pid=getpid();
	pint=(int *)msg.mtext;
	*pint=pid;
	msg.mtype=1;                     /*Ö¸¶¨ÏûÏ¢ÀàÐÍ*/
	msgsnd(msgqid,&msg,sizeof(int),0);       /*Íùmsgqid·¢ËÍÏûÏ¢msg*/
	msgrcv(msgqid,&msg,256,pid,0);        /*½ÓÊÕÀ´×Ô·þÎñÆ÷½ø³ÌµÄÏûÏ¢*/
	printf("client:receive from pid %d\n",*pint);
}

