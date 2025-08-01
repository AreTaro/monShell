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
    MaxMot = MaxLigne / 2,  // nbre max de mot dans la ligne
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

int main (int argc, char * argv[]) 
{
    
    char ligne[MaxLigne];
    char * mot[MaxMot];
    char * dirs[MaxDirs];
    int arriere_plan;

    signal(SIGINT, SIG_IGN);

    /* Decouper une partie de PATH en repertoires */
    decouper(strdup(getenv("PATH")),":",dirs, MaxDirs);

    /* Lire et traiter chaque ligne de commande */
    for (
        afficher_prompt();
        fgets(ligne, sizeof ligne, stdin) != 0;
        afficher_prompt() ) {

        /* Recherche les processus zombies */
        recuperer_enfant();

        arriere_plan = decouper(ligne, " \t\n", mot, MaxMot);
 
        if (mot[0] == 0)  // ligne vide
            continue;

        chercher_redirections(mot);

        if (executer_cmd_interne(mot)) {
                continue;
        }
	
        executer_cmd(mot,dirs, arriere_plan);

        }

    printf("Bye\n");
    return 0;

}
