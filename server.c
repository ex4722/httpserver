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

int main (int argc, char *argv[])
{
    struct sockaddr_in address;
    int socketfd, new_socket, file_fd;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( (socketfd = socket(AF_INET, SOCK_STREAM, 0 )) < 0){
        printf("SOCKET FAILED\n");
        return -1;
    }

    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        puts("setsockopt(SO_REUSEADDR) failed");
        return -1;
    }

    if (-1 == bind(socketfd, (struct sockaddr*)&address , sizeof(address))){
        printf("BIND FAILED\n");
        return -1;
    }

    if (-1 == listen(socketfd,3)){
        printf("LISTEN FAILED\n");
        return -1;
    }
    while (1){

        puts("AWATING CONNECTION");
        if ((new_socket = accept(socketfd, (struct sockaddr *)&address, (socklen_t*)&address))<0)
        {
            printf("LISTEN FAILED\n");
            return -1;
        }

        char request[0x100] = {0};
        read(new_socket, request, 0x100);
        char * token = strtok(request, "\n");

        /* while ((token = strtok(NULL, "\n")) != 0){ */
        /*     printf("%s\n", token); */
        /* } */

        char responce[0x100] = {0};
        char content[0x100] = {0};
        char tmp[0x100] = {0};

        /* file_fd = open(argv[1], O_RDONLY); */
        file_fd = open("./foo.html", O_RDONLY);
        char buf;
        while (0 < read(file_fd,&buf, 1) ){
            strcat(content, &buf);
        }


        strcat(responce, "HTTP/1.1 200 OK\n");
        strcat(responce, "Connection: close\n");
        /* sprintf(tmp,"Date: %s\n", generate_date()); */
        /* strcat(responce,tmp); */ 
        sprintf(tmp,"Content-Length: %li\n", strlen(content));
        strcat(responce,tmp); 

        strcat(responce, "\n\n");
        // Content starts here
        strcat(responce, content);
        strcat(responce, "\n\n");
        puts("WRITING RESPONCE");
        /* puts(responce); */

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
