#ifndef PARSER_H
#define PARSER_H

#include "command.h"

/**
 * @brief Analyse une ligne de commande complète.
 *
 * Cette fonction découpe la ligne en commandes simples (séparées par des pipes |)
 * et analyse chaque commande simple pour ses arguments et redirections.
 *
 * @param ligne La ligne de commande brute lue depuis l'entrée utilisateur.
 * @param sequence Le tableau de structures CommandeSimple à remplir.
 * @param nb_commandes Un pointeur vers un entier qui contiendra le nombre de commandes simples trouvées.
 * @return int 0 en cas de succès, -1 en cas d'erreur de syntaxe (ex: pipe sans commande).
 */
int analyser_ligne(char* ligne, CommandeSimple sequence[MAX_COMMANDES], int* nb_commandes);

#endif // PARSER_H
