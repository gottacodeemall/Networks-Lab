/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

int clientSocket, ack, i;

struct packet
 {char data[100];
  int seqno;
  int fin;
 };

struct rcvAck
 {int ack;
  int isNak;
 };

int main()
 {int N, n, j, seqno = 0, lastAcknowledged = -1, count, remainingRcvs = 0, lastSent, isNak;
  int * isAcknowledged;
  struct packet * buffer;
  struct rcvAck msg;
  struct packet dummy;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  pthread_t thread_id;

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
  tv.tv_sec = 45;
  tv.tv_usec = 0;
  setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));

  printf ("SELECTIVE REPEAT SIMULATOR\n");
  printf ("=============================\n\n");
  printf ("Enter Window Size:\n");
  scanf ("%d", &N);
  send(clientSocket, &N, sizeof(int), 0);
  printf ("How many packets?\n");
  scanf ("%d", &n);
  isAcknowledged = (int *)malloc(n * sizeof(int));
  buffer = (struct packet *)malloc((n + 1) * sizeof(struct packet));
  printf ("Enter data for each packet.\n");
  for (i = 0; i < n; i++)
   {scanf("%s", buffer[i].data);
    buffer[i].seqno = seqno++;
    buffer[i].fin = 0;
   }
  /*---- Setup FIN Signal ----*/
  buffer[n].fin = 1;

  printf ("Transmitting Packet %d through %d\nWaiting for ACK\n", buffer[0].seqno, (((buffer[0].seqno + N - 1) < n) ? (buffer[0].seqno + N - 1) : (n - 1)));
  for (j = 0; j < N; j++)
   {if (j < n)
     {send(clientSocket, &buffer[j], sizeof(struct packet), 0);
      remainingRcvs++;
      lastSent = j;
     }
    else
        send(clientSocket, &dummy, sizeof(struct packet), 0);
   }

  while (lastAcknowledged <  n - 1)
   {if(recv(clientSocket, &msg, sizeof(struct rcvAck), 0) == -1)
     {printf ("TIME OUT\n");
      printf ("Re-Transmitting Packet %d\nWaiting for ACK\n", buffer[lastAcknowledged + 1].seqno);
      send(clientSocket, &buffer[lastAcknowledged + 1], sizeof(struct packet), 0);
     }
    else
     {ack = msg.ack;
      isNak = msg.isNak;
      count = 0;
      if (isNak == 0)
       {remainingRcvs--;
        printf ("ACK %d received.\n", ack);
        isAcknowledged[ack] = 1;
        while (lastAcknowledged < n - 1 && isAcknowledged[lastAcknowledged + 1] == 1)
                lastAcknowledged++;
       }
      else
        printf ("NAK %d received.\n", ack);
      if (lastAcknowledged < ack)
       {for (j = lastAcknowledged + 1; j < ack && count < N; j++)
         {if (isAcknowledged[j] != 1)
           {printf ("Re-Transmitting Packet %d\n", buffer[j].seqno);
            send(clientSocket, &buffer[j], sizeof(struct packet), 0);
            count++;
           }
         }
        if (isNak == 1 && count < N)
         {printf ("Re-Transmitting Packet %d.\n", ack);
          send(clientSocket, &buffer[ack], sizeof(struct packet), 0);
          count++;
         }
        if (lastSent + 1 < n && count < N && isAcknowledged[lastSent + 1] != 1)
         {printf ("Transmitting Packet %d\n", buffer[lastSent + 1].seqno);
          send(clientSocket, &buffer[lastSent + 1], sizeof(struct packet), 0);
          lastSent = lastSent + 1;
          remainingRcvs++;
         }
        printf ("Waiting for ACK\n");
       }
      else
       {if (isNak == 1)
         {printf ("NAK %d received.\n", ack);
          printf ("Re-Transmitting Packet %d.\n", ack);
          send(clientSocket, &buffer[ack], sizeof(struct packet), 0);
          count++;
         }
        if (lastSent + 1 < n && count < N && isAcknowledged[lastSent + 1] != 1)
         {printf ("Transmitting Packet %d\nWaiting for ACK\n", buffer[lastSent + 1].seqno);
          send(clientSocket, &buffer[lastSent + 1], sizeof(struct packet), 0);
          lastSent = lastSent + 1;
          remainingRcvs++;
         }
       }
     }

   }

  while (remainingRcvs > 0)
   {recv(clientSocket, &msg, sizeof(struct rcvAck), 0);
    remainingRcvs--;
   }

  /*---- Send FIN Signal ----*/
  printf ("Sending FIN Signal\n");
  send(clientSocket, &buffer[n], sizeof(struct packet), 0);
  

  /*---- Print the received message ----*/
  free (buffer);

  return 0;
}

