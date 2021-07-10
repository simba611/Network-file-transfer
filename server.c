#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 8000
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char buffer1[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;         // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons(PORT);       // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    while (1)
    {
        if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        while (1)
        {
            send(new_socket, "Dummy", strlen("Dummy"), 0);
            valread = read(new_socket, buffer, 1024); // read infromation received into the buffer //READ ABCD
            if (strcmp(buffer, "exit") == 0)
            {
                break;
            }
            char com[1024][1024];
            int word_count = 0;
            int char_count = 0;
            for (int i = 0; i < strlen(buffer); i++)
            {
                if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\0')
                {
                    while (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\0')
                    {
                        com[word_count][char_count] = buffer[i];
                        i++;
                        char_count++;
                    }
                    com[word_count][char_count] = '\0';
                    word_count++;
                    char_count = 0;
                }
            }
            int exist[word_count];
            char error2[1024];
            int was_err = 0;
            int total_files = 0;
            memset(error2, 0, 1024);
            strcat(error2, "Files: ");
            for (int i = 0; i < word_count; i++)
            {
                exist[i] = 1;
            }
            for (int i = 1; i < word_count; i++)
            {
                struct stat st;
                int isdir=stat(com[i], &st);
                if (access(com[i], R_OK) != -1 && !(isdir==0 && S_ISDIR(st.st_mode)))
                {
                    total_files++;
                }
                else
                {
                    was_err = 1;
                    strcat(error2, com[i]);
                    strcat(error2, ", ");
                    exist[i] = 0;
                }
            }
            if (was_err == 1)
            {
                error2[strlen(error2) - 1] = '\0';
                error2[strlen(error2) - 1] = '\0';
            }
            strcat(error2, " do not exist or can't be read\nStarting download of existing files\n");
            // printf("%s\n", buffer);
            memset(buffer, 0, strlen(buffer));
            if (was_err == 1)
            {
                strcat(error2, "Y");
            }
            else
            {
                strcat(error2, "N");
            }
            struct stat aa;
            long long int total_size = 0;
            for (int i = 1; i < word_count; i++)
            {
                if (exist[i] == 1)
                {
                    stat(com[i], &aa);
                    total_size += aa.st_size;
                }
            }
            char string_size[1024];
            sprintf(string_size, "%lld", total_size);
            strcat(error2, ",");
            strcat(error2, string_size);
            memset(string_size, 0, strlen(string_size));
            strcat(error2, ",");
            sprintf(string_size, "%d", total_files);
            strcat(error2, string_size);
            memset(string_size, 0, strlen(string_size));
            send(new_socket, error2, strlen(error2), 0); // use sendto() and recvfrom() for DGRAM //SEND ABCD
            valread = read(new_socket, buffer, 1024); //READ ABCD
            for (int i = 1; i < word_count; i++)
            {
                if (exist[i] == 1)
                {
                    stat(com[i], &aa);
                    sprintf(string_size, "Downloading: %s\n, %ld", com[i], aa.st_size);
                    send(new_socket, string_size, strlen(string_size), 0); //SEND ABCD
                    memset(string_size, 0, strlen(string_size));
                    valread = read(new_socket, buffer1, 1024); //READ ABCD

                    sprintf(string_size, "%s", com[i]);
                    send(new_socket, string_size, strlen(string_size), 0); //SEND ABCD
                    memset(string_size, 0, strlen(string_size));
                    memset(buffer1, 0, strlen(buffer1));
                    valread = read(new_socket, buffer1, 1024); //READ ABCD
                    buffer1[valread]='\0';
                    if(buffer1[0]=='N')
                    {
                        continue;
                    }
                    int fdfd = open(com[i], O_RDONLY);
                    char buf[1030];
                    long long int file = aa.st_size;
                    // printf("file_size: %d\n", file);
                    while (file > 0)
                    {
                        // printf("before reading\n");
                        int so_far = read(fdfd, buf, 1024);
                        buf[so_far]='\0'; 
                        file -= so_far;
                        send(new_socket, buf, strlen(buf), 0); //SEND ABCD
                        memset(buf, 0, strlen(buf));
                        // printf("before reading22\n");
                        valread = read(new_socket, buffer1, 1024); //READ ABCD
                    }
                    close(fdfd);
                   // send(new_socket, "Downloaded", strlen("Downloaded"), 0); //SEND ABCD
                }
            }
            memset(buffer, 0, strlen(buffer));
            // send(new_socket, string_size, strlen(string_size), 0);
            // printf("First message sent\n");
        }
    }
    return 0;
}
