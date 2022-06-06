#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

int main()
{
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        return 1;
    }

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

    while (1)
    {
        printf("Listening on port %d...\n", port);

        clen = sizeof(caddr);
        if ((clientfd = accept(sockfd, (struct sockaddr *)&caddr, &clen)) < 0)
        {
            perror("Error accepting connection");
            return 1;
        }

        printf("A client connected!\n");

        char message[1024];

        int connected = 1;
        while (connected)
        {
            printf("Client: ");
            do
            {
                recv(clientfd, message, sizeof(message), 0);
                printf("%s", message);
                if (strcmp(message, "Disconnect\n") == 0)
                {
                    connected = 0;
                }
            } while (message[strlen(message) - 1] != '\n');

            if (!connected)
            {
                break;
            }

            printf("> ");
            do
            {
                fgets(message, sizeof(message), stdin);
                if (strcmp(message, "/dc\n") == 0)
                {
                    strcpy(message, "Disconnect\n");
                    printf("Disconnected.\n");
                    connected = 0;
                }
                send(clientfd, message, strlen(message) + 1, 0);
            } while (message[strlen(message) - 1] != '\n');
        }
        close(clientfd);
    }
    close(sockfd);
    return 0;
}
