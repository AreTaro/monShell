/*
 * cp-moncd.c
 * pour faire un chdir
 * Instruction lancement:
 */

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h> //fork & execl

int moncd(int ac, char * av[])
{
  char * dir;
  int t;
  char s[100];

  if (ac < 2)
    {
      dir = getenv("HOME");
      if (dir == 0)
        dir = "/tmp";
    }
  else if (ac > 2)
    {
      fprintf(stderr, "usage:%s [dir]\n", av[0]);
      return 1;
    }
  else {
    //printf("indicateur03\n");
    dir = av[1];
  }

  // faire le boulot
  t = chdir(dir);
  if (t < 0)
    {
      perror(dir);
      return 1;
    }

  //affiche le répertoire courant
  printf("%s\n", getcwd(s, 100));
  return 0;
}

/*
 * after compile with gcc -g -Wall cp-moncd.c
 * $ a.out ~/ied/l2
 /home/aretaro/ied/l2/sysEx/0
 ac 2
 /home/aretaro/ied/l2
 */

# ifdef TEST
int main(int ac, char * av[]) {
  char s[100];
  printf("%s\n", getcwd(s, 100));
  moncd(ac, av);
  return 0;
}
# endif
