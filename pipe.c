#include "pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> // Pour open()

// Déclaration d'une fonction d'aide pour gérer les redirections d'un seul processus
static void gerer_redirections(CommandeSimple* cmd);

int executer_sequence(CommandeSimple sequence[MAX_COMMANDES], int nb_commandes) {
    if (nb_commandes <= 0) {
        return 0; // Rien à faire
    }

    // Cas simple : une seule commande, pas besoin de pipe
    if (nb_commandes == 1) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return -1;
        }
        if (pid == 0) { // Processus enfant
            gerer_redirections(&sequence[0]);
            execvp(sequence[0].arguments[0], sequence[0].arguments);
            perror("execvp"); // S'affiche seulement si execvp échoue
            exit(EXIT_FAILURE);
        }
        // Processus parent
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }

    // Cas du pipe : au moins deux commandes
    int input_fd = STDIN_FILENO; // L'entrée de la première commande est stdin
    pid_t pids[MAX_COMMANDES];

    for (int i = 0; i < nb_commandes; i++) {
        int pipefd[2];

        // 1. Créer un pipe, sauf pour la dernière commande
        if (i < nb_commandes - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                return -1;
            }
        }

        // 2. Créer un processus enfant pour la commande actuelle
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return -1;
        }

        if (pids[i] == 0) { // --- Processus Enfant ---
            // 3a. Rediriger l'entrée
            if (input_fd != STDIN_FILENO) {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }

            // 3b. Rediriger la sortie
            if (i < nb_commandes - 1) {
                close(pipefd[0]); // L'enfant n'a pas besoin de lire sur le nouveau pipe
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            // 3c. Gérer les redirections de fichier (ex: <, >), qui peuvent écraser les pipes
            gerer_redirections(&sequence[i]);

            // 4. Exécuter la commande
            execvp(sequence[i].arguments[0], sequence[i].arguments);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // --- Processus Parent ---
        // 5. Mettre à jour les descripteurs de fichiers pour la prochaine itération
        if (input_fd != STDIN_FILENO) {
            close(input_fd);
        }
        if (i < nb_commandes - 1) {
            close(pipefd[1]); // Le parent n'a pas besoin d'écrire
            input_fd = pipefd[0]; // L'entrée de la prochaine commande est la sortie de celle-ci
        }
    }

    // 6. Attendre la fin de tous les enfants
    int status = 0;
    for (int i = 0; i < nb_commandes; i++) {
        waitpid(pids[i], &status, 0);
    }

    return WEXITSTATUS(status); // Retourne le statut du dernier processus
}

/**
 * @brief Gère les redirections <, > et >> pour un processus.
 * Cette fonction est appelée DEPUIS un processus enfant, juste avant execvp.
 */
static void gerer_redirections(CommandeSimple* cmd) {
    if (cmd->redir_in) {
        int fd = open(cmd->redir_in, O_RDONLY);
        if (fd == -1) {
            perror("open (in)");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (cmd->redir_out) {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->append_out) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC; // Écraser le fichier s'il existe
        }
        int fd = open(cmd->redir_out, flags, 0644);
        if (fd == -1) {
            perror("open (out)");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}
