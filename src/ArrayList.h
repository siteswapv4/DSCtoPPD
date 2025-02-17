#pragma once
#include <SDL3/SDL.h>

/* Default arrayList size, modify it to fit your needs */
#define ARRAYLIST_SIZE 1000

/* Structure containing all arrayList information, access the fields directly to get the data */
typedef struct
{
    void** data;
    int size;
    int len;
}ArrayList;

/* Allocates a new arrayList of ARRAYLIST_SIZE on the heap
 * Always call FreeArrayList after use
 */
ArrayList* newArrayList();

/* Adds the data at the end of the data array, resizes if necessary */
int arrayListAdd(ArrayList* array, void* data);

/* Adds the data at the specified index, resizes if necessary, index must be valid (> 0) */
int arrayListAddAt(ArrayList* array, void* data, int index);

/* Finds and removes the data, shrinks the array */
int arrayListRemove(ArrayList* array, void* data);

/* Removes the data at specified index, shrinks the array, returns the data */
void* arrayListRemoveAt(ArrayList* array, int index);

/* Frees an arrayList, if freeFunc != NULL it'll be called on each element of the data array */
int freeArrayList(ArrayList* array, void (*freeFunc)(void*));
