#ifndef COMMAND_H
#define COMMAND_H

// Nombre max de commandes pouvant etre enchainees par des pipes
#define MAX_COMMANDES 10

// Nombre max de mots pour une seule commande.
#define MAX_MOTS 64

// Structure des donnees
typedef struct {
        char* argument[MAX_MOTS];

        // Redirections laisses NULL si non utilise
        char* redir_int;
        char* redir_out;

        // Mode de redirection de sortie
        int append_out; // Mis a 1 se la redirection est en mode ajout (>>), 0 sinon,
}

#eniif
