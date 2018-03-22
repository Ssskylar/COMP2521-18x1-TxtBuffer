#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

int main(int argc, char *argv[]) {
    TB tb1 = newTB("1.1 abc\n1.2 def\n");
    char* str = dumpTB(tb1) ;     
    
    printf("Original Tb:\n%s\n",str) ; 
    free(str) ; 
        
    deleteTB(tb1,1,0) ; 
    str = dumpTB(tb1) ; 
    printf("Delete from 1 to 0...\n%s\n",str) ; 
    free(str) ;     
         
    deleteTB(tb1,0,1);
    str = dumpTB(tb1) ; 
    printf("Everything deleted...\n%s\n",str) ; 
    free(str) ;  

    releaseTB (tb1);
    printf("Released Tb \n");

    return EXIT_SUCCESS;
}