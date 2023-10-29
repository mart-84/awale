#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include "includes.h"

/**
 * Structure représentant une partie sauvegardée.
 * Elle contient le nombre de coups joués, les coups joués, les noms des deux joueurs,
 * et l'indice du joueur qui a commencé la partie.
 */
typedef struct PartieSauvegardee
{
    int nbCoups;
    int *coups;
    char nomJoueur1[BUF_SIZE];
    char nomJoueur2[BUF_SIZE];
    int premierJoueur; // 0 : joueur 1, 1 : joueur 2
} PartieSauvegardee;

/**
 * Initialise une sauvegarde avec les joueurs en précisant le joueur courant
 * @param nomJ1 nom du joueur 1
 * @param nomJ2 nom du joueur 2
 * @param premierJoueur joueur qui commence la Partie : 0 : joueur 1, 1 : joueur 2
 * @return PartieSauvegardee * Partie sauvegardée
 * */
PartieSauvegardee *creerSauvegarde(char *nomJ1, char *nomJ2, int premierJoueur);

/**
 * Ajoute des coups à la sauvegarde
 * @param Partie Partie sauvegardée
 * @param coups tableau des coups à ajouter
 * @param nbCoups nombre de coups à ajouter
 */
void ajouterCoups(PartieSauvegardee *partie, int *coups, int nbCoups);

/**
 * Sauvegarde une partie dans un fichier
 * @param Partie Partie à sauvegarder
 */
void sauvegarderPartie(PartieSauvegardee *partie);

/**
 * Charge une partie sauvegardée
 * @param filename nom du fichier de sauvegarde
 * @return PartieSauvegardee * Partie sauvegardée
 */
PartieSauvegardee *chargerPartie(char *filename);

/**
 * Liste les sauvegardes
 * @param filtreNom filtre pour les noms des fichiers
 * @return char * liste des sauvegardes, NULL si aucune sauvegarde
 */
char *listerSauvegardes(char *filtreNom);

#endif /* guard */