#ifndef EXECUTER_H
#define EXECUTER_H

#include "decouper.h"

/* executer_pipeline execute une ou plusieurs commandes potentiellement
 * reliees par des pipes.
 */

void executer_pipeline(Commandes cmds, 
		       int nb_cmds, 
		       char **dirs, 
		       int arriere_plan);

#endif
