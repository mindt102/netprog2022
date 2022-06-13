#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#define MAX_LENGTH 255
#define PORT 8784

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);

    struct sockaddr_in saddr;
    int sockfd;

    char hostname[MAX_LENGTH];
    if (argc > 1)
    {
        strncpy(hostname, argv[1], MAX_LENGTH);
    }
    else
    {
        printf("Enter a hostname: ");
        scanf("%255s", hostname);

        // Clear input buffer
        // https://www.geeksforgeeks.org/clearing-the-input-buffer-in-cc/
        while ((getchar()) != '\n')
            ; // I try to fix this but VSCode auto format feature hates me
    }

    struct hostent *host_ptr;
    if ((host_ptr = gethostbyname(hostname)) == NULL)
    {
        perror("Error Resolving.\n");
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error creating socket.\n");
        return 1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *)&saddr.sin_addr.s_addr, host_ptr->h_addr_list[0], host_ptr->h_length);
    saddr.sin_port = htons(PORT);

    setsockopt(sockfd, SOL_SOCKET,
               SO_REUSEADDR, &(int){1},
               sizeof(int));

    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("Error connecting.\n");
        return 1;
    }

    int sockfl = fcntl(sockfd, F_GETFL, 0);
    sockfl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, sockfl);

    struct pollfd stdin_pollfd;
    stdin_pollfd.fd = fileno(stdin);
    stdin_pollfd.events = POLLIN;

    char ip_addr[32];
    printf("Connected to %s\n> ", inet_ntop(host_ptr->h_addrtype, host_ptr->h_addr_list[0], ip_addr, sizeof(ip_addr)));

    char message[1024];

    int connected = 1;
    int read_status;
    while (connected)
    {
        read_status = recv(sockfd, message, sizeof(message), 0);
        if (read_status > 0)
        {
            printf("\rServer: ");
            printf("%s", message);

            if (strcmp(message, "/clear\n") == 0)
            {
                system("clear");
                printf("> ");
                continue;
            }

            while (message[strlen(message) - 1] != '\n')
            {
                recv(sockfd, message, sizeof(message), 0);
                printf("%s", message);
            }
            printf("> ");
        }
        else if (read_status == 0)
        {
            printf("\rServer disconnected.\n");
            break;
        }

        if (poll(&stdin_pollfd, 1, 0) > 0)
        {
            if (stdin_pollfd.revents & POLLIN)
            {
                fgets(message, sizeof(message), stdin);
                if (strcmp(message, "/quit\n") == 0)
                {
                    connected = 0;
                    break;
                }

                send(sockfd, message, strlen(message) + 1, 0);
                while (message[strlen(message) - 1] != '\n')
                {
                    fgets(message, sizeof(message), stdin);
                    send(sockfd, message, strlen(message) + 1, 0);
                }

                if (connected)
                {
                    printf("> ");
                }
            }
        }
    }
    close(sockfd);
    return 0;
}
