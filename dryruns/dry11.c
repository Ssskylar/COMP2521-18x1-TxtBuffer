#include "textbuffer.h" 

#include <stdio.h> 
#include <stdlib.h>


int main(int argc, char** argv) { 
  
  char str1[] = "1 line 01\n"
                "1 line 02\n"
                "1 line 03\n"
                "1 line 04\n"
                "1 line 05\n"; 
      
  char str2[] = "2 line 01\n"
                "2 line 02\n"
                "2 line 03\n"
                "2 line 04\n";
   
 
  char* s ;  
    
  TB tb1 = newTB(str1) ; 
  TB tb2 = newTB(str2) ; 
 
  // swap undo & redo
  printf("test 1 : swap undo & redo\n") ; 
  swapTB(tb1,0,2) ;
  swapTB(tb1,1,3) ;
  swapTB(tb1,2,4) ;   
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
   
  printf("\nafter undo 3 times...\n") ;
  undoTB(tb1) ; 
  undoTB(tb1) ; 
  undoTB(tb1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  
  printf("\nafter redo 2 times...\n") ; 
  redoTB(tb1) ; 
  redoTB(tb1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  printf("\n\n") ; 
  
  // delete
  releaseTB(tb1) ; 
  tb1 = newTB(str1) ; 
  printf("test 2 : delete undo & redo\n") ; 
  deleteTB(tb1,3,4) ; 
  deleteTB(tb1,0,1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ;
  
  printf("\nafter undo 1 time...\n") ;
  undoTB(tb1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ;
  
  printf("\nafter undo 1 time...\n") ;
  undoTB(tb1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ;
  printf("\n\n") ;
    
  
  // paste & merge undo 
  releaseTB(tb1) ; 
  tb1 = newTB(str1) ; 
  printf("test 3 : paste / merge undo & redo\n") ; 
  pasteTB(tb1,3,tb2) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s)  ;
  free(s) ; 
   
  printf("\nafter undo ...\n") ; 
  undoTB(tb1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  
  printf("\nafter merge ...\n") ; 
  mergeTB(tb1,linesTB(tb1),tb2) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  
  printf("\nafter redo ...\n") ; 
  redoTB(tb1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  
  undoTB(tb1) ; 
  printf("\nafter undo ...\n") ; 
  s = dumpTB(tb1) ;
  printf("%s",s) ; 
  free(s) ; 
      
  
  // cut undo
  releaseTB(tb1) ; 
  tb1 = newTB(str1) ; 
  
  printf("test 5 : cut undo & redo\n") ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  
  cutTB(tb1,3,4) ; 
  cutTB(tb1,0,1) ; 
  printf("\nafter 2 cuts ...\n") ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  
  printf("\nafter undo 2 times...\n") ; 
  undoTB(tb1) ; 
  undoTB(tb1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  
  printf("\nafter redo 1 time...\n") ; 
  redoTB(tb1) ; 
  s = dumpTB(tb1) ; 
  printf("%s",s) ; 
  free(s) ; 
  
  return EXIT_SUCCESS ;   
}
