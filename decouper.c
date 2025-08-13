/**
 * NOM: decouper.c
 * ROLE: Implementation de la fonction de decoupage de la ligne de commande.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: Ce fichier est compile et lie au programme principal du shell.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decouper.h"

/**
 * decouper - Analyse et decoupe une ligne de commande en un pipeline de commandes.
 * @ligne: La chaine de caracteres brute entree par l'utilisateur. Cette chaine
 *         sera modifiee en place par strtok_r.
 * @cmds: Le tableau de type Commandes qui sera rempli par la fonction.
 * @arriere_plan: Un pointeur vers un entier. La fonction mettra 1 a cette adresse
 *                si la commande doit s'executer en arriere-plan (&), 0 sinon.
 *
 * La fonction utilise strtok_r, une version reentrante de strtok, pour garantir
 * que les decoupages imbriques (d'abord sur '|', puis sur " ") fonctionnent
 * correctement. Elle modifie la chaine `ligne` en y inserant des caracteres nuls '\0'.
 *
 * Retourne:
 *  - Le nombre de commandes simples trouvees dans le pipeline.
 *  - -1 en cas d'erreur (par exemple, depassement de MAX_CMDS).
 */
int decouper(char *ligne, Commandes cmds, int *arriere_plan) {
    char *saveptr_pipe, *saveptr_space;
    char *cmd_str;
    int cmd_count = 0;

    // Etape 0: Verifier si la commande doit s'executer en arriere-plan.
    // On cherche le caractere '&' a la fin de la ligne.
    *arriere_plan = 0;
    char *bg_char = strrchr(ligne, '&'); // strrchr pour trouver la derniere occurrence
    if (bg_char != NULL && *(bg_char + 1) == '\0') { // Verifie que c'est bien a la fin
        *arriere_plan = 1;
        *bg_char = '\0'; // On supprime le '&' pour qu'il ne soit pas traite comme un argument.
    }

    // Etape 1: Decoupage de la ligne en commandes separees par le pipe '|'.
    // strtok_r est utilise pour permettre un decoupage imbrique.
    for (cmd_str = strtok_r(ligne, "|", &saveptr_pipe);
         cmd_str != NULL && cmd_count < MAX_CMDS;
         cmd_str = strtok_r(NULL, "|", &saveptr_pipe)) {

        char *mot;
        int mot_count = 0;

        // Etape 2: Pour chaque commande, decoupage en mots (arguments) separes par des espaces/tabulations.
        for (mot = strtok_r(cmd_str, " \t\n", &saveptr_space);
             mot != NULL && mot_count < MAX_MOTS - 1; // -1 pour garder de la place pour NULL
             mot = strtok_r(NULL, " \t\n", &saveptr_space)) {
            cmds[cmd_count][mot_count++] = mot;
        }
        // La convention pour execvp exige que le tableau d'arguments se termine par NULL.
        cmds[cmd_count][mot_count] = NULL;

        // On incremente le compteur de commandes uniquement si la commande n'est pas vide.
        if (mot_count > 0) {
            cmd_count++;
        }
    }

    // Verifie si le nombre maximum de commandes a ete depasse.
    if (cmd_count >= MAX_CMDS) {
        fprintf(stderr, "Erreur : Trop de commandes (limite: %d)\n", MAX_CMDS);
        return -1; // Retourne une erreur.
    }

    return cmd_count;
}