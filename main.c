/**
 * NOM: main.c
 * ROLE: Point d'entree principal et boucle de commande du monShell.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: Compiler tous les fichiers .c ensemble et executer le binaire resultant.
 *        (ex: gcc *.c -o monShell -lreadline)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Pour strcmp, strlen, etc.
#include <unistd.h> // Pour getcwd
#include <sys/types.h>
#include <sys/wait.h> // Pour waitpid
#include <signal.h>   // Pour signal
#include <readline/readline.h> // Pour une meilleure interface utilisateur
#include <readline/history.h>  // Pour la gestion de l'historique

#include "executer.h"
#include "cmd_interne.h"
#include "decouper.h"
#include "redirection.h"

enum {
    MaxLigne = 1024, // Longueur maximale d'une ligne de commande.
    MaxDirs = 100,   // Nombre maximal de repertoires dans la variable PATH.
};

/**
 * get_prompt - Construit et retourne la chaine de l'invite de commande.
 * La chaine retournee est statique, elle est donc ecrasee a chaque appel.
 *
 * Affiche le repertoire de travail courant. Si le repertoire courant est
 * dans le repertoire HOME de l'utilisateur, il est abrege avec un '~'.
 *
 * Retourne: Un pointeur vers la chaine de caracteres du prompt.
 */
char* get_prompt() {
    static char prompt[MaxLigne];
    char cwd[MaxLigne];
    char *home_dir = getenv("HOME");

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        // Si le chemin actuel commence par le chemin HOME, on le remplace par '~'.
        if (home_dir != NULL && strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
            snprintf(prompt, sizeof(prompt), "~%s? ", cwd + strlen(home_dir));
        } else {
            snprintf(prompt, sizeof(prompt), "%s? ", cwd);
        }
    } else {
        // En cas d'erreur de getcwd, affiche un prompt generique.
        snprintf(prompt, sizeof(prompt), "monShell? ");
    }
    return prompt;
}

/**
 * recuperer_enfant - Gere les processus fils termines.
 * Cette fonction est un "gestionnaire de signal" pour les processus en
 * arriere-plan. Elle utilise waitpid avec l'option WNOHANG pour ne pas
 * bloquer le shell. Elle "nettoie" les processus zombies en recuperant
 * leur statut de terminaison et affiche un message a l'utilisateur.
 */
void recuperer_enfant() {
    int status;
    pid_t pid;

    // Boucle tant qu'il y a des processus enfants termines a recuperer.
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("[%d] Termine\n", pid);
        } else if (WIFSIGNALED(status)) {
            printf("[%d] Termine par le signal %d\n", pid, WTERMSIG(status));
        }
    }
}

/**
 * decouper_path - Separe la variable d'environnement PATH en un tableau de repertoires.
 * @path: La chaine de caracteres de la variable PATH.
 * @dirs: Le tableau de chaines qui sera rempli avec les repertoires.
 * @max_dirs: La taille maximale du tableau `dirs`.
 */
void decouper_path(char* path, char* dirs[], int max_dirs) {
    char *path_copy = strdup(path); // On copie car strtok modifie la chaine.
    char *token;
    int i = 0;
    token = strtok(path_copy, ":");
    while (token != NULL && i < max_dirs - 1) {
        dirs[i++] = token;
        token = strtok(NULL, ":");
    }
    dirs[i] = NULL; // Termine le tableau par NULL.
}

/**
 * main - La fonction principale du shell.
 */
int main(int argc, char *argv[]) {
    char *ligne;
    Commandes cmds;
    char *dirs[MaxDirs];
    int arriere_plan;
    int nb_cmds;

    // Le shell principal doit ignorer le signal Ctrl+C (SIGINT).
    // Seuls les processus enfants en premier plan y repondront.
    signal(SIGINT, SIG_IGN);

    // Preparation: recuperer et decouper la variable PATH.
    decouper_path(getenv("PATH"), dirs, MaxDirs);

    // Configure la librairie readline pour l'autocompletion des noms de fichiers.
    rl_attempted_completion_function = (rl_completion_func_t *)rl_filename_completion_function;

    // Boucle de commande principale.
    while (1) {
        // Avant chaque nouvelle commande, on verifie si des processus en arriere-plan sont termines.
        recuperer_enfant();

        // Lecture de la ligne de commande avec readline pour une meilleure experience.
        ligne = readline(get_prompt());

        if (ligne == NULL) { // L'utilisateur a tape Ctrl+D.
            break;
        }

        if (strlen(ligne) > 0) {
            add_history(ligne); // Ajoute la ligne a l'historique.
        }

        // Decoupage de la ligne en commandes et arguments.
        nb_cmds = decouper(ligne, cmds, &arriere_plan);

        if (nb_cmds <= 0) { // Ligne vide, erreur de decoupage, ou commentaire.
            free(ligne);
            continue;
        }

        // Optimisation: si c'est une commande simple (pas de pipe),
        // on verifie si c'est une commande interne avant de forker.
        if (nb_cmds == 1) {
            if (executer_cmd_interne(cmds[0])) {
                free(ligne);
                continue; // Si c'etait une commande interne, on boucle.
            }
        }

        // Execution du pipeline de commandes (meme pour une seule commande externe).
        executer_pipeline(cmds, nb_cmds, dirs, arriere_plan);

        // Liberation de la memoire allouee par readline.
        free(ligne);
    }

    printf("Bye\n");
    return 0;
}