#include <stdlib.h>

#include "listeInvitation.h"

elementListeInvitation *ajouterInvitation(elementListeInvitation *liste, Invitation *invitation)
{
    elementListeInvitation *nouveau = malloc(sizeof(elementListeInvitation));
    nouveau->invitation = invitation;
    nouveau->suivant = liste;
    return nouveau;
}

Invitation *rechercherInvitationParClients(elementListeInvitation *liste, Client *c1, Client *c2)
{
    elementListeInvitation *ptr = liste;
    while (ptr != NULL)
    {
        if ((ptr->invitation->inviteur == c1 && ptr->invitation->invite == c2)
        || (ptr->invitation->inviteur == c2 && ptr->invitation->invite == c1))
        {
            return ptr->invitation;
        }
        ptr = ptr->suivant;
    }
    return NULL;
}

elementListeInvitation *supprimerInvitation(elementListeInvitation *liste, Invitation *invitation)
{
    elementListeInvitation *ptr = liste;
    elementListeInvitation *precedent = NULL;
    while (ptr != NULL)
    {
        if (ptr->invitation == invitation)
        {
            if (precedent == NULL)
            {
                liste = ptr->suivant;
            }
            else
            {
                precedent->suivant = ptr->suivant;
            }
            free(ptr->invitation);
            free(ptr);
            return liste;
        }
        precedent = ptr;
        ptr = ptr->suivant;
    }
    return liste;
}
