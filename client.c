// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#define PORT 8000

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    char buffer1[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    while (1)
    {
        printf("client> ");
        valread = read(sock, buffer, 1024);
        memset(buffer, 0, 1024);
        char *comman;
        comman = malloc(1024 * sizeof(char));
        size_t bufsize = 1024;
        getline(&comman, &bufsize, stdin);
        comman[strlen(comman) - 1] = '\0';
        char command[1024];
        int first_sp = 0;
        int gg = 0;
        for (int i = 0; i <= strlen(comman); i++)
        {
            if (first_sp == 0)
            {
                if (comman[i] == ' ' || comman[i] == '\t')
                {
                    continue;
                }
                else
                {
                    first_sp = 1;
                }
            }
            // printf("f4");
            command[gg] = comman[i];
            gg++;
        }
        char com[1024][1024];
        int word_count = 0;
        int char_count = 0;
        for (int i = 0; i < strlen(command); i++)
        {
            if (command[i] != ' ' && command[i] != '\t' && command[i] != '\0')
            {
                while (command[i] != ' ' && command[i] != '\t' && command[i] != '\0')
                {
                    com[word_count][char_count] = command[i];
                    i++;
                    char_count++;
                }
                com[word_count][char_count] = '\0';
                word_count++;
                char_count = 0;
            }
        }
        if (strcmp(com[0], "exit") == 0)
        {
            // printf("entered exit\n");
            send(sock, "exit", strlen("exit"), 0); //SEND ABCD
            break;
        }
        // printf("f1\n");
        char final_to[1024];
        memset(final_to, 0, 1024);
        if (strcmp(com[0], "get") == 0)
        {
            for (int i = 0; i < word_count; i++)
            {
                strcat(final_to, com[i]);
                strcat(final_to, " ");
            }

            // printf("final: %s\n", final_to);

            send(sock, final_to, strlen(final_to), 0); //SEND ABCD

            // send the message.
            // printf("Hello message sent\n");
            valread = read(sock, buffer, 1024);                            //READ ABCD
            send(sock, "message received", strlen("message received"), 0); //SEND ABCD
            // printf("complete: ***%s***\n", buffer);
            long long int tot_size = 0;
            int tot_files = 0;
            int marker;
            for (int i = strlen(buffer); i >= 0; i--)
            {
                if (buffer[i] == ',')
                {
                    marker = i;
                    break;
                }
            }
            tot_files = atoi(buffer + marker + 1);
            // printf("total file: %d\n", tot_files);
            buffer[marker] = '\0';
            for (int i = strlen(buffer); i >= 0; i--)
            {
                if (buffer[i] == ',')
                {
                    marker = i;
                    break;
                }
            }
            // printf("into atoi: %s\n", buffer+marker+1);
            tot_size = atoi(buffer + marker + 1);
            // printf("total size: %lld\n", tot_size);
            buffer[marker] = '\0';
            // printf("new: %s\n", buffer);
            if (buffer[strlen(buffer) - 1] == 'Y')
            {
                buffer[strlen(buffer) - 1] = '\0';
                printf("%s", buffer);
            }
            if (tot_size == 0)
            {
                printf("No files\n");
            }
            else if (tot_size > 0)
            {
                for (int i = 0; i < tot_files; i++)
                {
                    memset(buffer, 0, strlen(buffer));
                    valread = read(sock, buffer, 1024); //READ ABCD
                    int file_size = 0;
                    for (int i = strlen(buffer); i >= 0; i--)
                    {
                        if (buffer[i] == ',')
                        {
                            marker = i;
                            break;
                        }
                    }
                    file_size = atoi(buffer + marker + 1);
                    buffer[marker] = '\0';
                    printf("%s", buffer);
                    memset(buffer, 0, strlen(buffer));
                    send(sock, "message received", strlen("message received"), 0); //SEND ABCD
                    valread = read(sock, buffer, 1024);                            //READ ABCD 
                    // printf("buffer: %s\n", buffer);
                    int fdfd = open(buffer, O_RDWR | O_CREAT | O_TRUNC, 0600);
                    memset(buffer, 0, 1024);
                    if(fdfd<0)
                    {
                        printf("File already exists without permissions or is a directory\n");
                        send(sock, "N", strlen("N"), 0);
                        continue;
                    }
                    else
                    {
                        send(sock, "Y", strlen("Y"), 0);
                    }
                    char buf[1024];
                    // printf("file_size: %d\n", file_size);
                    printf("Progress:\n");
                    double progress=0;
                    double store=file_size;
                    while (file_size > 0)
                    {
                        valread = read(sock, buf, 1024); //READ ABCD
                        progress+=valread;
                        // printf("progress: %d\n", progress);
                        printf("\r%0.2lf%%", (double) (progress/store)*100);
                        buf[valread] = '\0';
                        // printf("buf: %s\n", buf);
                        file_size -= valread;
                        write(fdfd, buf, valread);
                        send(sock, "message received", strlen("message received"), 0); //SEND ABCD
                    }
                    close(fdfd);
                    printf("\n");
                    //valread = read(sock, buffer, 1024); //READ ABCD
                    memset(buffer, 0, strlen(buffer));
                }
            }
            memset(buffer, 0, strlen(buffer));
        }
    }
    return 0;
}
