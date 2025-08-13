/**
 * NOM: cmd_interne.h
 * ROLE: Declaration des fonctions pour la gestion des commandes internes.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: A inclure par les fichiers necessitant de verifier et d'executer
 *        une commande interne.
 */

#ifndef CMD_INTERNE_H
#define CMD_INTERNE_H

/**
 * executer_cmd_interne - Tente d'executer une commande interne.
 * @mots: Un tableau de chaines de caracteres representant la commande
 *        et ses arguments (ex: {"cd", "/tmp", NULL}).
 *
 * Cette fonction recherche si le premier mot de la commande correspond a une
 * commande interne connue (cd, exit, man). Si une correspondance est

 * trouvee, la fonction associee est appelee.
 *
 * Retourne:
 *  - 1 si la commande etait une commande interne et a ete executee.
 *  - 0 si aucune commande interne ne correspondait.
 */
int executer_cmd_interne(char **mots);

#endif