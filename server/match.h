#ifndef MATCH_H
#define MATCH_H

#include "awale/awale.h"
#include "client.h"
#include "listes/listeClient.h"
#include "sauvegarde.h"

/**
 * @struct MatchAwale
 * @brief Structure représentant un match d'Awale.
 * 
 * Cette structure contient les informations relatives à un match d'Awale, telles que les joueurs, la partie en cours, les spectateurs et la sauvegarde de la partie.
 */
typedef struct MatchAwale
{
    Client *joueur1; // lance le duel
    Client *joueur2; // accepte le duel
    Partie *partie;
    elementListeClient *spectateurs;
    PartieSauvegardee *sauvegarde;
} MatchAwale;

#endif /* guard */