#include <stdlib.h>
#include <string.h>

#include "listeClient.h"

elementListeClient *ajouterClient(elementListeClient *liste, Client *client)
{
    elementListeClient *nouveauClient = (elementListeClient *)malloc(sizeof(elementListeClient));
    nouveauClient->client = client;
    nouveauClient->suivant = NULL;

    if (liste == NULL)
    {
        return nouveauClient;
    }

    elementListeClient *ptr = liste;
    while (ptr->suivant != NULL)
        ptr = ptr->suivant;
    ptr->suivant = nouveauClient;
    return liste;
}

Client *rechercherClientParNom(elementListeClient *liste, char *nom)
{
    elementListeClient *ptr = liste;
    while (ptr != NULL)
    {
        if (strcmp(ptr->client->name, nom) == 0)
        {
            return ptr->client;
        }
        ptr = ptr->suivant;
    }
    return NULL;
}