#include "listeclient.h"

elementListeClient *ajouterClient(elementListeClient *element, Client *client)
{
    elementListeClient *nouveauClient = (elementListeClient *)malloc(sizeof(elementListeClient));
    nouveauClient->client = client;
    nouveauClient->suivant = NULL;

    if (element == NULL)
    {
        return nouveauClient;
    }

    elementListeClient *liste = element;
    while (element->suivant != NULL)
        element = element->suivant;
    element->suivant = nouveauClient;
    return liste;
}

Client *rechercherClientParNom(elementListeClient *liste, char *nom)
{
    while (liste != NULL)
    {
        if (strcmp(liste->client->name, nom) == 0)
        {
            return liste->client;
        }

        liste = liste->suivant;
    }

    return NULL;
}