/*this program will read the current value of led register, write the new value to turn led on
  display the new value, sleep for 2 second and write a value to turn led off*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int fd;
    ssize_t num_read, num_written;
    unsigned int  my_read_str;
    const char my_write_str[10] = "78";
    const char led_of[10] = "15";

    fd = open("/dev/chardev", O_RDWR); //open the file

    num_read = read(fd, &my_read_str, 0); //read the value
    printf("current value of the LED reg is 0x%06x, (%zd bytes)\n",
          my_read_str, num_read); //output the value

    num_written = write(fd, my_write_str, sizeof(my_write_str)); //write new value
    printf("successfully wrote %s (%zd bytes) to the LED reg to turn LED on\n",my_write_str, num_written);

    num_read = read(fd, &my_read_str, 0);
    printf("Value of the LED reg after being written is 0x%04x, (%zd bytes)\n", my_read_str, num_read);

    sleep(2);

    num_written = write(fd, led_of, sizeof(led_of));
    printf("successfully wrote %s (%zd bytes) to the LED reg to turn LED off\n",led_of, num_written);

    close(fd); //close the file

    return 0;
}
