#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

enum { BUCKET_COUNT = 509 };

struct Binding {
 char *key;
 const void *value;
 struct Binding *next;
};

/*should i make size size_t*/
struct SymTable {
 size_t size;
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

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount);
/* do we also free the table itself?*/
void SymTable_free(SymTable_T oSymTable){
  int i;
  assert(oSymTable != NULL);

  for(i = 0; i < oSymTable->bucketsNum; i++){
    struct Binding *current = oSymTable->buckets[i];
    if(current != NULL){
      struct Binding *pointer = current;
      while(pointer != NULL){
        struct Binding *temp = pointer;
        pointer = pointer->next;
        free(temp->key);
        free(temp);
      }
    }
  }
  free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
  assert(oSymTable != NULL);
  return oSymTable->size;
}

static SymTable_T SymTable_resize(SymTable_T oSymTable, int size);

int SymTable_put(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){
    size_t index;
    struct Binding *current;
    struct Binding *end;
    char *newKey;

    assert(oSymTable != NULL && pcKey != NULL);
    index = SymTable_hash(pcKey, oSymTable->bucketsNum);
    current = oSymTable->buckets[index];

    end = (struct Binding *) malloc(sizeof(struct Binding));
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


    if(current == NULL){
    oSymTable->buckets[index] = end;
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
    
    /*add resize function*/
    if(oSymTable->size > (size_t) oSymTable->bucketsNum 
    && oSymTable->bucketsNum != 65521){
      oSymTable = SymTable_resize(oSymTable, oSymTable->bucketsNum);
    }

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){
    size_t index;
    struct Binding *current;

    assert(oSymTable != NULL && pcKey != NULL);

    index = SymTable_hash(pcKey, oSymTable->bucketsNum);
    current = oSymTable->buckets[index];

    while(current != NULL){
      if(strcmp(current->key, pcKey) == 0){
        const void *temp = current->value;
        current->value = pvValue;
        return temp;
      }
      current = current->next;
    }
    return NULL;
}

/*same thing apply hashfunction*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
  size_t index;
  struct Binding *current;
  assert(oSymTable != NULL && pcKey != NULL);
  index = SymTable_hash(pcKey, oSymTable->bucketsNum);
  current = oSymTable->buckets[index];

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0) return 1;      
    current = current->next;
  }
  return 0;
}

/*apply hash function*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
  size_t index;
  struct Binding *current;
  assert(oSymTable != NULL && pcKey != NULL);
  index = SymTable_hash(pcKey, oSymTable->bucketsNum);
  current = oSymTable->buckets[index];

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0) return current->value;
    current = current->next;
  }
  return NULL;
}

/*update to use hashfunction*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    size_t index;
    struct Binding *current;
    struct Binding *before;

    assert(oSymTable != NULL && pcKey != NULL);
    index = SymTable_hash(pcKey, oSymTable->bucketsNum);
    current = oSymTable->buckets[index];

    if(current == NULL) return NULL;

    if(strcmp(current->key, pcKey) == 0){
      const void *val = current->value;
      struct Binding *after = current->next;
      oSymTable->buckets[index] = after;
      free(current->key);
      free(current);
      oSymTable->size -= 1;
      return val;
    }

    before = current;
    current = current->next;

    while(current != NULL){
      if(strcmp(current->key, pcKey) == 0){
        void *val = current->value;
        struct Binding *after = current->next;
        before->next = after;
        free(current->key);
        free(current);
        oSymTable->size -= 1;
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
    int i;
    assert(oSymTable != NULL && pfApply != NULL);

    for(i = 0; i < oSymTable->bucketsNum; i++){
      struct Binding *current = oSymTable->buckets[i];
      if(current != NULL){
        while(current != NULL){
          (*pfApply)(current->key, current->value, pvExtra);
          current = current->next;
        }
      }
    }
  }

static SymTable_T SymTable_resize(SymTable_T oSymTable, int size){
  SymTable_T newTable;
  int i;
  assert(oSymTable != NULL && size != NULL);

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

  for(i = 0; i < oSymTable->bucketsNum; i++){
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