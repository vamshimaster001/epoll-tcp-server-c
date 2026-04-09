#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/epoll.h>


#define PORT 9090

void logging(char *level,  char *msg , ...)
{
  time_t now = time(NULL);
  fprintf(stderr, "%ld %s:",now,level);
  va_list args;
  va_start(args,msg);
  vfprintf(stderr,msg,args);
  va_end(args);
  fprintf(stderr,"\n");
}

struct client
{
  int fd;
  char buffer[1024];
  int buffer_len;
};

int set_nonblocking(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  if(flags < 0)
    {
      return -1;
    }
  if(fcntl(fd,F_SETFL, flags | O_NONBLOCK) < 0)
    {
      return -1;
    }
  logging("INFO","socket descriptor %d is set to non blocking",fd);
  return 0;
}

int main()
{
  int client_fd, server_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);

  server_fd = socket(AF_INET,SOCK_STREAM,0);
  if(server_fd < 0)
    {
      logging("ERROR", strerror(errno));
      return 1;
    }
  logging("INFO","socket is created successfully");

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);


  if(set_nonblocking(server_fd) < 0)
    {
      logging("ERROR", "setting server socket to non  blocking mode failed");
      close(server_fd);
      return 1;
      
    }

  int opt =1;
  setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&opt, sizeof(opt));
    
  if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
      logging("ERROR", strerror(errno));
      close(server_fd);
      return 1;
    }
  logging("INFO", "server is bind to IP address : %s and port %ud", inet_ntoa(server_addr.sin_addr),PORT);

    
  if(listen(server_fd,5) < 0)
    {
      logging("ERROR",strerror(errno));
      close(server_fd);
      return 1;
    }

  logging("INFO","SERVER SCOKET is listening for connections");

    int epfd = epoll_create1(0);
  if(epfd < 0)
    {
      logging("ERROR", strerror(errno));
      close(server_fd);
      return 1;
    }


  struct epoll_event server_event;
  server_event.events = EPOLLIN;
  server_event.data.fd = server_fd;

  if(epoll_ctl(epfd,EPOLL_CTL_ADD, server_fd,&server_event) < 0)
   {
     logging("ERROR",strerror(errno));
     close(server_fd);
     close(epfd);
     return 1;
   }

  logging("INFO","server socket is added to epoll control");

  struct epoll_event events[100];
  while(1)
    {
      int nfds = epoll_wait(epfd,events,100,-1);

      for(int i = 0; i < nfds; i++)
	{
	  if(events[i].data.fd ==  server_fd)
	    {
	      while(1)
		{
		  client_fd = accept(server_fd,(struct sockaddr *)&client_addr, &client_len);
		  if(client_fd < 0)
		    {
		      if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
			  break;
			}
		      else
			{
			  logging("ERROR", strerror(errno));
			  continue;
			}
		    }
		  logging("CONNECT","client connection with IP address %s is established", inet_ntoa(client_addr.sin_addr));

                  if(set_nonblocking(client_fd) < 0 )
		    {
		      logging("ERROR",strerror(errno));
		      close(client_fd);
		      continue;
		    }
		  
                  struct client *c  = malloc(sizeof(struct client));
                  if(!c)
                    {
                     close(client_fd);
                     continue;

                    }
                  c->fd = client_fd;
                  c->buffer_len = 0;
                  memset(c->buffer, 0, sizeof(c->buffer));
                  
		  
		  struct epoll_event client_event;
		  client_event.events = EPOLLIN;
		  client_event.data.ptr = c;
		  
		  if(epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&client_event) < 0)
		    {
		      logging("ERROR", strerror(errno));
		      close(client_fd);
		      continue;
		    }

		  
	      }
	    }
	  else
	    {

	      struct client *c = events[i].data.ptr;

	      ssize_t bytes_read = recv(c->fd, c->buffer+c->buffer_len, sizeof(c->buffer)-c->buffer_len-1,0);

	      if(bytes_read < 0)
		{
		  if(errno == EAGAIN || errno == EWOULDBLOCK)
		    {
		      continue;
		    }
		  else
		    {
		      epoll_ctl(epfd,EPOLL_CTL_DEL,c->fd, NULL);
		      close(c->fd);
		      logging("DISCONNECT", "client with socket descriptor %d is disconnected from server", c->fd);
		      free(c);
		    }
		}
		else if(bytes_read == 0)
		  {

		      epoll_ctl(epfd,EPOLL_CTL_DEL,c->fd, NULL);
                      close(c->fd);
		      logging("DISCONNECT", "client with socket descriptor %d is disconnected from server", c->fd);
                      free(c);
		  }
		else
		  {
		    c->buffer_len += bytes_read; 
		    c->buffer[c->buffer_len] = '\0';
		    logging("INFO","message from client %d is %s",c->fd, c->buffer);

		    char response[1040];
		    snprintf(response, sizeof(response), "ECHO SERVER: %s", c->buffer);

		    if(send(c->fd,response, strlen(response),0) < 0 )
		      {
			logging("ERROR", strerror(errno));
			epoll_ctl(epfd,EPOLL_CTL_DEL,c->fd, NULL);
                        close(c->fd);
			logging("DISCONNECT", "client with socket descriptor %d is disconnected from server", c->fd);
			free(c);
		      }

		    logging("INFO","response from server %d is %s",c->fd, response);


		    c->buffer_len = 0;
		    memset(c->buffer, 0 , sizeof(c->buffer));
		    
		    
		  }
		  
		}
	    }
	}
    
  close(epfd);
  close(server_fd);
  return 0;
}
