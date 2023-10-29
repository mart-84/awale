#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include "includes.h"

typedef struct
{
    int nbCoups;
    int *coups;
    char nomJoueur1[BUF_SIZE];
    char nomJoueur2[BUF_SIZE];
    int premierJoueur;
} partieSauvegardee;

/**
 * Initialise une sauvegarde avec les joueurs en précisant le joueur courant
 * @param nomJ1 nom du joueur 1
 * @param nomJ2 nom du joueur 2
 * @param premierJoueur joueur qui commence la partie : 0 pour le joueur 1, 1 pour le joueur 2
 * @return partieSauvegardee * partie sauvegardée
 * */
partieSauvegardee *creerSauvegarde(char *nomJ1, char *nomJ2, int premierJoueur);

/**
 * Ajoute des coups à la sauvegarde
 * @param partie partie sauvegardée
 * @param coups tableau des coups à ajouter
 * @param nbCoups nombre de coups à ajouter
*/
void ajouterCoups(partieSauvegardee *partie, int *coups, int nbCoups);

/**
 * Sauvegarde une partie dans un fichier
 * @param partie partie à sauvegarder
*/
void sauvegarderPartie(partieSauvegardee *partie);

/**
 * Charge une partie sauvegardée
 * @param filename nom du fichier de sauvegarde
 * @return partieSauvegardee * partie sauvegardée
*/
partieSauvegardee *chargerPartie(char *filename);

/**
 * Liste les sauvegardes
 * @param filtreNom filtre pour les noms des fichiers
 * @return char * liste des sauvegardes
*/
char *listerSauvegardes(char *filtreNom);

#endif /* guard */