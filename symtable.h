/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Sevastian Venegas                                          */
/*--------------------------------------------------------------------*/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stddef.h>

typedef struct SymTable* SymTable_T;

/*SymTable_new creates and returns a new SymTable object that contains 
no bindings, or returns NULL if insufficient memory is available.*/
SymTable_T SymTable_new(void);

/*SymTable_free frees all memory occupied by oSymTable.*/
void SymTable_free(SymTable_T oSymTable);

/*SymTable_getLength returns a size_t of the number of bindings in 
oSymTable.*/
size_t SymTable_getLength(SymTable_T oSymTable);

/*SymTable_put adds a new binding containing key pcKey and value pvValue
to oSymTable and returns 1 (TRUE).Otherwise the function returns 
0 (FALSE) if a binding with pcKey already exits or insufficient memory 
is available*/
int SymTable_put(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue);

/*SymTable_replace searches for a binding with a key matching pcKey in 
oSymtable and replaces the binding's value with pvValue and returns the
old value. If oSymTable does not contain a matching binding it leaves
oSymTable unchanged and returns NULL.*/
void *SymTable_replace(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue);

/*SymTable_contains returns 1 (TRUE) if oSymTable contains a binding
with a matching key to pcKey, and 0 (FALSE) otherwise*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*SymTable_get returns the value of the binding within oSymTable whose 
key matches pcKey, or NULL if no binding exists.*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*SymTable_remove removes and frees memory associated with the binding 
containing key matching pcKey from oSymTable and return the binding's 
value. If no binding exists or otherwise the function leaves oSymTable
unchanged and returns NULL.*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*SymTable_map applys function *pfApply to each binding in oSymTable,
passing pvExtra as a parameter.*/
void SymTable_map(SymTable_T oSymTable,
  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
  const void *pvExtra);

#endif