/* redirection.h
 * definit les variables globales et la fonction chercher_redirections
 */
#ifndef REDIRECTION_H
#define REDIRECTION_H

/*
 * Variables globales pour la redirection.
 * Utiliser extern permet de les partager entre les fichier.c
 */

// mode_sortie: 0 = pas de redirection, 1 = > (ecraser), 2 = >> (ajouter)
extern int mode_sortie;
extern char *fichier_entree;
extern char *fichier_sortie;

/**
 * Analyse la liste des arguments a la recherche de chevrons de redirection 
 * 
 * Cette fonction parcourt les mots de la commande, identifie les
 * redirection, stocke les noms de fichiers associes dans les variables 
 * gobalesm et nettoie la liste des arguments en retirant les operateurs et
 * les noms de fichier de redirection pour que les commandes puisse etre 
 * executee correctement par execvp
 *
 * Le tableau de chaines de caracteres est l'argument de la commande 
 */

void chercher_redirections(char *mot[]);

#endif


