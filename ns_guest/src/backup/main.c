#include <printk.h>
#include <lwip/sockets.h>
#include <lwip/def.h>
#include <string.h>
#include <lwip/netdb.h>
#include "lwipopts.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "lwip/igmp.h"
#include "lwip/sys.h"
#include "lwip/errno.h"
#include <lwip/init.h>
#include <platform_config.h>


#define SERVER            "Server: Dynamic Thread\n"
#define CONTENT           "Content-Type: text/html\r\n"
#define TITLE             "Content-Type: text/html\r\n\
                          <html>\
                          <head>\
                          <title> Hello World </title> \
                          </head> \
                          </html>"


int TcpPort = 9090;	// default
int listenSock = 0;
struct sockaddr_in server;

int main_thread();
int second_thread();

u32 xInsideISR = 0;

void goodRequest(int sock, char* contentType, int size)
{
    char buffer[1024];

    // header
    strcpy(buffer, "HTTP/1.0 200 Ok\r\n");
    send(sock, buffer, strlen(buffer), 0);
    // content
    strcpy(buffer, contentType);
    send(sock, buffer, strlen(buffer), 0);
    // length
    sprintf(buffer, "Content-length: %d", size);
    send(sock, buffer, strlen(buffer), 0);
    // connection: close
    strcpy(buffer, "Connection: close\r\n");
    send(sock, buffer, strlen(buffer), 0);
    // end of header
    strcpy(buffer, "\r\n");
    send(sock, buffer, strlen(buffer), 0);

} //End of good request

int main (void)
{
  printk("NS Guest running!\n");
  sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,
                2048,
                1);
  // sys_thread_new("scd_thrd", (void(*)(void*))second_thread, 0,
  //               200,
  //               1);
  vTaskStartScheduler();
  while(1);
  return 0;
}

int main_thread ()
{
  int result;
  struct sockaddr_in client;
  int newSock = 0;
  unsigned int clientL = sizeof(client);
  printk("Main thread start\n");
  vTaskDelay(10);
  lwip_init();
  printk("LWIP initialized\n");

  listenSock = lwip_socket(AF_INET, SOCK_STREAM, 0);
  if(listenSock != -1)
  {
    printk("Successfully created socket!\n");
  }
  else
  {
    printk("Couldn't create socket!\n");
  }
  // result = lwip_setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = lwip_htonl(INADDR_ANY);
  server.sin_port = lwip_htons(TcpPort);
  result = lwip_bind(listenSock, (struct sockaddr*)&server, sizeof(server));
  if(result == 0)
  {
    printk("Binding: SUCCESS!\n");
  }
  else
  {
    printk("Binding: FAILED!\nClosing socket...");
    lwip_close(listenSock);
  }
  result = lwip_listen(listenSock, 0);
  if(result != -1)
  {
    printk("Listen on socket: SUCCESS!\n");
  }
  else
  {
    printk("Listen on socket: FAILED!\n");
  }
  while(1)
  {
    vTaskDelay(10);

    newSock = lwip_accept(listenSock, (struct sockaddr*)&client, &clientL);
    if(newSock == -1)
    {
      printk("Cannot accept connection!\n");
    }
    else
    {
      char buffer[1024*20];
      int readIn;
      readIn = read(newSock, buffer, sizeof(buffer));
      buffer[readIn] = '\0';
      if(readIn > 0)
      {
        if(!strcmp(buffer, "GET"))
        {
          printk("GET request\n");
          goodRequest(newSock, "Content-Type: text/html\r\n", strlen("Hello World!"));
          // lwip_write(newSock, TITLE, sizeof(TITLE));
          send(newSock, "Hello World!", strlen("Hello World!"), 0);
        }
      }
    }
    close(newSock);
  }
}

int second_thread ()
{
  int i;
  while(1)
  {
    vTaskDelay(10);
    for (i = 0; i<1000000; i++);
    printk("Second thread\n");
  }
}
