#ifndef AWALE_H
#define AWALE_H

#define NB_CASES 12
#define NB_GRAINES 4
#define NB_GRAINES_VICTOIRE 25

#define JOUEUR1 0
#define JOUEUR2 1

#define PAS_DE_GAGNANT -1
#define EGALITE 2

typedef int plateau[NB_CASES];
typedef int scores[2];

typedef struct
{
    plateau plateau;
    scores scores;
    int joueurCourant; // 0 : joueur 1, 1 : joueur 2
} partie;

// Prototypes

/**
 * Initialise une partie d'awale et le plateau
 * @param p : partie à initialiser
*/
void init(partie *p);

/**
 * Formatter le plateau de jeu
 * @param p : partie à afficher
*/
void printBoard(partie *p);

/**
 * Affiche le plateau de jeu
 * @param p : partie à afficher
 * @param buffer : buffer dans lequel écrire le plateau
 * @return buffer
*/
char * sprintBoard(partie *p, char *buffer, char *joueur1, char *joueur2);

/**
 * Interprète le coup joué par le joueur
 * Insensible à la casse
 * @param p : partie en cours
 * @param choix : choix du joueur entre A et F
 * @return case à jouer, entre 0 et 11, -1 si le coup est invalide
*/
int interpreterCoup(partie *p, char choix);

/**
 * Joue un coup
 * @param p : partie en cours
 * @param caseJouee : case à jouer, entre 0 et 11
 * @return -1 si le coup est invalide, 0 sinon
*/
int jouer(partie *p, int caseJouee);

/**
 * Détermine si une partie est terminée, et le gagnant le cas échéant
 * @param p : partie en cours
 * @return JOUEUR1 ou JOUEUR2, si l'un d'eux a gagné, EGALITE le cas échéant et PAS_DE_GAGNANT sinon
*/
int finDePartie(partie *p);

#endif // AWALE_H