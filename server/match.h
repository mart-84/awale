#ifndef MATCH_H
#define MATCH_H

#include "awale/awale.h"
#include "client.h"

typedef struct {
    Client *joueur1; // lance le duel
    Client *joueur2; // accepte le duel
    partie *partie;
} MatchAwale;

#endif /* guard */