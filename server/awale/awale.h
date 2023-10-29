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

/**
 * Structure représentant une partie d'Awale.
 * Elle contient le plateau de jeu, les scores des joueurs et l'indice du joueur courant.
 */
typedef struct Partie
{
    plateau plateau;
    scores scores;
    int joueurCourant; // 0 : joueur 1, 1 : joueur 2
} Partie;

/**
 * Initialise une partie d'awale et le plateau
 * @param p Partie à initialiser
 */
void init(Partie *p);

/**
 * Formatter le plateau de jeu
 * @param p Partie à afficher
 */
void printBoard(Partie *p);

/**
 * Affiche le plateau de jeu
 * @param p Partie à afficher
 * @param buffer buffer dans lequel écrire le plateau
 * @return buffer
 */
char *sprintBoard(Partie *p, char *buffer, char *joueur1, char *joueur2);

/**
 * Interprète le coup joué par le joueur
 * Insensible à la casse
 * @param p Partie en cours
 * @param choix choix du joueur entre A et F
 * @return case à jouer, entre 0 et 11, EXIT_FAILURE si le coup est invalide
 */
int interpreterCoup(Partie *p, char choix);

/**
 * Joue un coup
 * @param p Partie en cours
 * @param caseJouee case à jouer, entre 0 et 11
 * @return EXIT_FAILURE si le coup est invalide, 0 sinon
 */
int jouer(Partie *p, int caseJouee);

/**
 * Détermine si une partie est terminée, et le gagnant le cas échéant
 * @param p Partie en cours
 * @return JOUEUR1 ou JOUEUR2, si l'un d'eux a gagné, EGALITE le cas échéant et PAS_DE_GAGNANT sinon
 */
int finDePartie(Partie *p);

#endif // AWALE_H