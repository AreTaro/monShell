#ifndef EXECUTER_H
#define EXECUTER_H

#include "decouper.h" // Pour accéder au type "Commandes"

/*
 * executer_pipeline -- Exécute une ou plusieurs commandes potentiellement reliées par des pipes.
 * @param cmds La structure contenant le pipeline de commandes à exécuter.
 * @param nb_cmds Le nombre de commandes dans le pipeline.
 * @param dirs Le tableau des répertoires du PATH pour trouver les exécutables.
 * @param arriere_plan Vaut 1 si le pipeline doit s'exécuter en arrière-plan, 0 sinon.
 */
void executer_pipeline(Commandes cmds, int nb_cmds, char **dirs, int arriere_plan);

#endif
