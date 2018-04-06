/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

struct packet
 {char data[100];
  int seqno;
  int fin;
 };

int main()
 {int clientSocket, N, n, i, j, ack, seqno = 0;
  struct packet * buffer;
  struct packet dummy;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  dummy.seqno = -1;

  struct timeval tv;
  tv.tv_sec = 60;
  tv.tv_usec = 0;
  setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));

  printf ("GO-BACK-N SIMULATOR\n");
  printf ("===================\n\n");
  printf ("Enter Window Size:\n");
  scanf ("%d", &N);
  send(clientSocket, &N, sizeof(int), 0);
  printf ("How many packets?\n");
  scanf ("%d", &n);
  buffer = (struct packet *)malloc((n + 1) * sizeof(struct packet));
  printf ("Enter data for each packet.\n");
  for (i = 0; i < n; i++)
   {scanf("%s", buffer[i].data);
    buffer[i].seqno = seqno++;
    buffer[i].fin = 0;
   }
  /*---- Setup FIN Signal ----*/
  buffer[i].fin = 1;

  for (i = 0; i < n;)
   {printf ("Transmitting Packet %d through %d\nWaiting for ACK\n", buffer[i].seqno, (((buffer[i].seqno + N - 1) < n) ? (buffer[i].seqno + N - 1) : (n - 1)));
    for (j = 0; j < N; j++)
     {if (i + j < n)
          send(clientSocket, &buffer[i + j], sizeof(struct packet), 0);
      else
          send(clientSocket, &dummy, sizeof(struct packet), 0);
     }
    if(recv(clientSocket, &ack, sizeof(int), 0) == -1)
     {printf ("TIME OUT\n");
      printf ("Re-");
     }
    else
     {printf ("ACK %d received.\n", ack);
      i = ack + 1;
     }
   }

  /*---- Send FIN Signal ----*/
  printf ("Sending FIN Signal\n");
  send(clientSocket, &buffer[n], sizeof(struct packet), 0);
  

  /*---- Print the received message ----*/
  free (buffer);

  return 0;
}

