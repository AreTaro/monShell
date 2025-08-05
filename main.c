#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <signal.h>

#include "executer.h"
#include "cmd_interne.h"
#include "decouper.h"
#include "redirection.h"

enum {
    MaxLigne = 1024,        // longueur max d'une ligne de commande
    MaxDirs = 100,          // nbre max de repertoire dans PATH
};

// ... (les fonctions afficher_prompt et recuperer_enfant restent inchangées)
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

// La fonction pour découper le PATH reste nécessaire mais doit être adaptée
// pour ne pas utiliser notre nouvelle fonction "decouper".
// On utilise une fonction simple basée sur strtok pour cette tâche spécifique.
void decouper_path(char* path, char* dirs[], int max_dirs) {
    char* path_copy = strdup(path);
    char* token;
    int i = 0;
    token = strtok(path_copy, ":");
    while (token != NULL && i < max_dirs - 1) {
        dirs[i++] = token;
        token = strtok(NULL, ":");
    }
    dirs[i] = NULL;
}

int main (int argc, char * argv[]) 
{
    char ligne[MaxLigne];
    Commandes cmds; // Utilise notre nouveau type pour stocker le pipeline
    char * dirs[MaxDirs];
    int arriere_plan;
    int nb_cmds;

    signal(SIGINT, SIG_IGN); // Le shell principal ignore Ctrl+C

    // Découper la variable d'environnement PATH
    decouper_path(getenv("PATH"), dirs, MaxDirs);

    /* Lire et traiter chaque ligne de commande */
    for (
        afficher_prompt();
        fgets(ligne, sizeof ligne, stdin) != 0;
        afficher_prompt() 
    ) {
        recuperer_enfant(); // Recherche les processus en arrière-plan terminés

        // Utilisation de notre nouvelle fonction de découpage
        nb_cmds = decouper(ligne, cmds, &arriere_plan);
 
        if (nb_cmds <= 0) { // Ligne vide ou erreur de découpage
            continue;
        }

        if (nb_cmds == 1) {
            // NOTE: La redirection pour les commandes internes n'est pas gérée pour l'instant.
            // L'appel à chercher_redirections a été retiré pour corriger un bug avec les commandes externes.
            if (executer_cmd_interne(cmds[0])) {
                continue; // Si c'était une commande interne, on passe à la suite.
            }
        }
	
        // Exécution du pipeline (qu'il contienne 1 ou plusieurs commandes)
        executer_pipeline(cmds, nb_cmds, dirs, arriere_plan);
    }

    printf("Bye\n");
    return 0;
}
