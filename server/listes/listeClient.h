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

// supprimerClient = 1 : supprime le client de la liste
void clearListeClient(elementListeClient* liste, int supprimerClient);

#endif /* guard */