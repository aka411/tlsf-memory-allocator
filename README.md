# Two-Level Segregated Fit Memory Allocator(TLSF)
This project is a custom Two-Level Segregated Fit (TLSF) memory allocator implementation written in C++. TLSF is a high-performance dynamic memory allocator designed for environments where low-latency and deterministic performance are critical, such as embedded systems and real-time applications.

The allocator manages a pre-allocated memory pool and organizes free memory blocks using a two-level hierarchy of free lists. The core logic for segregating, coalescing, and managing memory is implemented to ensure high efficiency.

## Key Features


Hierarchical Free Lists: Uses a two-level bitmap and a 2D array of pointers to quickly locate free blocks based on their size.

Block Coalescing: Coalesces adjacent free memory blocks by merging them into a single, larger block. This is a critical feature that helps prevent external fragmentation over time.

Custom Memory Management: Handles memory blocks with custom headers and footers to manage block metadata, including size and status.

