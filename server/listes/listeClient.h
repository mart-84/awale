#ifndef LISTE_CLIENT_H
#define LISTE_CLIENT_H

#include "../client.h"

typedef struct elementListeClient
{
   struct elementListeClient* suivant;
   Client* client;
} elementListeClient;

elementListeClient* ajouterClient(elementListeClient* liste, Client* client);

Client* rechercherClientParNom(elementListeClient* liste, char* nom);

#endif /* guard */