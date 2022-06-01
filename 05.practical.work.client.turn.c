#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_LENGTH 255
#define PORT 8784

int main(int argc, char **argv)
{
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

    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("Error connecting.\n");
        return 1;
    }
    char ip_addr[32];
    printf("Connected to %s\n", inet_ntop(host_ptr->h_addrtype, host_ptr->h_addr_list[0], ip_addr, sizeof(ip_addr)));

    char message[1024];

    while (1)
    {
        printf("> ");
        fgets(message, 1024, stdin);
        send(sockfd, message, strlen(message) + 1, 0);
        recv(sockfd, message, sizeof(message), 0);
        printf("Server: %s", message);
    }
    return 0;
}