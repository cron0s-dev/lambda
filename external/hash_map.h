#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct Entry {
    char *key;
    void *value;

    struct Entry *next;
} Entry;

typedef struct {
    Entry **buckets;
    size_t capacity;
    size_t size;
} HashMap;

static uint64_t hm_hash(const char *key);
HashMap *hm_init(size_t capacity);
void hm_free(HashMap *map);
int hm_grow(HashMap *map);
bool hm_ins(HashMap *map, const char *key, void *value);
void *hm_get(const HashMap *map, const char *key);
bool hm_rm(HashMap *map, const char *key);

#ifdef HM_IMPLEMENTATION

static uint64_t hm_hash(const char *key)
{
    if (!key)
        return 0;

    uint64_t hash = FNV_OFFSET;
    for (const char *p = key; *p; ++p) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }

    return hash;
}

HashMap *hm_init(size_t capacity)
{
    if (!capacity)
        return NULL;

    HashMap *map = malloc(sizeof(*map));
    if (!map)
        return NULL;

    map->buckets = calloc(capacity, sizeof(*map->buckets));
    if (!map->buckets) {
        free(map);
        return NULL;
    }

    map->capacity = capacity;
    map->size = 0;
    return map;
}

void hm_free(HashMap *map)
{
    if (!map)
        return;

    for (size_t i = 0; i < map->capacity; ++i) {
        Entry * entry = map->buckets[i];
        while (entry) {
            Entry *next = entry->next;

            free(entry->key);
            free(entry);

            entry = next;
        }
    }

    free(map->buckets);
    free (map);
}

int hm_grow(HashMap *map)
{
    size_t new_capacity = map->capacity * 2;
    Entry **buckets = calloc(new_capacity, sizeof(*map->buckets));
    if (!buckets)
        return -1;

    for (size_t i = 0; i < map->capacity; ++i) {
        Entry *entry = map->buckets[i];

        while(entry) {
            Entry *next = entry->next;

            size_t idx = hm_hash(entry->key) % new_capacity;

            entry->next = buckets[idx];
            buckets[idx] = entry;

            entry = next;
        }
    }

    free(map->buckets);
    map->buckets = buckets;
    map->capacity = new_capacity;

    return 0;
}

bool hm_ins(HashMap *map, const char *key, void *value)
{
    if (!map || !key || !value)
        return false;

    if (map->size == map->capacity) {
        if (hm_grow(map) != 0)
            return false;
    }

    size_t idx = hm_hash(key) % map->capacity;

    for (Entry *e = map->buckets[idx]; e; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            e->value = value;
            return true;
        }
    }

    Entry *entry = malloc(sizeof(*entry));
    if (!entry)
        return false;

    entry->key = strdup(key);
    entry->value = value;

    entry->next = map->buckets[idx];
    map->buckets[idx] = entry;

    ++map->size;

    return true;
}

void *hm_get(const HashMap *map, const char *key)
{
    if (!map || !key)
        return NULL;

    size_t idx = hm_hash(key) % map->capacity;

    for (const Entry *e = map->buckets[idx]; e; e = e->next) {
        if (strcmp(e->key, key) == 0)
            return e->value;
    }

    return NULL;
}

bool hm_rm(HashMap *map, const char *key)
{
    if (!map || !key)
        return false;

    size_t idx = hm_hash(key) % map->capacity;

    Entry *prev = NULL;
    Entry *curr = map->buckets[idx];

    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev)
                prev->next = curr->next;
            else
                map->buckets[idx] = curr->next;
            
            free(curr->key);
            free(curr);

            --map->size;
            return true;
        }

        prev = curr;
        curr = curr->next;
    }

    return false;
}

#endif

#endif
