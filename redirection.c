#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "redirection.h"

// Initialisation des variables globales
int mode_sortie = 0; // 0 = rien, 1 = >, 2 = >>
char *fichier_entree = NULL;
char *fichier_sortie = NULL;

void chercher_redirections(char *mot[]) {
    int i = 0; // Index de lecture
    int j = 0; // Index d'écriture (pour compacter le tableau)

    // Réinitialiser à chaque nouvelle commande
    mode_sortie = 0;
    fichier_entree = NULL;
    fichier_sortie = NULL;

    while (mot[i] != NULL) {
        if (strcmp(mot[i], "<") == 0) {
            if (mot[i+1] != NULL) {
                fichier_entree = mot[i+1];
                i += 2; // On saute le chevron et le nom de fichier
            } else {
                fprintf(stderr, "Erreur: nom de fichier manquant après le chevron <\n");
                i++;
            }
        } else if (strcmp(mot[i], ">") == 0) {
            if (mot[i+1] != NULL) {
                mode_sortie = 1; // Mode écrasement
                fichier_sortie = mot[i+1];
                i += 2;
            } else {
                fprintf(stderr, "Erreur: nom de fichier manquant après le chevron >\n");
                i++;
            }
        } else if (strcmp(mot[i], ">>") == 0) {
            if (mot[i+1] != NULL) {
                mode_sortie = 2; // Mode ajout
                fichier_sortie = mot[i+1];
                i += 2;
            } else {
                fprintf(stderr, "Erreur: nom de fichier manquant après le chevron >>\n");
                i++;
            }
        } else {
            // Ce n'est pas une redirection, on garde l'argument
            mot[j] = mot[i];
            j++;
            i++;
        }
    }

    // On termine la nouvelle liste d'arguments compactée
    mot[j] = NULL;
}