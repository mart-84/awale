#ifndef CLIENT_H
#define CLIENT_H

#define BUF_SIZE    1024
typedef int SOCKET;

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   int isConnected;
} Client;

#endif /* guard */
