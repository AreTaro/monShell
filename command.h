/**
 * NOM: command.h
 * ROLE: Definition des structures de donnees et constantes principales.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: A inclure par tous les modules qui manipulent la structure d'une
 *        commande.
 */

#ifndef COMMAND_H
#define COMMAND_H

// Nombre maximal de commandes pouvant etre enchainees par des pipes (ex: cmd1 | cmd2 | ...).
#define MAX_COMMANDES 10

// Nombre maximal de mots (arguments) pour une seule commande simple.
#define MAX_MOTS 64

/**
 * struct CommandeSimple - Represente une seule commande et ses redirections.
 * @argument: Tableau de chaines de caracteres pour la commande et ses arguments.
 *            Le dernier element doit etre NULL (convention pour execvp).
 * @redir_int: Nom du fichier pour la redirection d'entree (<). NULL si non utilise.
 * @redir_out: Nom du fichier pour la redirection de sortie (> ou >>). NULL si non utilise.
 * @append_out: Un booleen (0 ou 1) indiquant le mode de redirection de sortie.
 *              - 0 pour le mode ecrasement (>).
 *              - 1 pour le mode ajout (>>).
 */
typedef struct {
    char* argument[MAX_MOTS];

    // Redirections (laisses a NULL si non utilisees)
    char* redir_int;
    char* redir_out;

    // Mode de redirection de sortie
    int append_out; // Mis a 1 si la redirection est en mode ajout (>>), 0 sinon.
} CommandeSimple;

#endif // Fin de la garde d'inclusion