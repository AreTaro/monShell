#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// --- Nouveaux modules pour le parsing et l'exécution ---
#include "command.h"
#include "parser.h"
#include "pipe.h"
// -----------------------------------------------------

#include "cmd_interne.h"

enum {
    MaxLigne = 1024,        // longueur max d'une ligne de commande
};

/* Affiche le dossier courant dans le prompt
 * et substitue le fichier HOME par le caractere ~ */
void afficher_prompt() 
{
    char cwd[MaxLigne];
    char *home_dir = getenv("HOME");
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (home_dir != NULL && strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
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
            // Optionnel: décommenter pour un feedback sur les processus terminés
            // printf("[%d] Done\n", pid);
        } else if (WIFSIGNALED(status)) {
            printf("[%d] Termine avec le signal %d\n", pid, WTERMSIG(status));
        }
    }
}

int main(void) 
{
    char ligne[MaxLigne];
    // La nouvelle structure pour stocker la séquence de commandes parsées
    CommandeSimple sequence[MAX_COMMANDES];
    int nb_commandes;

    signal(SIGINT, SIG_IGN);

    /* Lire et traiter chaque ligne de commande */
    for (
        afficher_prompt();
        fgets(ligne, sizeof ligne, stdin) != 0;
        afficher_prompt() ) {

        /* Recherche les processus zombies (utile pour les commandes en arrière-plan) */
        recuperer_enfant();

        // 1. Utiliser le nouveau parser
        if (analyser_ligne(ligne, sequence, &nb_commandes) != 0) {
            // Erreur de syntaxe, le parser a déjà affiché un message
            continue;
        }
 
        if (nb_commandes == 0) { // Ligne vide ou seulement des espaces
            continue;
        }

        // 2. Gérer les commandes internes (uniquement si pas de pipe)
        if (nb_commandes == 1) {
            if (executer_cmd_interne(sequence[0].arguments)) {
                continue; // La commande interne a été exécutée
            }
        }
	
        // 3. Exécuter la séquence de commandes (simple ou avec pipes)
        executer_sequence(sequence, nb_commandes);
    }

    printf("\nBye\n");
    return 0;
}
