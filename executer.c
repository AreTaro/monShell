#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h> // Pour gerer Ctrl + C dans arriere plan et + encore
#include <fcntl.h> // Pour gerer les flags de open() et + encore
#include "redirection.h"
#include "executer.h"


enum {
        MaxPathLength = 512, // longueur max d'un nom de fichier
};


// Fonction interne pour executer une seule commande
static void executer_une_commande(char **mot, char **dirs) {
        char pathname[MaxPathLength];
        int i;

	// Gerer les redirections de fichiers si elles ont ete detectees
	if(fichier_entree != NULL) {
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
		// TRUNC pour > et APPEND pour >> 
		flags |= (mode_sortie == 1) ? O_TRUNC : O_APPEND;
		int fd_out = open(fichier_sortie, flags, 0066);
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

	// Chercher et execter la commandee dans les repertoires du PATH
	for (i = 0; dirs[i] != 0; i++) {
	        snprintf(pathname, sizeof pathname, "%s/%s", dirs[i], mot[0]);
		execv(pathname, mot);
	}

	// Si execv retourne, c'est qu'il y a une erreur 
	fprintf(stderr, "%s: command not found\n", mot[0]);
	exit(1);
}


void executer_pipeline(Commandes cmds, 
		       int nb_cmds, 
		       char **dirs, 
		       int arriere_plan) {
        pid_t group_pid;
	/* Si en arriere plan, on fork une premiere fois 
	 * Le parent affiche le PID et l'enfant execute le pipeline */
	if (arriere_plan) {
	        group_pid = fork();
		if (group_pid < 0) {
		        perror("fork");
			return;
		} 
		if (group_pid > 0) { // Parent
			    printf("[%d]\n", group_pid);
			    return;
		}
		/* Enfant (futur leader du groupe de processus */
		setpgid(0,0); // Se detache du Shell
	}

	int i;
	pid_t pid;
	int fd_in = STDIN_FILENO;
	int pipefd[2];
	pid_t pids[MAX_CMDS];

	if (!arriere_plan) {
	        signal(SIGINT, SIG_DFL);
	} else {
	        /* les processus en arriere non tues par Ctrl + c */
	        signal(SIGINT, SIG_IGN);
	}

	for (i = 0; i < nb_cmds; i++) {
	        /* Creer un pipe pour toutes les commandes sauf la derniere */
	        if (i < nb_cmds -1) {
		        if (pipe(pipefd) < 0) {
			        perror("pipe");
				exit(EXIT_FAILURE);
			}
		}

		pid = fork();
		if (pid  < 0) {
		        perror("fork");
			exit(EXIT_FAILURE);
	        }
		
		if (pid == 0) { // enfant
		        if (!arriere_plan) {
			        signal(SIGINT, SIG_DFL);
			 }

			/* rediriger l'entree standard si pas premier commande */
			if (fd_in != STDIN_FILENO) {
			        dup2(fd_in, STDIN_FILENO);
				close(fd_in);
			}

			/* rediriger la sortie standard si pas derniere cmde */
		        if (i < nb_cmds -1) {
			        close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}

			/* Analyser la commande actuelle pour > et < */
			chercher_redirections(cmds[i]);

			/* executer la commande */
			executer_une_commande(cmds[i], dirs);
		}

		/* Procesus Parent 
		 * stock le PID de l'enfant */
		pids[i] = pid; 

		/* Fermer le descripteur d'entree du pipe precedent */
		if (fd_in != STDIN_FILENO) {
		        close(fd_in);
		}

	        /* Prochaine iteration, entree sera sortie pipe actuel */
		if (i < nb_cmds -1) {
		        close(pipefd[1]); // parent ecrit plus
			fd_in = pipefd[0];
		}

        }

	/* Process leader groupe attend enfants */
	for (i = 0; i < nb_cmds; i++) {
	        waitpid(pids[i], NULL, 0);
	}

	/* Si processus leader groupe alors doit terminer */
	if (arriere_plan) {
	        exit(0);
	}

	/* Si premier plan, restauration */
	signal(SIGINT, SIG_IGN);

}
