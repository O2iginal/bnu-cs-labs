#include <stdlib.h> 
#include <signal.h>
#include <stdio.h>
int p1,p2; 

main() 
{
	int fd[2];
	char OutPipe[100],InPipe[100];
	pipe(fd);               /*¸¸½ø³Ì½¨Á¢¹ÜµÀ*/
	while ((p1=fork())==-1);    /*´´½¨×Ó½ø³Ìp1,Ê§°ÜÊ±Ñ­»·*/
	if (p1==0)                   /*Ö´ÐÐ×Ó½ø³Ìp1*/
	{
		lockf(fd[1],1,0);      /*¼ÓËøËø¶¨Ð´Èë¶Ë*/
		sprintf(OutPipe,"child 1 process is sending message!");
		write(fd[1],OutPipe,50);   /*°ÑOutPipeÖÐµÄ50¸ö×Ö·ûÐ´Èë¹ÜµÀ*/
		sleep(5);                 /*Ë¯Ãß5Ãë*/
		lockf(fd[1],0,0);         /*ÊÍ·ÅÐ´Èë¶Ë*/
		exit(0);                  /*¹Ø±Õpid1*/
	}
	else /*´Ó¸¸½ø³Ì·µ»Ø£¬Ö´ÐÐ¸¸½ø³Ì*/
	{
		while ((p2=fork())==-1);   /*´´½¨×Ó½ø³Ìp2,Ê§°ÜÊ±Ñ­»·*/
		if (p2==0)                  /*ÓÉ×Ó½ø³Ìp2·µ»Ø£¬Ö´ÐÐ×Ó½ø³Ìpid2*/
		{
			lockf(fd[1],1,0);     /*¼ÓËøËø¶¨Ð´Èë¶Ë*/
			sprintf(OutPipe,"child 2 process is sending message!");
			write(fd[1],OutPipe,50);   /*°ÑOutPipeÖÐµÄ50¸ö×Ö·ûÐ´Èë¹ÜµÀ*/
			sleep(5);                  /*Ë¯Ãß5Ãë*/
			lockf(fd[1],0,0);          /*ÊÍ·ÅÐ´Èë¶Ë*/
			exit(0);                   /*¹Ø±Õpid2*/
		}
		else
		{
			read(fd[0],InPipe,50);   /*¶ÁÈ¡¹ÜµÀÖÐµÄÏûÏ¢*/
			printf("%s\n",InPipe);   /*´òÓ¡ÏûÏ¢*/
			read(fd[0],InPipe,50);
			printf("%s\n",InPipe);
			
		}

	}
}