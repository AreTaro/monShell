/* cn-decouper.c
  Un wrapper autour de strtok
*/

# include <stdio.h>
# include <string.h>
# include <stdlib.h>

/* decouper -- decouper une chaine en mots 
*  retourne 1 si la commande est en arriere plan, 0 sinon
*/

int decouper (char * ligne, char * separ, char * mot[], int maxmot) {

    int i;
    int arriere_plan = 0;

    mot[0] = strtok(ligne, separ); // Decoupe la ligne de commande

    for (i = 1; mot[i-1] != 0; i++) {

        // Gestion des erreurs (ligne de commande trop longue)
        if (i == maxmot) {
            fprintf(stderr, "Erreur de la fonction decouper: trop de mots\n");
            mot[i-1]=0;
            break;
        }

        mot[i] = strtok(NULL, separ); // Copie chaque mot dans un tableau
    }

    if (i > 1 && strcmp(mot[i-2], "&") == 0) {
        arriere_plan = 1;
        mot[i-2] = NULL;
    }

    return arriere_plan;

}

