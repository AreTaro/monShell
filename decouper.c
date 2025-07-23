#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void decouper(char * ligne, char * sep, char * mot[], int max) {
    int i;

    mot[0] = strtok(ligne, sep);
    for (i = 1; mot[i-1] != 0; i++) {
        if (i == max-1) {
            mot[i] = 0;
            fprintf(stderr, "trop de mots\n");
            return;
        }
        mot[i] = strtok(NULL, sep);
    }
}