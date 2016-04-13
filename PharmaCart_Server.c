//
//  PharmaCart_Server.c
//  
//
//  Created by Forrest Lee on 2/19/16.
//
//

#include "PharmaCart_Server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>

#define PORT 3595
#define sPORT "3595"
#define BACKLOG 5
#define BUFFSIZE 1024
#define NUMBOTS 3

time_t rawtime;
struct tm * timeinfo;
int status = 0;
int num_bots = 0;
int close_flag = 0;
struct bot_info bot_list[BACKLOG];
char queue[16][16];
int queue_count = 0;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &( ((struct sockaddr_in*)sa)->sin_addr);
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void printMap() {
    printf("\n");
    int i, j;
    for (i = 0; i < 11; ++i)
    {
        for (j = 0; j < 11; ++j)
        {
            printf("%c ", Map[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_data()
{
    int k, l;
    //print robot data
    printf("Command Queue (top - more recent):\n");
    if (queue_count == 0)
        printf("EMPTY\n");
    else
        for (l = 0; l < queue_count: l++)
            printf("%s\n", queue[l]);
    
    
    printf("Current PharmaCart Data\n");
    for (k = 0; k < NUMBOTS; k++)
    {
        printf("Cart %d:\n", k+1);
        if (!bot_list[k].vacant)
        {
            printf("IP: %s\n", bot_list[k].ip);
            //printf("Last location: %s\n", STRINGOFLOCATION);
            //printf("Inventory etc etc
        }
        else
            printf("VACANCY\n");
    }
    printf("Location Map:\n");
    printMap();
}

//ensure commmand is valid
int cmd_input( char * args, int client_sockfd )
{
    char cmd[BUFFSIZE];
    char basecmd[BUFFSIZE];
    char tmp[BUFFSIZE];
    int i;
    int valid = 0;
    strcpy(cmd, args);
    strcpy(basecmd, "disp bot_data");
    if (strstr(cmd, basecmd) != NULL) {
        printf("number of PharmaCarts connected %d\n", num_bots);
        for (i = 0; i < num_bots; i++)
            printf("%s\n", bot_list[i]);
        return 0;
    }
    strcpy(basecmd, "command");
    if (strstr(cmd, basecmd) != NULL)
        return 1;
    strcpy(basecmd, "done");
    if (strstr(cmd, basecmd) != NULL)
        return 1;
    strcpy(basecmd, "get_info");
    if (strstr(cmd, basecmd) != NULL)
        return 1;
    strcpy(basecmd, "disconnect");
    if (strstr(cmd, basecmd) != NULL)
    {
        close_flag = 1;
        return 1;
    }
    strcpy(basecmd, "shutdown server");
    if (strstr(cmd, basecmd) != NULL)
    {
        close(client_sockfd);
        exit(0);
        return 1;
    }
    
    printf("invalid command. type 'help' for list of commands\n");
    return 0;
    
}

int main()
{
    struct addrinfo hints, *res;
    int sockfd, client_len, i, size;
    char s[INET6_ADDRSTRLEN];
    struct command_str c_queue[100];
    int c_num = 0;
    int com_line_open = 1;
    int com_requested = 0;
    char init_buffer[BUFFSIZE];
    
    //need to change
    updateMap(0);
    
    //set up cart data structure
    int k;
    for (k = 0; k < NUMBOTS; k++)
    {
        bot_list[k].vacant = 1;
        strcpy(bot_list[k].ip, "");
        bot_list[k].last_location_x = 0;
        bot_list[k].last_location_y = 0;
        bot_list[k].inventory_a = 0;
        bot_list[k].inventory_b = 0;
        
    }
    
    //set up server socket
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    size = sizeof(struct sockaddr_in);
    getaddrinfo(NULL, sPORT, &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);

    while(1){
        
        struct sockaddr_in client_address;
        int client_sockfd;
        socklen_t addr_size;

        printf("Awaiting PharmaCart Connection\n");
        
        //update time stamps
        time ( &rawtime);
        timeinfo = localtime (&rawtime);
        
        //when robot connects
        if ((client_sockfd = accept(sockfd, (struct sockaddr *)&client_address, (socklen_t *) &size)) == -1 ) {
            perror("accept");
            exit(1);
        }
        char ip_address[16];
        strcpy(ip_address, inet_ntoa(client_address.sin_addr));
        printf("Server got connection from client at %s\n", ip_address);
        
        int num = 0;
        char time_text[100];
    
        if ((num = recv(client_sockfd, init_buffer, BUFFSIZE ,0))== -1) {
            perror("recv");
            exit(1);
        }
        
        //record message sent by client
        init_buffer[num] = '\0';
        printf("Client connected with Message: %s\n", init_buffer);
        char timestamp [10];
        
        
        strcat(init_buffer, " ... ");
        sprintf(timestamp, "%d", timeinfo->tm_hour);
        strcat(init_buffer, timestamp);
        strcat(init_buffer, ":");
        sprintf(timestamp, "%d", timeinfo->tm_min);
        strcat(init_buffer, timestamp);
        strcat(init_buffer, ":");
        sprintf(timestamp, "%d", timeinfo->tm_sec);
        strcat(init_buffer, timestamp);
        strcat(init_buffer, "\n");
        sprintf(time_text, "echo '%s' >> event_times.log;", init_buffer  );
        
        status = system(time_text);
        
        // update robot data logs, if data update
        int k = 0;
        for (k = 0; k < NUMBOTS; k++)
        {
            if (bot_list[k].vacant)
            {
                bot_list[k].vacant = 0;
                strcpy(bot_list[k].ip, ip_address);
                
                //update "inventory" TBD
                break;
            }
            else
            {
                //bot is already in system
                if (strstr(bot_list[k].ip, ip_address)!= NULL )
                {
                    // if information, update location and map
                    if (strstr(init_buffer, "com_request") == NULL)
                    {
                        int beacon_id = atoi(init_buffer);
                        bot_list[k].last_location_x = getX(beacon_id);
                        bot_list[k].last_location_y = getY(beacon_id);
                        
                        updateMap(beacon_id);
                        //^ will need to be moved for multiple robots
                    }
                    //update "inventory" TBD
                    break;
                }
            }
        }
        
        //if command was requested and not just a connection for updating info/location
        if (strstr(init_buffer, "com_request") != NULL)
        {
            c_num = 0;
            // insert commands into a queue
            while (com_line_open)
            {
                char cmd[BUFFSIZE];
                printf("enter query to add to command queue:\n type \"done\" to finish\n$");
                fgets(cmd, sizeof(cmd), stdin);
                if (cmd_input(cmd, client_sockfd)){
                    char basecmd[BUFFSIZE];
                    strcpy(basecmd, "done");
                    if (strstr(cmd, basecmd) != NULL)
                        com_line_open = 0;
                    else
                    {
                        clear_string(c_queue[c_num].txt, 256);
                        strcpy(c_queue[c_num].txt, cmd);
                        c_num++;
                    }
                    strcpy(cmd, clear_string(cmd, BUFFSIZE));
                }
            }
            // add finishing commands to command queue
            com_line_open = 1;
            strcpy(c_queue[c_num].txt, "get_info\n\0");
            c_num++;
            
            strcpy(c_queue[c_num].txt, "disconnect\n\0");
            c_num++;
    
        
            int com_sent = 0;
            while(com_sent < c_num) {
                
                char queue_buffer[BUFFSIZE];
                //copy command from queue to buffer to send
                strcpy(queue_buffer, c_queue[com_sent].txt);
                com_sent++;
                
                //update time stamps
                time ( &rawtime);
                timeinfo = localtime (&rawtime);
                
                printf("\n");
                
                if ((send(client_sockfd,queue_buffer, strlen(queue_buffer),0))== -1)
                {
                    fprintf(stderr, "Failure Sending Message or disconnect command issued \n");
                    close(client_sockfd);
                    break;
                }
                
                printf("Server:Msg being sent: %sNumber of bytes sent: %d\n", queue_buffer, strlen(queue_buffer));
                
                if (close_flag == 1)       // disconnect command issued
                    break;
                
                
                if ((num = recv(client_sockfd, queue_buffer, BUFFSIZE, 0))== -1) {
                    perror("recv");
                    exit(1);
                }
                else if (num == 0) {
                    printf("Connection closed\n");
                    //So I can now wait for another client
                    break;
                }
                queue_buffer[num] = '\0';
                printf("Server: Echo/response Received: %s\n", queue_buffer);
                
            } //End of Inner While...
            //Close Connection Socket
        }
        close(client_sockfd);
        close_flag = 0;
        
        //print robot data
        print_data();
    }
}







