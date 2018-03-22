#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

int main(int argc, char *argv[]) {
    TB tb1 = newTB("1.1 abc\n1.2 def\n");

    printf("Created Tb \n");
     
    releaseTB (tb1);
    printf("Released Tb \n");

    return EXIT_SUCCESS;
}
