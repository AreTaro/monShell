#
# NOM: Makefile
# ROLE: Definit les regles de compilation pour le projet monShell.
# AUTEURS: Alexandre Waechter, Gemini-CLI
# VERSION: V0.1 du 13 Aout 2025
# LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
# USAGE: Utiliser les commandes suivantes depuis le terminal:
#        - `make` ou `make all`: Pour compiler le projet.
#        - `make clean`: Pour supprimer les fichiers objets temporaires.
#        - `make fclean`: Pour supprimer tous les fichiers generes.
#        - `make re`: Pour recompiler completement le projet.
#

# === VARIABLES DE COMPILATION ===

# Le compilateur a utiliser. gcc est le standard pour C sur Linux.
CC = gcc

# Les options (flags) pour le compilateur.
# -g: Inclut les informations de debogage dans l'executable.
# -Wall -Wextra: Activent presque tous les avertissements du compilateur. C'est une bonne pratique.
# -I.: Ajoute le repertoire courant au chemin de recherche des headers (.h)
CFLAGS = -g -Wall -Wextra -I.

# Les bibliotheques externes a lier au projet.
# -lreadline: Pour utiliser la librairie readline (meilleure interface).
# -lncurses: Une dependance de readline.
LDLIBS = -lreadline

# Le nom de l'executable final que nous voulons produire.
EXEC = monShell

# La liste de tous les fichiers sources (.c) du projet.
# make va automatiquement en deduire les fichiers objets (.o) necessaires.
SRCS = main.c cmd_interne.c decouper.c executer.c redirection.c

# Genere automatiquement la liste des fichiers objets (.o) a partir des sources (.c).
# Remplace l'extension .c par .o pour chaque fichier dans la liste SRCS.
OBJS = $(SRCS:.c=.o)


# === REGLES DE COMPILATION ===

# Le mot-cle .PHONY indique a make que `all` n'est pas un vrai fichier.
# C'est la regle par defaut, executee quand on tape `make` sans argument.
.PHONY: all
all: $(EXEC)

# Regle de liaison: pour creer l'executable final.
# Cette regle depend de tous les fichiers objets (.o).
# Elle ne s'execute que si un des fichiers .o a ete modifie.
$(EXEC): $(OBJS)
	@echo "[LIAISON] Creation de l'executable $(EXEC)..."
	$(CC) $(OBJS) -o $(EXEC) $(LDLIBS)
	@echo "[SUCCES] L'executable $(EXEC) est pret."

# Regle de patron (pattern rule): pour compiler un fichier .c en .o
# Cette regle explique a make comment transformer n'importe quel fichier .c en .o.
# $< est une variable automatique qui represente le nom du fichier source (.c).
# $@ est une variable automatique qui represente le nom de la cible (le fichier .o).
%.o: %.c
	@echo "[COMPILATION] $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@


# === REGLES DE NETTOYAGE ===

# La regle `clean` supprime les fichiers generes par la compilation.
.PHONY: clean
clean:
	@echo "[NETTOYAGE] Suppression des fichiers objets..."
	rm -f $(OBJS)

# La regle `fclean` (force clean) fait comme `clean` et supprime aussi l'executable.
.PHONY: fclean
fclean: clean
	@echo "[NETTOYAGE] Suppression de l'executable $(EXEC)..."
	rm -f $(EXEC)

# La regle `re` (recompiler) nettoie tout et recompile le projet.
# C'est un raccourci pratique pour `make fclean` suivi de `make all`.
.PHONY: re
re: fclean all
