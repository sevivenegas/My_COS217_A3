#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

enum { BUCKET_COUNT = 509 };

struct Binding {
 char *key;
 void *value;
 struct Binding *next;
};

/*should i make size size_t*/
struct SymTable {
 int size;
 int bucketsNum;
 struct Binding **buckets;
}; 

SymTable_T SymTable_new(void){
  SymTable_T table;
  table = (SymTable_T) malloc(sizeof(struct SymTable));
  if(table == NULL) return NULL;

  table->buckets = calloc(BUCKET_COUNT, sizeof(struct Binding));
  if(table->buckets == NULL){
    free(table);
    return NULL;
  }
  table->size = 0;
  table->bucketsNum = BUCKET_COUNT;
  return table;
}
/* do we also free the table itself?*/
void SymTable_free(SymTable_T oSymTable){
  for(int i = 0; i < oSymTable->bucketsNum; i++){
    struct Binding *current = oSymTable->buckets[i];
    if(current != NULL){
      struct Binding *pointer = current;
      while(pointer != NULL){
        struct Binding *temp = pointer;
        pointer = pointer->next;
        free(temp);
      }
    }
    free(oSymTable->buckets[i]);
  }
  free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
  return oSymTable->size;
}

int SymTable_put(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){
    size_t index = SymTable_hash(pvValue, oSymTable->bucketsNum);
    struct Binding *current = oSymTable->buckets[index];
    struct Binding *before = NULL;

    while(current != NULL){
      if(strcmp(current->key, pcKey) == 0){
        return 0;
      }
      if(current->next == NULL) before = current;
      current = current->next;
    }
    struct Binding *tail;
    tail = malloc(sizeof(struct Binding));
    if(tail == NULL) return 0;
    tail->key = malloc(strlen(pcKey) + 1);
    if(tail->key == NULL) return 0;
    strcpy(tail->key, pcKey);
    tail->value = pvValue;
    tail->next = NULL;
    if(before == NULL) oSymTable->buckets[index] = tail;
    else before->next = tail;
    oSymTable->size += 1;
    
    /*add resize function*/
    if(oSymTable->size > oSymTable->bucketsNum 
    && oSymTable->bucketsNum != 65521){
      oSymTable = SymTable_resize(oSymTable, oSymTable->bucketsNum);
    }

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){
    size_t index = SymTable_hash(pcKey, oSymTable->bucketsNum);
    struct Binding *current = oSymTable->buckets[index];
    if(current == NULL) return NULL;

    while(current != NULL){
      if(strcmp(current->key, pcKey) == 0){
        void *temp = current->value;
        current->value = pvValue;
        return temp;
      }
      current = current->next;
    }
    return NULL;
}

/*same thing apply hashfunction*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
  size_t index = SymTable_hash(pcKey, oSymTable->bucketsNum);
  struct Binding *current = oSymTable->buckets[index];
  if(current == NULL) return 0;

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0) return 1;      
    current = current->next;
  }
  return 0;
}

/*apply hash function*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
  size_t index = SymTable_hash(pcKey, oSymTable->bucketsNum);
  struct Binding *current = oSymTable->buckets[index];
  if(current == NULL) return NULL;

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0) return current->value;
          current = current->next;
  }
  return NULL;
}

/*update to use hashfunction*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    size_t index = SymTable_hash(pcKey, oSymTable->bucketsNum);
    struct Binding *current = oSymTable->buckets[index];
    if(current == NULL) return NULL;

    if(strcmp(current->key, pcKey) == 0){
      void *val = current->value;
      struct Binding *after = current->next;
      free(current->key);
      free(current);
      oSymTable->buckets[index] = after;
      return val;
    }
    
    struct Binding *before = current;
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
    for(int i = 0; i < oSymTable->bucketsNum; i++){
      struct Binding *current = oSymTable->buckets[i];
      if(current != NULL){
        while(current != NULL){
          (*pfApply)(current->key, current->value, pvExtra);
          current = current->next;
        }
      }
    }
    return NULL;
  }

static SymTable_T SymTable_resize(SymTable_T oSymTable, int size){
  SymTable_T newTable;
  newTable = (SymTable_T) malloc(sizeof(struct SymTable));
  if(newTable == NULL) return oSymTable;

  if (size == 509) size = 1021;
  else if(size == 1021) size = 2039;
  else if (size == 2039) size = 4093;
  else if (size == 4093) size = 8191;
  else if (size == 8191) size = 16381;
  else if (size == 16381) size = 32749;
  else size = 65521;

  newTable->buckets = (struct Binding **) calloc(size, sizeof(struct Binding));
  if(newTable->buckets == NULL) return oSymTable;

  for(int i = 0; i < oSymTable->bucketsNum; i++){
    struct Binding *current = oSymTable->buckets[i];
    if(current != NULL){
      while(current != NULL){
        SymTable_put(newTable, current->key, current->value);
      }
    }
  }
  SymTable_free(oSymTable);
  return newTable;
}

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}