#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

#include "includes.h"

typedef struct {
    int nbCoups;
    int* coups;
    char nomJoueur1[BUF_SIZE];
    char nomJoueur2[BUF_SIZE];
    int premierJoueur;
} partieSauvegardee;


// initialise une partie avec les joueurs en précisant le joueur courant
partieSauvegardee* creerSauvegarde(char* nomJ1, char* nomJ2, int premierJoueur);

// ajoute nbCoups situés dans coups à la partie
void ajouterCoups(partieSauvegardee* partie, int* coups, int nbCoups);

// sauvegarde la partie dans un fichier
void sauvegarderPartie(partieSauvegardee* partie);

// charge une partie depuis un fichier
partieSauvegardee* chargerPartie(char* filename);

char* listerSauvegardes(char* filtreNom);

#endif /* guard */