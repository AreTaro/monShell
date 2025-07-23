#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Pour chdir()

// Structure pour définir une commande interne
typedef struct {
    const char *nom;
    int (*fonction)(char **);
} CommandeInterne;

// Implémentation de la commande 'cd'
int cmd_cd(char **mots) {
    if (mots[1] == NULL) {
        // Si aucun argument, aller au répertoire personnel
        chdir(getenv("HOME"));
    } else {
        if (chdir(mots[1]) != 0) {
            perror("cd");
        }
    }
    return 0; // Indique que la commande a été traitée
}

// Implémentation de la commande 'exit'
int cmd_exit(char **mots) {
    int code = 0;
    if (mots[1] != NULL) {
        code = atoi(mots[1]);
    }
    exit(code); // Quitte le shell
    return 0; // Ne devrait pas être atteint
}

// Tableau des commandes internes
CommandeInterne commandes_internes[] = {
    {"cd", cmd_cd},
    {"exit", cmd_exit},
    {NULL, NULL} // Marqueur de fin de tableau
};

// Fonction pour exécuter une commande interne
// Renvoie 1 si la commande est interne (et l'exécute), 0 sinon.
int executer_cmd_interne(char **mots) {
    if (mots[0] == NULL) {
        return 0; // Ligne vide
    }

    for (int i = 0; commandes_internes[i].nom != NULL; i++) {
        if (strcmp(mots[0], commandes_internes[i].nom) == 0) {
            commandes_internes[i].fonction(mots);
            return 1; // Commande interne trouvée et exécutée
        }
    }
    return 0; // Pas une commande interne
}
