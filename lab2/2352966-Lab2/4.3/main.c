#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define FIFO_NAME "abc_fifo_name"

// rcv: opens the FIFO for reading, reads a message, and prints it.
int rcv();
// fifo_snd: opens the FIFO for writing and sends the provided message.
int fifo_snd(const char *msg);

int main()
{
    // Create FIFO
    if (mkfifo(FIFO_NAME, 0666) != 0)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            return 1;
        }
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        // Child process: act as receiver.
        if (rcv() != 0)
            perror("rcv");
        return 0;
    }
    else
    {
        // Parent process: act as sender.
        const char *message = "Hello FIFO message passing!";
        if (fifo_snd(message) != 0)
            perror("fifo_snd");

        // Wait for the child process to finish.
        wait(NULL);

        // Remove the FIFO file after use.
        unlink(FIFO_NAME);
    }

    return 0;
}

int rcv()
{
    // Open the FIFO for reading.
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd < 0)
    {
        perror("open FIFO");
        return -1;
    }

    // Read from the FIFO.
    char buffer[256];
    int readEnd = read(fd, buffer, 255);
    if (readEnd < 0)
    {
        perror("read FIFO");
        close(fd);
        return -1;
    }
    buffer[readEnd] = '\0'; // Null-terminate the string.

    printf("Received message: %s\n", buffer);

    close(fd);
    return 0;
}

int fifo_snd(const char *msg)
{
    // Open the FIFO for writing.
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd < 0)
    {
        perror("open FIFO");
        return -1;
    }

    // Write the message into the FIFO.
    if (write(fd, msg, strlen(msg)) < 0)
    {
        perror("write FIFO");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}