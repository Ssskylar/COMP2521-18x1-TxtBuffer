#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

int main(int argc, char *argv[]) {
    TB tb1 = newTB("1.1 abc\n1.2 def\nxxxxx\n");
    TB tb2 = newTB("new stuff\nblah blah blah\nqwer\n");
    char* str = dumpTB(tb1) ;     
  
    printf("Original Tb:\n%s\n",str) ; 
    
    free(str) ; 
   
    pasteTB(tb1,1,tb2);        
    str = dumpTB(tb1) ; 
    char* str2 = dumpTB(tb2) ;    
    printf("Pasted tb1...\n%s\n",str) ; 
    printf("Original tb2 \n%s\n",str2);
    free(str) ;     
    free(str2);

    releaseTB(tb2);
    releaseTB (tb1);
    printf("Released Tb \n");

    return EXIT_SUCCESS;
}
