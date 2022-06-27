// Server side C/C++ program to demonstrate Socket
// programming
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

const int PORT = 9001;

char * generate_date();

struct http_request {
    int status_code;
    char path[0x100];
};

int main (int argc, char *argv[])
{
    struct sockaddr_in address;
    int socketfd, new_socket, file_fd;


    // Create socket
    if ( (socketfd = socket(AF_INET, SOCK_STREAM, 0 )) < 0){
        printf("SOCKET FAILED\n");
        return -1;
    }

    // Get rid of stupid timeout
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        puts("setsockopt(SO_REUSEADDR) failed");
        return -1;
    }

    // Setup address struct for bind
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (-1 == bind(socketfd, (struct sockaddr*)&address , sizeof(address))){
        printf("BIND FAILED\n");
        return -1;
    }

    // Mark the socket as listenable
    if (-1 == listen(socketfd,3)){
        printf("LISTEN FAILED\n");
        return -1;
    }

    // Server loop
    while (1){
        puts("AWATING CONNECTION");
        // New connection created, socket is one to rd/w to
        if ((new_socket = accept(socketfd, (struct sockaddr *)&address, (socklen_t*)&address))<0)
        {
            printf("LISTEN FAILED\n");
            return -1;
        }

        char request[0x1000] = {0};
        read(new_socket, request, 0x1000);
        char * token = strtok(request, "\n");
        char header[0x100] = {0}, value[0x100] = {0};
        char host[0x100] = {0}, type[0x100] = {0};
        char user_agent[0x100] = {0}, path[0x100] = {0}, version[0x100] = {0};
        int status_code;

        // Parse request
        sscanf(token, "GET /%s %s", path, version);
        printf("PATH: %s\n", path);

        while (strcmp(token, "\r") != 0){
            sscanf(token, "%[^:]: %s", header, value);
            printf("HEADER: %s\nVALUE: %s\n", header, value);
            token = strtok(NULL, "\n");
        }

        char responce[0x1000] = {0};
        char content[0x1000] = {0};
        // Used for sprintfing
        char tmp[0x100] = {0};
        char file_contents[0x2] = {0};

        status_code = 200;
        if ((file_fd = open(path, O_RDONLY)) < 0){
            puts("FILE NOT EXIST");
            status_code = 404;
            file_fd = open("404.html", O_RDONLY);
        }
        while (0 < read(file_fd,&file_contents, 1) ){
            strcat(content, file_contents);
        }

        switch (status_code) {
            case 404:
                strcat(responce, "HTTP/1.1 404 Not Found\n");
                break;
            case 200:
                strcat(responce, "HTTP/1.1 200 OK\n");
                break;
            default:
                strcat(responce, "HTTP/1.1 1337 WHAT\n");
                break;
        }
        strcat(responce, "Connection: close\n");
        strcat(responce,"Server: Eddie's 1337 server"); 
        sprintf(tmp,"Date: %s\n", generate_date());
        strcat(responce,tmp); 
        sprintf(tmp,"Content-Length: %li\n", strlen(content));
        strcat(responce,tmp); 

        strcat(responce, "\n\n");

        // Content starts here
        strcat(responce, content);
        strcat(responce, "\n\n");
        puts("WRITING RESPONCE");
        puts(responce);

        write(new_socket, responce, sizeof(responce));
    }
    close(socketfd);
    close(new_socket);
    return 0;
}


char * generate_date(){
    static char buf[0x100] = {0};
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return (char *)buf;
}
