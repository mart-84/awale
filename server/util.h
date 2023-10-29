#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Découpe une chaîne de caractères en utilisant un délimiteur spécifié et stocke les résultats dans un tableau de chaînes de caractères.
 *
 * @param buffer La chaîne de caractères à découper.
 * @param delimiter Le caractère délimiteur à utiliser pour découper la chaîne.
 * @param tokens Un pointeur vers un tableau de pointeurs de chaînes de caractères qui contiendra les résultats.
 * @return Le nombre de chaînes de caractères stockées dans le tableau de résultats.
 */
int tokeniserChaineDeCaracteres(char *buffer, char delimiter, char ***tokens);

#endif /* guard*/