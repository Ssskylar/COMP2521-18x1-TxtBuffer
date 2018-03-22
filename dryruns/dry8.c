#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

int main(int argc, char *argv[]) {
    TB tb1 = newTB("1.1 abc\ndef\nghc\nklmnbc\n");
    char* str = dumpTB(tb1) ;     
    printf("Original Tb:\n%s\n",str) ; 
    free(str) ; 
    replaceText (tb1, "bc", "YY");
    str = dumpTB(tb1);
    printf("replaced  bc   with YY...\n%s\n",str) ; 
    free(str) ;     
         
    releaseTB (tb1);
    printf("Released Tb \n");

    return EXIT_SUCCESS;
}
