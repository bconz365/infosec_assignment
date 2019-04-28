
// Repurposed code from one of my netprog homeworks


#include <time.h>
#include <stdbool.h>
#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> 
#include <string.h>
#include <regex.h>   
     
#define TRUE   1  
#define FALSE  0  
     
     
int main(int argc , char *argv[])   
{   

    int opt = TRUE;   

    int master_socket , addrlen , new_socket , client_socket[25] ,  
          max_clients = 25 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;   
 
    
    
    char buffer[1024];  //words wont exceed 1024 bytes
    
    //set of socket descriptors  
    fd_set readfds;   
         
    //a message  
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *message = "";   
    strftime(message, sizeof(message), "%c", tm);
     
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    //set master socket to allow multiple connections ,  
   
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
    
    int port = 13;
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons(port);   
         
    //bind the socket to localhost port *random*  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    //getsockname(master_socket, (struct sockaddr *)&address, sockaddr_len);
    printf("Listener on localhost, port %d \n", ntohs(address.sin_port)); 
      
    
     //try to specify maximum of 5 pending connections for the master socket  
    if (listen(master_socket, 25) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    } 
    
        
         

    //accept the incoming connection  
    addrlen = sizeof(address);

    
    while(TRUE)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
  
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d " 
                  "\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
                  (address.sin_port));   
           
            //send new connection greeting message  
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )   
            {   
                perror("send");   
            } else {                 
                puts("Welcome message sent successfully");
                close(new_socket);
                break;
            }
                 
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   
        
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];
               
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                    close( sd );   
                    client_socket[i] = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    buffer[valread] = '\0';
                    char* strPtr;
                    char buffer_copy[1024];
                    strcpy(buffer_copy, buffer);
                    strPtr = strtok (buffer_copy," ");
                    
                    send(sd, buffer, valread, 0); 
                }
                        
            }    
        }   
    }
    
    return 0;   
}
