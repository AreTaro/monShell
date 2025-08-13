/**
 * NOM: executer.h
 * ROLE: Declaration de la fonction principale d'execution des commandes.
 * AUTEURS: Alexandre Waechter, Gemini-CLI
 * VERSION: V0.1 du 13 Aout 2025
 * LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
 * USAGE: A inclure par le module principal (main.c) pour lancer l'execution
 *        d'une ligne de commande parsee.
 */

#ifndef EXECUTER_H
#define EXECUTER_H

#include "decouper.h" // Pour le type Commandes

/**
 * executer_pipeline - Execute une ou plusieurs commandes, potentiellement
 *                     reliees par des pipes et/ou en arriere-plan.
 * @cmds: Le tableau de commandes (pipeline) a executer.
 * @nb_cmds: Le nombre de commandes dans le tableau `cmds`.
 * @dirs: Le tableau des repertoires du PATH pour trouver les executables.
 * @arriere_plan: Un booleen (1 ou 0) indiquant si le pipeline doit etre
 *                execute en arriere-plan.
 *
 * Cette fonction est le coeur de l'execution. Elle gere la creation des
 * processus fils, la mise en place des pipes pour connecter les commandes
 * entre elles, et la gestion des processus en arriere-plan.
 */
void executer_pipeline(Commandes cmds,
                       int nb_cmds,
                       char **dirs,
                       int arriere_plan);

#endif