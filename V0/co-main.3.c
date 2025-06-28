/*
 * version 3.0.0 monShell chap6
 * modification de la condition if globale pour n'en faire qu'une
 * compilation : $ gcc -g -Wall co-main.3.c cp-moncd.c rdg.3.2.0.c cn-decouper.c arylength.c
 */

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <assert.h>
# include <string.h> //strdup()

enum {
      maxLigne = 1024, // longueur max d'une ligne de commande
      maxMot = maxLigne/2, //nbre max de mots dans la ligne
      maxDirs = 100, // nbre max de repertoires dans PATH
      maxPathLength = 512, //longueur max d'un nom de fichier
};

void decouper (char *, char *, char **, int);
int moncd(int, char **);
int rediriger();
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
  const char * redirection;
  cd = "moncd";
  monexit = "monexit";
  redirection = "rdg";
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

              if (souscmds[0] == 0) //ligne vide
                continue;

              if (strcmp(souscmds[0],cd)==0)
                {
                  // calcule le nombre d'elements passe sur la ligne de commande
                  k = 0;
                  for (j = 0; souscmds[j] != NULL;j++) k +=1 ;

                  //lance moncd
                  moncd(k, souscmds);
                  continue;
                }

              if (strcmp(souscmds[0],redirection)==0)
                {
                  // calcule le nombre d'elements passe sur la ligne de commande
                  k = 0;
                  for (j = 0; souscmds[j] != NULL;j++) k +=1 ;

                  //lance moncd
                  rediriger(k,souscmds);
                  continue;
                }

              if (strcmp(souscmds[0],monexit)==0) {
                int codeSortie = 42;
                close(pex[0]); // ferme l'extremite de lecture
                write(pex[1], &codeSortie, sizeof(int));
                close(pex[1]);
                exit(0);
              }

              for(int j = 0; dirs[j] != 0; j++) {
                //printf("je lance la commande mot[%d]: %s\n", i, mot[i]);
                //printf("souscmds: %s\n", *souscmds);
                //printf("souscmds[1]: %s\n", souscmds[1]);
                snprintf(pathName, sizeof pathName, "%s/%s", dirs[j], souscmds[0]);
                execv(pathName, souscmds);
              }

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
