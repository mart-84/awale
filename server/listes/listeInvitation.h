#ifndef LISTE_INVITATION_H
#define LISTE_INVITATION_H

#include "../invitation.h"

/**
 * @struct elementListeInvitation
 * @brief Structure représentant un élément de la liste d'invitations.
 *
 * Cette structure contient un pointeur vers une invitation et un pointeur vers l'élément suivant de la liste.
 */
typedef struct elementListeInvitation
{
    Invitation *invitation;
    struct elementListeInvitation *suivant;
} elementListeInvitation;

/**
 * Ajoute une invitation à la liste d'invitations.
 *
 * @param liste La liste d'invitations à laquelle ajouter l'invitation.
 * @param invitation L'invitation à ajouter à la liste.
 * @return Un pointeur vers le premier élément de la liste mise à jour.
 */
elementListeInvitation *ajouterInvitation(elementListeInvitation *liste, Invitation *invitation);

/**
 * Recherche une invitation entre deux clients dans la liste d'invitations.
 * 
 * @param liste La liste d'invitations à parcourir.
 * @param c1 Le premier client de l'invitation recherchée.
 * @param c2 Le deuxième client de l'invitation recherchée.
 * 
 * @return Un pointeur vers l'invitation correspondante si elle est trouvée, NULL sinon.
 */
Invitation *rechercherInvitationParClients(elementListeInvitation *liste, Client *c1, Client *c2);

/**
 * Recherche les invitations associées à un client dans une liste d'invitations.
 * 
 * @param liste La liste d'invitations à parcourir.
 * @param client Le client à rechercher dans les invitations.
 * @return Une liste d'invitations correspondant au client, ou NULL si non trouvée.
 */
elementListeInvitation *rechercherInvitationClient(elementListeInvitation *liste, Client *client);

/**
 * Supprime une invitation de la liste d'invitations.
 * 
 * @param liste La liste d'invitations.
 * @param invitation L'invitation à supprimer.
 * @return Le pointeur vers le premier élément de la liste mise à jour.
 */
elementListeInvitation *supprimerInvitation(elementListeInvitation *liste, Invitation *invitation);

/**
 * Libère la mémoire allouée pour une liste d'invitations.
 * 
 * @param liste La liste d'invitations à libérer.
 * @param freeInvitation Si 0, les invitations ne seront pas libérées. Si 1, les invitations seront libérées.
 */
void clearListeInvitation(elementListeInvitation *liste, int freeInvitation);

#endif /* guard */