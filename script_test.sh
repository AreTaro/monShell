#!/bin/bash

# Script de test pour monShell

# Couleurs pour les messages
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Compilation de monShell...${NC}"
make
if [ $? -ne 0 ]; then
    echo -e "${RED}La compilation a échoué. Arrêt des tests.${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation réussie.${NC}"

# Fonction pour afficher les en-têtes de section
section_header() {
    echo -e "\n${YELLOW}--- $1 ---${NC}"
}

# Fonction pour exécuter et afficher une commande de test
run_test() {
    local description="$1"
    local command="$2"
    echo -e "\n> Test: $description"
    echo "  Commande: ./monShell <<< \"$command\""
    echo "  --- Sortie de monShell ---"
    # Exécute la commande dans un sous-shell pour que 'exit' ne quitte pas le script
    (./monShell <<< "$command")
    echo "  --------------------------"
    # Commandes de vérification supplémentaires si nécessaire
    if [ ! -z "$3" ]; then
        echo "  --- Vérification ---"
        eval "$3"
        echo "  --------------------"
    fi
}

# Nettoyage avant de commencer
rm -f test_*.txt

section_header "Test de la commande 'cd'"
run_test "cd vers un répertoire valide (/tmp)" "cd /tmp && pwd"
run_test "cd vers le répertoire HOME" "cd && pwd"
run_test "Erreur: cd vers un répertoire inexistant" "cd /non_existent_dir"
run_test "Erreur: cd avec trop d'arguments" "cd /tmp /etc"

section_header "Test de la commande 'exit'"
echo -e "\n> Test: exit simple (devrait quitter le shell avec le code 0)"
echo "  Commande: ./monShell <<< \"exit\""
(./monShell <<< "exit")
echo "  Code de sortie: $? (devrait être 0)"

echo -e "\n> Test: exit avec un code de retour (42)"
echo "  Commande: ./monShell <<< \"exit 42\""
(./monShell <<< "exit 42")
echo "  Code de sortie: $? (devrait être 42)"

run_test "Erreur: exit avec un argument non-numérique" "exit abc"
run_test "Erreur: exit avec trop d'arguments" "exit 42 42"

section_header "Test de la commande 'man'"
run_test "Affichage du manuel pour 'cd'" "man cd"
run_test "Affichage du manuel pour 'exit'" "man exit"
run_test "Affichage du manuel pour 'monShell'" "man monShell"
run_test "Erreur: manuel pour une commande inconnue" "man commande_inconnue"

section_header "Test de la redirection de sortie '>'"
run_test "Redirection de 'ls' vers un fichier" "ls > test_ls.txt" "cat test_ls.txt"
run_test "Redirection de 'echo' vers un fichier" "echo 'hello world' > test_echo.txt" "cat test_echo.txt"
run_test "Écrasement d'un fichier avec '>'" "echo 'contenu écrasé' > test_echo.txt" "cat test_echo.txt"
run_test "Erreur: redirection vers un répertoire inexistant" "ls > non_existent_dir/file.txt"

section_header "Test de la redirection de sortie en ajout '>>'"
echo "ligne initiale" > test_append.txt
run_test "Ajout à un fichier avec '>>'" "echo 'ligne ajoutée' >> test_append.txt" "cat test_append.txt"
run_test "Ajout à un fichier qui n'existe pas" "echo 'nouvelle ligne' >> test_new_append.txt" "cat test_new_append.txt"
run_test "Erreur: ajout vers un répertoire inexistant" "ls >> non_existent_dir/file.txt"

section_header "Test de la redirection d'entrée '<'"
echo "ligne 1 pour le test" > test_input.txt
echo "ligne 2" >> test_input.txt
run_test "Lecture depuis un fichier avec '<'" "wc -l < test_input.txt"
run_test "Utilisation de 'sort' avec '<'" "sort < test_input.txt"
run_test "Erreur: lecture depuis un fichier inexistant" "cat < non_existent_file.txt"

section_header "Test des pipes '|'"
run_test "Pipe simple: ls | wc -l" "ls -F | wc -l"
run_test "Pipe avec grep" "ls | grep main"
run_test "Pipe multiple: echo | grep | wc -c" "echo 'un test avec des pipes' | grep 'pipes' | wc -c"
run_test "Erreur: commande invalide dans un pipe" "ls | commande_invalide"

section_header "Test de l'auto-complétion (manuel)"
echo -e "\nL'auto-complétion est une fonctionnalité interactive et doit être testée manuellement."
echo "Pour cela, lancez './monShell' et essayez les commandes suivantes :"
echo "1. Tapez 'ma' puis appuyez sur la touche <TAB>. Le shell devrait compléter ou proposer des options (ex: 'man/', 'main.c')."
echo "2. Tapez 'ls journal/2025080' puis <TAB> pour voir les complétions de fichiers."
echo "3. Tapez 'cat README' puis <TAB>."

section_header "Nettoyage"
echo "Suppression des fichiers de test..."
rm -f test_*.txt

echo -e "\n${GREEN}Script de test terminé.${NC}"
echo "Veuillez inspecter les résultats ci-dessus pour vérifier le bon fonctionnement de monShell."
