# Two-Level Segregated Fit Memory Allocator(TLSF)
This repository contains a high-performance, custom-built Two-Level Segregated Fit (TLSF) memory allocator implementation written in C++. The TLSF algorithm is optimized for low-latency, deterministic memory management, making it ideal for real-time and embedded systems.


## Design Overview

The TLSF allocator manages a pre-allocated memory pool, organizing it into a series of blocks. Each block is prefixed with a header and suffixed with a footer to store metadata such as size and allocation status. The memory pool itself is bounded by unallocatable sentinel blocks at the beginning and end, which act as boundary markers.

### **1. The Memory Pool Structure**

The entire memory pool is defined by special blocks at the beginning and end, which are not allocatable. These act as **sentinel blocks** or **boundary markers** for the allocator.


![Start-of-pool marker](diagrams/start-marker.png)

*The diagram above shows the structure of the start-of-pool marker.*


![End-of-pool marker](diagrams/end-marker.png)

*The diagram above shows the structure of the end-of-pool marker.*



#### **2. Typical Allocatable Block**

When memory is requested, the allocator finds or creates a block of the required size. This block is what the user receives a pointer to. A typical allocated block has the following structure:

* **Tlsf Header & Footer:** These contain critical metadata about the block, such as its size and state (allocated or free).
* **User Area:** This is the actual memory that the user's application can use.
* **Padding:** Padding is often added to ensure that the header, footer, and user area are correctly aligned in memory. This is crucial for performance and preventing issues on certain architectures.

![Typical Allocatable Block](diagrams/typical-allocatable-block.png)

### **3. The O(1) Lookup System**

The core of the TLSF allocator's speed is its O(1) lookup system, which relies on a two-level hierarchy of bitmaps and free lists. The system is based on the find-first-set (FFS) operation, which efficiently locates the first set bit in a bitmask.

- **First-Level (FL) Bitmap:** A bit is set if at least one free block exists in the corresponding size class.

- **Second-Level (SL) Bitmap:** For each first-level group, a second bitmap tracks free blocks in more granular subclasses.

By using a find-first-set (FFS) operation on these bitmaps, the allocator can locate the appropriate free list in constant time, allowing for extremely fast allocation and deallocation.

#### **Bitmaps for O(1) Lookup**
To quickly find a free block of a suitable size, the TLSF allocator uses two bitmaps. The Bitmaps are at the core of the allocator's performance. The indices we get from them are used to get the allocatable free block in O(1) time.

![Bitmaps for fast lookup](diagrams/bitmaps.png)


#### **Free List Array**

The actual free blocks are stored in a 2D array of pointers. The indices derived from the FL and SL bitmaps are used to directly access the head of the correct free list. This direct lookup is what provides the fast O(1) constant-time performance, as the allocator doesn't need to traverse a linked list to find a suitable block.

![bins containing the allocatable block](diagrams/bins.png)

## Core Operations
Allocation: When a memory request comes in, the allocator calculates the required FL and SL indices based on the requested size. It then uses the bitmaps to find the smallest available free block that can satisfy the request. The selected block is removed from its free list, marked as allocated, and potentially split if it's much larger than the requested size.

Deallocation: When a block is freed, the allocator marks it as free and checks its adjacent blocks. If an adjacent block is also free, the allocator coalesces (merges) the two blocks into a single, larger free block. This process helps to combat external fragmentation, where memory is available but scattered in many small, unusable chunks. The newly coalesced block is then inserted back into the appropriate free list based on its new, larger size.

## Key Features

* **Fast Allocation:** The two-level segregated list structure allows for quick searching of free blocks.
* **Hierarchical Free Lists:** Uses a two-level bitmap and a 2D array of pointers to quickly locate free blocks based on their size.
* **Minimal Fragmentation:** Adjacent free blocks are automatically coalesced (merged) to reduce external fragmentation.
* **Custom Memory Management:** Handles memory blocks with custom headers and footers to manage block metadata, including size and status.

## Performance Metrics

## Prerequisites


## Build Instructions
Clone the repository:


## Usage Example
Here is a simple example demonstrating how to initialize the allocator and manage memory.


## License
This project is licensed under the Apache-2.0 license License - see the LICENSE file for details.

## Acknowledgements

Thanks for these awesome resources that were used during the development
