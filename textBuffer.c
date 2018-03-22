#include <stdio.h>
#include <stdlib.h>
#include "textbuffer.h"
#include <string.h>
#include <assert.h>

#define GREEN   "\x1B[32m" 
#define RESET "\x1B[0m"

#define success printf("%sSuccess\n%s", GREEN, RESET);


typedef struct line *line;

struct line {
    char* text; //dynamically sized line of text
    line prev; //pointer to previous line
    line next; //pointer to next line
};

struct textbuffer { //this is TB implementation
    line firstLine;
    line lastLine;
    int size; //no of lines
};


/* 
 ======================================
            Static functions
 ======================================
 */



/**
 * Takes a pointer to a string and on
 * consecutive calls returns the next
 * string split by a newline character '\n'
 * Returns NULL after reaching \0 character
 */
static char* tokenise(char* text) {
    if(text == NULL) {
        return NULL;
    }
    
    static int i = 0;
    int temp = i;
    int size = 0;

    for(char c = text[i++]; c != '\0'; c = text[i++]) {
        size++;
        if(c == '\n') break;
    }

    if(size == 0) { //only if c = '\0'
        i = 0; //reset function if no more tokens
        return NULL;
    }

    char* ret = calloc(1, sizeof(char) * size);
    if(!ret) {
        abort();
    }

    for(int j = 0; j < size - 1; j++) {
        ret[j] = text[temp++];
    }
    ret[size-1] = '\0';
    return ret;
}

/**
 * Takes a TB @tb and position in the list
 * and returns the line at that position in
 * the list (index from 0) or returns NULL
 * if out of bounds
 */
static line getLineAtPos(TB tb, int pos) {
    if(tb == NULL || tb->size == 0 || pos < 0 || pos >= linesTB(tb)) 
        return NULL;

    line current = tb->firstLine;

    for(int i = 0; i < pos; i++) {
        current = current->next;
    }

    return current;
}


/**
 * Takes a TB @tb, and creates a copy with the 
 * same lines, texts and order of original @tb
 */
static TB duplicateTB(TB tb) {
    char* dump = dumpTB(tb);
    TB new = newTB(dump);
    free(dump);
    return new;
}

/**
 * Takes a line @l, and creates a copy with the 
 * same text and no ->next, ->prev set.
 */
static line duplicateLine(line l) {
    line new = calloc(1, sizeof(struct line));
    if(!new) {
        abort();
    }

    new->text = malloc(sizeof(char) * (strlen(l->text)  + 1));
    if(!new->text) {
        abort();
    }

    if(new->text == NULL) 
        exit(1);

    strcpy(new->text, l->text);

    return new;
}


static void duplicateInto(TB copyInto, TB copyFrom) {
    line current = copyFrom->firstLine;
    if(current == NULL) return;

    line tempCurrent = NULL;

    while(current != NULL) {
        if(tempCurrent == NULL) {
            tempCurrent = duplicateLine(current);
            copyInto->firstLine = tempCurrent;
        } else {
            tempCurrent->next = duplicateLine(current);
            tempCurrent->next->prev = tempCurrent;
            tempCurrent = tempCurrent->next;
        }

        copyInto->size++;
        copyInto->lastLine = tempCurrent;
        current = current->next;
    }
}

static int sizeOfText(TB tb) {
    if(tb == NULL) 
        return 0;

    int size = 0;

    for(line current = tb->firstLine; current != NULL; current = current->next) {
        size += strlen(current->text) + 1; //add 1 for new line
    }

    return size + 1; //1 for EOF char
}


/* 
 ======================================
           .h implementation
 ======================================
 */


/* Allocate a new textbuffer whose contents is initialised with the text given
 * in the array.
 */
TB newTB (char text[]) {
    if(text == NULL) 
        return NULL;

    TB txt = calloc(1, sizeof(struct textbuffer)); //create first line
    
    if(!txt) {
        abort();
    }

    char* token = tokenise(text);

    line previous = NULL;
    line current = NULL;

    while(token != NULL) {
        current = calloc(1, sizeof(struct line));
        if(!current) {
            abort();
        }

        current->text = token;

        //Handle setting the ->next and ->prev
        if(previous == NULL) {
            previous = current;
        } else {
            current->prev = previous;
            previous->next = current;
            previous = current;
        }

        //handle setting the textbuffer
        txt->size += 1;
        if(txt->firstLine == NULL) {
            txt->firstLine = current;
        }

        token = tokenise(text);
    }

    txt->lastLine = current;
    return txt;
}



/* Free the memory occupied by the given textbuffer.  It is an error to access
 * the buffer afterwards.
 */
void releaseTB (TB tb) {
    line current = tb->firstLine;

    while(current != NULL) {
        line temp = current->next;
        free(current->text);
        free(current);

        current = temp;
    }

    free(tb);
}


/* Allocate and return an array containing the text in the given textbuffer.
 */
char *dumpTB (TB tb) {
    if(tb == NULL) 
        return NULL;

    char* text = calloc(1, sizeof(char) * sizeOfText(tb));

    if(!text) {
        abort();
    }

    int counter = 0;

    for(line current = tb->firstLine; current != NULL; current = current->next) {
        int i;
        for(i = 0; i < strlen(current->text); i++) {
            text[counter++] = current->text[i];
        }
        text[counter++] = '\n';
    }

    return text;
}

/* Return the number of lines of the given textbuffer.
 */
int linesTB(TB tb) {
    if(tb == NULL)
        return 0;
    return tb->size;
}

/* Swap the two given lines in the textbuffer.
 *
 * - The program is to abort() with an error message if line 'pos1' or line
 *   'pos2' is out of range.  The first line of a textbuffer is at position 0.
 */
void swapTB(TB tb, int pos1, int pos2) {
    if(pos1 == pos2) 
        return;

    if(pos1 > pos2) { //order pos1 and pos2 for later
        int temp = pos2;
        pos2 = pos1;
        pos1 = temp;
    }

    line first = getLineAtPos(tb, pos1);
    line second = getLineAtPos(tb, pos2);

    if(first == NULL || second == NULL) {
        printf("SwapTB: %d, %d Position out of range!\n", pos1, pos2);
        abort();
    }

    if(first->next == second) { //if they are sequential
        first->next = second->next;
        second->prev = first->prev;

        if(first->next != NULL) 
            first->next->prev = first;
    
        if(second->prev != NULL)  
            second->prev->next = second;

        second->next = first;
        first->prev = second;
    } else {
        line secondPrev = second->prev;
        line secondNext = second->next;

        second->prev = first->prev;
        second->next = first->next;

        first->prev = secondPrev;
        first->next = secondNext;

        if(second->next != NULL) 
            second->next->prev = second;

        if(second->prev != NULL) 
            second->prev->next = second;

        if(first->next != NULL) 
            first->next->prev = first;

        if(first->prev != NULL) 
            first->prev->next = first;
    
    }
    
    if(pos1 == 0) 
        tb->firstLine = second;
    if(pos2 == linesTB(tb) - 1) 
        tb->lastLine = first;
}

/* Remove the lines between and including 'from' and 'to' from the textbuffer
 * 'tb'.
 *
 * - The program is to abort() with an error message if 'from' or 'to' is out
 *   of range. 
 */
void deleteTB (TB tb, int from, int to) {
    if(from > to) {
        return;
    }

    line first = getLineAtPos(tb, from);
    line second = getLineAtPos(tb, to);

    if(first == NULL || second == NULL) {
        printf("deleteTB: %d, %d Position out of range!\n", from, to);
        abort();
    }

    if(first->prev) 
        first->prev->next = second->next; //join the deleted gap in the list
    if(second->next)
        second->next->prev = first->prev;


    if(from == 0) 
        tb->firstLine = second->next;
    if(to == linesTB(tb) - 1) 
        tb->lastLine = first->prev;

    line curr = first;
    line end = second->next;

    while(curr != end) {
        line temp = curr->next;
        free(curr->text);
        free(curr);
        curr = temp;
    }

    tb->size = tb->size - (to - from + 1);
}

/* Merge 'tb2' into 'tb1' at line 'pos'.
 * 
 * - Afterwards line 0 of 'tb2' will be line 'pos' of 'tb1'.
 * - The old line 'pos' of 'tb1' will follow after the last line of 'tb2'.
 * - After this operation 'tb2' can not be used anymore (as if we had used
 *   releaseTB() on it).
 * - The program is to abort() with an error message if 'pos' is out of range.
 */
void mergeTB (TB tb1, int pos, TB tb2) {
    if(tb1 == tb2 || tb1 == NULL || tb2 == NULL) //if they are same text buffers
        return;

    if(tb2->size == 0) { //if tb2 is empty 
        free(tb2); //add it onto tb1 then free It
        return;
    }

    line p = getLineAtPos(tb1, pos);

    if(p == NULL) { //eg, empty list, pos 0 dont exit
        //This next if checks if we are adding it to the end of the tb
        if(tb1 == NULL || pos != tb1->size) {
            printf("%d\n", tb1->size);
            printf("mergeTB: %d, Position out of range!\n", pos);
            abort();
        }
    }

    if(pos == 0) {
        if(p == NULL) {
            tb1->firstLine = tb2->firstLine;
            tb1->lastLine = tb2->lastLine;
        } else {
            tb1->firstLine = tb2->firstLine;

            p->prev = tb2->lastLine;
            tb2->lastLine->next = p;
        }
    } else if(pos == linesTB(tb1)) {
        tb1->lastLine->next = tb2->firstLine;
        tb2->firstLine->prev = tb1->lastLine;

        tb1->lastLine = tb2->lastLine;
    } else {
        p->prev->next = tb2->firstLine;
        tb2->firstLine->prev = p->prev;

        tb2->lastLine->next = p;
        p->prev = tb2->lastLine;
    }

    tb1->size += tb2->size;
    free(tb2);
}


/* Copy 'tb2' into 'tb1' at line 'pos'.
 * 
 * - Afterwards line 0 of 'tb2' will be line 'pos' of 'tb1'.
 * - The old line 'pos' of 'tb1' will follow after the last line of 'tb2'.
 * - After this operation 'tb2' is unmodified and remains usable independent
 *   of 'tb1'. 
 * - The program is to abort() with an error message if 'pos' is out of range.
 */
void pasteTB (TB tb1, int pos, TB tb2) {
    mergeTB(tb1, pos, duplicateTB(tb2)); //is this cheating?
}


/* Cut the lines between and including 'from' and 'to' out of the textbuffer
 * 'tb'.
 *
 * - The result is a new textbuffer (much as one created with newTB()).
 * - The cut lines will be deleted from 'tb'.
 * - The program is to abort() with an error message if 'from' or 'to' is out
 *   of range. 
 */
TB cutTB (TB tb, int from, int to) {
    if(from > to) {
        return NULL;
    }

    line first = getLineAtPos(tb, from);
    line second = getLineAtPos(tb, to);

    if(first == NULL || second == NULL) {
        printf("cutTB: %d, %d Position out of range!\n", from, to);
        abort();
    }

    if(first->prev) 
        first->prev->next = second->next; //join the deleted gap in the list
    if(second->next)
        second->next->prev = first->prev;


    if(from == 0) 
        tb->firstLine = second->next;
    if(to == linesTB(tb)) 
        tb->lastLine = first->prev;

    TB new = newTB("");

    new->firstLine = first;
    new->lastLine = second;

    first->prev = NULL;
    second->next = NULL;

    new->size = to - from + 1;
    tb->size = tb->size - (to - from + 1);

    return new;
}

/* Copy the lines between and including 'from' and 'to' of the textbuffer
 * 'tb'.
 *
 * - The result is a new textbuffer (much as one created with newTB()).
 * - The textbuffer 'tb' will remain unmodified.
 * - The program is to abort() with an error message if 'from' or 'to' is out
 *   of range. 
 */
TB copyTB (TB tb, int from, int to) {
    // TB new = duplicateTB(tb);
    // TB ret = cutTB(new, from, to);
    // releaseTB(new);
    // return ret;

    if(from > to) { //order pos1 and pos2 for later
        return NULL;
    }

    line first = getLineAtPos(tb, from);
    line second = getLineAtPos(tb, to);

    if(first == NULL || second == NULL) {
        printf("copyTB: %d, %d Position out of range!\n", from, to);
        abort();
    }

    TB new = newTB("");


    line prev = NULL;
    line curr = first;

    while(curr != second->next) {
        line temp = duplicateLine(curr);

        if(!new->firstLine)
            new->firstLine = temp;

        temp->prev = prev; //set previous
        if(prev)
            prev->next = temp;

        prev = temp;

        curr = curr->next;
    }

    new->lastLine = prev;
    new->size = to - from + 1;
    return new;
}


/* Search every line of tb for each occurrence of str1 and replaces them 
 * with str2
 */
void replaceText (TB tb, char* str1, char* str2) {
    int done = 0;
    int offset = 0;

    char* textAsAWhole = dumpTB(tb);
    int length = 0;

    while(!done) {
        if(textAsAWhole == NULL || str1 == NULL || strlen(str1) == 0 || offset < 0 || offset >= strlen(textAsAWhole))
            break; 
        done = 1;

        char* firstSubstring = strstr(textAsAWhole + offset, str1); 

        if(firstSubstring != NULL) {
            int originalLength = strlen(textAsAWhole); //length of original string
            length = originalLength + strlen(str2) - strlen(str1) + 1; //length of new string
            int position = firstSubstring - textAsAWhole; //offset in string of the substring

            char* newText = calloc(1, sizeof(char) * length);
            if(!newText) {
                abort();
            }
            strncat(newText, textAsAWhole, (firstSubstring - textAsAWhole)); 
            strcat(newText, str2); //Copy over the original string with @str2 replacing @str1
            strcat(newText, (firstSubstring + strlen(str1)));

            free(textAsAWhole);
            textAsAWhole = newText; //set the new string

            //search for more occurences of @str1 inside @textAsAWhole
            offset = position + strlen(str2);
            done = 0; //go again
        }
    }

    if(offset > 0 && *(textAsAWhole + length - 2) != '\n') { //ensure ends in \n
        char* newText = calloc(1, sizeof(char) * length + 1);
        strcat(newText, textAsAWhole);
        strcat(newText, "\n"); 
        free(textAsAWhole);
        textAsAWhole = newText; //if the last chatacter isnt \n make it
    }

    //empty TB
    line current = tb->firstLine;
    while(current != NULL) {
        line temp = current->next;
        free(current->text);
        free(current);

        current = temp;
    }
    tb->firstLine = NULL;
    tb->lastLine = NULL;
    tb->size = 0;

    //recreate TB
    TB tempTB = newTB(textAsAWhole);
    if(tempTB == NULL || linesTB(tempTB) == 0) 
        return;

    duplicateInto(tb, tempTB); //copy over the lines of the new tb

    releaseTB(tempTB);
    free(textAsAWhole);
}


/**

    Start with first line of tb1
    Search through tb2 until the line is found

    Keep a track of how far along changes we are 
    by duplicating tb1 and tb2 and modyifying them

    do {
    if line is found                      TB1= <2, 3, 4>  TB2 = <a,b,2,3,4,c,d,e>
    ->   +, <line number>, line           EG here, we search in tb2 until we find '2'
    ->   for every line before it         Then we add 'a', 'b', '2' to our list of changes
    if line is not found
    ->   -, <line number>, line
    ->  as it is not in the text buffer and should be removed
    } while (duplicateTB1 is not empty)

    Now tb1 is empty                      From aboves example
    Copy all values from tb2 into tb1.    TB1 = <>  TB2 = <c,d,e>
                                          changes += +c, +d, +e at end of tb1

    #done
*/
char* diffTB (TB tb1, TB tb2) { 
    assert(tb1 != NULL && tb2 != NULL);

    int maxChanges = sizeOfText(tb1) + sizeOfText(tb2); //delete all of tb1 add all of tb2 is worse case
    if(maxChanges == 0)
        return "";


    char* changes = calloc(1, sizeof(char) * maxChanges * 5); // +,- and spaces per line

    TB tb1Clone = duplicateTB(tb1); //so we can actually write to them
    TB tb2Clone = duplicateTB(tb2);

    int tb2CounterEnd = 0;
    int currentLineNumber = 0;

    for(line current = tb1Clone->firstLine; current != NULL;) {
        line tb2Current = tb2Clone->firstLine;
        
        for(tb2CounterEnd = 0; tb2Current != NULL; tb2Current = tb2Current->next) {
            if(strcmp(tb2Current->text, current->text) == 0) {
                break; //break out if we are equal
            }
            tb2CounterEnd++;
        }

        if(tb2Current == NULL) {
            current = current->next;
            deleteTB(tb1Clone, currentLineNumber, currentLineNumber);
            strcat(changes, "-,"); //-,16

            char num[16] = {0};//16 digit number max
            sprintf(num, "%d", currentLineNumber);
            strcat(changes, num);

            strcat(changes, "\n");

            currentLineNumber--;
        } else {
            //paste tb2First -> tb2Current before @current
            line temp = tb2Clone->firstLine;
            int tempLineNumb = currentLineNumber;
            for(int i = 0; i < tb2CounterEnd; i++) {
                strcat(changes, "+,");
                char num[16] = {0};//16 digit number max
                sprintf(num, "%d", tempLineNumb );
                strcat(changes, num);
                strcat(changes, ","); 
                strcat(changes, temp->text); 

                strcat(changes, "\n"); 
                tempLineNumb++;
                temp = temp->next;
                if(temp == NULL)
                    break;
            }
            deleteTB(tb2Clone, tb2CounterEnd, tb2CounterEnd);

            if(tb2CounterEnd-- > 0) {
                TB toCopy = cutTB(tb2Clone, 0, tb2CounterEnd);
                mergeTB(tb1Clone, currentLineNumber, toCopy);
            }
            current = current->next;
            currentLineNumber = tempLineNumb;
        }

        currentLineNumber++;
    }


    for(line current = tb2Clone->firstLine; current != NULL; current=current->next) {
        strcat(changes, "+,");
        char num[16] = {0};//16 digit number max
        sprintf(num, "%d", currentLineNumber);
        strcat(changes, num);
        strcat(changes, ","); 
        strcat(changes, current->text); 

        strcat(changes, "\n"); 
        currentLineNumber++;
    }

    releaseTB(tb1Clone);
    releaseTB(tb2Clone);

    return changes;
}


/* 
 ======================================
           Whitebox testing
 ======================================
 */

//Used in the testing functions
static int isEqual(TB a1, TB a2) {
    line a = a1->firstLine;
    line b = a2->firstLine;
    for(int i = 0; i < a1->size; i++) {
        if(strcmp(a->text, b->text) != 0)
            return 0;

        a = a->next;
        b = b->next;
    }

    a = a1->lastLine;
    b = a2->lastLine;

    for(int i = 0; i < a1->size; i++) {
        if(strcmp(a->text, b->text) != 0)
            return 0;

        a = a->prev;
        b = b->prev;
    }

    return 1;
}

static void check(char* msg) {
    printf("%s", msg);
    success
}

// Testing tokenise()
static void WB_test_tokenise() {
    //Test against a NULL String
    assert(tokenise(NULL) == NULL);
    check("Tokenise: NULL String: ");

    //Test against an empty String
    assert(tokenise("") == NULL);
    check("Tokenise: empty String: ");


    char* input = "Testing 123\n";
    char* expectedOutput[] = {"Testing 123", NULL};

    //get output from function
    char* output[2];
    for(int i = 0; i < 2; i++)
        output[i] = tokenise(input);

    for(int i = 0; i < 2; i++) { //test output
        if(output[i] != NULL && expectedOutput[i] != NULL)
            assert(strcmp(output[i], expectedOutput[i]) == 0);
        else
            assert(output[i] == expectedOutput[i]);
    }
    check("Tokenise: One line String: ");

    for(int i = 0; i < 2; i++) {
        free(output[i]);
    }
}


static void WB_test_tokenise_multipleline_string() {
    char* input = "Testing 123\nI am good\nHow are you\n";
    char* expectedOutput[] = {"Testing 123", "I am good", "How are you", NULL};

    char* output[4];
    for(int i = 0; i < 4; i++)
        output[i] = tokenise(input);

    for(int i = 0; i < 4; i++) {
        if(output[i] != NULL && expectedOutput[i] != NULL)
            assert(strcmp(output[i], expectedOutput[i]) == 0);
        else
            assert(output[i] == expectedOutput[i]);
    }

    check("Tokenise: Multiple line String: ");

    for(int i = 0; i < 4; i++) {
        free(output[i]);
    }
}


//Test getLineAtPos
static void WB_test_getlineTB() {
    //Test against a NULL TB
    assert(getLineAtPos(NULL, 0) == NULL);
    check("getLineAtPos: NULL TB: ");

    //Test against an empty TB
    TB empty = newTB("");
    assert(getLineAtPos(empty, 0) == NULL);
    check("getLineAtPos: empty TB: ");

    releaseTB(empty);



    TB tb = newTB("1\n2\n3\n4\n");

    assert(getLineAtPos(tb, -1) == NULL && getLineAtPos(tb, 4) == NULL);
    check("getLineAtPos: invalid position: "); 

    releaseTB(tb);

    tb = newTB("1\n2\n3\n4\n");

    assert(strcmp(getLineAtPos(tb, 0)->text, "1") == 0);
    assert(strcmp(getLineAtPos(tb, 1)->text, "2") == 0);
    assert(strcmp(getLineAtPos(tb, 2)->text, "3") == 0);
    assert(strcmp(getLineAtPos(tb, 3)->text, "4") == 0);

    check("getLineAtPos: normal position: ");

    releaseTB(tb);
}

//test duplicateTB
static void WB_test_duplicateTB() {
    //Test against a NULL TB
    assert(duplicateTB(NULL) == NULL);
    check("duplicateTB: NULL TB: ");

    //Test against an empty TB
    TB empty = newTB("");
    TB dup = duplicateTB(empty);

    assert(dup->size == 0);
    check("duplicateTB: empty TB: ");

    releaseTB(empty);
    releaseTB(dup);


    //Test against an empty TB
    TB new = newTB("1\n2\n3\n4\n");
    dup = duplicateTB(new);

    line newCurrent = new->firstLine;
    line dupCurrent = dup->firstLine;

    for(int i = 0; i < 4; i++) {
        assert(newCurrent != NULL && dupCurrent != NULL);
        assert(strcmp(newCurrent->text, dupCurrent->text) == 0);

        newCurrent = newCurrent->next;
        dupCurrent = dupCurrent->next;
    }

    check("duplicateTB: normal TB: ");

    releaseTB(new);
    releaseTB(dup);
}


static void WB_test_sizeofText() {
    assert(sizeOfText(NULL) == 0);
    check("sizeOfText(): null input: ");

    TB empty = newTB(""); //null character
    assert(sizeOfText(empty) == 1);
    
    check("sizeOfText(): empty input: ");
    releaseTB(empty);


    TB newLine = newTB("\n");
    assert(sizeOfText(newLine) == 2);
    
    check("sizeOfText(): null input: ");
    releaseTB(newLine);


    TB tb = newTB("Hey Peter\nI'm well thanks\n");
    assert(sizeOfText(tb) == 27);

    releaseTB(tb);

    check("sizeOfText(): null input: ");
}



//Whitebox newTB tests
static void WB_test_newTB() {
    TB new = newTB("Hey there matey\nHow are you?\n\nLol\n");

    assert(new != NULL);

    line first = new->firstLine;
    line second = first->next;
    line third = second->next;
    line fourth = third->next;
    line fifth = fourth->next;

    assert(strcmp(first->text, "Hey there matey") == 0);
    assert(strcmp(second->text, "How are you?") == 0);
    assert(strcmp(third->text, "") == 0);
    assert(strcmp(fourth->text, "Lol") == 0);
    assert(fifth == NULL);
    assert(fourth->prev == third);
    assert(new->size == 4);

    releaseTB(new);


    TB emptyLine = newTB("\n");
    assert(strcmp(emptyLine->firstLine->text, "") == 0);
    assert(emptyLine->size == 1);
    releaseTB(emptyLine);
    check("newTB(): ");

}

static void WB_test_dumpTB() {
    char* text = "Hey there matey\nHow are you?\n\nLol\n";
    TB new = newTB(text);
    char* dump = dumpTB(new);

    for(int i = 0; i < strlen(text); i++) {
        assert(text[i] == dump[i]); //check each character is the same
    }

    assert(new != NULL); //didnt break new
    assert(new->size == 4);

    releaseTB(new);
    free(dump);
    check("dumpTB(): ");
}

static void WB_test_swapTB() {
    TB a = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\nk mate\n");
    TB b = duplicateTB(a);

    swapTB(a, 1,3);
    assert(a != NULL && b != NULL);


    line aLine = a->firstLine;
    line bLine = b->firstLine;
    for(int i = 0; i < 4; i++) { //check the ->next's
        if(i == 1 || i == 3) 
            assert(strcmp(aLine->text, bLine->text) != 0);
        else 
            assert(strcmp(aLine->text, bLine->text) == 0);
        aLine = aLine->next;
        bLine = bLine->next;
    }



    aLine = a->lastLine;
    bLine = b->lastLine;
    for(int i = 0; i < 4; i++) { //check the ->prev's
        if(i == 0 || i == 2) 
            assert(strcmp(aLine->text, bLine->text) != 0);
        else 
            assert(strcmp(aLine->text, bLine->text) == 0);
        aLine = aLine->prev;
        bLine = bLine->prev;
    }

    assert(strcmp(a->firstLine->next->text, b->firstLine->next->next->next->text) == 0);
    assert(strcmp(b->firstLine->next->text, a->firstLine->next->next->next->text) == 0);
    releaseTB(a);
    releaseTB(b);

    check("swapTB(): ");
}


 static void WB_test_deleteTB() {
    TB a = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\nk mate\n");
    TB b = newTB("I'm Good thanks\nLol\nk mate\n");

    deleteTB(a, 0, 0);
    assert(a->size == 3);

    assert(isEqual(a, b));    

    deleteTB(a, 1, 2);
    assert(a->size == 1);
    assert(a->firstLine == a->lastLine);
    assert(strcmp(a->firstLine->text, "I'm Good thanks") == 0);
    releaseTB(a);
    releaseTB(b);

    check("deleteTB(): ");

}

static void WB_test_mergeTB() {
    TB a = newTB("Hey there bro how are you?\nLol\nk mate\n");
    TB b = newTB("I'm Good thanks\n");
    TB c = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\nk mate\n");
    mergeTB(a, 1, b);

    assert(a != NULL);
    
    assert(isEqual(a, c));    
    
    releaseTB(a);
    releaseTB(c);

    check("mergeTB(): ");
}


static void WB_test_pasteTB() {
    TB a = newTB("Hey there bro how are you?\nLol\nk mate\n");
    TB b = newTB("I'm Good thanks\n");
    TB c = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\nk mate\n");
    pasteTB(a, 1, b);

    assert(a != NULL);
    assert(strcmp(b->firstLine->text, "I'm Good thanks") == 0);
    assert(b->lastLine == b->firstLine);
    assert(b->size == 1);

    assert(isEqual(a, c));    

    releaseTB(a);
    releaseTB(b);
    releaseTB(c);
    check("pasteTB(): ");

}

static void WB_test_cutTB() {
    TB a = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\nk mate\n");
    TB b = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\n");
    TB c = cutTB(a, 0, 2);


    assert(a->size == 1);
    assert(strcmp(a->firstLine->text, "k mate") == 0);
    assert(a->firstLine == a->lastLine);

    assert(isEqual(b, c));    



    releaseTB(a);
    releaseTB(b);
    releaseTB(c);
    check("cutTB(): ");

}


static void WB_test_copyTB() {
    TB a = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\nk mate\n");
    TB aDup = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\nk mate\n");
    TB b = newTB("Hey there bro how are you?\nI'm Good thanks\nLol\n");
    TB c = copyTB(a, 0, 2);


    assert(a->size == 4);
    assert(isEqual(a, aDup)); //ensure A is still intac   
    assert(isEqual(b, c));    //ensure copy was successfull


    releaseTB(a);
    releaseTB(aDup);
    releaseTB(b);
    releaseTB(c);
    check("copyTB(): ");

}

static void WB_test_replaceText() {
    TB a = newTB("Hey there bro how are you?\nI'm Good thanks\nHey how are you?\nI'm not too badd Lol\n");
    TB b = newTB("Hx\ny thx\nrx\n bro how arx\n you?\nI'm Good thanks\nHx\ny how arx\n you?\nI'm not too badd Lol\n");
    TB c = newTB("Hxy thxrx bro how arx you?I'm Good thanksHxy how arx you?I'm not too badd Lol\n");

    replaceText(a, "e", "x\n");

    assert(a != NULL);
    assert(a->size == 10);
    assert(isEqual(a, b));

    replaceText(a, "\n", "");
    assert(a->size == 1);
    assert(isEqual(a, c));

    releaseTB(a);
    releaseTB(b);
    releaseTB(c);

    check("replaceText(): ");
}

/******************************************************************************



                        White box Tests



*******************************************************************************/
void whiteboxtests() {
    WB_test_tokenise();
    WB_test_tokenise_multipleline_string();
    WB_test_getlineTB();
    WB_test_duplicateTB();
    WB_test_sizeofText();


    //Test .h implementation
    WB_test_newTB();
    WB_test_dumpTB();
    WB_test_swapTB();
    WB_test_deleteTB();
    WB_test_mergeTB();
    WB_test_pasteTB();
    WB_test_cutTB();
    WB_test_copyTB();
    WB_test_replaceText();
} 


// int main() {
//     whiteboxtests();
// }