# Two-Level Segregated Fit Memory Allocator(TLSF)
This project is a custom Two-Level Segregated Fit (TLSF) memory allocator implementation written in C++. TLSF is a high-performance dynamic memory allocator designed for environments where low-latency and deterministic performance are critical, such as embedded systems and real-time applications.

The allocator manages a pre-allocated memory pool and organizes free memory blocks using a two-level hierarchy of free lists. The core logic for segregating, coalescing, and managing memory is implemented to ensure high efficiency.


## Design

The Tlsf allocator organizes memory into a pool and manages it using a system of blocks. Each block contains metadata (headers and footers) to track its size and status.

#### The Memory Pool Structure

The entire memory pool is defined by special blocks at the beginning and end, which are not allocatable. These act as **sentinel blocks** or **boundary markers** for the allocator.


![Start-of-pool marker](diagrams/start-marker.png)

*The diagram above shows the structure of the start-of-pool marker.*


![End-of-pool marker](diagrams/end-marker.png)

*The diagram above shows the structure of the end-of-pool marker.*

#### Typical Allocatable Block

When memory is requested, the allocator finds or creates a block of the required size. This block is what the user receives a pointer to. A typical allocated block has the following structure:

![Typical Allocatable Block](diagrams/typical-allocatble-block.png)

* **Tlsf Header & Footer:** These contain critical metadata about the block, such as its size and state (allocated or free).
* **User Area:** This is the actual memory that the user's application can use.
* **Padding:** Padding is often added to ensure that the header, footer, and user area are correctly aligned in memory. This is crucial for performance and preventing issues on certain architectures.

## Key Features

* **Fast Allocation:** The two-level segregated list structure allows for quick searching of free blocks.
* **Hierarchical Free Lists:** Uses a two-level bitmap and a 2D array of pointers to quickly locate free blocks based on their size.
* **Minimal Fragmentation:** Adjacent free blocks are automatically coalesced (merged) to reduce external fragmentation.
* **Custom Memory Management:** Handles memory blocks with custom headers and footers to manage block metadata, including size and status.

