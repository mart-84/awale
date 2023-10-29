#ifndef INVITATION_H
#define INVITATION_H

#include "client.h"

/**
 * @struct Invitation
 * @brief Structure représentant une invitation entre deux clients.
 * 
 * Cette structure contient deux pointeurs vers des clients, l'inviteur et l'invité.
 */
typedef struct Invitation
{
    Client *inviteur; // Client qui invite
    Client *invite; // Client qui reçoit l'invitation
} Invitation;

#endif /* guard */