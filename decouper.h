#ifndef DECOUPER_H
#define DECOUPER_H

#define MAX_CMDS 10
#define MAX_MOTS 50

typedef char* Commandes[MAX_CMDS][MAX_MOTS];

int decouper(char * ligne, Commandes cmds, int *arriere_plan);

#endif
