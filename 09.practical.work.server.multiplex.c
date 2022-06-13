#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/select.h>
#define MAX_CLIENT 100
int main()
{
    setbuf(stdout, NULL);
    int clientfds[MAX_CLIENT];
    memset(clientfds, 0, sizeof(clientfds));

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
    char message[1024];

    printf("Listening on port %d...\n", port);

    while (running)
    {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(sockfd, &set);
        int maxfd = sockfd;

        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (clientfds[i] > 0)
                FD_SET(clientfds[i], &set);
            if (clientfds[i] > maxfd)
                maxfd = clientfds[i];
        }

        select(maxfd + 1, &set, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &set))
        {
            clientfd = accept(sockfd, (struct sockaddr *)&caddr, &clen);
            printf("Client %d connected!\n", clientfd);

            int clientfl = fcntl(clientfd, F_GETFL, 0);
            clientfl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, clientfl);

            for (int i = 0; i < MAX_CLIENT; i++)
            {
                if (clientfds[i] == 0)
                {
                    clientfds[i] = clientfd;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (clientfds[i] > 0 && FD_ISSET(clientfds[i], &set))
            {
                int read_status;
                read_status = recv(clientfds[i], message, sizeof(message), 0);
                if (read_status > 0)
                {
                    printf("\rClient %d: %s", clientfds[i], message);

                    while (message[strlen(message) - 1] != '\n')
                    {
                        recv(clientfds[i], message, sizeof(message), 0);
                        printf("%s", message);
                    }
                }
                else if (read_status == 0)
                {
                    printf("\rClient %d has disconnected.\n", clientfds[i]);
                    close(clientfds[i]);
                    clientfds[i] = 0;
                }
            }
        }
    }
    close(sockfd);
    return 0;
}
