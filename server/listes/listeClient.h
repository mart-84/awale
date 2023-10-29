#ifndef LISTE_CLIENT_H
#define LISTE_CLIENT_H

#include "../client.h"

typedef struct elementListeClient
{
   struct elementListeClient *suivant;
   Client *client;
} elementListeClient;

/**
 * Ajoute un nouveau client à la liste des clients.
 *
 * @param liste La liste actuelle des clients.
 * @param client Le client à ajouter à la liste.
 * @return La liste mise à jour des clients.
 */
elementListeClient *ajouterClient(elementListeClient *liste, Client *client);

/**
 * Recherche un client dans une liste de clients par son nom.
 *
 * @param liste La liste de clients à parcourir.
 * @param nom Le nom du client à rechercher.
 * @return Un pointeur vers le client recherché s'il est trouvé, NULL sinon.
 */
Client *rechercherClientParNom(elementListeClient *liste, char *nom);

/**
 * Supprime tous les éléments de la liste chaînée de clients.
 *
 * @param liste Pointeur vers le premier élément de la liste.
 * @param supprimerClient Si égal à 1, libère également la mémoire occupée par les clients.
 */
void clearListeClient(elementListeClient *liste, int supprimerClient);

#endif /* guard */