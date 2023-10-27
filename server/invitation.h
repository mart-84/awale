#ifndef INVITATION_H
#define INVITATION_H

#include "client.h"

typedef struct {
    Client *inviteur;
    Client *invite;
} Invitation;

#endif /* guard */