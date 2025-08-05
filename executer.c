#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "redirection.h"
#include "executer.h"

enum {
    MaxPathLength = 512, // longueur max d'un nom de fichier
};

// Fonction interne pour exécuter UNE SEULE commande. C'est le coeur de l'ancien executer_cmd.
static void executer_une_commande(char **mot, char **dirs) {
    char pathname[MaxPathLength];
    int i;

    // Gérer les redirections de fichiers si elles ont été détectées
    if (fichier_entree != NULL) {
        int fd_in = open(fichier_entree, O_RDONLY);
        if (fd_in == -1) {
            perror(fichier_entree);
            exit(1);
        }
        if (dup2(fd_in, STDIN_FILENO) == -1) {
            perror("dup2 stdin");
            exit(1);
        }
        close(fd_in);
    }

    if (mode_sortie > 0) {
        int flags = O_WRONLY | O_CREAT;
        flags |= (mode_sortie == 1) ? O_TRUNC : O_APPEND; // TRUNC pour > et APPEND pour >>
        int fd_out = open(fichier_sortie, flags, 0666);
        if (fd_out == -1) {
            perror(fichier_sortie);
            exit(1);
        }
        if (dup2(fd_out, STDOUT_FILENO) == -1) {
            perror("dup2 stdout");
            exit(1);
        }
        close(fd_out);
    }

    // Chercher et exécuter la commande dans les répertoires du PATH
    for (i = 0; dirs[i] != 0; i++) {
        snprintf(pathname, sizeof pathname, "%s/%s", dirs[i], mot[0]);
        execv(pathname, mot);
    }

    // Si execv retourne, c'est qu'il y a eu une erreur
    fprintf(stderr, "%s: command not found\n", mot[0]);
    exit(1);
}

void executer_pipeline(Commandes cmds, int nb_cmds, char **dirs, int arriere_plan) {
    pid_t group_pid;

    // Si la commande est en arrière-plan, on fork une première fois.
    // Le parent (le shell) affiche le PID et retourne immédiatement.
    // L'enfant (le leader du groupe) exécute le pipeline.
    if (arriere_plan) {
        group_pid = fork();
        if (group_pid < 0) {
            perror("fork");
            return;
        }
        if (group_pid > 0) { // Parent (le shell)
            printf("[%d]\n", group_pid);
            return;
        }
        // Enfant (futur leader du groupe de processus)
        setpgid(0, 0); // Se détache du shell
    }

    int i;
    pid_t pid;
    int fd_in = STDIN_FILENO;
    int pipefd[2];
    pid_t pids[MAX_CMDS];

    if (!arriere_plan) {
        signal(SIGINT, SIG_DFL);
    } else {
        // Les processus en arrière-plan ne doivent pas être tués par Ctrl+C du shell
        signal(SIGINT, SIG_IGN);
    }


    for (i = 0; i < nb_cmds; i++) {
        // Créer un pipe pour toutes les commandes sauf la dernière
        if (i < nb_cmds - 1) {
            if (pipe(pipefd) < 0) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // --- Processus Enfant ---
            if (!arriere_plan) {
                signal(SIGINT, SIG_DFL);
            }

            // Rediriger l'entrée standard si ce n'est pas la première commande
            if (fd_in != STDIN_FILENO) {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            // Rediriger la sortie standard si ce n'est pas la dernière commande
            if (i < nb_cmds - 1) {
                close(pipefd[0]); // L'enfant n'a pas besoin de lire sur le nouveau pipe
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            // Analyser la commande actuelle pour les redirections de type < et >
            chercher_redirections(cmds[i]);

            // Exécuter la commande
            executer_une_commande(cmds[i], dirs);
        }

        // --- Processus Parent ---
        pids[i] = pid; // Stocker le PID de l'enfant

        // Fermer le descripteur d'entrée du pipe précédent
        if (fd_in != STDIN_FILENO) {
            close(fd_in);
        }

        // Pour la prochaine itération, l'entrée sera la sortie du pipe actuel
        if (i < nb_cmds - 1) {
            close(pipefd[1]); // Le parent n'a pas besoin d'écrire sur le pipe
            fd_in = pipefd[0];
        }
    }

    // Le processus qui gère le pipeline (le shell ou le leader du groupe) doit attendre ses enfants.
    for (i = 0; i < nb_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }

    // Si on est dans le processus leader du groupe (créé pour l'arrière-plan),
    // il doit se terminer maintenant que son pipeline est fini.
    if (arriere_plan) {
        exit(0);
    }

    // Si on est dans le shell principal (exécution en avant-plan), on restaure le handler.
    signal(SIGINT, SIG_IGN);
}
