#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>


#define MSGKEY 75
#define MSGSIZE 8
#define SERVER 1

struct msgform
{
    long mtype;
    int mfrom;
    int mindex;
} msg;

int main()
{
    printf("%ld\n", sizeof(struct msgform));
    printf("%ld\n", sizeof(msg));
    printf("%ld\n", sizeof(int));
    printf("%ld\n", sizeof(long));

    return 0;
}