#include<stdio.h> 
#include<stdlib.h> 
#include <sys/shm.h>

#define SHMKEY 75         /*¶¨Òå¹²ÏíÇø¹Ø¼ü´Ê*/

int shmid,p1,p2;
int *addr;
 
void CLIENT()
{
	int i;
	shmid=shmget(SHMKEY,1024,0777);  /*»ñÈ¡¹²ÏíÇø£¬³¤¶ÈÎª1024£¬¹Ø¼ü´ÊSHMKEY*/
	addr=(int *)shmat(shmid,0,0);           /*¹²ÏíÇøÆðÊ¼µØÖ·Îªaddr*/
	for(i=9;i>=0;i--)
	{
		while(*addr!=-1);
		printf("(client)sent%d\n",i);      /*´òÓ¡(client)sent*/
		*addr=i;                           /*°Ñi¸³¸øaddr*/
	}
	exit(0);
}

void SERVER()
{
	shmid=shmget(SHMKEY,1024,0777|IPC_CREAT);     /*´´½¨¹²ÏíÇø*/
	addr=(int *)shmat(shmid,0,0);                 /*¹²ÏíÇøÆðÊ¼µØÖ·Îªaddr*/
	do
	{
		*addr=-1;
		while(*addr==-1);
		printf("(server)received %d\n",*addr);	 /*·þÎñ½ø³ÌÊ¹ÓÃ¹²ÏíÇø*/
	}while(*addr);
	shmctl(shmid,IPC_RMID,0);
	exit(0);
}


int main()
{
	
	while ((p1=fork())==-1);                      /*´´½¨×Ó½ø³Ìp1,Ê§°ÜÊ±Ñ­»·*/
	if (!p1) SERVER();                             /*×Ó½ø³Ìp1Ö´ÐÐSERVER()*/
	while ((p2=fork())==-1);                      /*´´½¨×Ó½ø³Ìp2,Ê§°ÜÊ±Ñ­»·*/   
	if (!p2) CLIENT();                             /*×Ó½ø³Ìp2Ö´ÐÐCLIENT()*/
	wait(0);
	wait(0);
	return 0;
}