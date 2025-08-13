/**
 * NOM: cmd_interne.c
 * ROLE: Implementation des commandes internes du shell (cd, exit, man).
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: Ce fichier est compile et lie au programme principal du shell.
 *        Il n'est pas destine a etre execute directement.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Pour chdir()

#include "cmd_interne.h"

/**
 * struct CommandeInterne - Associe un nom de commande a une fonction.
 * @nom: Le nom de la commande interne (ex: "cd").
 * @fonction: Un pointeur vers la fonction qui implemente cette commande.
 *            La fonction prend un tableau de chaines de caracteres en
 *            argument (la commande et ses arguments) et retourne un entier.
 */
typedef struct {
    const char *nom;
    int (*fonction)(char **);
} CommandeInterne;

/**
 * mon_cd - Change le repertoire courant.
 * @mots: Le tableau d'arguments. mots[0] est "cd", mots[1] est le
 *        repertoire de destination.
 *
 * Si aucun argument n'est fourni, change vers le repertoire HOME de
 * l'utilisateur. Sinon, change vers le repertoire specifie.
 * Affiche une erreur si le changement de repertoire echoue.
 *
 * Retourne: Toujours 0 pour indiquer que le shell peut continuer.
 */
int mon_cd(char **mots) {
    // Si aucun argument n'est donne apres "cd", on va au repertoire HOME.
    if (mots[1] == NULL) {
        chdir(getenv("HOME"));
    } else {
        // Essaie de changer vers le repertoire specifie.
        if (chdir(mots[1]) != 0) {
            // perror affiche un message d'erreur systeme plus detaille.
            perror("cd");
        }
    }
    return 0; // La commande a ete traitee.
}

/**
 * mon_exit - Termine le processus du shell.
 * @mots: Le tableau d'arguments. mots[0] est "exit", mots[1] (optionnel)
 *        est le code de retour numerique.
 *
 * Si un code de retour est fourni, il est utilise. Sinon, le shell se
 * termine avec un code de 0 (succes).
 *
 * Retourne: Cette fonction ne retourne jamais, car elle termine le programme.
 */
int mon_exit(char **mots) {
    int code = 0; // Code de sortie par defaut.

    // Si un argument est present (ex: "exit 1"), on le convertit en entier.
    if (mots[1] != NULL) {
        code = atoi(mots[1]);
    }

    exit(code); // Termine le shell avec le code de retour.
}

/**
 * mon_man - Affiche le contenu d'une page de manuel simple.
 * @mots: Le tableau d'arguments. mots[0] est "man", mots[1] est le nom
 *        de la page de manuel a afficher.
 *
 * Recherche un fichier texte dans le repertoire "man/" et affiche son
 * contenu sur la sortie standard.
 *
 * Retourne: 0 en cas de succes, 1 en cas d'erreur (page non trouvee, etc.).
 */
int mon_man(char **mots) {
    // Verifie si l'utilisateur a bien specifie une page.
    if (mots[1] == NULL) {
        fprintf(stderr, "man: veuillez specifier la page.\n");
        return 1;
    }

    char filepath[256];
    // Construit le chemin vers le fichier de manuel (ex: "man/cd.txt").
    snprintf(filepath, sizeof(filepath), "man/%s.txt", mots[1]);

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "man: aucune entree pour %s \n", mots[1]);
        return 1;
    }

    // Lit le fichier caractere par caractere et l'affiche.
    char c;
    while ((c = fgetc(file)) != EOF) {
        putchar(c);
    }
    putchar('\n');

    fclose(file);
    return 0; // Succes.
}

// Tableau statique qui repertorie toutes les commandes internes disponibles.
CommandeInterne commandes_internes[] = {
    {"cd", mon_cd},
    {"exit", mon_exit},
    {"man", mon_man},
    {NULL, NULL} // Marqueur de fin de tableau, essentiel pour les boucles.
};

/**
 * executer_cmd_interne - Point d'entree pour les commandes internes.
 * @mots: La ligne de commande decoupee en mots.
 *
 * Parcourt le tableau `commandes_internes`. Si une correspondance est
 * trouvee entre le premier mot de la commande et un nom dans le tableau,
 * la fonction associee est executee.
 *
 * Retourne: 1 si une commande interne a ete trouvee et executee, 0 sinon.
 */
int executer_cmd_interne(char **mots) {
    // Si la ligne est vide, ce n'est pas une commande interne.
    if (mots[0] == NULL) {
        return 0;
    }

    // Boucle sur le tableau jusqu'a trouver le marqueur de fin (NULL).
    for (int i = 0; commandes_internes[i].nom != NULL; i++) {
        if (strcmp(mots[0], commandes_internes[i].nom) == 0) {
            // La commande a ete trouvee, on appelle la fonction correspondante.
            commandes_internes[i].fonction(mots);
            return 1; // On signale qu'une commande interne a ete geree.
        }
    }

    return 0; // Aucune commande interne ne correspondait.
}