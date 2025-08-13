/**
 * NOM: redirection.c
 * ROLE: Implementation de la logique de detection des redirections.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: Ce fichier est compile et lie au programme principal du shell.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "redirection.h"

// Definition et initialisation des variables globales declarees dans le .h.
// Ces variables conserveront l'etat de la redirection pour une commande donnee.
int mode_sortie = 0;      // 0 = rien, 1 = > (ecraser), 2 = >> (ajouter)
char *fichier_entree = NULL;
char *fichier_sortie = NULL;

/**
 * chercher_redirections - Analyse et traite les redirections dans une commande.
 * @mot: Le tableau de mots (arguments) d'une seule commande. Ce tableau est
 *       modifie en place.
 *
 * La fonction utilise une technique a deux indices pour parcourir le tableau:
 * - `i` est l'indice de lecture, il avance a chaque mot examine.
 * - `j` est l'indice d'ecriture, il n'avance que lorsqu'un mot doit etre conserve.
 * Cela permet de "tasser" les arguments valides au debut du tableau, ecrasant
 * ainsi les operateurs de redirection et leurs fichiers.
 */
void chercher_redirections(char *mot[]) {
    int i = 0; // Indice de lecture du tableau original.
    int j = 0; // Indice d'ecriture dans le tableau modifie.

    // Reinitialisation des variables globales pour chaque nouvelle commande analysee.
    mode_sortie = 0;
    fichier_entree = NULL;
    fichier_sortie = NULL;

    while (mot[i] != NULL) {
        // Cas 1: Redirection d'entree.
        if (strcmp(mot[i], "<") == 0) {
            if (mot[i + 1] != NULL) {
                fichier_entree = mot[i + 1];
                i += 2; // On saute l'operateur '<' et le nom du fichier.
            } else {
                fprintf(stderr, "Erreur: nom de fichier manquant apres le chevron <\n");
                i++; // On saute seulement l'operateur pour eviter une boucle infinie.
            }
        // Cas 2: Redirection de sortie (ecrasement).
        } else if (strcmp(mot[i], ">") == 0) {
            if (mot[i + 1] != NULL) {
                mode_sortie = 1; // Mode ecrasement.
                fichier_sortie = mot[i + 1];
                i += 2;
            } else {
                fprintf(stderr, "Erreur: nom de fichier manquant apres le chevron >\n");
                i++;
            }
        // Cas 3: Redirection de sortie (ajout).
        } else if (strcmp(mot[i], ">>") == 0) {
            if (mot[i + 1] != NULL) {
                mode_sortie = 2; // Mode ajout.
                fichier_sortie = mot[i + 1];
                i += 2;
            } else {
                fprintf(stderr, "Erreur: nom de fichier manquant apres le chevron >>\n");
                i++;
            }
        // Cas 4: Ce n'est pas un operateur de redirection.
        } else {
            // On conserve cet argument. On le place a la position `j`.
            mot[j] = mot[i];
            j++;
            i++;
        }
    }

    // On termine la nouvelle liste d'arguments par NULL, comme l'exige execvp.
    mot[j] = NULL;
}