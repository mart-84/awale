#ifndef LISTE_INVITATION_H
#define LISTE_INVITATION_H

#include "../invitation.h"

typedef struct elementListeInvitation {
    Invitation *invitation;
    struct elementListeInvitation *suivant;
} elementListeInvitation;

elementListeInvitation *ajouterInvitation(elementListeInvitation* liste, Invitation *invitation);

Invitation *rechercherInvitationParClients(elementListeInvitation* liste, Client *c1, Client *c2);

elementListeInvitation *supprimerInvitation(elementListeInvitation *liste, Invitation *invitation);

#endif /* guard */