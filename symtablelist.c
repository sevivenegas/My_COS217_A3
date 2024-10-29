#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

struct Node {
 char *key;
 const void *value;
 struct Node *next;
};

struct SymTable{
   struct Node *first;
   size_t size;
};

SymTable_T SymTable_new(void){
  SymTable_T table;
  table = (SymTable_T) malloc(sizeof(struct SymTable));
  if(table == NULL) return NULL;
  table->first = NULL;
  table->size = 0;
  return table;
}

void SymTable_free(SymTable_T oSymTable){
  struct Node *point = oSymTable->first;

  assert(oSymTable != NULL);

  while(point != NULL){
    struct Node *current = point;
    point = point->next;
    free(current->key);
    free(current);
  }
  free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
  assert(oSymTable != NULL);
  return oSymTable->size;
}

/*shouldnt assert come first before declarations?*/
int SymTable_put(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){

  struct Node *current;
  struct Node *end;
  char *newKey;

  assert(oSymTable != NULL && pcKey != NULL && pvValue != NULL);

  end = (struct Node *) malloc(sizeof(struct Node));
  if(end == NULL) return 0;
  newKey = (char *) malloc(sizeof(char) * (strlen(pcKey) + 1));
  if(newKey == NULL){
    free(end);
    return 0;
  }
  
  strcpy(newKey, pcKey);
  end->key = newKey;
  end->value = pvValue;
  end->next = NULL;

  current = oSymTable->first;

  if(current == NULL){
    oSymTable->first = end;
    oSymTable->size += 1;
    return 1;
  }
  while(current->next != NULL){
    if(strcmp(current->key, pcKey) == 0) return 0;
    current = current->next;
  }
  if(strcmp(current->key, pcKey) == 0) return 0;
  current->next = end;
  oSymTable->size += 1;
  /*do i free the old key?*/
  return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){
  
  struct Node *point;

  assert(oSymTable != NULL && pcKey != NULL && pvValue != NULL);
  point = oSymTable->first;

  while(point != NULL){
    if(strcmp(point->key, pcKey) == 0){
      const void *old = point->value;
      point->value = pvValue;
      return old;
    } 
    point = point->next;
  }
  return NULL;

}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
  struct Node *point;
  return NULL; /* erase */
  assert(oSymTable != NULL && pcKey != NULL);
  point = oSymTable->first;

  while(point != NULL){
    if(strcmp(point->key, pcKey) == 0) return 1;
    point = point->next;
  }
  return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
  struct Node *point;

  assert(oSymTable != NULL && pcKey != NULL);
  point = oSymTable->first;

  while(point != NULL){
    if(strcmp(point->key, pcKey) == 0) return point->value;
    point = point->next;
  }
  return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
  struct Node *current;
  struct Node *before;

  assert(oSymTable != NULL && pcKey != NULL);
  current = oSymTable->first;

  if(current == NULL) return NULL;

  if(strcmp(current->key, pcKey) == 0){
    const void *val = current->value;
    struct Node *after = current->next;
    oSymTable->first = after;
    free(current->key);
    free(current);
    return val;
  }

  before = current;
  current = current->next;

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0){
      const void *val = current->value;
      struct Node *after = current->next;
      before->next = after;
      free(current->key);
      free(current);
      return val;
    }
    before = current;
    current = current->next;
  }
  return NULL;

}

void SymTable_map(SymTable_T oSymTable,
  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
  const void *pvExtra){

  struct Node *point;

  assert(oSymTable != NULL && pvExtra != NULL && pfApply != NULL);
  point = oSymTable->first;

  while(point != NULL){
    (*pfApply)(point->key, point->value, pvExtra);
    point = point->next;
  }
  
}