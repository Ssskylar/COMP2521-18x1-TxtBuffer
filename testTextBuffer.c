#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "textbuffer.h"
#include <assert.h>
 
#ifndef GREEN 
#define GREEN   "\x1B[32m" 
#define RED   "\x1B[31m" 
#define RESET "\x1B[0m"
#define success printf("%sSuccess\n%s", GREEN, RESET);
#endif

static void check(char* msg) {
    printf("%s: ", msg);
    success
}

/**
 * Takes a TB @tb, and creates a copy with the 
 * same lines, texts and order of original @tb

 marmaduke
 */
static TB duplicateTB(TB tb) {
    char* dump = dumpTB(tb);
    TB new = newTB(dump);
    free(dump);
    return new;
}

//Used in the testing functions
static int isEqual(TB a, TB b) {
    char* ac = dumpTB(a);
    char* bc = dumpTB(b);

    int ret = !strcmp(ac, bc);
    free(ac);
    free(bc);

    return ret;
}

//TB newTB (char text[]);
void test_newTB() {
    TB null = newTB(NULL);
    assert(null == NULL);
    check("Test newTB(NULL) == NULL" );

    TB empty = newTB("");
    assert(linesTB(empty) == 0);
    check("Test newTB(\"\")  lines == 0");
    releaseTB(empty); //Run with valgrind 


    TB new = newTB("Testing 123\n\nLol How are you?\n");    
    assert(linesTB(new) == 3);
    check("Test newTB(..\\n\\n..\\n)  lines == 3");

    releaseTB(new); //Run with valgrind 
    printf("Test releaseTB " RED "Use valgrind to ensure no memory leaks!!!\n" RESET); //make sure we still alive
}


void test_dumpTB() {
    char* null = dumpTB(NULL);
    assert(null == NULL);
    check("Test dumpTB(NULL) == NULL");

    TB emptyTB = newTB("");
    char* empty = dumpTB(emptyTB);
    assert(strcmp(empty, "") == 0);
    check("Test dumpTB(emptyBuffer) == \"\"");
    free(empty);
    releaseTB(emptyTB);


    TB oneLineTB = newTB("Hello How Are You?\n");
    char* oneLine = dumpTB(oneLineTB);
    assert(strcmp(oneLine, "Hello How Are You?\n") == 0);
    check("Test dumpTB(oneLine) == Correct text");
    free(oneLine);
    releaseTB(oneLineTB);


    TB multipleLinesTB = newTB("Hey Peter\nOne sympathy mark pls\nThanks\n");
    char* multipleLines = dumpTB(multipleLinesTB);
    assert(strcmp(multipleLines, "Hey Peter\nOne sympathy mark pls\nThanks\n") == 0);
    check("Test dumpTB(multipleLines) == Correct text");
    free(multipleLines);
    releaseTB(multipleLinesTB);


    TB emptyLinesTB = newTB("\n\n\n\n\n\n\n");
    char* emptyLines = dumpTB(emptyLinesTB);
    assert(strcmp(emptyLines, "\n\n\n\n\n\n\n") == 0);
    check("Test dumpTB(emptyLines) == Correct text");
    free(emptyLines);
    releaseTB(emptyLinesTB);


    char* string = "John Fowler served twenty years in the US Air Force before retiring\
and starting a second career in the IT field.\n\nHe is also a Lay Pastor\
serving a small church near his home in Texas.\nHis hobbies include\
reading, golfing, writing,\n\nand now oil painting.\n";


    TB allTogetherTB = newTB(string);
    char* allTogether = dumpTB(allTogetherTB);
    assert(strcmp(allTogether, string) == 0);
    check("Test dumpTB(normal use) == Correct text");
    free(allTogether);
    releaseTB(allTogetherTB);
}


void test_swapTB() {
    TB singleTB = newTB("Hello\n");
    TB singleTbDup = duplicateTB(singleTB);
    swapTB(singleTB, 0, 0);
    
    assert(isEqual(singleTB, singleTbDup));

    check("Test swapTB(single line)");
    releaseTB(singleTB);
    releaseTB(singleTbDup);


    //test first/last line swapping
    TB firstLastTB = newTB("line0\nline1\nline2\n");
    TB firstLastExpectedTB = newTB("line2\nline1\nline0\n");
    swapTB(firstLastTB, 0, 2);

    assert(isEqual(firstLastTB, firstLastExpectedTB));
    releaseTB(firstLastTB);
    releaseTB(firstLastExpectedTB);
    check("Test swapTB(firstLast)");

    //Test normal usage
    TB normalTB = newTB("line0\nline1 ?\nline2\nline3\n");
    TB normalExpectedTB = newTB("line0\nline2\nline1 ?\nline3\n");
    TB normalExpected2TB = newTB("line0\nline3\nline1 ?\nline2\n");

    swapTB(normalTB, 1, 2);
    assert(isEqual(normalTB, normalExpectedTB));


    swapTB(normalTB, 1, 3);
    assert(isEqual(normalTB, normalExpected2TB));

    check("Test swapTB(normal usage)");

    releaseTB(normalTB);
    releaseTB(normalExpectedTB);
    releaseTB(normalExpected2TB);
}

void test_deleteTB() {
    TB singleTB = newTB("Hello\n");
    TB singleExpectedTB = newTB("");
    deleteTB(singleTB, 0, 0);
    
    assert(isEqual(singleTB, singleExpectedTB));
    assert(linesTB(singleTB) == 0);
    check("Test deleteTB(single)");
    releaseTB(singleTB);
    releaseTB(singleExpectedTB);


    //test first/last line swapping
    TB firstLastTB = newTB("line0\nline1\nline2\nline3\nline4\n");
    TB firstLastExpectedTB = newTB("");
    deleteTB(firstLastTB, 0, 4);

    assert(isEqual(firstLastTB, firstLastExpectedTB));
    releaseTB(firstLastTB);
    releaseTB(firstLastExpectedTB);
    check("Test deleteTB(firstLast)");


    //test first/last line swapping
    TB backwardsTB = newTB("line0\nline1\nline2\nline3\nline4\n");
    TB backwardExpectedTB = duplicateTB(backwardsTB);
    deleteTB(firstLastTB, 3, 1); //should do nada

    assert(isEqual(backwardsTB, backwardExpectedTB));
    releaseTB(backwardsTB);
    releaseTB(backwardExpectedTB); 
    check("Test deleteTB(to < from)");


    //Test normal usage
    TB normalTB = newTB("line0\nline1 ?\nline2\nline3\nline4\n");
    TB normalExpectedTB = newTB("line0\nline3\nline4\n");
    TB normalExpected2TB = newTB("line0\n");

    deleteTB(normalTB, 1, 2);
    assert(isEqual(normalTB, normalExpectedTB));

    deleteTB(normalTB, 1, 2);
    assert(isEqual(normalTB, normalExpected2TB));

    check("Test deleteTB(normal usage)");

    releaseTB(normalTB);
    releaseTB(normalExpectedTB);
    releaseTB(normalExpected2TB);
}


void test_mergeTB() {
    //1 line, at beginning
    TB test1a = newTB("SingleLine1\n");
    TB test1b = newTB("SingleLine2\n");

    mergeTB(test1a, 0, test1b);
    char* test1 = dumpTB(test1a);
    assert(strcmp(test1, "SingleLine2\nSingleLine1\n") == 0);

    releaseTB(test1a);
    free(test1);
    check("Test mergeTB(One liner)");


    //1 line, at end
    TB test2a = newTB("SingleLine1\n");
    TB test2b = newTB("SingleLine2\n");

    mergeTB(test2a, 1, test2b);
    char* test2 = dumpTB(test2a);
    assert(strcmp(test2, "SingleLine1\nSingleLine2\n") == 0);

    releaseTB(test2a);
    free(test2);
    check("Test mergeTB(One liner at end)");


    //Adding onto empty one
    TB test3a = newTB("SingleLine1\n");
    TB test3b = newTB("SingleLine2\n");
    deleteTB(test3a, 0, 0);

    mergeTB(test3a, 0, test3b);
    char* test3 = dumpTB(test3a);
    assert(strcmp(test3, "SingleLine2\n") == 0);

    releaseTB(test3a);
    free(test3);
    check("Test mergeTB(merging from empty line)");

    //Adding empty onto normal line
    TB test4a = newTB("SingleLine1\n");
    TB test4b = newTB("\n");
    deleteTB(test4b, 0, 0);

    mergeTB(test4a, 0, test4b);
    char* test4 = dumpTB(test4a);
    assert(strcmp(test4, "SingleLine1\n") == 0);

    releaseTB(test4a);
    free(test4);
    check("Test mergeTB(merging an empty line)");


    //testing normal usage adding at end
    TB test5a = newTB("SingleLine1\nSingleLine2\nSingleLine3\n");
    TB test5b = newTB("SingleLine5\nSingleLine6\nSingleLine7\nSingleLine8\n");

    mergeTB(test5a, 3, test5b);
    char* test5 = dumpTB(test5a);
    assert(strcmp(test5, "SingleLine1\nSingleLine2\nSingleLine3\nSingleLine5\nSingleLine6\nSingleLine7\nSingleLine8\n") == 0);

    releaseTB(test5a);
    free(test5);
    check("Test mergeTB(normal usage adding at end)");


    //testing normal usage adding at middle
    TB test6a = newTB("SingleLine1\nSingleLine2\nSingleLine3\n");
    TB test6b = newTB("SingleLine5\nSingleLine6\nSingleLine7\nSingleLine8\n");

    mergeTB(test6a, 2, test6b);
    char* test6 = dumpTB(test6a);
    assert(strcmp(test6, "SingleLine1\nSingleLine2\nSingleLine5\nSingleLine6\nSingleLine7\nSingleLine8\nSingleLine3\n") == 0);

    releaseTB(test6a);
    free(test6);
    check("Test mergeTB(normal usage adding at middle)");
}


//same as merge excpect ensure second tb isnt destroyed!
void test_pasteTB() {
    //1 line, at beginning
    TB test1a = newTB("line1\n");
    TB test1b = newTB("line2\n");
    TB test1bDup = duplicateTB(test1b);

    pasteTB(test1a, 0, test1b);
    char* test1 = dumpTB(test1a);
    assert(strcmp(test1, "line2\nline1\n") == 0);
    assert(isEqual(test1b, test1bDup));

    releaseTB(test1a);
    releaseTB(test1b);
    releaseTB(test1bDup);
    free(test1);
    check("Test pasteTB(One liner)");


    //1 line, at end
    TB test2a = newTB("line1\n");
    TB test2b = newTB("line2\n");
    TB test2bDup = duplicateTB(test2b);

    pasteTB(test2a, 1, test2b);
    char* test2 = dumpTB(test2a);
    assert(strcmp(test2, "line1\nline2\n") == 0);
    assert(isEqual(test2b, test2bDup));

    releaseTB(test2a);
    releaseTB(test2b);
    releaseTB(test2bDup);
    free(test2);
    check("Test pasteTB(One liner at end)");


    //Adding onto empty one
    TB test3a = newTB("line1\n");
    TB test3b = newTB("line2\n");
    deleteTB(test3a, 0, 0);
    TB test3bDup = duplicateTB(test3b);


    pasteTB(test3a, 0, test3b);
    char* test3 = dumpTB(test3a);
    assert(strcmp(test3, "line2\n") == 0);
    assert(isEqual(test3b, test3bDup));

    releaseTB(test3a);
    releaseTB(test3b);
    releaseTB(test3bDup);
    free(test3);
    check("Test pasteTB(merging from empty line)");

    //Adding empty onto normal line
    TB test4a = newTB("line1\n");
    TB test4b = newTB("\n");
    deleteTB(test4b, 0, 0);
    TB test4bDup = duplicateTB(test4b);

    pasteTB(test4a, 0, test4b);
    char* test4 = dumpTB(test4a);
    assert(strcmp(test4, "line1\n") == 0);
    assert(isEqual(test4b, test4bDup));

    releaseTB(test4a);
    releaseTB(test4b);
    releaseTB(test4bDup);
    free(test4);
    check("Test pasteTB(merging an empty line)");


    //testing normal usage adding at end
    TB test5a = newTB("line1\nline2\nline3\n");
    TB test5b = newTB("line5\nline6\nline7\nline8\n");
    TB test5bDup = duplicateTB(test5b);

    pasteTB(test5a, 3, test5b);
    char* test5 = dumpTB(test5a);
    assert(strcmp(test5, "line1\nline2\nline3\nline5\nline6\nline7\nline8\n") == 0);
    assert(isEqual(test5b, test5bDup));

    releaseTB(test5a);
    releaseTB(test5b);
    releaseTB(test5bDup);
    free(test5);
    check("Test pasteTB(normal usage adding at end)");


    //testing normal usage adding at middle
    TB test6a = newTB("line1\nline2\nline3\n");
    TB test6b = newTB("line5\nline6\nline7\nline8\n");
    TB test6bDup = duplicateTB(test6b);

    pasteTB(test6a, 2, test6b);
    char* test6 = dumpTB(test6a);
    assert(strcmp(test6, "line1\nline2\nline5\nline6\nline7\nline8\nline3\n") == 0);
    assert(isEqual(test6b, test6bDup));

    releaseTB(test6a);
    releaseTB(test6b);
    releaseTB(test6bDup);

    free(test6);
    check("Test pasteTB(normal usage adding at middle)");
}

void test_cutTB() {
    TB test1a = newTB("Hey sir\n");
    TB test1b = cutTB(test1a, 0, 0);
    char* test1 = dumpTB(test1b);

    assert(linesTB(test1a) == 0); //make sure original tb is empty
    assert(strcmp(test1, "Hey sir\n") == 0); //make sure it got cut correctly

    releaseTB(test1a);
    releaseTB(test1b);
    free(test1);
    check("Test cutTB(Cutting single line from length 1 buffer)");


    //test 2,  range 0-2 cut //starting at beginning ending at middle
    TB test2a = newTB("line1\nline2\nline3\nline4\n");
    TB test2b = newTB("line1\nline2\nline3\n");
    TB test2aExpected = newTB("line4\n");
    TB test2Output = cutTB(test2a, 0, 2);

    assert(isEqual(test2Output, test2b)); //make sure the output is lit
    assert(isEqual(test2a, test2aExpected)); //make sure the cut removed from original one

    releaseTB(test2a);
    releaseTB(test2b);
    releaseTB(test2aExpected);
    releaseTB(test2Output);
    check("Test cutTB(Cutting range (starting at 0) from buffer)");


    //test 3,  range 1-3 cut starting at middle ending at end
    TB test3a = newTB("line1\nline2\nline3\nline4\n");
    TB test3b = newTB("line2\nline3\nline4\n");
    TB test3aExpected = newTB("line1\n");
    TB test3Output = cutTB(test3a, 1, 3);

    assert(isEqual(test3Output, test3b)); //make sure the output is lit
    assert(isEqual(test3a, test3aExpected)); //make sure the cut removed from original one

    releaseTB(test3a);
    releaseTB(test3b);
    releaseTB(test3aExpected);
    releaseTB(test3Output);
    check("Test cutTB(Cutting range (ending at end) from buffer)");


    //test 4,  range 1-2 cut starting at middle ending at middle
    TB test4a = newTB("line1\nline2\nline3\nline4\nline5\n");
    TB test4b = newTB("line2\nline3\nline4\n");
    TB test4aExpected = newTB("line1\nline5\n");
    TB test4Output = cutTB(test4a, 1, 3);

    assert(isEqual(test4Output, test4b)); //make sure the output is lit
    assert(isEqual(test4a, test4aExpected)); //make sure the cut removed from original one

    releaseTB(test4a);
    releaseTB(test4b);
    releaseTB(test4aExpected);
    releaseTB(test4Output);
    check("Test cutTB(Cutting range (middle) from buffer)");


    //test 5,  range cut all
    TB test5a = newTB("line1\nline2\nline3\nline4\nline5\n");
    TB test5b = newTB("line1\nline2\nline3\nline4\nline5\n");
    TB test5Output = cutTB(test5a, 0, 4);

    assert(linesTB(test5a) == 0); //make sure the output is lit
    assert(isEqual(test5b, test5Output)); //make sure the cut removed from original one

    releaseTB(test5a);
    releaseTB(test5b);
    releaseTB(test5Output);
    check("Test cutTB(Cutting all from buffer)");
}


//same as cutTB except original TB is unmodified
void test_copyTB() {
    TB test1a = newTB("Hey sir\n");
    TB test1aExpected = newTB("Hey sir\n");
    TB test1b = copyTB(test1a, 0, 0);
    char* test1 = dumpTB(test1b);

    assert(isEqual(test1a, test1aExpected)); //make sure original tb is unmodified
    assert(strcmp(test1, "Hey sir\n") == 0); //make sure it got copied correctly

    releaseTB(test1a);
    releaseTB(test1aExpected);
    releaseTB(test1b);
    free(test1);
    check("Test cutTB(Cutting single line from length 1 buffer)");


    //test 2,  range 0-2 cut //starting at beginning ending at middle
    TB test2a = newTB("line1\nline2\nline3\nline4\n");
    TB test2b = newTB("line1\nline2\nline3\n");
    TB test2aExpected = newTB("line1\nline2\nline3\nline4\n");
    TB test2Output = copyTB(test2a, 0, 2);

    assert(isEqual(test2Output, test2b)); //make sure the output is lit
    assert(isEqual(test2a, test2aExpected)); //make sure the copy didnt remove from original one

    releaseTB(test2a);
    releaseTB(test2b);
    releaseTB(test2aExpected);
    releaseTB(test2Output);
    check("Test copyTB(Cutting range (starting at 0) from buffer)");


    //test 3,  range 1-3 cut starting at middle ending at end
    TB test3a = newTB("line1\nline2\nline3\nline4\n");
    TB test3b = newTB("line2\nline3\nline4\n");
    TB test3aExpected = newTB("line1\nline2\nline3\nline4\n");
    TB test3Output = copyTB(test3a, 1, 3);

    assert(isEqual(test3Output, test3b)); //make sure the output is lit
    assert(isEqual(test3a, test3aExpected)); //make sure the copy didnt remove from original one

    releaseTB(test3a);
    releaseTB(test3b);
    releaseTB(test3aExpected);
    releaseTB(test3Output);
    check("Test copyTB(Cutting range (ending at end) from buffer)");


    //test 4,  range 1-2 cut starting at middle ending at middle
    TB test4a = newTB("line1\nline2\nline3\nline4\nline5\n");
    TB test4b = newTB("line2\nline3\nline4\n");
    TB test4aExpected = newTB("line1\nline2\nline3\nline4\nline5\n");
    TB test4Output = copyTB(test4a, 1, 3);

    assert(isEqual(test4Output, test4b)); //make sure the output is lit
    assert(isEqual(test4a, test4aExpected)); //make sure the cut removed from original one

    releaseTB(test4a);
    releaseTB(test4b);
    releaseTB(test4aExpected);
    releaseTB(test4Output);
    check("Test copyTB(Cutting range (middle) from buffer)");


    //test 5,  range cut all
    TB test5a = newTB("line1\nline2\nline3\nline4\nline5\n");
    TB test5aExpected = newTB("line1\nline2\nline3\nline4\nline5\n");
    TB test5b = newTB("line1\nline2\nline3\nline4\nline5\n");
    TB test5Output = copyTB(test5a, 0, 4);

    assert(isEqual(test5a, test5aExpected)); //make sure the output is lit
    assert(isEqual(test5b, test5Output)); //make sure the cut removed from original one

    releaseTB(test5a);
    releaseTB(test5aExpected);
    releaseTB(test5b);
    releaseTB(test5Output);
    check("Test copyTB(Cutting all from buffer)");
}

void test_replaceTB() {
    //replace nothing
    TB test1a = newTB("Line 1\n");
    TB test1b = newTB("Line 1\n");
    replaceText(test1a, "", " ");

    assert(isEqual(test1a, test1b));
    releaseTB(test1a);
    releaseTB(test1b);
    check("Test replaceText(replacing empty text)");

    //replace a sentance with nothing
    TB test2a = newTB("Hi sir 1\nG'day ?\n");
    TB test2b = newTB(" 1\nG'day ?\n");
    replaceText(test2a, "Hi sir", "");

    assert(isEqual(test2a, test2b));
    releaseTB(test2a);
    releaseTB(test2b);
    check("Test replaceText(deleting sentance)");

    //replace a sentance with other text (multiple occurencess)
    TB test3a = newTB("Hi sir 1\nG'day ?\nHi how are  you?\nBonjour is a synonym for HiI think\n");
    TB test3b = newTB("Hello sir 1\nG'day ?\nHello how are  you?\nBonjour is a synonym for HelloI think\n");
    replaceText(test3a, "Hi", "Hello");

    assert(isEqual(test3a, test3b));
    releaseTB(test3a);
    releaseTB(test3b);
    check("Test replaceText(replacing word multiple times)");


    //replace a sentance with other text (multiple occurencess)
    TB test4a = newTB("Hi sir 1\nG'day ?\nHi how are  you?\nBonjour is a synonym for HiI think\n");
    TB test4b = newTB("Hello\n\n sir 1\nG'day ?\nHello\n\n how are  you?\nBonjour is a synonym for Hello\n\nI think\n");
    replaceText(test4a, "Hi", "Hello\n\n");

    assert(linesTB(test4a) == linesTB(test4b));
    assert(isEqual(test4a, test4b));
    releaseTB(test4a);
    releaseTB(test4b);
    check("Test replaceText(replacing word multiple times with a newline character)");


    //replace a new line character with other text (multiple occurencess)
    TB test5a = newTB("Hi\nsir\n1\nG'day\n?\nHi\nhow\nare\n\nyou?\nBonjour\nis\na\nsynonym\nfor\nHiI\nthink\n");
    TB test5b = newTB("Hsir\n1\nG'day\n?\nHhow\nare\n\nyou?\nBonjour\nis\na\nsynonym\nfor\nHiI\nthink\n");
    replaceText(test5a, "i\n", "");

    assert(linesTB(test5a) == linesTB(test5b));
    assert(isEqual(test5a, test5b));
    releaseTB(test5a);
    releaseTB(test5b);
    check("Test replaceText(replacing word multiple times with a newline character being replaced)");


    //replace a new line character with other text (multiple occurencess)
    TB test6a = newTB("Hey mateoo\nHow you going ?\n\nI'm going good thanks lol\n");
    TB test6b = newTB("Hey mateoo No thanks\nI'm going good thanks lol\n");
    replaceText(test6a, "\nHow you going ?\n", " No thanks");

    assert(linesTB(test6a) == linesTB(test6b));
    assert(isEqual(test6a, test6b));
    releaseTB(test6a);
    releaseTB(test6b);
    check("Test replaceText(normal usage)");

}



void test_abort_funs() { //out of bound funcs
    TB swapTBOutOfBounds = newTB("Hi\n");
    swapTB(swapTBOutOfBounds, 0, 3);
    swapTB(swapTBOutOfBounds, 5, 0);
    releaseTB(swapTBOutOfBounds);
    printf("swapTB(Out of bounds): " RED "failed\n" RESET);

    TB deleteTBOutOfBounds = newTB("Hi\n");
    deleteTB(deleteTBOutOfBounds, 0, 2);
    deleteTB(deleteTBOutOfBounds, 5, 0);
    releaseTB(deleteTBOutOfBounds);
    printf("deleteTB(Out of bounds): " RED "failed\n" RESET);

    TB mergeA = newTB("Hi\n");
    TB mergeB = newTB("Bye\n");
    mergeTB(mergeA, 2, mergeB);
    mergeTB(mergeA, -1, mergeB);
    releaseTB(mergeA);
    releaseTB(mergeB);
    printf("mergeTB(Out of bounds): " RED "failed\n" RESET);

    TB pasteA = newTB("Hi\n");
    TB pasteB = newTB("Bye\n");
    pasteTB(pasteA, 2, pasteB);
    pasteTB(pasteA, -1, pasteB);
    releaseTB(pasteA);
    releaseTB(pasteB);
    printf("pasteTB(Out of bounds): " RED "failed\n" RESET);

    TB cut = newTB("Hi\n");
    cutTB(cut, 0, 2);
    cutTB(cut, -1, 1);
    releaseTB(cut);
    printf("cutTB(Out of bounds): " RED "failed\n" RESET);


    TB copy = newTB("Hi\n");
    cutTB(copy, 0, 2);
    cutTB(copy, -1, 1);
    releaseTB(copy);
    printf("copyTB(Out of bounds): " RED "failed\n" RESET);
}


int main() {

    test_newTB();
    printf("\n");
    test_dumpTB();
    printf("\n");
    test_swapTB();
    printf("\n");
    test_deleteTB();
    printf("\n");
    test_mergeTB();
    printf("\n");
    test_pasteTB();
    printf("\n");
    test_cutTB();
    printf("\n");
    test_copyTB();
    printf("\n");
    test_replaceTB();

    printf("\n");
    //test_abort_funs();
}
