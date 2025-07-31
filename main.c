# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <assert.h>

#include "executer.h"
#include "cmd_interne.h"
#include "decouper.h"

enum {
    MaxLigne = 1024,        // longueur max d'une ligne de commande
    MaxMot = MaxLigne / 2,  // nbre max de mot dans la ligne
    MaxDirs = 100,          // nbre max de repertoire dans PATH
};

void afficher_prompt() {
    char cwd[MaxLigne];
    char *home_dir = getenv("HOME");
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (home_dir != NULL && strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
            printf("~%s? ", cwd + strlen(home_dir));
        } else {
            printf("%s? ", cwd);
        }
    } else {
        printf("? ");
    }
    fflush(stdout);
}

int main (int argc, char * argv[]) {
    
    char ligne[MaxLigne];
    char * mot[MaxMot];
    char * dirs[MaxDirs];

    /* Decouper une partie de PATH en repertoires */
    decouper(strdup(getenv("PATH")),":",dirs, MaxDirs);

    /* Lire et traiter chaque ligne de commande */
    for (
        afficher_prompt();
        fgets(ligne, sizeof ligne, stdin) != 0;
        afficher_prompt()
    ) {
        decouper(ligne, " \t\n", mot, MaxMot);
        
        if (mot[0] == 0)  // ligne vide
            continue;

        if (executer_cmd_interne(mot)) {
            continue;
        }
	
        executer_cmd(mot,dirs);
    }

    printf("Bye\n");
    return 0;

}
