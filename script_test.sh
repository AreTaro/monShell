#!/bin/bash

# NOM: script_test.sh
# ROLE: Script de test automatisant la verification des fonctionnalites du programme monShell.
# AUTEURS: Alexandre Waechter, Gemini-CLI
# VERSION: V0.1 du 13 Aout 2025
# LICENCE: Realise dans le cadre du cours de Systeme d'Exploitation pour l'IED8
# USAGE: Lancer ce script depuis le repertoire principal du projet. Il executera une
#        serie de commandes dans monShell et affichera les resultats. Il est necessaire
#        d'inspecter visuellement les sorties pour confirmer le bon comportement.

# -- Section de configuration --
# Definition de codes de couleur pour rendre la sortie du script plus lisible.
# Cela aide a rapidement identifier les succes (vert), les echecs (rouge)
# et les informations generales (jaune).
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color - Code pour reinitialiser la couleur du texte.

# -- Etape 1: Compilation du programme --
# Avant de tester, nous devons nous assurer que la derniere version du code
# est compilee. La commande 'make' s'en occupe en se basant sur le Makefile.
echo -e "${YELLOW}Compilation de monShell...${NC}"
make
# On verifie le code de retour de la commande 'make'.
# Si ce code est different de 0, cela signifie qu'une erreur s'est produite.
if [ $? -ne 0 ]; then
    echo -e "${RED}La compilation a echoue. Arret des tests.${NC}"
    exit 1 # On quitte le script car les tests ne peuvent pas etre executes.
fi
echo -e "${GREEN}Compilation reussie.${NC}"

# -- Section des fonctions utilitaires --

# Fonction: section_header
# Role: Affiche un titre de section pour organiser la sortie des tests.
# Argument 1: La chaine de caracteres a afficher comme titre.
section_header() {
    echo -e "\n${YELLOW}--- $1 ---${NC}"
}

# Fonction: run_test
# Role: Execute une commande de test dans monShell et affiche le resultat.
#       C'est la fonction centrale de ce script de test.
# Argument 1: Une chaine decrivant le but du test.
# Argument 2: La commande a envoyer a monShell.
# Argument 3 (Optionnel): Une commande shell a executer pour verifier le resultat
#                         (par exemple, 'cat' pour afficher le contenu d'un fichier).
run_test() {
    local description="$1"
    local command="$2"
    echo -e "\n> Test: $description"
    echo "  Commande: ./monShell <<< \"$command\""
    echo "  --- Sortie de monShell ---"
    # La commande est executee dans un sous-shell.
    # L'operateur '<<<' (here string) envoie la chaine de commande a l'entree standard de monShell.
    # Le sous-shell (parentheses) est crucial pour que la commande 'exit' a l'interieur
    # de monShell ne quitte pas ce script de test.
    (./monShell <<< "$command")
    echo "  --------------------------"
    # Si un troisieme argument est fourni, on l'evalue.
    # Ceci est utilise pour les tests de redirection, pour verifier le contenu
    # des fichiers crees.
    if [ ! -z "$3" ]; then
        echo "  --- Verification ---"
        eval "$3"
        echo "  --------------------"
    fi
}

# -- Etape 2: Initialisation de l'environnement de test --
# On supprime les fichiers de test des executions precedentes pour garantir
# que les tests sont realises dans un environnement propre.
section_header "Initialisation et Nettoyage"
rm -f test_*.txt
echo "Anciens fichiers de test supprimes."

# -- Etape 3: Execution des tests fonctionnels --

section_header "Test de la commande interne 'cd'"
run_test "cd vers un repertoire valide (/tmp)" "cd /tmp && pwd"
run_test "cd vers le repertoire HOME de l'utilisateur" "cd && pwd"
run_test "Erreur: cd vers un repertoire qui n'existe pas" "cd /non_existent_dir"
run_test "Erreur: cd avec trop d'arguments" "cd /tmp /etc"

section_header "Test de la commande interne 'exit'"
echo -e "\n> Test: exit simple (devrait quitter le shell avec le code 0)"
echo "  Commande: ./monShell <<< \"exit\""
(./monShell <<< "exit")
# On verifie le code de sortie du sous-shell. Il doit etre 0.
echo "  Code de sortie: $? (devrait etre 0)"

echo -e "\n> Test: exit avec un code de retour (42)"
echo "  Commande: ./monShell <<< \"exit 42\""
(./monShell <<< "exit 42")
# Le code de sortie doit correspondre a la valeur donnee a exit.
echo "  Code de sortie: $? (devrait etre 42)"

run_test "Erreur: exit avec un argument non-numerique" "exit abc"
run_test "Erreur: exit avec trop d'arguments" "exit 42 42"

section_header "Test de la commande interne 'man'"
run_test "Affichage du manuel pour 'cd'" "man cd"
run_test "Affichage du manuel pour 'exit'" "man exit"
run_test "Affichage du manuel pour 'monShell'" "man monShell"
run_test "Erreur: manuel pour une commande inconnue" "man commande_inconnue"

section_header "Test de la redirection de sortie '>' (ecrasement)"
run_test "Redirection de 'ls' vers un fichier" "ls > test_ls.txt" "cat test_ls.txt"
run_test "Redirection de 'echo' vers un fichier" "echo 'hello world' > test_echo.txt" "cat test_echo.txt"
run_test "Ecrasement d'un fichier existant avec '>'" "echo 'contenu ecrasé' > test_echo.txt" "cat test_echo.txt"
run_test "Erreur: redirection vers un repertoire inexistant" "ls > non_existent_dir/file.txt"

section_header "Test de la redirection de sortie en ajout '>>'"
# On cree un fichier de base pour le test d'ajout.
echo "ligne initiale" > test_append.txt
run_test "Ajout a un fichier existant avec '>>'" "echo 'ligne ajoutee' >> test_append.txt" "cat test_append.txt"
run_test "Creation d'un fichier avec '>>' s'il n'existe pas" "echo 'nouvelle ligne' >> test_new_append.txt" "cat test_new_append.txt"
run_test "Erreur: ajout vers un repertoire inexistant" "ls >> non_existent_dir/file.txt"

section_header "Test de la redirection d'entree '<'"
# On cree un fichier qui servira d'entree pour les commandes.
echo "ligne 1 pour le test" > test_input.txt
echo "ligne 2" >> test_input.txt
run_test "Lecture depuis un fichier avec 'wc -l' et '<'" "wc -l < test_input.txt"
run_test "Utilisation de 'sort' avec la redirection d'entree '<'" "sort < test_input.txt"
run_test "Erreur: lecture depuis un fichier qui n'existe pas" "cat < non_existent_file.txt"

section_header "Test des pipes '|' (tunnels)"
run_test "Pipe simple: ls | wc -l" "ls -F | wc -l"
run_test "Pipe avec un filtre 'grep'" "ls | grep main"
run_test "Pipe multiple: echo | grep | wc -c" "echo 'un test avec des pipes' | grep 'pipes' | wc -c"
run_test "Erreur: commande invalide dans un pipe" "ls | commande_invalide"

section_header "Test de l'auto-completion (a faire manuellement)"
echo -e "\nL'auto-completion est une fonctionnalite interactive."
echo "Elle doit etre verifiee manuellement car un script ne peut pas simuler l'appui sur la touche <TAB>."
echo "Pour cela, lancez './monShell' et essayez les commandes suivantes :"
echo "1. Tapez 'ma' puis appuyez sur <TAB>. Le shell devrait completer ou proposer des options (ex: 'man/', 'main.c')."
echo "2. Tapez 'ls journal/2025080' puis <TAB> pour voir les completions de fichiers."
echo "3. Tapez 'cat README' puis <TAB>."

# -- Etape 4: Nettoyage final --
# La proprete est importante. On supprime les fichiers generes par les tests
# pour ne pas polluer le repertoire de travail.
section_header "Nettoyage final"
echo "Suppression des fichiers de test..."
rm -f test_*.txt

echo -e "\n${GREEN}Script de test termine.${NC}"
echo "Veuillez inspecter les resultats ci-dessus pour verifier le bon fonctionnement de monShell."