#include <stdlib.h>
#include <string.h>

#include "listeClient.h"

elementListeClient *ajouterClient(elementListeClient *liste, Client *client)
{
    elementListeClient *nouveau = (elementListeClient *)malloc(sizeof(elementListeClient));
    nouveau->client = client;
    nouveau->suivant = liste;
    return nouveau;
}

Client *rechercherClientParNom(elementListeClient *liste, char *nom)
{
    if (nom == NULL)
        return NULL;
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

void clearListeClient(elementListeClient *liste, int supprimerClient)
{
    elementListeClient *ptr = liste;
    while (ptr != NULL)
    {
        elementListeClient *supprimer = ptr;
        ptr = ptr->suivant;
        if (supprimerClient == 1)
        {
            free(supprimer->client);
        }
        free(supprimer);
    }
}