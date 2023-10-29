#ifndef LISTE_MATCH_H
#define LISTE_MATCH_H

#include "../match.h"

/**
 * Structure représentant un élément d'une liste chaînée de matchs.
 */
typedef struct elementListeMatch
{
    MatchAwale *match;                 // Le match en question.
    struct elementListeMatch *suivant; // Pointeur vers l'élément suivant dans la liste.
} elementListeMatch;

/**
 * Ajoute un match à la liste des matches.
 *
 * @param liste La liste des matches.
 * @param match Le match à ajouter.
 * @return La nouvelle liste des matches.
 */
elementListeMatch *ajouterMatch(elementListeMatch *liste, MatchAwale *match);

/**
 * Supprime un match de la liste, et renvoie la liste modifiée
 * Si supprimerMatch vaut 1, supprime le match, sinon supprime juste l'élément de la liste
 *
 * @param liste La liste de matchs
 * @param match Le match à supprimer
 * @param supprimerMatch 1 si le match doit être supprimé, 0 sinon
 * @return La liste modifiée
 */
elementListeMatch *supprimerMatch(elementListeMatch *liste, MatchAwale *match, int supprimerMatch);

/**
 * Recherche un match entre deux clients dans une liste de matchs.
 * Un seul match peut être trouvé, car un joueur ne peut avoir qu'un seul match en cours.
 *
 * @param liste La liste de matchs à parcourir.
 * @param c1 Le premier client du match recherché.
 * @param c2 Le deuxième client du match recherché.
 * @return Un pointeur vers le match trouvé, ou NULL si aucun match n'a été trouvé.
 */
MatchAwale *rechercherMatchClients(elementListeMatch *liste, Client *c1, Client *c2);

/**
 * Recherche un match pour un client donné dans une liste de matchs.
 * Un seul match peut être trouvé, car un joueur ne peut avoir qu'un seul match en cours.
 *
 * @param liste La liste de matchs à parcourir.
 * @param c1 Le client pour lequel on recherche un match.
 * @return Un pointeur vers le match correspondant au client, ou NULL si aucun match n'a été trouvé.
 */
MatchAwale *rechercherMatchClient(elementListeMatch *liste, Client *c1);

/**
 * Efface tous les éléments de la liste de matchs.
 *
 * @param liste La liste de matchs à effacer.
 */
void clearListeMatch(elementListeMatch *liste);

#endif /* guard */