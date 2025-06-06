/*
 * NOM: rdg
 * VERSION: 3.2.0
 * USAGE: rdg [CHEVRON] [FICHIER] [PROCESSUS]
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int rediriger(int k, char * lc [])
{

  int status;

  if (k < 4)
    {
      printf("USAGE: rdg [CHEVRON] [FILE] [PROCESSUS]");
      return 1;
    }

  // lancement du processus enfant
  pid_t tmp = fork();

  if (tmp < 0) // en cas d'erreur
    {
      perror("fork");
    }

  if (tmp != 0)
    {
      //attendre que le processus enfant s'execute
      //recuperer le status
      waitpid(tmp, &status, 0);
      if(WIFEXITED(status))
        {
          int exit_status = WEXITSTATUS(status);
          // si executer correctement
          if (exit_status == 0) return 0;
          //si pas executer correctement
          else return 1;
        }
    }

  // une fois execute, lancer le second processus
  if (tmp == 0)
    {
      FILE *fp;
      if (strcmp(lc[1],">") == 0) {
        fp = freopen(lc[2], "w+", stdout);
        execvp(lc[3], lc + 3);
        perror("excep");
        fclose(fp);
      } else if (strcmp(lc[1],">>") == 0) {
        fp = freopen(lc[2], "a", stdout);
        execvp(lc[3], lc + 3);
        perror("excep");
        fclose(fp);
      } else if (strcmp(lc[1],"2>&1") == 0) {
        fp = freopen(lc[2], "w", stdout);
        dup2(fileno(stdout), fileno(stderr));
        //printf("sdtout: lc[2] = %s\n", lc[2]);
        //fprintf(stderr, "stderr lc[3]: %s \n", lc[3]);
        fprintf(stderr, "Je ne suis pas une erreur!\n");
        execvp(lc[3], lc + 3);
        perror("excep");
        fclose(fp);
      } else if (strcmp(lc[1],"2>") == 0) {
        fp = freopen(lc[2], "w+", stderr);
        execvp(lc[3], lc + 3);
        perror("excep");
        fclose(fp);
      } else if (strcmp(lc[1],"2>>") == 0) {
        fp = freopen(lc[2], "a", stderr);
        execvp(lc[3], lc + 3);
        perror("excep");
        fclose(fp);
      } else {
        printf("USAGE: rdg [CHEVRON] [FILE] [PROCESSUS]");
        return 1;
      }
    }
  return 0;
}

/* info sur wait() trouvé sur :
 * https://www.geeksforgeeks.org/wait-system-call-c/
 * et sur https://www.geeksforgeeks.org/exit-status-child-process-linux/
 */
