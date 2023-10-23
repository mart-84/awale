#ifndef AWALE_H
#define AWALE_H

#define NB_CASES 12
#define NB_GRAINES 4

#define JOUEUR1 0
#define JOUEUR2 1

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
 * Affiche le plateau de jeu
 * @param p : partie à afficher
*/
void printBoard(partie *p);

/**
 * Joue un coup
 * @param p : partie en cours
 * @param caseJouee : case à jouer, entre 0 et 11
 * @return 0 si le coup est invalide, 1 sinon
*/
int jouer(partie *p, int caseJouee);

/**
 * Détermine si une partie est terminée, et le gagnant le cas échéant
 * @param p : partie en cours
*/
int finDePartie(partie *p);

#endif // AWALE_H