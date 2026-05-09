#include "mathelisophe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * NexOS - Predictive Memory Management Demo
 * 
 * This program demonstrates the pressure prediction algorithm
 * and shows when prefetching and pinning actions are triggered.
 */

static void print_header(void)
{
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║            NexOS - Predictive Memory Management                ║\n");
    printf("║  Algorithm: S = pow(p/(100-p), 3.9)                           ║\n");
    printf("║  Action: if (S > 10) prefetch; if (S > 100) pin               ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
}

static void print_pressure_table(void)
{
    printf("Pressure Threshold Table:\n");
    printf("┌─────────────────┬──────────────┬──────────────┬────────────┐\n");
    printf("│ Pressure (%%)    │ Score (S)    │ Prefetch?    │ Pin?       │\n");
    printf("├─────────────────┼──────────────┼──────────────┼────────────┤\n");
    
    double pressures[] = {10, 20, 30, 40, 50, 60, 70, 75, 80, 85, 90, 95};
    int count = sizeof(pressures) / sizeof(pressures[0]);
    
    for (int i = 0; i < count; i++) {
        double p = pressures[i];
        double s = calculate_pressure_score(p);
        const char *prefetch = should_prefetch(s) ? "YES ✓" : "no";
        const char *pin = should_pin(s) ? "YES ✓" : "no";
        
        printf("│ %6.1f         │ %12.2f │ %-12s │ %-10s │\n", p, s, prefetch, pin);
    }
    
    printf("└─────────────────┴──────────────┴──────────────┴────────────┘\n\n");
}

static void demo_cache_management(void)
{
    printf("=== Cache Management Demo ===\n\n");
    
    /* Create pressure monitor */
    struct pressure_state *pressure = pressure_state_create();
    if (!pressure) {
        fprintf(stderr, "Failed to create pressure state\n");
        return;
    }
    
    /* Create sample cache entries */
    struct cache_entry *entries[5];
    for (int i = 0; i < 5; i++) {
        entries[i] = cache_entry_create(0x1000 + i, malloc(4096), 4096);
    }
    
    /* Simulate increasing memory pressure */
    double pressures[] = {30, 50, 70, 80, 95};
    
    for (int p_idx = 0; p_idx < 5; p_idx++) {
        double p = pressures[p_idx];
        printf("\nMemory Pressure: %.1f%%\n", p);
        printf("─────────────────────────────\n");
        
        pressure_state_update(pressure, p);
        double score = pressure->last_score;
        
        printf("Pressure Score (S): %.2f\n", score);
        printf("Status: ");
        if (should_pin(score)) {
            printf("CRITICAL - Pin to RAM\n");
        } else if (should_prefetch(score)) {
            printf("HIGH - Prefetch Data\n");
        } else {
            printf("Normal\n");
        }
        printf("\nActions:\n");
        
        /* Apply actions to cache entries */
        if (should_pin(score)) {
            for (int i = 0; i < 5; i++) {
                cache_entry_pin(entries[i]);
            }
        } else if (should_prefetch(score)) {
            for (int i = 0; i < 3; i++) {
                cache_entry_prefetch(entries[i]);
            }
        } else {
            printf("  [NO ACTION] System operating normally\n");
        }
    }
    
    /* Cleanup */
    for (int i = 0; i < 5; i++) {
        if (entries[i]->value) free(entries[i]->value);
        cache_entry_destroy(entries[i]);
    }
    pressure_state_destroy(pressure);
}

static void benchmark_pressure_calculation(void)
{
    printf("\n=== Pressure Calculation Benchmark ===\n\n");
    
    /* Warm up */
    for (int i = 0; i < 1000; i++) {
        calculate_pressure_score(50.0);
    }
    
    /* Benchmark */
    clock_t start = clock();
    int iterations = 1000000;
    
    for (int i = 0; i < iterations; i++) {
        double p = (double)(i % 100);
        calculate_pressure_score(p);
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    double ns_per_calc = (elapsed * 1e9) / iterations;
    
    printf("Iterations: %d\n", iterations);
    printf("Time elapsed: %.3f seconds\n", elapsed);
    printf("Time per calculation: %.2f ns\n", ns_per_calc);
    printf("Throughput: %.2f M ops/sec\n\n", iterations / (elapsed * 1e6));
}

int main(int argc, char *argv[])
{
    print_header();
    
    /* Check for benchmark flag */
    bool run_bench = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--bench") == 0) {
            run_bench = true;
        }
    }
    
    /* Show pressure thresholds */
    print_pressure_table();
    
    /* Run demo */
    demo_cache_management();
    
    /* Run benchmark if requested */
    if (run_bench) {
        benchmark_pressure_calculation();
    }
    
    printf("\n✓ NexOS demo complete\n\n");
    return 0;
}
