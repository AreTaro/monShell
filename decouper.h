#ifndef DECOUPER_H
#define DECOUPER_H

// On définit une limite pour le nombre de commandes dans un pipe et de mots par commande.
#define MAX_CMDS 10
#define MAX_MOTS 50

// Le nouveau type: un tableau de commandes. Chaque commande est un tableau de "mots" (char*).
// C'est l'équivalent d'un char* [MAX_CMDS][MAX_MOTS];
typedef char* Commandes[MAX_CMDS][MAX_MOTS];

/*
 * decouper -- decoupe la ligne en commandes (séparées par |) puis en mots.
 * @param ligne La ligne de commande à traiter.
 * @param cmds La structure (notre tableau 2D) à remplir avec les commandes.
 * @param arriere_plan Un pointeur vers un entier qui sera mis à 1 si la commande
 *                     doit s'exécuter en arrière-plan, 0 sinon.
 * @return Le nombre de commandes trouvées dans la ligne.
 */
int decouper(char *ligne, Commandes cmds, int *arriere_plan);

#endif
