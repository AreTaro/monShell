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

## Suggested Manual Test Cases for Redirection

The following test cases can be used to manually verify the implemented redirection functionalities.
Ensure `monShell` is compiled before running these tests.
(Note: `cat` and `wc` are standard Unix commands. If your shell has internal versions, behavior might differ.)

1.  **Output Redirection (`>`):**
    *   Command: `ls > list_files.txt`
    *   Expected: Creates `list_files.txt` with the output of `ls`. Overwrites if file exists.

2.  **Append Output Redirection (`>>`):**
    *   Command: `echo "append test" >> list_files.txt`
    *   Expected: Appends "append test" to `list_files.txt`. Creates file if it doesn't exist.

3.  **Input Redirection (`<`):**
    *   Command: `cat < list_files.txt`
    *   Expected: Displays the content of `list_files.txt`. (Assumes `cat` or a similar command is executable).
    *   Alternatively, if `moncd` could take commands from a file: `moncd < some_commands.txt`

4.  **Error Redirection (`2>`):**
    *   Command: `ls /nonexistent_dir 2> error_log.txt`
    *   Expected: Creates `error_log.txt` with the error message from `ls` (e.g., "ls: cannot access '/nonexistent_dir': No such file or directory"). Overwrites if file exists. Standard output (if any) goes to the terminal.

5.  **Append Error Redirection (`2>>`):**
    *   Command: `ls /another_nonexistent 2>> error_log.txt`
    *   Expected: Appends the error message from `ls` to `error_log.txt`. Creates file if it doesn't exist.

6.  **Redirect stderr to stdout (`2>&1`):**
    *   Command: `ls -l /nonexistent_dir > out_and_err.txt 2>&1`
    *   Expected: Creates `out_and_err.txt` containing both standard output (if any, though likely none for a non-existent dir with -l) and standard error from `ls`.
    *   Test with a command that produces both: `echo "Good output" > out_and_err.txt ; ls /nonexistent_dir >> out_and_err.txt 2>&1` (This is a sequence for setup, then test). A direct command like `find / -name "hopefully_nonexistent_file_pattern" -print > out_and_err.txt 2>&1` might also work.

7.  **Pipeline with Output Redirection:**
    *   Command: `echo "test pipeline" | cat > pipeline_out.txt`
    *   Expected: Creates `pipeline_out.txt` with the content "test pipeline".

8.  **Pipeline with Input and Output Redirection:**
    *   Command: `cat < list_files.txt | wc -l > line_count.txt`
    *   Expected: Creates `line_count.txt` with the number of lines in `list_files.txt`.
