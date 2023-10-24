#include <stdlib.h>
#include <stdio.h>

#include "listeMatch.h"

elementListeMatch *ajouterMatch(elementListeMatch *liste, MatchAwale *match)
{
    elementListeMatch *nouveauElement = malloc(sizeof(elementListeMatch));
    nouveauElement->match = match;
    nouveauElement->suivant = NULL;

    if (liste == NULL)
    {
        return nouveauElement;
    }

    elementListeMatch *ptr = liste;
    while (ptr->suivant != NULL)
        ptr = ptr->suivant;
    ptr->suivant = nouveauElement;
    return liste;
}

elementListeMatch *supprimerMatch(elementListeMatch *liste, MatchAwale *match)
{
    if (liste->match == match)
    {
        elementListeMatch *ptr = liste->suivant;
        free(liste);
        return ptr;
    }
    elementListeMatch *ptr = liste;
    while (ptr->suivant != NULL)
    {
        if (ptr->suivant->match == match)
        {
            elementListeMatch *supprimer = ptr->suivant;
            ptr->suivant = supprimer->suivant;
            free(supprimer);
            return liste;
        }
        ptr = ptr->suivant;
    }
    return liste;
}

elementListeMatch *rechercherMatchClients(elementListeMatch *liste, Client *c1, Client *c2)
{
    elementListeMatch *ptr = liste;
    elementListeMatch *matchs = NULL;
    while (ptr != NULL)
    {
        if ((ptr->match->joueur1 == c1 && ptr->match->joueur2 == c2) || (ptr->match->joueur1 == c2 && ptr->match->joueur2 == c1))
        {
            matchs = ajouterMatch(matchs, ptr->match);
        }
        ptr = ptr->suivant;
    }
    return matchs;
}

void clearListeMatch(elementListeMatch *liste)
{
    elementListeMatch *ptr = liste;
    while (ptr != NULL)
    {
        elementListeMatch *supprimer = ptr;
        ptr = ptr->suivant;
        free(supprimer);
    }
}