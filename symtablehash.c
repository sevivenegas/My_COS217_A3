/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Sevastian Venegas                                          */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/*BUCKET_COUNT is starting size of Hash Table*/
enum { BUCKET_COUNT = 509 };

/*A Binding is a pair of key and value which is setup to be a linked 
list (within a bucket of SymTable) with Binding *next pointing to 
following Binding*/
struct Binding {
  /*key used to identify Binding*/
  char *key;
  /*value of Binding*/
  const void *value;
  /*pointer pointing to next Binding in linked list*/
  struct Binding *next;
};

/*A SymTable is series key value pair Bindings sorted by hash values 
from their key into respective "buckets". Within the buckets are linked
lists for Bindings sharing the same hash.*/
struct SymTable {
  /*size is number of key value pairs or bindings*/
  size_t size;
  /*bucketsNum is number of buckets or binding pointers in SymTable*/
  int bucketsNum;
  /*buckets is an array of binding pointers with this being the initial
  pointer*/
 struct Binding **buckets;
}; 

/*Return a hash code for pcKey that is between 0 and uBucketCount-1,
inclusive.*/
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount);

/*Returns a pointer to a new SymTable which is an expanded version of 
oSymTable*/
static SymTable_T SymTable_resize(SymTable_T oSymTable);

SymTable_T SymTable_new(void){

  SymTable_T table;

  table = (SymTable_T) malloc(sizeof(struct SymTable));
  if(table == NULL) return NULL;

  /*only allocates memory for array of pointers not for actual bindings.
  This will be done as we put Bindings*/
  table->buckets = calloc(BUCKET_COUNT, sizeof(struct Binding *));
  if(table->buckets == NULL){
    free(table);
    return NULL;
  }
  table->size = 0;
  table->bucketsNum = BUCKET_COUNT;
  return table;
}

static void SymTable_freeInside(SymTable_T oSymTable){
  int i;

  assert(oSymTable != NULL);

  for(i = 0; i < oSymTable->bucketsNum; i++){
    struct Binding *current = oSymTable->buckets[i];
    if(current != NULL){
      struct Binding *pointer = current;
      while(pointer != NULL){
        /* temp is temporary only used to free Binding*/
        struct Binding *temp = pointer;
        pointer = pointer->next;
        /*frees key and Binding, values untouched*/
        free(temp->key);
        free(temp);
      }
    }
  }
  /*frees all pointers and table*/
  free(oSymTable->buckets);
}

void SymTable_free(SymTable_T oSymTable){
  assert(oSymTable != NULL);
  SymTable_freeInside(oSymTable);
  free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
  assert(oSymTable != NULL);
  return oSymTable->size;
}

int SymTable_put(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){
    size_t index;
    struct Binding *current;
    struct Binding *end;
    char *newKey;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey, (size_t) oSymTable->bucketsNum);
    current = oSymTable->buckets[index];

    if(current != NULL) {
      /*updates current until we are at the last Binding and 
      checks if there is duplicate key*/
      while(current->next != NULL){
        if(strcmp(current->key, pcKey) == 0) return 0;
        current = current->next;
      }
      /*special case where table only has one binding and
      there is an attempt to add binding with same key*/
      if(strcmp(current->key, pcKey) == 0) return 0;
    }

    /*creates a new Binding end which will be potentially 
    added to end of linked list*/
    end = (struct Binding *) malloc(sizeof(struct Binding));
    if(end == NULL) return 0;
    /*defensive copy of key*/
    newKey = (char *) malloc(sizeof(char) * (strlen(pcKey) + 1));
    if(newKey == NULL){
      free(end);
      return 0;
    } 
    /*assigns values of Binding end*/
    strcpy(newKey, pcKey);
    end->key = newKey;
    end->value = pvValue;
    end->next = NULL;

    /*adds end as first Binding if list is currently empty*/
    if(current == NULL) oSymTable->buckets[index] = end;
    /*adds end Binding to end of linked list*/
    else current->next = end;
    oSymTable->size += 1;
    
    /*resizes symtable if there are certain number of 
    bindings compared to number of buckets*/
    if((oSymTable->size > (size_t) oSymTable->bucketsNum)
    && (oSymTable->bucketsNum != 65521)){
      SymTable_T temp = SymTable_resize(oSymTable);
      if(temp != NULL) *oSymTable = *temp;
      SymTable_free(temp);
    }

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
  const char *pcKey, const void *pvValue){

    size_t index;
    struct Binding *current;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey, (size_t) oSymTable->bucketsNum);
    current = oSymTable->buckets[index];

    while(current != NULL){
      if(strcmp(current->key, pcKey) == 0){
        void *temp = (void *) current->value;
        current->value = pvValue;
        return temp;
      }
      current = current->next;
    }
    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){

  size_t index;
  struct Binding *current;

  assert(oSymTable != NULL);
  assert(pcKey != NULL);

  index = SymTable_hash(pcKey, (size_t) oSymTable->bucketsNum);
  current = oSymTable->buckets[index];

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0) return 1;      
    current = current->next;
  }
  return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){

  size_t index;
  struct Binding *current;

  assert(oSymTable != NULL);
  assert(pcKey != NULL);

  index = SymTable_hash(pcKey, (size_t) oSymTable->bucketsNum);
  current = oSymTable->buckets[index];

  while(current != NULL){
    if(strcmp(current->key, pcKey) == 0) return (void *) current->value;
    current = current->next;
  }
  return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){

    size_t index;
    struct Binding *current;
    struct Binding *before;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey, (size_t) oSymTable->bucketsNum);
    current = oSymTable->buckets[index];

    /*empty table nothing to remove*/
    if(current == NULL) return NULL;

    /*if the starting Binding needs to be removed*/
    if(strcmp(current->key, pcKey) == 0){
      void *val = (void *) current->value;
      struct Binding *after = current->next;
      /*updates starting Binding*/
      oSymTable->buckets[index] = after;
      free(current->key);
      free(current);
      oSymTable->size -= 1;
      return val;
    }

    /*before keeps track of previous Binding to update pointer next
    when current is removed*/
    before = current;
    current = current->next;

    while(current != NULL){
      if(strcmp(current->key, pcKey) == 0){
        /*connects Bindings after removal*/
        void *Oldval = (void *) current->value;
        struct Binding *after = current->next;
        before->next = after;
        /*frees key and Binding, values untouched*/
        free(current->key);
        free(current);
        oSymTable->size -= 1;
        return Oldval;
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

    assert(oSymTable != NULL); 
    assert(pfApply != NULL);

    for(i = 0; i < oSymTable->bucketsNum; i++){
      struct Binding *current = oSymTable->buckets[i];
      if(current != NULL){
        while(current != NULL){
          (*pfApply)(current->key, (void *) current->value, (void *) pvExtra);
          current = current->next;
        }
      }
    }
  }

static SymTable_T SymTable_resize(SymTable_T oSymTable){
  SymTable_T newTable;
  int i;
  int size;

  assert(oSymTable != NULL);
  size = oSymTable->bucketsNum;

  newTable = (SymTable_T) malloc(sizeof(struct SymTable));
  if(newTable == NULL) return NULL;

  /*determines size of newTable based on sizes and conditions given
  in assignments*/
  if (size == 509) size = 1021;
  else if(size == 1021) size = 2039;
  else if (size == 2039) size = 4093;
  else if (size == 4093) size = 8191;
  else if (size == 8191) size = 16381;
  else if (size == 16381) size = 32749;
  else size = 65521;

  newTable->buckets = (struct Binding **) calloc(size, sizeof(struct Binding *));
  if(newTable->buckets == NULL){
    free(newTable);
    return NULL;
  } 

  newTable->bucketsNum = size;
  newTable->size = 0;

  /*re-adds all bindings from previous table into new table*/
  for(i = 0; i < oSymTable->bucketsNum; i++){
    struct Binding *current = oSymTable->buckets[i];
    if(current != NULL){
      while(current != NULL){
        SymTable_put(newTable, current->key, current->value);
        current = current->next;
      }
    }
  }
  /*completely frees all memory associated with old table*/
  SymTable_freeInside(oSymTable);
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