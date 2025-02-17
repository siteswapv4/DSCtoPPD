#include "ArrayList.h"

int increaseArrayList(ArrayList* array)
{
    array->size += ARRAYLIST_SIZE;
    array->data = SDL_realloc(array->data, array->size * sizeof(void*));

    return 0;
}

ArrayList* newArrayList()
{
    ArrayList* array = SDL_malloc(sizeof(ArrayList));

    array->data = SDL_malloc(ARRAYLIST_SIZE * sizeof(void*));
    array->size = ARRAYLIST_SIZE;
    array->len = 0;

    return array;
}

int arrayListAdd(ArrayList* array, void* data)
{
    if (array->size == array->len)
    {
        increaseArrayList(array);
    }

    array->data[array->len] = data;
    array->len++;

    return 0;
}

int arrayListAddAt(ArrayList* array, void* data, int index)
{
    if (array->size == array->len)
    {
        increaseArrayList(array);
    }

    for (int i = array->len; i > index; i--)
    {
        array->data[i] = array->data[i-1];
    }

    array->data[index] = data;
    array->len++;

    return 0;
}

int arrayListRemove(ArrayList* array, void* data)
{
    for (int i = 0; i < array->len; i++)
    {
        if (array->data[i] == data)
        {
            for (int j = i + 1; j < array->len; j++)
            {
                array->data[j - 1] = array->data[j];
            }

            array->len--;

            return 0;
        }
    }

    return 1;
}

void* arrayListRemoveAt(ArrayList* array, int index)
{
    void* data = array->data[index];
    
    for (int i = index + 1; i < array->len; i++)
    {
        array->data[i - 1] = array->data[i];
    }

    array->len--;

    return data;
}

int freeArrayList(ArrayList* array, void (*freeFunc)(void*))
{
    if (freeFunc != NULL)
    {
        for (int i = 0; i < array->len; i++)
        {
            freeFunc(array->data[i]);
        }
    }

    SDL_free(array->data);
    SDL_free(array);

    return 0;
}