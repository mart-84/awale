#ifndef LISTE_MATCH_H
#define LISTE_MATCH_H

#include "match.h"

typedef struct elementListeMatch
{
    MatchAwale *match;
    struct elementListeMatch *suivant;
} elementListeMatch;

elementListeMatch *ajouterMatch(elementListeMatch *liste, MatchAwale *match);

// Supprime un match de la liste, et renvoie la liste modifiée
// Si supprimerMatch vaut 1, supprime le match, sinon supprime juste l'élément de la liste
elementListeMatch *supprimerMatch(elementListeMatch *liste, MatchAwale *match, int supprimerMatch);

// Renvoie un seul match, car on ne peut avoir qu'un seul match entre deux clients, que ce soit en attente ou en cours
MatchAwale *rechercherMatchClients(elementListeMatch *liste, Client *c1, Client *c2);

// Renvoie tous les matchs d'un client, car un client peut avoir des invitations de plusieurs joueurs
elementListeMatch *rechercherMatchClient(elementListeMatch *liste, Client *c1);

void clearListeMatch(elementListeMatch *liste);

#endif /* guard */