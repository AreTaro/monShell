/* cn-decouper.c
  Nouvelle version qui gère les pipes |
*/

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include "decouper.h"

/* decouper -- decouper une chaine en commandes, puis en mots
 * retourne le nombre de commandes trouvées.
 */
int decouper(char *ligne, Commandes cmds, int *arriere_plan) {
    char *saveptr_pipe, *saveptr_space;
    char *cmd_str;
    int cmd_count = 0;

    // On vérifie d'abord si la commande doit être en arrière-plan.
    // On cherche le caractère '&' à la fin de la ligne.
    *arriere_plan = 0;
    char *bg_char = strchr(ligne, '&');
    if (bg_char != NULL) {
        *arriere_plan = 1;
        *bg_char = '\0'; // On supprime le '&' pour ne pas qu'il soit traité comme un argument.
    }

    // Étape 1: Découpage de la ligne principale en fonction du délimiteur de pipe "|".
    // On utilise strtok_r pour permettre un découpage imbriqué et sécurisé.
    for (cmd_str = strtok_r(ligne, "|", &saveptr_pipe); cmd_str != NULL && cmd_count < MAX_CMDS; cmd_str = strtok_r(NULL, "|", &saveptr_pipe)) {
        
        char *mot;
        int mot_count = 0;

        // Étape 2: Pour chaque commande, on la découpe en mots (arguments).
        // Le délimiteur est un ensemble d'espaces, tabulations et sauts de ligne.
        for (mot = strtok_r(cmd_str, " \t\n", &saveptr_space); mot != NULL && mot_count < MAX_MOTS - 1; mot = strtok_r(NULL, " \t\n", &saveptr_space)) {
            cmds[cmd_count][mot_count++] = mot;
        }
        cmds[cmd_count][mot_count] = NULL; // On termine chaque liste d'arguments par NULL, comme l'exige execvp.
        
        // On ne compte la commande que si elle contient au moins un mot.
        if (mot_count > 0) {
            cmd_count++;
        }
    }

    if (cmd_count >= MAX_CMDS) {
        fprintf(stderr, "Erreur: Trop de commandes dans le pipe (limite: %d)\n", MAX_CMDS);
        return -1; // Retourne une erreur
    }

    return cmd_count;
}

