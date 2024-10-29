/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Sevastian Venegas                                          */
/*--------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/* A Node is a pair of key and value which is setup to be a linked list
with Node *next pointing to following Node*/
struct Node {
  /*key used to identify Node*/
  char *key;
  /*value of Node*/
  const void *value;
  /*pointer pointing to next node in linked list*/
  struct Node *next;
};

/*A SymTable is series key value pairs in Nodes represented as a 
linked list*/
struct SymTable{
  /*first points to first Node in linked list*/
  struct Node *first;
  /*size is number of key value pairs or Nodes*/
  size_t size;
};

SymTable_T SymTable_new(void){
  SymTable_T table;
  table = (SymTable_T) malloc(sizeof(struct SymTable));
  if(table == NULL) return NULL;
  /*sets table to an empty symtable*/
  table->first = NULL;
  table->size = 0;
  return table;
}

void SymTable_free(SymTable_T oSymTable){

  struct Node *current;

  assert(oSymTable != NULL);
  current = oSymTable->first;

  while(current != NULL){
    /* temp is temporary only used to free node*/
    struct Node *temp = current;
    current = current->next;

    /*frees key and node, values untouched*/
    free(temp->key);
    free(temp);
  }
  free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
  assert(oSymTable != NULL);
  return oSymTable->size;
}

int SymTable_put(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){

  struct Node *current;
  struct Node *end;
  char *newKey;

  assert(oSymTable != NULL);
  assert(pcKey != NULL);

  current = oSymTable->first;
  if(current != NULL) {
    /*updates current until we are at the last node and 
    checks if there is duplicate key*/
    while(current->next != NULL){
      if(strcmp(current->key, pcKey) == 0) return 0;
      current = current->next;
    }
    /*special case where table only has one binding and
    there is an attempt to add binding with same key*/
    if(strcmp(current->key, pcKey) == 0) return 0;
  }

  /*creates a new node end which will be potentially 
  added to end of linked list*/
  end = (struct Node *) malloc(sizeof(struct Node));
  if(end == NULL) return 0;
  /*defensive copy of key*/
  newKey = (char *) malloc(sizeof(char) * (strlen(pcKey) + 1));
  if(newKey == NULL){
    free(end);
    return 0;
  }
  strcpy(newKey, pcKey);
  end->key = newKey;
  end->value = pvValue;
  end->next = NULL;

  /*adds end node to end of linked list*/
  current->next = end;
  oSymTable->size += 1;
  return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){
  
  struct Node *current;

  assert(oSymTable != NULL && pcKey != NULL);
  current = oSymTable->first;

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0){
      void *oldValue = (void *) current->value;
      current->value = pvValue;
      return oldValue;
    } 
    current = current->next;
  }
  return NULL;

}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
  struct Node *current;

  assert(oSymTable != NULL && pcKey != NULL);
  current = oSymTable->first;

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0) return 1;
    current = current->next;
  }
  return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
  struct Node *current;

  assert(oSymTable != NULL && pcKey != NULL);
  current = oSymTable->first;

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0) return (void *)current->value;
    current = current->next;
  }
  return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
  struct Node *current;
  struct Node *before;

  assert(oSymTable != NULL && pcKey != NULL);
  current = oSymTable->first;

  /*empty table nothing to remove*/
  if(current == NULL) return NULL;

  if(strcmp(current->key, pcKey) == 0){
    void *val = (void *) current->value;
    struct Node *after = current->next;
    oSymTable->first = after;
    free(current->key);
    free(current);
    oSymTable->size -= 1;
    return val;
  }

  before = current;
  current = current->next;

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0){
      void *val = (void *) current->value;
      struct Node *after = current->next;
      before->next = after;
      oSymTable->size -= 1;

       /*frees key and node, values untouched*/
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

  struct Node *current;

  assert(oSymTable != NULL && pfApply != NULL);
  current = oSymTable->first;

  while(current != NULL){
    (*pfApply)(current->key, (void *) current->value, (void *) pvExtra);
    current = current->next;
  }
  
}