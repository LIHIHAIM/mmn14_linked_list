/* [mmn 14 : symbolTable.c]:
in this file:  prototypes of functions which are operating on the symbol-table or are related to it. 

author: Uri K.H,   Lihi Haim       Date: 21.3.2021 
ID: 215105321,     313544165       Tutor: Danny Calfon */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "symbolTable.h"
#include "util.h"
#include "label.h"
#include "privateSymTabFuncs.h"

typedef struct{
    int address;
    char *name;
} extTable;

static symbolTable *symTab;
static int symSize;
static extTable *externals;
static int extSize;

static boolean definedAs(char *, char *);
static boolean addAttribToSymTab(char *, char *);

/* pushExtern(): the function gets as parameters a line and a pointer to a external variable
 in the line and it's length,
and pushes it if it's a valid external variable to symbol-table */

boolean pushExtern(char *line, int *lInd, int lineCnt)
{
    char *label;
    boolean added;

    jumpSpaces(line, lInd);
    label = readWord(line, lInd);
    if (!isValidLabel(label, lineCnt, FALSE) || isIlegalName(label, lineCnt) || wasDefined(label, lineCnt, TRUE, 1) || !(added = addToSymTab(label, "external", lineCnt)))
        return FALSE;
    if (added == ERROR)
        return ERROR;
    if(!isBlank(line, *lInd)){
        printf("error [line %d]: extranous text after end of command\n", lineCnt);
        return FALSE;
    }
    return TRUE;
}

/* pushEntry(): the function gets as parameters a line and a pointer to a entry variable
 in the line and it's length,
and pushes it if it's a valid entry variable to symbol-table */

boolean pushEntry(char *line, int* lInd, int lineCnt){
    char *operand, *operation;
    boolean added;
    /*int i;*/

    jumpSpaces(line, lInd);
    if (!(operation = readWord(line, lInd)))
        return ERROR;
    /*(for(i = 0; i< 100; i++)
        printf(" ");
    printf("\r");*/
    if(strcmp(operation, ".entry") == 0){
        free(operation);
        jumpSpaces(line, lInd);
        if (!(operand = readWord(line, lInd)))
            return ERROR;
        if(definedAs(operand, "external")){
            printf("error [line %d]: operand cnnot be named \".extern\" and \".entry\" at the same time\n", lineCnt);
            return FALSE;
        }
        if(!(added = addAttribToSymTab(operand, "entry")))
            return FALSE;
        if(added == ERROR)
            return ERROR;
        return TRUE;
    }
    else if(strcmp(operation, ".string") == 0 || strcmp(operation, ".data") == 0 || strcmp(operation, ".extern") == 0){
        free(operation);
        *lInd = -1;
        return TRUE;
    }
    return TRUE;
}

/* checkEntry(): the function gets as parameters a line, the pointer to an entry variable in line and it's length,
to check if the entry variable is valid and return true or false accordingly */

boolean checkEntry(char *line, int *lInd, int lineCnt){
    char *operand;
    jumpSpaces(line, lInd);
    if (!(operand = readWord(line, lInd)))
        return ERROR;
    if(strcmp(operand, "\0") == 0){ /* could not get a label */
        printf("error [line %d]: missing operand after directive \".entry\"\n", lineCnt);
        return FALSE;
    }
    if(!isValidLabel(operand, lineCnt, FALSE))
        return FALSE;
    if(!isBlank(line, *lInd)){
        printf("error [line %d]: extranous text after end of command\n", lineCnt);
        return FALSE;
    }
    return TRUE;
}

/* addToSymTab(): the function gets as parameters a symbol name, directive type
 and the line and it's length,
 and pushes it if it's a valid variable to symbol-table
 in case of success TRUE will be returned and False in case of invalidation */
boolean addToSymTab(char *name, char *attrib, int lineCnt)
{   
    int tabInd;
    int *temp;
    if (symSize == 0){
        symTab =(symbolTable*)calloc(1,sizeof(symbolTable));
        symTab.next=NULL;
        symTab.prev=NULL;
        symSize++;
    }
    else
    {
        temp =(symbolTable*)calloc(1,sizeof(symbolTable));
        if (!isAlloc(symTab))
        return ERROR;
        temp.prev=&symTab;
        temp.next=NULL;
        symTab=temp;
        symSize++;

    }
        
        temp =(symbolTable*)calloc(1,sizeof(symbolTable));
        if (!isAlloc(symTab))
        return ERROR;
        temp.prev=&symTab;
        temp.next=NULL;
        symTab=temp;
        symSize++;
    tabInd = symSize-1;
    if (isIlegalName(name, lineCnt) || wasDefined(name, lineCnt, TRUE, 1))
        return FALSE;
    symTab.symbol = malloc(strlen(name) + 1);
    symTab.attribute1 = malloc(strlen(attrib) + 1);
    symTab.attribute2 = NULL;
    if (!isAlloc(symTab.symbol) || !isAlloc(symTab.attribute1)){
        free(symTab.symbol);
        free(symTab.attribute1);
        return ERROR;
    }
    strcpy(symTab.symbol, name);
    strcpy(symTab.attribute1, attrib);
    if (strcmp(attrib, "data") == 0)
        symTab.address = getDC();
    else if (strcmp(attrib, "code") == 0)
        symTab.address = getIC();
    else if (strcmp(attrib, "external") == 0)
        symTab.address = 0;
    return TRUE;
}

/* addToExtTab(): the function gets as parameters the name of the external variable and the address
 in the memory on which it is suposse to be writen. And then pushes it to the externals table.  */
boolean addToExtTab(char *name, int add){
    if (extSize == 0){
        externals = malloc(sizeof(extTable));
        extSize++;
    }
    else
        externals = realloc(externals, ++extSize * sizeof(extTable));
    if (!isAlloc(externals))
        return ERROR;
    externals[extSize-1].name = malloc(strlen(name));
    if(!isAlloc(externals[extSize-1].name))
        return ERROR;
    externals[extSize-1].address = add + OS_MEM;
    strcpy(externals[extSize-1].name, name);
    return TRUE;
}

/* addAttribToSymTab(): the function gets as parameters a symbol name to search for in symbol-table
 and the directive type to add to it,
 and pushes the directive type if it's a valid to symbol-table
 in case of success TRUE will be returned and False in case of invalidation */
static boolean addAttribToSymTab(char *sym, char *attrib)
{
    int i;
    int *temp=symTab;
    for (i = 0; i < symSize, temp!=NULL; i++)
    {
        if (strcmp(symTab.symbol, sym) == 0)
        {
            temp4.attribute2 = malloc(strlen(attrib) + 1);
            if (!isAlloc(temp.attribute2))
                return ERROR;
            strcpy(temp.attribute2, attrib);
            temp=temp.prev;
            return TRUE;
        }
    }
    return FALSE;
}

void encPlusIC(){
    int i;
    int *temp=symTab;
    for(i = 0; i < symSize, temp!=NULL; i++){
        if(strcmp(temp.attribute1, "data") == 0)
            temp.address += getIC();
            temp=temp.prev;
    }
}

/* wasdefined(): the function gets as parameters a symbol name 
 and it's length type to search for in symbol-table 
 and the function checks if the symbol is already in the symbol table and return true or false accordingly  */

boolean wasDefined(char *sym, int lineCnt, boolean printErr, int pass)
{   
    int i;
    int *temp=symTab;
    for (i = 0; i < ((pass == 1)?(symSize-1):(symSize)); i++){
        if (strcmp(temp.symbol, sym) == 0){
            if(printErr)
                printf("error [line %d]: label has been already defined\n", lineCnt);
            return TRUE;
        }
        temp=temp.prev;
    }
    return FALSE;
}

/* definedAs(): the function gets as parameters a symbol name 
 and a directive type to search for in symbol-table 
 and the checks if the symbol with the matching directive is already in the symbol table
 and return true or false accordingly  */
static boolean definedAs(char *sym, char *attrib){
    int i;
    int *temp=symTab;
    boolean wasFound = FALSE;
    for (i = 0; i < symSize; i++){
        if (strcmp(temp.symbol, sym) == 0){
            wasFound = TRUE;
            if (strcmp(temp.attribute1, attrib) == 0/* || strcmp(symTab[i].attribute2, attrib) == 0*/)
                return TRUE;
        }
        temp=temp.prev;
    }
    if (!wasFound)
        return ERROR;
    return FALSE;
}


/* getFromSymTab(): searches for a matching symbol name in the symbol-table for a name
 and returns the row of the symbol. If the symbol is not defined in the symbol-table 
 a row will be returned with a symbol name is a NULL pointer.
 parameters: sym - the symbol name to search for 
 */
symbolTable getFromSymTab(char *sym){
    int i = 0;
    int *temp=symTab;
    symbolTable error;
    error.symbol = NULL;

    while (i < symSize){
        if (strcmp(temp.symbol, sym) == 0)
            return temp;
        i++;
        temp=temp.prev;
    }
    return error;
}

/* externalExist(): returns if there is an external variable in the symbol-table */
boolean entryExist(){
    int i;
    int *temp=symTab;
    for (i = 0; i < symSize; i++){
        if (temp.attribute2 != NULL){
            if(strcmp(temp.attribute2, "entry") == 0)
                return TRUE;
        }
        temp=temp.prev;
    }
    return FALSE;
}

/* externalExist(): returns if there is an external variable in the symbol-table */
boolean externalExist(){
    int i;
    int *temp=symTab;
    if(externals == NULL) /* there is no useage of external variables in the assembly code */
        return FALSE;
    for (i = 0; i < symSize; i++){
        if (strcmp(temp.attribute1, "external") == 0)
            return TRUE;
        temp=temp.prev;
    }
    return FALSE;
}

void pushEntryToFile(FILE *fd){
    int i;
    int *temp=symTab;
    for(i = 0; i < symSize; i++){
        if (temp.attribute2 != NULL){
            if (strcmp(temp.attribute2, "entry") == 0)
                fprintf(fd, "%s %d\n", temp.symbol, temp.address);
        }
        temp=temp.prev;
    }
    return;
}

void pushExternToFile(FILE *fd){
    int i;
    int *temp=symTab;
    for(i = 0; i < extSize; i++)
    {
        fprintf(fd, "%s %d\n", temp.name, temp.address);
        temp=temp.prev;
    }
        
    /*for(i = 0; i < symSize; i++){
        if (strcmp(symTab[i].attribute1, "external") == 0){
            for(j = 0; j < extSize; j++)
                fprintf(fd, "%s %d\n", externals[i].name, externals[i].address);
        }
    }*/
    return;
}

/* cleanSymAndExtTab(): frees the pointer to the symbol-table */
void cleanSymAndExtTab(){
    int i;
    /*if(symTab){
        for(i = 0; i < symSize-1; i++){
            free(symTab[i].attribute1);
            free(symTab[i].symbol);
        }
    }
    if(externals){
        for(i = 0; i < extSize; i++)
            free(externals[i].name);
    }*/
    externals = NULL;
    symTab = NULL;
    symSize = 0;
    extSize = 0;
    return;
}

void printSymTab(){
    int i;
    int *temp=symTab;
    for(i = 0; i < symSize; i++){
        printf("sym %d: %s , %d, %s %s\n", i, temp.symbol, temp.address, temp.attribute1, temp.attribute2);
        temp=temp.prev;
    }
}