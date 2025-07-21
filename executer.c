#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum {
        MaxPathLength = 512, // longueur max d'un nom de fichier
};

void executer_cmd(char **mot, char **dirs) {
       int tmp;
       char pathname[MaxPathLength];
       int i;

        tmp = fork();     // lancer le processus enfant

        if (tmp < 0) {
            perror("fork");
            return;
        }

        if (tmp != 0) {
            while(wait(0) != tmp);
            return;
        }

        // enfant: exec du programme
        for(i = 0; dirs[i] != 0; i++) {
            snprintf(pathname, sizeof pathname, "%s/%s", dirs[i], mot[0]);
            execv(pathname, mot);
        }

        fprintf( stderr, "%s: not found\n", mot[0]);
        exit(1);

}
