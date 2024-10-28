#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

struct Node {
 char *key;
 void *value;
 struct Node *next;
};

struct SymTable{
   struct Node *first;
   int size;
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

int SymTable_put(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){
  assert(oSymTable != NULL && pcKey != NULL && pvValue != NULL);

  struct Node *point = oSymTable->first;
  struct Node *end;

  end = (struct Node *) malloc(sizeof(struct Node));
  if(end == NULL) return 0;
  char *newKey = (char *) malloc(strlen(pcKey) + 1);
  if(newKey == NULL){
    free(end);
    return 0;
  }
  strcpy(newKey, pcKey);
  end->key = newKey;
  end->value = pvValue;
  end->next = NULL;


  while(point->next != NULL){
    if(strcmp(point->key, pcKey) == 0) return 0;
    if(point->next == NULL){
      point->next = end;
      oSymTable->size += 1;
      return 1;
    }
    point = point->next;
  }
  /*do i free the old key?*/
  oSymTable->first = end;
  oSymTable->size += 1;
  return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){

  struct Node *point = oSymTable->first;

  assert(oSymTable != NULL && pcKey != NULL && pvValue != NULL);

  while(point != NULL){
    if(strcmp(point->key, pcKey) == 0){
      void *old = point->value;
      point->value = pvValue;
      return old;
    } 
    point = point->next;
  }
  return NULL;

}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
  struct Node *point = oSymTable->first;

  assert(oSymTable != NULL && pcKey != NULL);

  while(point != NULL){
    if(strcmp(point->key, pcKey) == 0) return 1;
    point = point->next;
  }
  return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
  struct Node *point = oSymTable->first;

  assert(oSymTable != NULL && pcKey != NULL);

  while(point != NULL){
    if(strcmp(point->key, pcKey) == 0) return point->value;
    point = point->next;
  }
  return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
  struct Node *current = oSymTable->first;

  assert(oSymTable != NULL && pcKey != NULL);

  if(current == NULL) return NULL;

  if(strcmp(current->key, pcKey) == 0){
    void *val = current->value;
    struct Node *after = current->next;
    free(current->key);
    free(current);
    oSymTable->first = after;
    return val;
  }

  struct Node *before = current;
  current = current->next;

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0){
      void *val = current->value;
      struct Binding *after = current->next;
      free(current->key);
      free(current);
      before->next = after;
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

  struct Node *point = oSymTable->first;

  assert(oSymTable != NULL && pvExtra != NULL && pfApply != NULL);

  while(point != NULL){
    (*pfApply)(point->key, point->value, pvExtra);
    point = point->next;
  }
  
}