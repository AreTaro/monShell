# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <assert.h>

enum {
    MaxLigne = 1024,        // longueur max d'une ligne de commande
    MaxMot = MaxLigne / 2,  // nbre max de mot dans la ligne
    MaxDirs = 100,          // nbre max de repertoire dans PATH
    MaxPathLength = 512,    // longueur max d'un nom de fichier
};

void decouper(char *, char *, char **, int);

# define PROMPT "? "

int main (int argc, char * argv[]) {
    
    char ligne[MaxLigne];
    char pathname[MaxPathLength];
    char * mot[MaxMot];
    char * dirs[MaxDirs];
    int i, tmp;

    /* Decouper une partie de PATH en repertoires */
    decouper(strdup(getenv("PATH")),":",dirs, MaxDirs);

    /* Lire et traiter chaque ligne de commande */
    for (
        printf(PROMPT);
        fgets(ligne, sizeof ligne, stdin) != 0;
        printf(PROMPT) ) {

        decouper(ligne, " \t\n", mot, MaxMot);
        
        if (mot[0] == 0)  // ligne vide
            continue;

        tmp = fork();     // lancer le processus enfant

        if (tmp < 0) {
            perror("fork");
            continue;
        }

        if (tmp != 0) {
            while(wait(0) != tmp);
            continue;
        }

        // enfant: exec du programme
        for(i = 0; dirs[i] != 0; i++) {
            snprintf(pathname, sizeof pathname, "%s/%s", dirs[i], mot[0]);
            execv(pathname, mot);
        }

        fprintf( stderr, "%s: not found\n", mot[0]);
        exit(1);

        }

    printf("Bye\n");
    return 0;

}
