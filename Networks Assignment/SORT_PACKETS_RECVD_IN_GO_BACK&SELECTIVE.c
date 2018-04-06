/****************** SERVER CODE ****************/

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

void sort(struct packet a[], int l, int u)
 {if (l >= u)
      return;
  sort(a, l, (l+u)/2);
  sort(a, (l+u)/2 + 1, u);
  struct packet * b = (struct packet *)malloc((u-l+1)*sizeof(struct packet));
  int i, j, k = 0;
  for (i=l, j=(l+u)/2+1; i<=(l+u)/2 && j<=u;)
   {if (a[i].seqno < a[j].seqno)
     {b[k++] = a[i];
      i++;
     }
    else
     {b[k++] = a[j];
      j++;
     }
   }
  for (; i<=(l+u)/2; i++)
       b[k++] = a[i];
  for (; j<=u; j++)
       b[k++] = a[j];
  k = 0;
  for (i=l; i<=u; i++)
       a[i] = b[k++];
  free(b);
 }

int main()
 {int welcomeSocket, newSocket, n;
  struct packet * buffer = (struct packet *)malloc(1024 * sizeof(struct packet));
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

  serverAddr.sin_family = AF_INET;

  serverAddr.sin_port = htons(7892);

  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  if(listen(welcomeSocket, 5) == 0)
     printf("Listening\n");
  else
     printf("Error\n");

  addr_size = sizeof serverStorage;
  newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

  recv(newSocket, &n, sizeof(int), 0);
  recv(newSocket, buffer, 1024 * sizeof(int), 0);

  sort(buffer, 0, n - 1);

  send(newSocket, &n, sizeof(int), 0);
  send(newSocket, buffer, 1024 * sizeof(int), 0);

  free (buffer);

  return 0;
 }

