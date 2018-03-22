#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

int main(int argc, char *argv[]) {
    TB tb1 = newTB("1.1 abc\n1.2 def\nxxxxx\n");
    char* str = dumpTB(tb1) ;     
    
    printf("Original Tb:\n%s\n",str) ; 
    free(str) ; 
        
    deleteTB(tb1,0,1) ; 
    str = dumpTB(tb1) ; 
    printf("First and second line are deleted...\n%s\n",str) ; 
    free(str) ;     
         
    releaseTB (tb1);
    printf("Released Tb \n");

    return EXIT_SUCCESS;
}
