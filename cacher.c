#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

typedef struct {
    char key[1024];
    int clock_bit;
} CacheEntry;

typedef struct CacheNode {
    char key[1024];
    struct CacheNode *next;
} CacheNode;

typedef struct {
    CacheEntry *entries;
    CacheNode *head;
    CacheNode *tail;
    int front;
    int rear;
    int size;
    int compulsory_misses;
    int capacity_misses;
    int clock_pointer;
} Cache;

void initializeCache(Cache *cache, int size) {
    cache->entries = (CacheEntry *) malloc(size * sizeof(CacheEntry));
    cache->front = 0;
    cache->rear = -1;
    cache->size = size;

    for (int i = 0; i < size; i++) {
        strcpy(cache->entries[i].key, "");
        cache->entries[i].clock_bit = 0;
    }

    cache->head = NULL;
    cache->tail = NULL;
    cache->compulsory_misses = 0;
    cache->capacity_misses = 0;
    cache->clock_pointer = 0;
}

void printCache(Cache *cache) {
    for (int i = 0; i < cache->size; i++) {
        int index = (cache->front + i) % cache->size;
        printf("%s, ", cache->entries[index].key);
    }
    printf("\n");
}

int lookup(Cache *cache, const char *key) {
    for (int i = 0; i < cache->size; i++) {
        if (strcmp(cache->entries[i].key, key) == 0) {
            return i;
        }
    }
    return -2;
}

int isFull(Cache *cache) {
    return ((cache->rear + 1) % cache->size) == cache->front && cache->rear != -1;
}

void LLInsert(Cache *cache, const char *key) {
    CacheNode *newNode = (CacheNode *) malloc(sizeof(CacheNode));
    strcpy(newNode->key, key);
    newNode->next = NULL;

    if (cache->head == NULL) {
        cache->head = newNode;
        cache->tail = newNode;
    } else {
        cache->tail->next = newNode;
        cache->tail = newNode;
    }
}

int LLLookup(const Cache *cache, const char *key) {
    CacheNode *current = cache->head;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void fifo(Cache *cache, const char *key) {
    int index = lookup(cache, key);
    if (index != -2) {
        printf("HIT\n");
    } else {
        printf("MISS\n");

        if (isFull(cache)) {
            if (LLLookup(cache, key)) {
                cache->capacity_misses += 1;
            } else {
                cache->compulsory_misses += 1;
            }
            strcpy(cache->entries[cache->front].key, "");
            cache->front = (cache->front + 1) % cache->size;
            cache->rear = (cache->rear + 1) % cache->size;
            strcpy(cache->entries[cache->rear].key, key);
        } else {
            cache->rear = (cache->rear + 1) % cache->size;
            strcpy(cache->entries[cache->rear].key, key);
            cache->compulsory_misses += 1;
        }
    }
}

void lru(Cache *cache, const char *key) {
    int index = lookup(cache, key);

    if (index != -2) {
        printf("HIT\n");
        strcpy(cache->entries[index].key, "");
        int cindex = (index + cache->front) % cache->size;

        for (int i = cindex; i < cache->size - 1; i++) {
            int j = (cache->front + i) % cache->size;
            strcpy(cache->entries[j].key, cache->entries[j + 1].key);
        }

        strcpy(cache->entries[cache->rear].key, key);
    } else {
        printf("MISS\n");

        if (isFull(cache)) {
            if (LLLookup(cache, key)) {
                cache->capacity_misses += 1;
            } else {
                cache->compulsory_misses += 1;
            }
            strcpy(cache->entries[cache->front].key, "");
            cache->front = (cache->front + 1) % cache->size;
            cache->rear = (cache->rear + 1) % cache->size;
            strcpy(cache->entries[cache->rear].key, key);
        } else {
            cache->rear = (cache->rear + 1) % cache->size;
            strcpy(cache->entries[cache->rear].key, key);
            cache->compulsory_misses += 1;
        }
    }
}

void clockEvict(Cache *cache, const char *key) {
    int index = lookup(cache, key);
    if (index != -2) {
        printf("HIT\n");
        cache->entries[index].clock_bit = 1;

    } else {
        printf("MISS\n");
        if (LLLookup(cache, key)) {
            cache->capacity_misses += 1;
        } else {
            cache->compulsory_misses += 1;
        }
        while (1) {
            if (cache->entries[cache->clock_pointer].clock_bit == 0) {
                strcpy(cache->entries[cache->clock_pointer].key, key);
                cache->clock_pointer = (cache->clock_pointer + 1) % cache->size;
                return;

            } else {
                cache->entries[cache->clock_pointer].clock_bit = 0;
                cache->clock_pointer = (cache->clock_pointer + 1) % cache->size;
            }
        }
    }
}

void enqueue(Cache *cache, const char *key, char eviction_policy) {
    switch (eviction_policy) {
    case 'F': fifo(cache, key); break;
    case 'L': lru(cache, key); break;
    case 'C': clockEvict(cache, key); break;
    default: break;
    }
}

void freeCache(Cache *cache) {
    free(cache->entries);
    CacheNode *current = cache->head;

    while (current != NULL) {
        CacheNode *temp = current;
        current = current->next;
        free(temp);
    }

    cache->head = NULL;
    cache->tail = NULL;
}

int main(int argc, char *argv[]) {
    int cache_size = 0;
    char eviction_policy = 'F';
    int opt;

    while ((opt = getopt(argc, argv, "N:FLC")) != -1) {
        switch (opt) {
        case 'N': cache_size = atoi(optarg); break;
        case 'F': break;
        case 'L': eviction_policy = opt; break;
        case 'C': eviction_policy = opt; break;
        default:
            if (argc > 3) {
                fprintf(stderr, "Usage: %s [-N size] <policy>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        }
        if (argc < 3) {
            fprintf(stderr, "Usage: %s [-N size] <policy>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (cache_size <= 0) {
        fprintf(stderr, "Invalid cache size\n");
        exit(EXIT_FAILURE);
    }

    Cache cache;
    initializeCache(&cache, cache_size);
    char input[1024];

    while (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = '\0';
        enqueue(&cache, input, eviction_policy);
        LLInsert(&cache, input);
    }

    printf("%d %d\n", cache.compulsory_misses, cache.capacity_misses);

    freeCache(&cache);
    return 0;
}
