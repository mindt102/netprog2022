#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

int main()
{
    setbuf(stdout, NULL);

    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    clen = sizeof(caddr);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        return 1;
    }

    setsockopt(sockfd, SOL_SOCKET,
               SO_REUSEADDR, &(int){1},
               sizeof(int));

    int sockfl = fcntl(sockfd, F_GETFL, 0);
    sockfl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, sockfl);

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("Error binding");
        return 1;
    }

    if (listen(sockfd, 5) < 0)
    {
        perror("Error listening");
        return 1;
    }

    struct pollfd stdin_pollfd;
    stdin_pollfd.fd = fileno(stdin);
    stdin_pollfd.events = POLLIN;

    int running = 1;
    printf("Listening on port %d...\n", port);
    while (running)
    {
        clientfd = accept(sockfd, (struct sockaddr *)&caddr, &clen);
        if (clientfd > 0)
        {
            printf("A client connected!\n");

            int clientfl = fcntl(clientfd, F_GETFL, 0);
            clientfl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, clientfl);

            char message[1024];

            printf("> ");
            int connected = 1;
            int read_status;
            while (connected)
            {
                read_status = recv(clientfd, message, sizeof(message), 0);
                if (read_status > 0)
                {
                    printf("\rClient: ");
                    printf("%s", message);

                    while (message[strlen(message) - 1] != '\n')
                    {
                        recv(clientfd, message, sizeof(message), 0);
                        printf("%s", message);
                    }

                    printf("> ");
                }
                else if (read_status == 0)
                {
                    printf("\rClient disconnected.\n");
                    break;
                }

                if (poll(&stdin_pollfd, 1, 0) > 0)
                {
                    if (stdin_pollfd.revents & POLLIN)
                    {
                        fgets(message, sizeof(message), stdin);
                        if (strcmp(message, "/dc\n") == 0)
                        {
                            connected = 0;
                            break;
                        }
                        else if (strcmp(message, "/quit\n") == 0)
                        {
                            strcpy(message, "Disconnect\n");
                            printf("Disconnected.\n");
                            connected = 0;
                            running = 0;
                        }
                        else if (strcmp(message, "/clear\n") == 0)
                        {
                            system("clear");
                            printf("> ");
                            continue;
                        }

                        send(clientfd, message, strlen(message) + 1, 0);
                        while (message[strlen(message) - 1] != '\n')
                        {
                            fgets(message, sizeof(message), stdin);
                            send(clientfd, message, strlen(message) + 1, 0);
                        };

                        if (connected)
                        {
                            printf("> ");
                        }
                    }
                }
            }
            close(clientfd);
            if (running)
            {
                printf("Listening on port %d...\n", port);
            }
        }
    }
    close(sockfd);
    return 0;
}
