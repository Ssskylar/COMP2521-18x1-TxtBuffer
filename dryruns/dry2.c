#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

int main(int argc, char *argv[]) {
    TB tb1 = newTB("1.1 abc\n1.2 def\n");
    char* str = dumpTB(tb1) ;     
    
    printf("Dumped Tb containing %d lines...\n%s\n",linesTB(tb1),str);
     
    free(str) ;     
    releaseTB (tb1);
    printf("Released Tb \n");

    return EXIT_SUCCESS;
}
