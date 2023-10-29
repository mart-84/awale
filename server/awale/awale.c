#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "awale.h"

#define FIN_PARTIE -102

void init(Partie *p)
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

void printBoard(Partie *p)
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

char *sprintBoard(Partie *p, char *buffer, char *joueur1, char *joueur2)
{
    char line[100];
    strcpy(buffer, "\n\n+----+-----------------------+----+\n");
    if (p->joueurCourant == 0)
    {
        // J2 | plateau : 11 -> 6 | score J2
        sprintf(line, "| J2 | %d | %d | %d | %d | %d | %d | %02d |\n", p->plateau[11], p->plateau[10], p->plateau[9], p->plateau[8], p->plateau[7], p->plateau[6], p->scores[1]);
    }
    else
    {
        // J1 | plateau : 5 -> 0 | score J1
        sprintf(line, "| J1 | %d | %d | %d | %d | %d | %d | %02d |\n", p->plateau[5], p->plateau[4], p->plateau[3], p->plateau[2], p->plateau[1], p->plateau[0], p->scores[0]);
    }
    strcat(buffer, line);
    strcpy(line, "|    |-----------------------|    |\n");
    strcat(buffer, line);
    if (p->joueurCourant == 0)
    {
        // score J1 | plateau : 0 -> 5 | J1
        sprintf(line, "| %02d | %d | %d | %d | %d | %d | %d | J1 |\n", p->scores[0], p->plateau[0], p->plateau[1], p->plateau[2], p->plateau[3], p->plateau[4], p->plateau[5]);
    }
    else
    {
        // score J2 | plateau : 6 -> 11 | J2
        sprintf(line, "| %02d | %d | %d | %d | %d | %d | %d | J2 |\n", p->scores[1], p->plateau[6], p->plateau[7], p->plateau[8], p->plateau[9], p->plateau[10], p->plateau[11]);
    }
    strcat(buffer, line);
    strcpy(line, "+----+-----------------------+----+\n");
    strcat(buffer, line);
    strcpy(line, "       A   B   C   D   E   F\n");
    strcat(buffer, line);
    sprintf(line, "Au tour de %s de jouer ...\n", p->joueurCourant == 0 ? joueur1 : joueur2);
    strcat(buffer, line);
    return buffer;
}

int interpreterCoup(Partie *p, char choix)
{
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

int calculerGrainesJoueur(int *plateau, int joueur)
{
    int somme = 0;
    int offset = joueur == 0 ? 0 : 6;
    for (int i = 0; i < 6; i++)
    {
        somme += plateau[i + offset];
    }
    return somme;
}

int calculerScore(Partie *p, int caseArrivee, int *plateau)
{
    int score = 0;
    int limite = p->joueurCourant == 0 ? 6 : 0;

    int plateauTmp[NB_CASES];
    for (int i = 0; i < NB_CASES; i++)
    {
        plateauTmp[i] = plateau[i];
    }

    // verification que la case d'arrivée se situe dans la bonne moitié du tableau selon le joueur courant
    if ((p->joueurCourant == 0 && caseArrivee <= 5) || (p->joueurCourant == 1 && caseArrivee >= 6))
    {
        return 0;
    }
    else
    {
        while ((plateauTmp[caseArrivee] == 2 || plateauTmp[caseArrivee] == 3) && caseArrivee >= limite)
        {
            score += plateauTmp[caseArrivee];
            plateauTmp[caseArrivee] = 0;
            caseArrivee--;
        }
    }

    // verification que toutes les cases de l'adversaire ne sont pas égales à 0
    if (calculerGrainesJoueur(plateauTmp, (p->joueurCourant + 1) % 2) == 0)
    {
        printf("Impossible de ramasser, risque de famine !\n");
        return 0; // famine, on ne ramasse pas les graines
    }

    for (int i = 0; i < NB_CASES; i++)
    {
        plateau[i] = plateauTmp[i];
    }

    return score;
}

int jouer(Partie *p, int coup)
{
    int nombreGraines = p->plateau[coup];
    if (nombreGraines == 0)
    {
        printf("Case vide !\n");
        return EXIT_FAILURE;
    }

    int plateau[NB_CASES];
    for (int i = 0; i < NB_CASES; i++)
    {
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

    if (score != -1) // si le coup est légal
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
void jouerCoupClavier(Partie *p)
{
    char choix;
    int coup, status;

    printBoard(p);
    int gagnant = finDePartie(p);
    if (gagnant != PAS_DE_GAGNANT)
    {
        printf("Fin de partie : ");

        if (gagnant == JOUEUR1)
        {
            printf("victoire du joueur 1\n");
        }
        else if (gagnant == JOUEUR2)
        {
            printf("victoire du joueur 2\n");
        }
        else if (gagnant == EGALITE)
        {
            printf("égalité\n");
        }
        else
        {
            printf("????????????");
        }

        exit(EXIT_SUCCESS);
    }

    do
    {
        printBoard(p);
        do
        {
            printf("Choix de la case : ");
            scanf(" %c", &choix);
            fflush(stdin);
            coup = interpreterCoup(p, choix);
        } while (coup == EXIT_FAILURE);
        status = jouer(p, coup);
    } while (status == EXIT_FAILURE);
}

int finDePartie(Partie *p)
{
    if (p->scores[0] >= NB_GRAINES_VICTOIRE)
    {
        return JOUEUR1;
    }

    if (p->scores[1] >= NB_GRAINES_VICTOIRE)
    {
        return JOUEUR2;
    }
    int nbGrainesJoueurCourant = calculerGrainesJoueur(p->plateau, p->joueurCourant);
    int nbGrainesAdversaire = calculerGrainesJoueur(p->plateau, (p->joueurCourant + 1) % 2);

    if (nbGrainesJoueurCourant == 0)
    {
        printf("Toutes les cases du joueur courant sont vides !\n");
        // l'adversaire ramasse ses graines
        printf("Le joueur %d ramasse ses graines\n", (p->joueurCourant) + 1);
        p->scores[(p->joueurCourant + 1) % 2] += nbGrainesAdversaire;
        if (p->scores[0] > p->scores[1])
        {
            return JOUEUR1;
        }

        if (p->scores[1] > p->scores[0])
        {
            return JOUEUR2;
        }

        if (p->scores[0] == p->scores[1])
        {
            return EGALITE;
        }
    }
    else if (nbGrainesAdversaire == 0)
    {
        printf("Toutes les cases de l'adversaire sont vides !\n");
        int offset = p->joueurCourant == 0 ? 0 : 6;

        // recherche d'un coup pour nourrir l'adversaire
        int i = offset;
        int coupPossible = 0;
        while (i < 6 + offset && coupPossible == 0)
        {
            if (p->plateau[i] >= (6 - i + offset))
            {
                coupPossible = 1;
            }
            i++;
        }

        if (coupPossible == 0)
        {
            // le joueur courant ramasse ses graines
            printf("Le joueur %d ramasse ses graines\n", (p->joueurCourant) + 1);
            p->scores[p->joueurCourant] += nbGrainesJoueurCourant;

            printf("J1 [%d] - [%d] J2\n", p->scores[0], p->scores[1]);

            if (p->scores[0] > p->scores[1])
            {
                return JOUEUR1;
            }

            if (p->scores[1] > p->scores[0])
            {
                return JOUEUR2;
            }

            if (p->scores[0] == p->scores[1])
            {
                return EGALITE;
            }
        }
    }

    return PAS_DE_GAGNANT;
}
