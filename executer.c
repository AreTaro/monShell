/**
 * NOM: executer.c
 * ROLE: Implementation de la logique d'execution des commandes externes,
 *       des pipelines, et des taches de fond.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: Ce fichier est compile et lie au programme principal du shell.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h> // Pour la gestion des signaux (Ctrl+C)
#include <fcntl.h>  // Pour les options de la fonction open()

#include "redirection.h" // Pour les variables globales de redirection
#include "executer.h"

/**
 * executer_une_commande - Execute une seule commande externe.
 * @mot: Le tableau d'arguments de la commande (ex: {"ls", "-l", NULL}).
 * @dirs: Les repertoires du PATH ou chercher l'executable.
 *
 * Cette fonction est appelee a l'interieur d'un processus fils.
 * Elle applique d'abord les redirections si necessaire, puis cherche
 * l'executable de la commande dans les repertoires du PATH.
 * Si elle le trouve, elle le remplace par le processus courant via `execv`.
 * Si `execv` retourne, c'est qu'une erreur s'est produite.
 */
static void executer_une_commande(char **mot, char **dirs) {
    char pathname[512];

    // Etape 1: Appliquer les redirections d'entree/sortie.
    // Les variables globales (fichier_entree, etc.) ont ete prealablement
    // definies par chercher_redirections().

    // Redirection de l'entree standard (stdin).
    if (fichier_entree != NULL) {
        int fd_in = open(fichier_entree, O_RDONLY);
        if (fd_in == -1) {
            perror(fichier_entree);
            exit(1); // Quitte le processus fils en cas d'echec.
        }
        // dup2 remplace le descripteur de fichier de stdin par celui du fichier ouvert.
        if (dup2(fd_in, STDIN_FILENO) == -1) {
            perror("dup2 stdin");
            exit(1);
        }
        close(fd_in); // Le descripteur n'est plus necessaire apres dup2.
    }

    // Redirection de la sortie standard (stdout).
    if (mode_sortie > 0) { // 1 pour > (ecraser), 2 pour >> (ajouter)
        int flags = O_WRONLY | O_CREAT;
        flags |= (mode_sortie == 1) ? O_TRUNC : O_APPEND;
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

    // Etape 2: Chercher et executer la commande dans les repertoires du PATH.
    for (int i = 0; dirs[i] != NULL; i++) {
        snprintf(pathname, sizeof(pathname), "%s/%s", dirs[i], mot[0]);
        execv(pathname, mot);
    }

    // Si on arrive ici, c'est que `execv` a echoue pour tous les repertoires.
    fprintf(stderr, "%s: command not found\n", mot[0]);
    exit(1); // Important: quitte le processus fils.
}

/**
 * executer_pipeline - Orchestre l'execution d'un pipeline de commandes.
 */
void executer_pipeline(Commandes cmds, int nb_cmds, char **dirs, int arriere_plan) {
    pid_t group_pid;

    // Si la commande est en arriere-plan, on cree un processus intermediaire
    // qui deviendra le chef de groupe du pipeline.
    if (arriere_plan) {
        group_pid = fork();
        if (group_pid < 0) {
            perror("fork");
            return;
        }
        if (group_pid > 0) { // Parent (le shell)
            printf("[%d]\n", group_pid); // Affiche le PID du groupe
            return; // Et retourne immediatement au prompt.
        }
        // Enfant (futur chef de groupe): il se detache du shell.
        setpgid(0, 0);
    }

    // Ignore le signal Ctrl+C (SIGINT) pour le shell lui-meme.
    // Les enfants le restaureront s'ils sont en premier plan.
    signal(SIGINT, arriere_plan ? SIG_IGN : SIG_DFL);

    int i;
    pid_t pid;
    int fd_in = STDIN_FILENO; // L'entree de la 1ere commande est stdin.
    int pipefd[2];
    pid_t pids[MAX_CMDS];

    // Boucle principale: cree un processus pour chaque commande du pipeline.
    for (i = 0; i < nb_cmds; i++) {
        // Si ce n'est pas la derniere commande, on cree un pipe pour la connecter a la suivante.
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

        if (pid == 0) { // Code execute par le processus enfant
            if (!arriere_plan) {
                signal(SIGINT, SIG_DFL); // Les processus en premier plan repondent a Ctrl+C.
            }

            // Redirige l'entree si ce n'est pas la premiere commande.
            if (fd_in != STDIN_FILENO) {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            // Redirige la sortie si ce n'est pas la derniere commande.
            if (i < nb_cmds - 1) {
                close(pipefd[0]); // Ferme l'extremite de lecture du pipe.
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            // Analyse la commande pour les redirections de type <, > et >>.
            chercher_redirections(cmds[i]);

            // Execute la commande.
            executer_une_commande(cmds[i], dirs);
        }

        // Code execute par le processus parent (le shell ou le chef de groupe)
        pids[i] = pid; // Stocke le PID de l'enfant.

        // Ferme le descripteur d'entree du pipe precedent.
        if (fd_in != STDIN_FILENO) {
            close(fd_in);
        }

        // Pour la prochaine iteration, l'entree sera la sortie du pipe actuel.
        if (i < nb_cmds - 1) {
            close(pipefd[1]); // Le parent ne va pas ecrire dans le pipe.
            fd_in = pipefd[0]; // On garde l'extremite de lecture pour le prochain fils.
        }
    }

    // Le parent attend la fin de tous ses enfants.
    for (i = 0; i < nb_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }

    // Si on etait un groupe de processus en arriere-plan, le chef de groupe se termine.
    if (arriere_plan) {
        exit(0);
    }

    // Restaure la gestion par defaut du signal pour le shell.
    signal(SIGINT, SIG_IGN);
}