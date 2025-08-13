/**
 * NOM: decouper.h
 * ROLE: Declaration de la fonction de decoupage de la ligne de commande.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: A inclure par les modules qui ont besoin de parser une chaine de
 *        caracteres en structure de commandes.
 */

#ifndef DECOUPER_H
#define DECOUPER_H

// Nombre maximal de commandes pouvant etre enchainees par des pipes.
#define MAX_CMDS 10
// Nombre maximal de mots pour une seule commande.
#define MAX_MOTS 50

/**
 * typedef Commandes - Definit un type pour stocker un pipeline de commandes.
 * C'est un tableau a deux dimensions de chaines de caracteres.
 * - La premiere dimension indexe les commandes dans le pipeline (jusqu'a MAX_CMDS).
 * - La deuxieme dimension indexe les mots de chaque commande (jusqu'a MAX_MOTS).
 * Exemple: cmds[0] est la premiere commande, cmds[0][0] est le premier mot de cette commande.
 */
typedef char* Commandes[MAX_CMDS][MAX_MOTS];

/**
 * decouper - Analyse et decoupe une ligne de commande en un pipeline de commandes.
 * @ligne: La chaine de caracteres brute entree par l'utilisateur.
 * @cmds: Le tableau de type Commandes qui sera rempli par la fonction.
 * @arriere_plan: Un pointeur vers un entier. La fonction mettra 1 a cette adresse
 *                si la commande doit s'executer en arriere-plan (&), 0 sinon.
 *
 * La fonction decoupe d'abord la ligne en fonction du caractere pipe '|'.
 * Ensuite, chaque segment de commande est decoupe en mots (arguments) en
 * fonction des espaces et tabulations.
 * Elle detecte aussi le caractere '&' a la fin de la ligne.
 *
 * Retourne:
 *  - Le nombre de commandes simples trouvees dans le pipeline.
 *  - -1 en cas d'erreur (trop de commandes).
 */
int decouper(char * ligne, Commandes cmds, int *arriere_plan);

#endif