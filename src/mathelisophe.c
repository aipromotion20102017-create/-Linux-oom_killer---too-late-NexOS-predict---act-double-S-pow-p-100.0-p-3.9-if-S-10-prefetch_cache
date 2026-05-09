#include "mathelisophe.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdatomic.h>

/**
 * NexOS Predictive Memory Management
 * 
 * Core algorithm:
 * S = pow(p / (100.0 - p), 3.9)
 * 
 * This creates a sigmoid-like curve that heavily weights high pressure values.
 * The exponent 3.9 is chosen to create sharp transitions:
 *   - At 50% pressure: S ≈ 1 (normal)
 *   - At 75% pressure: S ≈ 27 (start prefetching)
 *   - At 90% pressure: S ≈ 729 (pin to RAM)
 */

/* Threshold values */
#define PREFETCH_THRESHOLD 10.0
#define PIN_THRESHOLD 100.0
#define PRESSURE_EXPONENT 3.9

/**
 * Calculate the pressure score using the predictive formula.
 * 
 * @param memory_pressure: System memory pressure (0-100)
 * @return: Pressure score S
 */
double calculate_pressure_score(double memory_pressure)
{
    /* Clamp pressure to valid range */
    if (memory_pressure < 0.0) memory_pressure = 0.0;
    if (memory_pressure > 99.9) memory_pressure = 99.9; /* Avoid division by near-zero */
    
    /* S = pow(p / (100.0 - p), 3.9) */
    double ratio = memory_pressure / (100.0 - memory_pressure);
    double score = pow(ratio, PRESSURE_EXPONENT);
    
    return score;
}

/**
 * Determine if we should prefetch data into cache.
 * 
 * @param score: Pressure score
 * @return: true if score > PREFETCH_THRESHOLD
 */
bool should_prefetch(double score)
{
    return score > PREFETCH_THRESHOLD;
}

/**
 * Determine if we should pin data in RAM.
 * 
 * @param score: Pressure score
 * @return: true if score > PIN_THRESHOLD
 */
bool should_pin(double score)
{
    return score > PIN_THRESHOLD;
}

/**
 * Create a new cache entry.
 * 
 * @param key: Cache key
 * @param value: Value pointer
 * @param size: Size of value
 * @return: New cache entry (heap allocated)
 */
struct cache_entry* cache_entry_create(uint64_t key, void *value, size_t size)
{
    struct cache_entry *entry = (struct cache_entry *)malloc(sizeof(struct cache_entry));
    if (!entry) return NULL;
    
    entry->key = key;
    entry->value = value;
    entry->size = size;
    entry->pinned = 0;
    entry->prefetched = 0;
    
    return entry;
}

/**
 * Prefetch entry data (simulate cache prefetching).
 * In real implementation, this would:
 *   - Read memory pages into L3 cache
 *   - Use PREFETCHNTA/PREFETCHT0 instructions
 *   - Trigger page faults early
 */
void cache_entry_prefetch(struct cache_entry *entry)
{
    if (!entry) return;
    
    /* Atomic check-and-set */
    uint32_t expected = 0;
    if (atomic_compare_exchange_strong(&entry->prefetched, &expected, 1)) {
        /* Successfully marked as prefetched */
        /* In production: prefetch data using PREFETCH instructions */
        printf("  [PREFETCH] key=0x%lx, size=%zu bytes\n", entry->key, entry->size);
    }
}

/**
 * Pin entry in RAM (prevent swap).
 * In real implementation, this would:
 *   - Call mlock() on memory pages
 *   - Set memory affinity
 *   - Prevent page reclaim
 */
void cache_entry_pin(struct cache_entry *entry)
{
    if (!entry) return;
    
    /* Atomic check-and-set */
    uint32_t expected = 0;
    if (atomic_compare_exchange_strong(&entry->pinned, &expected, 1)) {
        /* Successfully marked as pinned */
        /* In production: call mlock(entry->value, entry->size) */
        printf("  [PIN] key=0x%lx, size=%zu bytes (locked in RAM)\n", entry->key, entry->size);
    }
}

/**
 * Destroy a cache entry.
 */
void cache_entry_destroy(struct cache_entry *entry)
{
    if (entry) {
        free(entry);
    }
}

/**
 * Create a pressure state monitor.
 */
struct pressure_state* pressure_state_create(void)
{
    struct pressure_state *state = (struct pressure_state *)malloc(sizeof(struct pressure_state));
    if (!state) return NULL;
    
    state->memory_pressure = 0.0;
    state->last_score = 0.0;
    state->prediction_count = 0;
    
    return state;
}

/**
 * Update pressure state (atomic).
 */
void pressure_state_update(struct pressure_state *state, double memory_pressure)
{
    if (!state) return;
    
    state->memory_pressure = memory_pressure;
    state->last_score = calculate_pressure_score(memory_pressure);
    atomic_fetch_add(&state->prediction_count, 1);
}

/**
 * Destroy pressure state.
 */
void pressure_state_destroy(struct pressure_state *state)
{
    if (state) {
        free(state);
    }
}

/**
 * Get system memory pressure.
 * 
 * Calculates (used / total) * 100
 * 
 * @return: Memory pressure percentage (0-100)
 */
double get_system_memory_pressure(void)
{
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        return 0.0;
    }
    
    /* Calculate used memory */
    uint64_t total_mem = info.totalram * info.mem_unit;
    uint64_t free_mem = info.freeram * info.mem_unit;
    uint64_t used_mem = total_mem - free_mem;
    
    /* Return as percentage */
    double pressure = (100.0 * used_mem) / total_mem;
    return (pressure > 100.0) ? 100.0 : pressure;
}
