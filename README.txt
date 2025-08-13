Ce dépôt est le projet final du cours sur les systèmes d'exploitation
par Philippe Kislin-Duval en deuxième année de licence informatique
de l'IED à l'université Paris 8.

This repository is le final project of the course on operation systems
by Philippe Kislin-Duval in the second years of computer science bachelor at
IED Paris University 8.

Exigences :

Modifier le shell élémentaire en intégrant les modificatiosn suivantes :

- les commandes cd et exit au mini-shell
- les procédures en arrière plan
- les redirections
- les pipes
- l'auto-complétion des noms de fichier
- l'accès à la documentation en utilisant MAN
- une makefile complète permettant de construire l'exécutable


---

COMPILATIONN

Pour compiler le projet, assurez-vous d'avoir la bibliothèque
`readline` installée. Sur les systèmes basés sur Debian (comme
Ubuntu), vous pouvez l'installer avec : `sudo apt-get install
libreadline-dev`

Ensuite, compilez le programme avec la commande suivante, qui lie
tous les modules nécessaires : 

`gcc -o monShell main.c cmd_interne.c executer.c decouper.c
redirection.c -lreadline`

