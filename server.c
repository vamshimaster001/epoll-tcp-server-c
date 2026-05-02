#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define SERVER_PORT 9999

int main()
{
  int server_fd, client_fd;
  struct sockaddr_in server_address;
  socklen_t server_len = sizeof(server_address);
  
  server_fd = socket(AF_INET, SOCK_STREAM,0);
  if(server_fd < 0)
    {
      printf("%s",strerror(errno));
    }

  memset(&server_address,0,sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = SERVER_PORT;

  if(bind(server_fd,(struct sockaddr *)&server_address,server_len)<0)
    {
      printf("%s",strerror(errno));
      close(server_fd);
    }
  if(listen(server_fd,5)< 0)
    {
      printf("%s",strerror(errno));
      close(server_fd);
    }

  return 0;
}

