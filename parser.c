#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> // Pour NULL

// Déclaration d'une fonction d'aide (privée à ce fichier) que nous définirons plus tard.
static int parser_commande_simple(char* segment, CommandeSimple* cmd);

/**
 * @brief Analyse une ligne de commande complète.
 */
int analyser_ligne(char* ligne, CommandeSimple sequence[MAX_COMMANDES], int* nb_commandes) {
    *nb_commandes = 0;
    char* segment;
    char* saveptr;

    // 1. Découper la ligne en segments en utilisant le délimiteur "|"
    segment = strtok_r(ligne, "|", &saveptr);

    while (segment != NULL && *nb_commandes < MAX_COMMANDES) {
        // Initialiser la structure CommandeSimple pour ce segment
        CommandeSimple* cmd = &sequence[*nb_commandes];
        // La fonction d'aide fera le travail détaillé.
        if (parser_commande_simple(segment, cmd) != 0) {
            fprintf(stderr, "Erreur de syntaxe près de : %s\n", segment);
            return -1; // Erreur de parsing
        }

        (*nb_commandes)++;
        // Passer au segment suivant
        segment = strtok_r(NULL, "|", &saveptr);
    }

    if (segment != NULL) {
        fprintf(stderr, "Erreur : Trop de commandes dans le pipe (max: %d)\n", MAX_COMMANDES);
        return -1;
    }

    return 0; // Succès
}

/**
 * @brief Analyse un segment de commande (pas de pipe ici) pour les arguments et redirections.
 * @param segment La chaîne de caractères contenant une seule commande (ex: "ls -l > out.txt")
 * @param cmd La structure CommandeSimple à remplir.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 */
static int parser_commande_simple(char* segment, CommandeSimple* cmd) {
    // Initialisation de la structure
    for (int i = 0; i < MAX_MOTS; i++) {
        cmd->arguments[i] = NULL;
    }
    cmd->redir_in = NULL;
    cmd->redir_out = NULL;
    cmd->append_out = 0;

    int arg_count = 0;
    char* mot;
    char* saveptr_mot;
    const char* delim_mots = " \t\n\r";

    mot = strtok_r(segment, delim_mots, &saveptr_mot);
    while (mot != NULL) {
        if (strcmp(mot, ">") == 0) {
            // Récupérer le mot suivant qui est le nom du fichier
            mot = strtok_r(NULL, delim_mots, &saveptr_mot);
            if (mot == NULL) {
                fprintf(stderr, "Erreur de syntaxe: nom de fichier manquant après '>'.\n");
                return -1;
            }
            cmd->redir_out = mot;
            cmd->append_out = 0; // Mode écrasement
        } else if (strcmp(mot, ">>") == 0) {
            // Récupérer le mot suivant qui est le nom du fichier
            mot = strtok_r(NULL, delim_mots, &saveptr_mot);
            if (mot == NULL) {
                fprintf(stderr, "Erreur de syntaxe: nom de fichier manquant après '>>'.\n");
                return -1;
            }
            cmd->redir_out = mot;
            cmd->append_out = 1; // Mode ajout
        } else if (strcmp(mot, "<") == 0) {
            // Récupérer le mot suivant qui est le nom du fichier
            mot = strtok_r(NULL, delim_mots, &saveptr_mot);
            if (mot == NULL) {
                fprintf(stderr, "Erreur de syntaxe: nom de fichier manquant après '<'.\n");
                return -1;
            }
            cmd->redir_in = mot;
        } else {
            // C'est un argument normal
            if (arg_count < MAX_MOTS - 1) {
                cmd->arguments[arg_count++] = mot;
            } else {
                fprintf(stderr, "Erreur: Trop d'arguments pour une commande (max: %d)\n", MAX_MOTS);
                return -1;
            }
        }
        // Passer au mot suivant
        mot = strtok_r(NULL, delim_mots, &saveptr_mot);
    }

    if (arg_count == 0) {
        fprintf(stderr, "Erreur: Commande vide détectée (peut-être 'cmd1 | | cmd2').\n");
        return -1;
    }

    cmd->arguments[arg_count] = NULL; // Marqueur de fin pour execvp

    return 0; // Succès
}
