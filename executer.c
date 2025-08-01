#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h> // Pour gerer Ctrl + C dans arriere plan

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

        for(i = 0; dirs[i] != 0; i++) {
            snprintf(pathname, sizeof pathname, "%s/%s", dirs[i], mot[0]);
            execv(pathname, mot);
        }

        fprintf( stderr, "%s: not found\n", mot[0]);
        exit(1);

}
