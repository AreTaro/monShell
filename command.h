#ifndef COMMAND_H
#define COMMAND_H

// --- Limites Configurables ---
// Augmenter ces valeurs si nécessaire, mais elles sont suffisantes pour un usage courant.

// Nombre maximum de commandes pouvant être enchaînées par des pipes (ex: cmd1 | cmd2 | ... | cmdN)
#define MAX_COMMANDES 10

// Nombre maximum de "mots" (commande + arguments) pour une seule commande simple.
#define MAX_MOTS 64

// --- Structures de Données ---

/**
 * @brief Représente une seule commande simple (ex: "ls -l > out.txt").
 *
 * Une ligne de commande complexe est une séquence de une ou plusieurs de ces structures.
 */
typedef struct {
    // Tableau des arguments pour execvp(). Le premier élément (arguments[0]) est la commande.
    // Le tableau doit être terminé par un pointeur NULL.
    char* arguments[MAX_MOTS];

    // Redirections (laissés à NULL s'ils ne sont pas utilisés)
    char* redir_in;   // Nom du fichier pour la redirection d'entrée (<)
    char* redir_out;  // Nom du fichier pour la redirection de sortie (>)

    // Mode de redirection de sortie
    int append_out;   // Mis à 1 si la redirection est en mode ajout (>>), 0 sinon.
} CommandeSimple;

#endif // COMMAND_H
