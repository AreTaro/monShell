# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <assert.h>
# include <signal.h>

#include "executer.h"
#include "cmd_interne.h"
#include "decouper.h"
#include "redirection.h"

enum {
    MaxLigne = 1024,        // longueur max d'une ligne de commande
    MaxDirs = 100,          // nbre max de repertoire dans PATH
};

/* Affiche le dossier courant dans le prompt
 * et substitue le fichier HOME par le caractere ~ */
void afficher_prompt() 
{
        char cwd[MaxLigne];
        char *home_dir = getenv("HOME");
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                if (home_dir != NULL && strncmp(cwd, home_dir, 
                        strlen(home_dir)) == 0) {
                                printf("~%s? ", cwd + strlen(home_dir));
                } else {
                        printf("%s? ", cwd);
                }
                fflush(stdout);
        }
}

/* Recupere les processus termines pour eviter les zombies */
void recuperer_enfant() {
        int status;
        pid_t pid;

        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
                if (WIFEXITED(status)) {
                        printf("[%d] Done\n", pid);
                } else if (WIFSIGNALED(status)) {
                        printf("[%d] Termine avec le signal %d\n", 
                                pid, WTERMSIG(status));
               }
        }
}

void decouper_path(char* path, char* dirs[], int max_dirs) {
        char *path_copy = strdup(path);
	char *token;
	int i = 0;
	token = strtok(path_copy, ":");
	while (token != NULL && i < max_dirs -1) {
	        dirs[i++] = token;
		token = strtok(NULL, ":");
	}
	dirs[i] = NULL;
}

int main (int argc, char * argv[]) 
{
    
    char ligne[MaxLigne];
    Commandes cmds;
    char * dirs[MaxDirs];
    int arriere_plan;
    int nb_cmds;

    /* Shell principal ignore Ctrl + C */
    signal(SIGINT, SIG_IGN); 

    /* Decouper une partie de PATH en repertoires */
    decouper_path(getenv("PATH"), dirs, MaxDirs);

    /* Lire et traiter chaque ligne de commande */
    for (
        afficher_prompt();
        fgets(ligne, sizeof ligne, stdin) != 0;
        afficher_prompt() 
    ) {

        /* Recherche les processus zombies */
        recuperer_enfant();

        nb_cmds = decouper(ligne, cmds, &arriere_plan);
 
        if (nb_cmds <= 0) { // Ligne vide ou erreur de decoupage
            continue;
        }
	
	if (nb_cmds == 1) {
	/* Commande interne non gere par la redirection */
	        if (executer_cmd_interne(cmds[0])) {
		  continue; // si commande interne
                }
        }

	executer_pipeline(cmds, nb_cmds, dirs, arriere_plan);
	
    }

    printf("Bye\n");
    return 0;

}
