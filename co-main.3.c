/*
 * version 3.0.0 monShell chap6
 * modification de la condition if globale pour n'en faire qu'une
 * compilation : $ gcc -g -Wall co-main.3.c cp-moncd.c cn-decouper.c arylength.c
 */

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <assert.h>
# include <string.h> //strdup()
 # include <fcntl.h> // For open, if needed, though freopen is used

enum {
      maxLigne = 1024, // longueur max d'une ligne de commande
      maxMot = maxLigne/2, //nbre max de mots dans la ligne
      maxDirs = 100, // nbre max de repertoires dans PATH
      maxPathLength = 512, //longueur max d'un nom de fichier
};

void decouper (char *, char *, char **, int);
int moncd(int, char **);
// int rediriger(); // Removed
int arrlen(char * array[]);

# define PROMPT "? "

int main(int argc, char * argv[])
{
  char ligne[maxLigne];
  char pathName[maxPathLength];
  char * mot[maxMot];
  char * dirs[maxDirs];
  int i;
  const char * cd;
  const char * monexit;
  // const char * redirection; // Removed
  cd = "moncd";
  monexit = "monexit";
  // redirection = "rdg"; // Removed
  int j,k;

  /* decouper UNE COPIE de PATH en repertoires */
  decouper(strdup(getenv("PATH")), ":", dirs, maxDirs);

  /* Lire et traiter chaque ligne de commande */
  for(printf(PROMPT); fgets(ligne, sizeof ligne, stdin) != 0; printf(PROMPT))
    {
      // separe la ligne de commande selon le caractere pipe
      // et compte le nombre de commandes
      char *temp = strdup(ligne);
      decouper(temp, "|", mot, maxMot);

      // calcule le nombre de commande avec la fonction arrlen()
      // qui compte le nombre d'element dans un tableau
      int nbcmds = 0;
      nbcmds = arrlen(mot);

        int in_fd = 0;
        int fd[2], pex[2];
        //int maxSouscmds = 100;
        //char * souscmds[maxSouscmds];
        char * souscmds[maxMot];

        for (i = 0; i < nbcmds; i++) {
            // Crée un pipe entre la commande courante et la suivante
            pipe(fd);
            pipe(pex);

            pid_t pid = fork();

            //printf("i=%d\n", i);

            if (pid < 0)
              {
                perror("fork");
                continue;
              }

            if (pid != 0)
              {
                while(wait(0) != pid)
                  ;
                int codeSortie;
                close(pex[1]);
                read(pex[0], &codeSortie, sizeof(int));
                close(pex[0]);
                if (codeSortie == 42) {
                  exit(0);
                } else {
                  close(fd[1]); // Ferme l'extrémité d'écriture inutilisée dans le parent
                  in_fd = fd[0]; // Sauvegarde l'extrémité de lecture pour l'itération suivante
                  continue;
                }
              }

              // si ce n'est pas la premier commande
              // redirige le pipe vers l'entree standard
              if (in_fd != 0) {
                  dup2(in_fd, STDIN_FILENO);
                  close(in_fd);
              }
              // si ce n'est pas la derniere commande
              // redirige le pipe vers la sortie standard
              if (fd[1] != 1 && i < nbcmds - 1) {
                  dup2(fd[1], STDOUT_FILENO);
                  close(fd[1]);
              }

              // Exécutez la commande
              //char *pathName[maxPathLength];
              //printf("et moi alors bordel!\n");
              decouper(mot[i]," \t\n", souscmds, maxMot);

              // Initialization for redirection
              char *inputFile = NULL;
              char *outputFile = NULL;
              char *errorFile = NULL;
              int appendOutput = 0;
              int appendError = 0;
              int redirectErrorToOutput = 0;
              char *command_args[maxMot];
              int command_arg_count = 0;
              int l; // Loop index for souscmds

              // Scan souscmds for redirection and build command_args
              for (l = 0; souscmds[l] != NULL; l++) {
                if (strcmp(souscmds[l], ">") == 0) {
                  if (souscmds[l+1] != NULL) {
                    outputFile = souscmds[l+1];
                    appendOutput = 0;
                    l++; // Skip filename
                  } else {
                    perror("Syntax error: no output file after >");
                    // Decide how to handle error, e.g., break or set an error flag
                    // For now, let child process fail on exec if command_args[0] is NULL
                    command_args[0] = NULL; // Mark as invalid command
                    break;
                  }
                } else if (strcmp(souscmds[l], ">>") == 0) {
                  if (souscmds[l+1] != NULL) {
                    outputFile = souscmds[l+1];
                    appendOutput = 1;
                    l++; // Skip filename
                  } else {
                    perror("Syntax error: no output file after >>");
                    command_args[0] = NULL;
                    break;
                  }
                } else if (strcmp(souscmds[l], "<") == 0) {
                  if (souscmds[l+1] != NULL) {
                    inputFile = souscmds[l+1];
                    l++; // Skip filename
                  } else {
                    perror("Syntax error: no input file after <");
                    command_args[0] = NULL;
                    break;
                  }
                } else if (strcmp(souscmds[l], "2>") == 0) {
                  if (souscmds[l+1] != NULL) {
                    errorFile = souscmds[l+1];
                    appendError = 0;
                    redirectErrorToOutput = 0; // Explicitly set
                    l++; // Skip filename
                  } else {
                    perror("Syntax error: no error file after 2>");
                    command_args[0] = NULL;
                    break;
                  }
                } else if (strcmp(souscmds[l], "2>>") == 0) {
                  if (souscmds[l+1] != NULL) {
                    errorFile = souscmds[l+1];
                    appendError = 1;
                    redirectErrorToOutput = 0; // Explicitly set
                    l++; // Skip filename
                  } else {
                    perror("Syntax error: no error file after 2>>");
                    command_args[0] = NULL;
                    break;
                  }
                } else if (strcmp(souscmds[l], "2>&1") == 0) {
                  redirectErrorToOutput = 1;
                  // No filename expected after 2>&1, so l is not incremented further for a filename
                } else {
                  // Part of the command or its arguments
                  command_args[command_arg_count++] = souscmds[l];
                }
              }
              command_args[command_arg_count] = NULL; // Null-terminate command_args

              if (command_args[0] == 0) //ligne vide or syntax error in redirection
                continue;

              // Apply Redirection (in child process, before execv)
              if (inputFile) {
                FILE *fp_in = freopen(inputFile, "r", stdin);
                if (!fp_in) {
                  perror("freopen input");
                  exit(EXIT_FAILURE);
                }
              }
              if (outputFile) {
                FILE *fp_out = freopen(outputFile, appendOutput ? "a" : "w", stdout);
                if (!fp_out) {
                  perror("freopen output");
                  exit(EXIT_FAILURE);
                }
                if (redirectErrorToOutput) {
                  if (dup2(fileno(stdout), fileno(stderr)) == -1) {
                    perror("dup2 stderr to stdout");
                    exit(EXIT_FAILURE);
                  }
                }
              }
              if (errorFile && !redirectErrorToOutput) { // only if errorFile is specified AND not already redirected to stdout
                FILE *fp_err = freopen(errorFile, appendError ? "a" : "w", stderr);
                if (!fp_err) {
                  perror("freopen error");
                  exit(EXIT_FAILURE);
                }
              }

              if (strcmp(command_args[0],cd)==0)
                {
                  //lance moncd using command_arg_count
                  moncd(command_arg_count, command_args);
                  continue; // moncd is a shell built-in, should not be executed in the main command loop by execv
                           // and also should not exit the child process, but affect the parent.
                           // This placement after fork needs review for 'cd'.
                           // For now, assuming 'cd' like commands might lead to child exiting without exec.
                           // A real 'cd' should be handled before fork or communicate back to parent.
                           // Given the current structure, this 'cd' will only affect the child if it were to exec something.
                           // The prompt loop continues in the parent.
                           // Let's assume for this exercise, 'moncd' might print something or check args,
                           // then this child process will exit, and parent continues.
                           // To make 'moncd' effective for the main shell, it must be run in the parent process.
                           // The original code runs moncd in the child, which is unusual. Let's keep it that way for now.
                }

              // Old redirection block removed

              if (strcmp(command_args[0],monexit)==0) {
                int codeSortie = 42;
                close(pex[0]); // ferme l'extremite de lecture
                write(pex[1], &codeSortie, sizeof(int));
                close(pex[1]);
                exit(0);
              }

              for(int j = 0; dirs[j] != 0; j++) {
                //printf("je lance la commande mot[%d]: %s\n", i, mot[i]);
                //printf("command_args[0]: %s\n", command_args[0]);
                //if (command_args[1]) printf("command_args[1]: %s\n", command_args[1]);
                snprintf(pathName, sizeof pathName, "%s/%s", dirs[j], command_args[0]);
                execv(pathName, command_args);
              }
              // If execv returns, it's an error
              perror("execv failed");
              exit(EXIT_FAILURE); // Exit child if execv fails for all paths

            }
        // Assurez-vous que le dernier descripteur de fichier est fermé
        if (in_fd != 0)
            close(in_fd);
      }
  //}

  printf("Bye\n");
  return 0;

}

/*
 * BUG REPERTORIES:
 * $ ls -al | sor
 * $ rdg > aClasser.txt sort
 */
