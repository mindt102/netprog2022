#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define MAX_LENGTH 255

int main(int argc, char **argv)
{
    char domain[MAX_LENGTH];
    if (argc > 1)
    {
        strncpy(domain, argv[1], MAX_LENGTH);
    }
    else
    {
        printf("Enter a domain name: ");
        scanf("%255s", domain);
    }
    struct hostent *host_ptr = gethostbyname(domain);

    if (host_ptr == NULL)
    {
        printf("Cannot find address for hostname: %s\n", domain);
        return 0;
    }

    printf("Official hostname: %s\n", host_ptr->h_name);

    char ip_addr[32];
    int total_addr = sizeof(host_ptr->h_addr_list) / sizeof(host_ptr->h_addr_list[0]);

    for (int i = 0; i < total_addr; i++)
    {
        printf("Address: %s\n", inet_ntop(host_ptr->h_addrtype, host_ptr->h_addr_list[i], ip_addr, sizeof(ip_addr)));
    }
    return 0;
}