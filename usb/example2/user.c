#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
int main(int argc, char *argv[])
{
    int fd;
    ssize_t num_read, num_written;
    char my_read_str[100];
    /* Open kernel */
    fd = open("/dev/pen0", O_RDWR);
    if (fd==-1)
    {
        perror("Cannot open file description from kernel \n");
        return -1;
    }

    /* Read value data from kernel */
    num_read = read(fd, &my_read_str, 100);
    if (num_read==-1) /* Check error*/
    {
        perror("Cannot read information from kernel \n");
        return -1;
    }
    printf("returned %d from the system call, num bytes read: %d\n",*my_read_str, (int)num_read);

    /* Write back the new value data to kernel */
    num_written = write(fd, argv[1], sizeof(argv[1]));
    if (num_written==-1) /* Check error */
    {
        perror("Cannot write back to kernel \n");
        return -1;
    }
    printf("successfully wrote %s (%d bytes) to the kernel\n", argv[1],(int)num_written);

    close(fd);

    return 0;
}
