# NexOS - Predictive Memory Management System

## Overview

NexOS is a high-performance, lock-free memory management system that predicts memory pressure and proactively manages resources before the system runs out of memory. Unlike traditional Linux systems that rely on the OOM killer (reactive), NexOS uses statistical prediction to take preventive action.

## Key Innovation

Instead of waiting for the OOM killer to terminate processes, NexOS predicts memory exhaustion through a probabilistic model:

```
S = pow(p / (100.0 - p), 3.9)

if (S > 10)  → prefetch_cache(key)    // Prepare for pressure
if (S > 100) → pin_in_ram(key)        // Lock critical data
```

Where `p` is the current memory pressure percentage (0-100).

## Project Structure

```
NexOS/
├── README.md              # Project documentation
├── Makefile              # Build configuration
├── src/
│   ├── mathelisophe.h   # Lock-free data structures (header)
│   ├── mathelisophe.c   # Lock-free implementation
│   └── main.c           # Main entry point with demo
└── bench/
    └── results.txt      # Benchmark results
```

## Building

```bash
make build
```

## Running

```bash
make run
```

## Benchmarking

```bash
make bench
```

## Architecture

- **Lock-free**: Uses atomic operations for thread-safe memory access
- **Predictive**: Calculates pressure score before critical conditions
- **Adaptive**: Scales response based on severity (prefetch vs pin)
- **Efficient**: Minimal overhead for memory prediction

## Memory Pressure Stages

| Pressure (p) | Score (S) | Action | Threshold |
|---|---|---|---|
| < 50% | < 10 | Normal operation | - |
| 50-75% | 10-100 | Prefetch critical data | S > 10 |
| > 75% | > 100 | Pin to RAM | S > 100 |

## License

GNU General Public License v3.0
