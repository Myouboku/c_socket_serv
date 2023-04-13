#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8080

int server_fd, new_socket;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);

void *receive_message(void *arg)
{
    char buffer[1024] = {0};
    while (1)
    {
        int valread = read(new_socket, buffer, 1024);
        if (valread > 0)
        {
            printf("Client: %s\n", buffer);
            memset(buffer, 0, 1024);
        }
        else
        {
            printf("Client déconnecté.\n");
            break;
        }
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket échoué");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Association échouée");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("écoute");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_t tid;
        pthread_create(&tid, NULL, receive_message, NULL);

        char message[1024] = {0};
        while (1)
        {
            fgets(message, 1024, stdin);
            message[strcspn(message, "\n")] = 0;
            send(new_socket, message, strlen(message), 0);
            memset(message, 0, 1024);
        }
    }
    return 0;
}
