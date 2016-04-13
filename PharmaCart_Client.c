//
//  PharmaCart_bot_beta.c
//
//
//  Created by Forrest Lee on 2/10/16.
//
//


#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 3595
#define BUFFSIZE 1024

char * clear_string(char str[], int strlen)
{
    int i;
    for (i = 0; i < strlen; i++)
        str[i] = '\0';
    return str;
}

int main(int argc, char * argv[])
{
    int sockfd;
    struct sockaddr_in address;
    int result, num;
    int status;
    char tmpbuffer[BUFFSIZE];
    int location_transmission = 0;
    
    time_t rawtime;
    struct tm * timeinfo;
    
    //update time stamps
    time ( &rawtime);
    timeinfo = localtime (&rawtime);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("oops: socket");
        exit(1);
    }
    
    address.sin_family = AF_INET;
    //address = anyaddress;
    address.sin_addr.s_addr = inet_addr(argv[1]);
    address.sin_port = htons(PORT);
    
    result = connect(sockfd, (struct sockaddr*) &address, sizeof(address));
    
    if(result == -1) {
        perror("oops: connect");
        exit(1);
    }
    
    if (argc >= 3)
    {
        location_transmission = 1;
        strcpy(tmpbuffer, argv[2]);
    }
    else
    {
        location_transmission = 0;
        strcpy(tmpbuffer, "com_request");
    }
    if ((send(sockfd, tmpbuffer, strlen(tmpbuffer),0))== -1)
    {
        fprintf(stderr, "Failure Sending Initial Message\n");
        close(sockfd);
    }
    
    while(!location_transmission) {
        
        char buffer[BUFFSIZE];
        char cmd_text[BUFFSIZE];
        char * tmp_p;
        char tmp[BUFFSIZE];
        
        //update time stamps
        time ( &rawtime);
        timeinfo = localtime (&rawtime);
        
        clear_string(buffer, BUFFSIZE);
        
        num = recv(sockfd, buffer, sizeof(buffer),0);
        if ( num <= 0 )
        {
            printf("Either Connection Closed via Server or Error\n");
            //Break from the While
            break;
        }
        
        printf("Client:Message Received From Server -  %s\n",buffer);
        
        if (strstr(buffer, "disconnect") != NULL) {
            printf("Server: disconnected\n");
            close(sockfd);
            exit(0);
        }
        
        //not a get_info or disconnect request
        if (strstr(buffer, "command") != NULL && strstr(buffer, "command get_info") == NULL) {
            tmp_p = &(buffer[8]);
            memcpy(tmp, tmp_p, BUFFSIZE-9);
            sprintf(cmd_text, "echo '%s' >> command.queue;", tmp );
            status = system(cmd_text);
        }
        else //is get_info command
        {
            sprintf(buffer, "inventory:\n a:0 \n b:0" );
        }

        if ((send(sockfd,buffer, sizeof(buffer),0))== -1)
        {
            fprintf(stderr, "Failure Sending confirmation Message\n");
            close(sockfd);
            break;
        }
    }
}
