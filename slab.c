#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SLAB_SIZE 1024
#define NUM_SLABS 10
#define MIN_CHUNK_SIZE 32

typedef struct chunk_header
{
    struct chunk_header *next;
    bool free;
} chunk_header_t;

typedef struct slab
{
    void *mem;
    chunk_header_t *free_list;
} slab_t;

slab_t slabs[NUM_SLABS];

void init_slabs()
{
    int i;
    for (i = 0; i < NUM_SLABS; i++)
    {
        slabs[i].mem = malloc(SLAB_SIZE);
        chunk_header_t *chunk = (chunk_header_t *)slabs[i].mem;
        chunk->next = NULL;
        chunk->free = true;
        slabs[i].free_list = chunk;
    }
}

void *alloc(int size)
{
    int i;
    int chunk_size = MIN_CHUNK_SIZE;
    while (chunk_size < size)
    {
        chunk_size *= 2;
    }
    for (i = 0; i < NUM_SLABS; i++)
    {
        if (chunk_size <= SLAB_SIZE - sizeof(chunk_header_t))
        {
            if (slabs[i].free_list != NULL)
            {
                chunk_header_t *chunk = slabs[i].free_list;
                slabs[i].free_list = chunk->next;
                chunk->free = false;
                return (void *)((char *)chunk + sizeof(chunk_header_t));
            }
        }
    }
    return NULL;
}

void dealloc(void *ptr)
{
    int i;
    for (i = 0; i < NUM_SLABS; i++)
    {
        if ((ptr > slabs[i].mem) && (ptr < (void *)((char *)slabs[i].mem + SLAB_SIZE)))
        {
            chunk_header_t *chunk = (chunk_header_t *)((char *)ptr - sizeof(chunk_header_t));
            chunk->free = true;
            chunk->next = slabs[i].free_list;
            slabs[i].free_list = chunk;
            return;
        }
    }
}

int main()
{
    init_slabs();

    int *a = (int *)alloc(sizeof(int));
    if (a == NULL)
    {
        printf("Failed to allocate memory\n");
        return 1;
    }
    *a = 10;
    printf("a = %d\n", *a);

    char *b = (char *)alloc(12 * sizeof(char));
    if (b == NULL)
    {
        printf("Failed to allocate memory\n");
        return 1;
    }
    strcpy(b, "Hello world");
    printf("b = %s\n", b);

    dealloc(a);
    dealloc(b);

    return 0;
}
