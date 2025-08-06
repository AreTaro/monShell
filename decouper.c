/* cn-decouper.c
 * Une nouvelle version qui gere les pipes
*/

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include "decouper.h"

/* decouper -- decouper une chaine en commandes, puis en mots
 * retourne le nombre de commande trouvees */

int decouper (char * ligne, Commandes cmds, int* arriere_plan) {
        char *saveptr_pipe, *saveptr_space;
	char *cmd_str;
	int cmd_count = 0;

	// On verifie d'abord si la commande doit etre en arrire plan
	// On cherche le caractere & a la fin de la ligne
	*arriere_plan = 0;
	char *bg_char = strchr(ligne, '&');
	if (bg_char != NULL) {
	          *arriere_plan = 1;
		  *bg_char = '\0'; // on supprime le & pour ne pas le traiter
	}

	// etape 1 : decoupage en fonction de |
	for (cmd_str = strtok_r(ligne, "|", &saveptr_pipe);
	     cmd_str != NULL && cmd_count < MAX_CMDS; 
	     cmd_str = strtok_r(NULL, "|", &saveptr_pipe)) {
	        char *mot;
		int mot_count = 0;

		// etape 2 : decouper chaque commande en mots
		for (mot = strtok_r(cmd_str, " \t\n", &saveptr_space);
		     mot != NULL && cmd_count < MAX_MOTS -1;
		     mot = strtok_r(NULL, " \t\n", &saveptr_space)) {
		        cmds[cmd_count][mot_count++] = mot;
		}
		cmds[cmd_count][mot_count] = NULL; // termine liste par NULL

		// compte commande si au moins un mot
		if (mot_count > 0) {
		        cmd_count++;
		}
	}

        if (cmd_count >= MAX_CMDS) {
	        fprintf(stderr, "Erreur : Trop de commandes dans le pipe\n",
			MAX_CMDS);
		return -1; // retourne une erreur
        }
       
	return cmd_count;

}

