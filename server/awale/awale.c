#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "awale.h"

#define FIN_PARTIE -102


void init(partie *p)
{
    srand(time(NULL) + 2);
    p->joueurCourant = rand() % 2;
    p->scores[0] = 0;
    p->scores[1] = 0;

    for (int i = 0; i < NB_CASES; i++)
    {
        p->plateau[i] = 4;
    }
}

void printBoard(partie *p)
{
    printf("\n\n+----+-----------------------+----+\n");
    if (p->joueurCourant == 0)
    {
        // J2 | plateau : 11 -> 6 | score J2
        printf("| J2 | %d | %d | %d | %d | %d | %d |  %d |\n", p->plateau[11], p->plateau[10], p->plateau[9], p->plateau[8], p->plateau[7], p->plateau[6], p->scores[1]);
    }
    else
    {
        // J1 | plateau : 5 -> 0 | score J1
        printf("| J1 | %d | %d | %d | %d | %d | %d |  %d |\n", p->plateau[5], p->plateau[4], p->plateau[3], p->plateau[2], p->plateau[1], p->plateau[0], p->scores[0]);
    }
    printf("|    |-----------------------|    |\n");
    if (p->joueurCourant == 0)
    {
        // score J1 | plateau : 0 -> 5 | J1
        printf("|  %d | %d | %d | %d | %d | %d | %d | J1 |\n", p->scores[0], p->plateau[0], p->plateau[1], p->plateau[2], p->plateau[3], p->plateau[4], p->plateau[5]);
    }
    else
    {
        // score J2 | plateau : 6 -> 11 | J2
        printf("|  %d | %d | %d | %d | %d | %d | %d | J2 |\n", p->scores[1], p->plateau[6], p->plateau[7], p->plateau[8], p->plateau[9], p->plateau[10], p->plateau[11]);
    }
    printf("+----+-----------------------+----+\n");
    printf("       A   B   C   D   E   F\n");
    printf("A J%d de jouer...\n", p->joueurCourant + 1);
}

int interpreterCoup(partie *p, char choix) {
    int coup;

    switch (choix)
    {
    case 'A':
    case 'a':
        coup = 0;
        break;
    case 'B':
    case 'b':
        coup = 1;
        break;
    case 'C':
    case 'c':
        coup = 2;
        break;
    case 'D':
    case 'd':
        coup = 3;
        break;
    case 'E':
    case 'e':
        coup = 4;
        break;
    case 'F':
    case 'f':
        coup = 5;
        break;

    default:
        return -1;
    }

    if (p->joueurCourant == 1)
    { // J2
        coup += 6;
    }

    return coup;
}

int calculerGrainesJoueur(int *plateau, int joueur) {
    int somme = 0;
    int offset = joueur == 0 ? 0 : 6;
    for (int i = 0; i < 6; i++)
    {
        somme += plateau[i + offset];
    }
    return somme;
}

int calculerScore(partie *p, int caseArrivee, int *plateau)
{
    int score = 0;
    int limite = p->joueurCourant == 0 ? 6 : 0;

    int plateauTmp[NB_CASES];
    for (int i = 0; i < NB_CASES; i++) {
        plateauTmp[i] = plateau[i];
    }
    
    // verification que la case d'arrivée se situe dans la bonne moitié du tableau selon le joueur courant
    if ((p->joueurCourant == 0 && caseArrivee <= 5) 
    || (p->joueurCourant == 1 && caseArrivee >= 6))
    {
        printf("Case d'arrivée chez le joueur courant !\n");
        return 0;

    } else {
        while((plateauTmp[caseArrivee] == 2 || plateauTmp[caseArrivee] == 3) && caseArrivee >= limite )
        {
            score += plateauTmp[caseArrivee];
            plateauTmp[caseArrivee] = 0;
            caseArrivee--;
        }
    }

    // verification que toutes les cases de l'adversaire ne sont pas égales à 0
    int somme = calculerGrainesJoueur(plateauTmp, (p->joueurCourant + 1) % 2);
    if (somme == 0)
    {
        printf("Impossible de ramasser, risque de famine !\n");
        return 0; // famine, on ne ramasse pas les graines
    }

    for (int i = 0; i < NB_CASES; i++) {
        plateau[i] = plateauTmp[i];
    }
    
    return score;
}

int jouer(partie *p, int coup)
{
    int somme = calculerGrainesJoueur(p->plateau, p->joueurCourant);
    if (somme == 0)
    {
        printf("Toutes les cases du joueur courant sont vides !\n");
        int scoreAdversaire = calculerGrainesJoueur(p->plateau, (p->joueurCourant + 1) % 2);
        p->scores[(p->joueurCourant + 1) % 2] += scoreAdversaire;
        printf("Fin de la partie\n");
        return FIN_PARTIE;
    }

    if (calculerGrainesJoueur(p->plateau, (p->joueurCourant + 1) % 2) == 0) {
        printf("Toutes les cases de l'adversaire sont vides !\n");
        int offset = p->joueurCourant == 0 ? 0 : 6;

        int i = offset;
        int coupPossible = 0;
        while (i < 6 + offset && coupPossible == 0) {
            if (p->plateau[i] >= (6 - i + offset)) {
                printf("Au moins un coup est possible ;) !\n");
                coupPossible = 1;
            }
            i++;
        }
        if (coupPossible == 0) {
            printf("Fin de la partie\n");
            return FIN_PARTIE;
        }
    }

    int nombreGraines = p->plateau[coup];
    if (nombreGraines == 0)
    {
        printf("Case vide !\n");
        return EXIT_FAILURE;
    }

    int plateau[NB_CASES];
    for (int i = 0; i < NB_CASES; i++) {
        plateau[i] = p->plateau[i];
    }

    plateau[coup] = 0;
    int caseArrivee = -1;
    for (int i = 1; i <= nombreGraines; i++)
    {
        if (i % NB_CASES != 0)
        {
            caseArrivee = (coup + i) % NB_CASES;
            plateau[caseArrivee]++;
        }
    }

    int score = calculerScore(p, caseArrivee, plateau);

    if (score != -1) //si le coup est légal
    {
        p->scores[p->joueurCourant] += score;
        for (int i = 0; i < NB_CASES; i++)
        {
            p->plateau[i] = plateau[i];
        }
    }
    else
    {
        printf("Coup illégal !\n");
        return EXIT_FAILURE;
    }

    p->joueurCourant = (p->joueurCourant + 1) % 2;
    
    // calcul du score du awale

    return EXIT_SUCCESS;
}

// fonction pour lire au clavier le coup et le jouer
void jouerCoupClavier(partie *p)
{
    char choix;
    int coup, status;
    do
    {
        printBoard(p);
        do
        {
            printf("Choix de la case : ");
            scanf(" %c", &choix);
            fflush(stdin);
            coup = interpreterCoup(p, choix);
        } while (coup == -1);
        status = jouer(p, coup);
    } while (status == EXIT_FAILURE);
    if (status == FIN_PARTIE)
    {
        printf("Quelqu'un a gagné, flemme de dire qui c'est \n");
        exit(EXIT_SUCCESS);
    }

}

// int main()
// {
//     partie p;
//     init(&p);

//     while(1) {
//         jouerCoupClavier(&p);
//     }
//     printf("C'est la faim, donc c'est la fin ;)\n\n");
//     // int coup = interpreterCoup(&p, 'd');
//     // jouerCoup(&p, coup);
//     // printPartie(&p);
//     // coup = interpreterCoup(&p, 'A');
//     // jouerCoup(&p, coup);
//     // printPartie(&p);
//     // coup = interpreterCoup(&p, 'C');
//     // jouerCoup(&p, coup);
//     // printPartie(&p);
//     // coup = interpreterCoup(&p, 'E');
//     // jouerCoup(&p, coup);
//     // printPartie(&p);

//     return EXIT_SUCCESS;
// }

