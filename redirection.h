#ifndef REDIRECTION_H
#define REDIRECTION_H

/*
  Variables globales pour la redirection.
  Utiliser `extern` permet de les partager entre les fichiers .c.
*/

// mode_sortie: 0 = pas de redirection, 1 = > (écraser), 2 = >> (ajouter)
extern int mode_sortie;
extern char *fichier_entree;
extern char *fichier_sortie;

/**
 * @brief Analyse la liste des arguments à la recherche de chevrons de redirection.
 * 
 * Cette fonction parcourt les mots de la commande, identifie les redirections
 * (`<`, `>`, `>>`), stocke les noms de fichiers associés dans les variables
 * globales, et nettoie la liste d'arguments en retirant les opérateurs et
 * les noms de fichiers de redirection pour que la commande puisse être
 * exécutée correctement par `execvp`.
 * 
 * @param mot Le tableau de chaînes de caractères (arguments de la commande).
 */
void chercher_redirections(char *mot[]);

#endif
