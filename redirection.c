#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "redirection.h"

// Initialisation des variables globales
int mode_sortie = 0; // 0 = rien, 1 = >, 2 = >>
char *fichier_entree = NULL;
char *fichier_sortie = NULL;

void chercher_redirections(char *mot[])
{
        int i = 0; // Index de lecture
        int j = 0; // Index d'ecriture (pour compacter le tableau)

        // Reinitialiser chaque nouvelle commande
        mode_sortie = 0;
        fichier_entree = NULL;
	fichier_sortie = NULL;

	while (mot[i] != NULL) {
	        if (strcmp(mot[i], "<") == 0) {
                        fichier_entree = mot[i+1];
                        i += 2; // On saute le chevron et le nom de fichier
                } else {}
        }
}
