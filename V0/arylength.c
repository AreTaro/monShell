#include <stdio.h>

int arrlen(char * array[])
{
  int taille = 0;

  while(array[taille] != NULL) {
      taille++;
    }

  return taille;

}
