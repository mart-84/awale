#ifndef MATCH_H
#define MATCH_H

#include "awale/awale.h"
#include "client.h"
#include "listes/listeClient.h"
#include "sauvegarde.h"

typedef struct {
    Client *joueur1; // lance le duel
    Client *joueur2; // accepte le duel
    partie *partie;
    elementListeClient *spectateurs;
    partieSauvegardee *sauvegarde;
} MatchAwale;

#endif /* guard */