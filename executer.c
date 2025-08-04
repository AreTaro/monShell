#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h> // Pour gerer Ctrl + C dans arriere plan
#include <fcntl.h> // Pour gerer les flags de open()
#include <redirection.h>

enum {
        MaxPathLength = 512, // longueur max d'un nom de fichier
};

void executer_cmd(char **mot, char **dirs, int arriere_plan) {
        pid_t tmp;
        char pathname[MaxPathLength];
        int i;

        if (!arriere_plan) {
	        signal(SIGINT,SIG_DFL);
        }

        tmp = fork();     // lancer le processus enfant

        if (tmp < 0) {
            perror("fork");
            return;
        }

        if (tmp != 0) { // Parent
                if (!arriere_plan) {
                        waitpid(tmp, NULL, 0);
			signal(SIGINT, SIG_IGN);
                } else {
                        printf("[%d]\n", tmp);
                }
            return;
        }

        // enfant: exec du programme
	if (arriere_plan) {
                setpgid(0, 0);
        } else {
                signal(SIGINT, SIG_DFL); // Enfant ignore pas le signal
        }  

	// --- DEBUT BLOC REDIRECTION ---
	int fd_in, fd_out;

	// Redirection de l'entrée
	if (fichier_entree != NULL) {
                fd_in = open(fichier_entree, O_RDONLY);
		if (fd_in == -1) {
		        perror(fichier_entree);
			exit(1);
		}
		if (dup2(fd_in, STDIN_FILEN0) == -1) {
                        perror("dup2 stdin");
			exit(1);
		}
		close(fd_in);
	}

	// Redirection de la sortie
	if (mode_sortie > 0) {
	        int flags = O_WRONLY | O_CREAT;
		if (mode_sortie == 1) { // >
		        flags |= O_TRUNC;
		} else { // >>
                        flags |= O_APPEND;
                }

	        fd_oud = open(fichier_sortie, flag, 0666); // 066 = rw-rw-rw-
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
	// --- FIN BLOC REDIRECTION

        for(i = 0; dirs[i] != 0; i++) {
            snprintf(pathname, sizeof pathname, "%s/%s", dirs[i], mot[0]);
            execv(pathname, mot);
        }

        fprintf( stderr, "%s: not found\n", mot[0]);
        exit(1);

}
