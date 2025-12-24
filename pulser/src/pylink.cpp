/*****************************************************************************/
/* 
    pylink.cpp

        Connect to python for fancy geometry propcessing 

        Use system calls or TCP to hook to the extenral world from the GUI 

        
    


    
    MIT License

    Copyright (c) 2025 Keith Legg - keithlegg23@gmail.com

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.


*/
         

/*****************************************************************************/

#include <iostream>

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <string.h> 
#include "globals.h"


#define BACKLOG 10     /* how many pending connections queue will hold */


extern cncglobals cg;

extern int TCP_PORT; 


//extern bool tog_grid;





/*********************************************************/
/*********************************************************/


void exe_python(void)
{
    std::cout << "ACTIVE OBJ FILE IS " << cg.active_filepath << "\n";

    
    char buffer[256];

    if (cg.active_filepath.size() )
    {
        snprintf(buffer, sizeof(buffer), "python3 ../py/pycore.py %s", cg.active_filepath.c_str());
    }

    //std::cout << "COMMAND SENT FROM C "<< buffer << "\n";

    system(buffer);

}
 



/*********************************************************/
/*********************************************************/



/*
Client:
    Create the socket.
    connect to it.
    recv data.
    If recv returns 0, it means the other end has performed an orderly shutdown. Go to step 7.
    send response.
    Go to step 3.
    Stop.

Server:

    Create the socket.
    bind the socket to an address.
    Mark the socket as listening.
    accept a connection.
    If accepted connection is invalid, go to step 4.
    send data.
    recv response.
    If recv returns 0, it means the other end has performed an orderly shutdown. Go to step 4 to accept a new connection.
    Go to step 6.
*/


/*********************************************************/
/*********************************************************/


/*
//http://www.cs.tau.ac.il/~eddiea/samples/Non-Blocking/tcp-nonblocking-server.c.html
void nonblockingtest(void) 
{
    int             sockfd, new_fd;     // listen on sock_fd, new connection on new_fd 
    struct  sockaddr_in     my_addr;    // my address information 
    struct  sockaddr_in     their_addr; // connector's address information 
    int             sin_size;
    char            string_read[255];
    int             n,i;
    int         last_fd;    // Thelast sockfd that is connected 

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    
    last_fd = sockfd;

        my_addr.sin_family = AF_INET;           // host byte order 
        my_addr.sin_port = htons(TCP_PORT);     // short, network byte order 
        my_addr.sin_addr.s_addr = INADDR_ANY;   // auto-fill with my IP 
        bzero(&(my_addr.sin_zero), 8);          // zero the rest of the struct 

        if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) \
                                                                      == -1) {
            perror("bind");
            exit(1);
        }

        if (listen(sockfd, BACKLOG) == -1) {
            perror("listen");
            exit(1);
        }

        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, \
                &sin_size)) == -1) {
            perror("accept");
                    }
        fcntl(last_fd, F_SETFL, O_NONBLOCK); // Change the socket into non-blocking state   
        fcntl(new_fd, F_SETFL, O_NONBLOCK);  // Change the socket into non-blocking state    

    while(1){
        for (i=sockfd;i<=last_fd;i++){
            printf("Round number %d\n",i);
                if (i = sockfd){
                sin_size = sizeof(struct sockaddr_in);
                    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, \
                            &sin_size)) == -1) {
                        perror("accept");
                    }
                    printf("server: got connection from %s\n", \
                     inet_ntoa(their_addr.sin_addr)); 
                        fcntl(new_fd, F_SETFL, O_NONBLOCK);
                last_fd = new_fd;
            }
            else{
                    n=recv(new_fd,string_read,sizeof(string_read),0);
                if (n < 1){ 
                    perror("recv - non blocking \n");
                        printf("Round %d, and the data read size is: n=%d \n",i,n);
                }
                else{
                        string_read[n] = '\0';
                        printf("The string is: %s \n",string_read);
                            if (send(new_fd, "Hello, world!\n", 14, 0) == -1)
                                perror("send");
                }
                }
        }
    }
        
}
*/


/***********************************************************/

/*----------------------------------------------------------------------
 NON BLOCKING EXAMPLE UNTESTED 
 FROM http://www.kegel.com/dkftpbench/nonblocking.html

 Portable function to set a socket into nonblocking mode.
 Calling this on a socket causes all future read() and write() calls on
 that socket to do only as much as they can immediately, and return 
 without waiting.
 If no data can be read or written, they return -1 and set errno
 to EAGAIN (or EWOULDBLOCK).
 Thanks to Bjorn Reese for this code.


int setNonblocking(int fd)
{
    int flags;

    // If they have O_NONBLOCK, use the Posix way to do it  
#if defined(O_NONBLOCK)
    // Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5.  
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    // Otherwise, use the old way of doing it  
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}     

#define BUFSIZE 1024
class filesender_t {
    int m_fd;                file being sent 
    char m_buf[BUFSIZE];     current chunk of file 
    int m_buf_len;           bytes in buffer 
    int m_buf_used;          bytes used so far; <= m_buf_len 
    enum { IDLE, SENDING } m_state; // what we're doing 

public:
    filesender() {
        m_state = IDLE;     // not doing anything initially 
    }

    // Start sending a file on the given socket.
    // Set the socket to be nonblocking.
    // 'filename' is the name of the file to send.
    // 'socket' is an open network connection.
     
    void sendFile(const char *filename, int socket)
    {
        int nread; 
        int nwrite, i;

        // Force the network socket into nonblocking mode 
        setNonblocking(socket);

        // Open the file 
        m_fd = open(filename, O_RDONLY);
        if (m_fd < 0)
            fatal_error("open failed");

        // Start sending it 
        m_buf_len = 0;
        m_buf_used = 0;
        m_socket = socket;
        m_state = SENDING;
    }

     // Continue sending the file started by sendFile().
     // Call this periodically.
     // Returns nonzero when done.
     
    int handle_io()
    {
        if (m_state == IDLE)
            return 2;     // nothing to do 

        // If buffer empty, fill it 
        if (m_buf_used == m_buf_len) {
            // Get one chunk of the file from disk 
            m_buf_len = read(m_fd, m_buf, BUFSIZE);
            if (m_buf_len == 0) {
                // All done; close the file and the socket. 
                close(m_fd);
                close(m_socket);
                m_state = IDLE;
                return 1;
            }
            m_buf_used = 0;
        }

        // Send one chunk of the file 
        assert(m_buf_len > m_buf_used);
        nwrite = write(m_socket, m_buf + m_buf_used, m_buf_len - m_buf_used);
        if (nwrite < 0) 
            fatal_error("write failed");
        m_buf_used += nwrite;
        return 0;
    }
}

*/


/***********************************************************/


//http://www.linuxhowtos.org/C_C++/socket.htm
void sockettest3(void) 
{ 
     int sockfd, newsockfd;

     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        printf("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(TCP_PORT);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              printf("ERROR on binding");

    
     bool stayopen = true;

     while(stayopen)
     {
         listen(sockfd,5);
         clilen = sizeof(cli_addr);
         newsockfd = accept(sockfd, 
                     (struct sockaddr *) &cli_addr, 
                     &clilen);
         if (newsockfd < 0) 
              printf("ERROR on accept");
         bzero(buffer,256);

         n = read(newsockfd,buffer,255);
         
         printf( "byte read " );
         printf( "%x\n",n );

         //if (n < 0) printf("ERROR reading from socket");
         //printf("%i bytes received, message is %s\n", n, buffer);
     
         if(buffer[0]==0x00)
         {
             printf("NULL received, closing socket \n");            
             close(newsockfd);
             close(sockfd);
             stayopen = false;
         }

         if (strncmp(buffer, "vpfgrd_", 7) == 0)  
         {
             char subbuff[5];
             memcpy( subbuff, &buffer[7], 1 ); //copy from position 7 one more 
             subbuff[1] = '\0'; //add null byte terminator to sub buffer
             //printf("sub buffer %s\n ", subbuff);  

             if (strncmp(subbuff, "1", 1) == 0)
             {
                //tog_grid = true;
                printf("vpfgrd_ 1 command received!! %s\n ", buffer);            
             }
             if (strncmp(subbuff, "0", 1) == 0)
             {
                //tog_grid = false;
                printf("vpfgrd_ 0 command received!! %s\n ", buffer);            
             }
             stayopen = false; 

         }



    }

}


/*********************************************************/
/*********************************************************/

/*
void sockettest(void) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 

    // Forcefully attaching socket to the port   
    //if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
    //                                              &opt, sizeof(opt))) 

    // Forcefully attaching socket to the port   
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port   
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 

 
    valread = read( new_socket , buffer, 1); 
    printf("%s\n",buffer );

    

    //send(new_socket , hello , strlen(hello) , 0 ); 
    //printf("Hello message sent\n");

} 
*/

/*********************************************************/
/*********************************************************/

/*
//https://www.thegeekstuff.com/2011/12/c-socket-programming/
void sockettest2(void) 
{ 

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff)); 

        close(connfd);
        sleep(1);
     }
}
*/


