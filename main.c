#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <receiver_ip> <receiver_port> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sender_fd;
    struct sockaddr_in sender_addr;
    char buffer[BUFFER_SIZE];

    // Extract command-line arguments
    char *receiver_ip = argv[1];
    int receiver_port = atoi(argv[2]);
    char *message = argv[3];

    // Create sender socket
    if ((sender_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize sender address
    memset(&sender_addr, 0, sizeof(sender_addr));
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_port = htons(PORT);
    sender_addr.sin_addr.s_addr = INADDR_ANY;

    // Prepare receiver address
    struct sockaddr_in receiver_addr;
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(receiver_port);
    inet_pton(AF_INET, receiver_ip, &receiver_addr.sin_addr);

    // Prepare message
    strncpy(buffer, message, BUFFER_SIZE);

    // Send the message using sendmsg
    struct iovec iov[1];
    iov[0].iov_base = buffer;
    iov[0].iov_len = strlen(buffer) + 1;

    struct msghdr message_header;
    memset(&message_header, 0, sizeof(message_header));
    message_header.msg_name = &receiver_addr;
    message_header.msg_namelen = sizeof(receiver_addr);
    message_header.msg_iov = iov;
    message_header.msg_iovlen = 1;

    ssize_t bytes_sent = sendmsg(sender_fd, &message_header, 0);
    if (bytes_sent < 0) {
        perror("Sendmsg syscall failed");
        exit(EXIT_FAILURE);
    }

    printf("Message sent successfully\n");

    return 0;
}
