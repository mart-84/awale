#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "sauvegarde.h"
#include "includes.h"

#define SAVEDIR "saves/"
#define FILE_EXTENSION "awale"

partieSauvegardee *creerSauvegarde(char *nomJ1, char *nomJ2, int premierJoueur)
{
    partieSauvegardee *partie = malloc(sizeof(partieSauvegardee));
    strcpy(partie->nomJoueur1, nomJ1);
    strcpy(partie->nomJoueur2, nomJ2);
    partie->nbCoups = 0;
    partie->coups = NULL;
    partie->premierJoueur = premierJoueur;

    return partie;
}

void ajouterCoups(partieSauvegardee *partie, int *coups, int nbCoups)
{
    // agrandissment de la zone mémoire
    partie->coups = (int *)realloc(partie->coups, sizeof(int) * (nbCoups + partie->nbCoups));
    // ajout des nouveaux coups à la fin du tableau
    for (int i = 0; i < nbCoups; i++)
    {
        partie->coups[partie->nbCoups + i] = coups[i];
    }
    partie->nbCoups += nbCoups;
}

void sauvegarderPartie(partieSauvegardee *partie)
{
    char filename[BUF_SIZE * 2 + 16];
    sprintf(filename, "%s%s_%s_%d.%s", SAVEDIR, partie->nomJoueur1, partie->nomJoueur2, (int)time(NULL), FILE_EXTENSION);
    mkdir(SAVEDIR, 0777);
    FILE *f = fopen(filename, "w+");
    printf("Sauvegarde de la partie dans le fichier %d\n", f->_fileno);
    fwrite(&partie->nbCoups, sizeof(int), 1, f);
    fwrite(partie->coups, sizeof(int), partie->nbCoups, f);

    // lecture des noms des joueurs
    int longueurNomJ1 = strlen(partie->nomJoueur1);
    int longueurNomJ2 = strlen(partie->nomJoueur2);
    fwrite(&longueurNomJ1, sizeof(int), 1, f);
    fwrite(partie->nomJoueur1, sizeof(char), longueurNomJ1, f);
    fwrite(&longueurNomJ2, sizeof(int), 1, f);
    fwrite(partie->nomJoueur2, sizeof(char), longueurNomJ2, f);

    fwrite(&partie->premierJoueur, sizeof(char), 1, f);
    fclose(f);
}

partieSauvegardee *chargerPartie(char *filename)
{
    char file[strlen(filename) + strlen(SAVEDIR)];
    sprintf(file, "%s%s", SAVEDIR, filename);
    printf("Ouverture du fichier %s\n", filename);
    FILE *f = fopen(file, "rb");

    if (f == NULL)
        return NULL;

    partieSauvegardee *partie = malloc(sizeof(partieSauvegardee));

    fread(&(partie->nbCoups), sizeof(int), 1, f);

    partie->coups = malloc(sizeof(int) * partie->nbCoups);
    fread(partie->coups, sizeof(int), partie->nbCoups, f);

    // lecture des noms des joueurs
    int longueurBuffer;
    fread(&longueurBuffer, sizeof(int), 1, f);
    fread(partie->nomJoueur1, sizeof(char), longueurBuffer, f);
    fread(&longueurBuffer, sizeof(int), 1, f);
    fread(partie->nomJoueur2, sizeof(char), longueurBuffer, f);

    fread(&partie->premierJoueur, sizeof(char), 1, f);
    fclose(f);

    return partie;
}

char *listerSauvegardes(char *filtreNom)
{
    DIR *d = opendir(SAVEDIR);
    struct dirent *fichier = readdir(d);
    char *buffer = malloc(sizeof(char) * BUF_SIZE);

    // parcours des fichiers du dossier
    while (fichier != NULL)
    {
        if (strstr(fichier->d_name, FILE_EXTENSION) != NULL && strstr(fichier->d_name, filtreNom) != NULL)
        {
            strncat(buffer, "- ", BUF_SIZE - strlen(buffer) - 1);
            strncat(buffer, fichier->d_name, BUF_SIZE - strlen(buffer) - 1);
            strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
        }

        fichier = readdir(d);
    }

    return buffer;
}

// int main() {
//     printf("%s\n", listerSauvegardes("pierre"));
// }
