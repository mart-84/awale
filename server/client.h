#ifndef CLIENT_H
#define CLIENT_H

#include "includes.h"

/**
 * Structure représentant un client connecté au serveur.
 */
typedef struct Client
{
   SOCKET sock; // Socket du client
   char name[BUF_SIZE]; // Nom du client
   int isConnected; // connecté 1 | déconnecté 0
   char *bio; // Biographie du client
} Client;

#endif /* guard */
