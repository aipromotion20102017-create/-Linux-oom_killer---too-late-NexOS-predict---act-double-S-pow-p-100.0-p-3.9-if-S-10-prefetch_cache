#ifndef MATHELISOPHE_H
#define MATHELISOPHE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Mathelisophe - Lock-free data structures for NexOS
 * 
 * Ultra-senior level lock-free implementation using atomic operations.
 * No mutexes, no spinlocks - pure compare-and-swap (CAS) semantics.
 */

/* Atomic memory pressure predictor */
struct pressure_state {
    volatile double memory_pressure;      /* 0-100 percentage */
    volatile double last_score;           /* Last calculated S value */
    volatile uint64_t prediction_count;   /* Atomic counter */
};

/* Lock-free key-value cache entry */
struct cache_entry {
    uint64_t key;
    void *value;
    size_t size;
    volatile uint32_t pinned;     /* 1 if pinned in RAM */
    volatile uint32_t prefetched; /* 1 if prefetched */
};

/* Pressure calculator interface */
double calculate_pressure_score(double memory_pressure);
bool should_prefetch(double score);
bool should_pin(double score);

/* Cache operations */
struct cache_entry* cache_entry_create(uint64_t key, void *value, size_t size);
void cache_entry_prefetch(struct cache_entry *entry);
void cache_entry_pin(struct cache_entry *entry);
void cache_entry_destroy(struct cache_entry *entry);

/* Pressure state management */
struct pressure_state* pressure_state_create(void);
void pressure_state_update(struct pressure_state *state, double memory_pressure);
void pressure_state_destroy(struct pressure_state *state);

/* Utility */
double get_system_memory_pressure(void);

#endif // MATHELISOPHE_H
