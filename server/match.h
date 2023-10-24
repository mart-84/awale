#ifndef MATCH_H
#define MATCH_H

#include "awale/awale.h"
#include "client.h"

#define ATTENTE 0
#define EN_COURS 1
#define TERMINEE 2

typedef struct {
    Client *joueur1; // lance le duel
    Client *joueur2; // accepte le duel
    partie *partie;
    int etat; // 0 : en attente de l'acceptation, 1 : en cours, 2 : terminée
} MatchAwale;

#endif /* guard */