# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <assert.h>

#include "executer.h"

enum {
    MaxLigne = 1024,        // longueur max d'une ligne de commande
    MaxMot = MaxLigne / 2,  // nbre max de mot dans la ligne
    MaxDirs = 100,          // nbre max de repertoire dans PATH
};

void decouper(char *, char *, char **, int);

# define PROMPT "? "

int main (int argc, char * argv[]) {
    
    char ligne[MaxLigne];
    char * mot[MaxMot];
    char * dirs[MaxDirs];

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
	
        executer_cmd(mot,dirs);
        }

    printf("Bye\n");
    return 0;

}
