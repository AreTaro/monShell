#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Pour chdir()

// Structure pour definir une commande interne
typedef struct {
        const char *nom;
        int (*fonction)(char **);
} CommandeInterne;

// commande mon_cd
int mon_cd(char **mots) {

        if (mots[1] == NULL) {
                // Si aucun argument, retour au repertoire home
                chdir(getenv("HOME"));
        } else {
                if (chdir(mots[1]) != 0) {
                        perror("cd");
                }
        }

        return 0; // Indique que la commande a ete traitee

}

int mon_exit(char **mots) {

        int code = 0; // Code de sortie par default

        // Si un code de sortie est indique en argument
        if (mots[1] != NULL) {

                code = atoi(mots[1]); // Conversion du code en chiffre

        }

        exit(code); // Quitte le shell avec le code

        return 0; // Ne devrait pas etre atteint

}

// Tableau des commandes internes
CommandeInterne commandes_internes[] = {
        {"cd", mon_cd},
        {"exit",mon_exit},
        {NULL, NULL} // Marque la fin du tableau
};

// Fonction pour executer une commande interne
// Renvoie 1 si la commande est interne (et l'execute), sinon 0.
int executer_cmd_interne(char **mots) {

        // Gestion des erreurs eventuels (optionnel)       
        if (mots[0] == NULL) {
                return 0;
        }

        for (int i = 0; commandes_internes[i].nom != NULL; i++) {
                
                if (strcmp(mots[0], commandes_internes[i].nom) == 0) {
                        commandes_internes[i].fonction(mots);
                        return 1; // Commande interne trouvee et executee
                }
        }

        return 0; // Pas de commande interne (ne devrait pas etre atteint)

}

/** 
 * Sources:
 * Structure
 * https://www.emse.fr/~boissier/enseignement/c/LATEX-WWW/SEMESTER-I/COURS-C/c-cours007.html
 * exit
 * https://eric-sanchis.developpez.com/linux/shell/bash-gabarit/?page=code-retour#LVII-3
 */
