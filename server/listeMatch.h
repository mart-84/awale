#ifndef LISTE_MATCH_H
#define LISTE_MATCH_H

#include "match.h"

typedef struct elementListeMatch
{
    struct elementListeMatch *suivant;
    MatchAwale *match;
} elementListeMatch;

elementListeMatch *ajouterMatch(elementListeMatch *liste, MatchAwale *match);

elementListeMatch *supprimerMatch(elementListeMatch *liste, MatchAwale *match);

elementListeMatch *rechercherMatchClients(elementListeMatch *liste, Client *c1, Client *c2);

void clearListeMatch(elementListeMatch *liste);

#endif /* guard */