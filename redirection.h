/**
 * NOM: redirection.h
 * ROLE: Declaration des variables et fonctions pour la gestion des redirections.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: A inclure par les modules qui ont besoin de traiter les redirections
 *        d'entree/sortie (<, >, >>).
 */

#ifndef REDIRECTION_H
#define REDIRECTION_H

/**
 * Variables globales pour la redirection.
 * L'utilisation de variables globales est generalement deconseillee, mais peut
 * se justifier dans un petit programme comme celui-ci pour simplifier la
 * communication entre le parsing de la redirection et son execution.
 * Le mot-cle `extern` indique que ces variables sont definies dans un autre
 * fichier (.c) et permet de les partager.
 */

// mode_sortie: Indique le type de redirection de sortie.
// 0 = pas de redirection, 1 = > (ecraser), 2 = >> (ajouter).
extern int mode_sortie;

// fichier_entree: Stocke le nom du fichier pour la redirection d'entree (<).
extern char *fichier_entree;

// fichier_sortie: Stocke le nom du fichier pour la redirection de sortie (> ou >>).
extern char *fichier_sortie;

/**
 * chercher_redirections - Analyse et traite les redirections dans une commande.
 * @mot: Le tableau de mots (arguments) d'une seule commande.
 *
 * Cette fonction parcourt les arguments d'une commande pour trouver les
 * operateurs de redirection (`<`, `>`, `>>`).
 * Quand elle en trouve un, elle configure les variables globales correspondantes
 * (mode_sortie, fichier_entree, fichier_sortie).
 * Crucialement, elle supprime ensuite les operateurs de redirection et les noms
 * de fichiers associes du tableau `mot`, le laissant "propre" pour un appel
 * a `execvp`.
 */
void chercher_redirections(char *mot[]);

#endif