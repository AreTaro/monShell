#ifndef PIPE_H
#define PIPE_H

#include "command.h"

/**
 * @brief Exécute une séquence de commandes, en gérant les pipes et les redirections.
 *
 * @param sequence Le tableau de commandes simples à exécuter.
 * @param nb_commandes Le nombre de commandes dans le tableau.
 * @return int Le statut du dernier processus de la séquence, ou -1 en cas d'erreur grave.
 */
int executer_sequence(CommandeSimple sequence[MAX_COMMANDES], int nb_commandes);

#endif // PIPE_H
