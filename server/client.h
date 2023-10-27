#ifndef CLIENT_H
#define CLIENT_H

#include "includes.h"

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   int isConnected; // connecté 1 | déconnecté 0
   char* bio;
} Client;

#endif /* guard */
