#include "util.h"

int tokeniserChaineDeCaracteres(char* buffer, char delimiter, char*** tokens) {
    int taille = strlen(buffer);
    int i = 0;
    int debut = -1;
    int compteur = 0;
    char* token;

    *tokens = NULL;

    while(i <= taille) {
        if ((buffer[i] == delimiter || buffer[i] == '\0') && debut != -1) { // fin du token
            debut = -1;

            if ((i - debut) > 1) {
                token[i - debut] = '\0';
                *tokens = (char**) realloc(*tokens, sizeof(char*) * (compteur + 1));
                (*tokens)[compteur] = token;
                compteur++;
            }
        } else if (debut == -1) { // debut du token
            token = malloc(sizeof(char) * taille);
            token[0] = buffer[i];
            debut = i;
        } else {
            token[i - debut] = buffer[i];
        }
        i++;
    }

    return compteur;
}


// int main() {
//     char** tokens;
//     int nbTokens = tokeniserChaineDeCaracteres("j'adore awale ;)", 'a', &tokens);
//     printf("%d\n", nbTokens);

//     for (int i=0; i<nbTokens; i++) {
//         printf("%s\n", tokens[i]);
//     }
// }
