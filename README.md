# Project 2

- Name: Laura Loughmiller
- Email: laurajones2@u.boisestate.edu
- Class: 452-001

## Known Bugs or Issues
No known Bugs or Issues

## Experience
It's done here's a break down of my approach

### Goal: implement multithreaded sorting
I needed to split the list into halves, spawn two threads to sort each half on the same list, merge the two halves afterwards, and not use locks/semaphores or other synchronized primitices.

### Challegnes:
Safely partitioning the list into [0,mid] and [mid, n] ranges without the threads stepping on each other.
Passing arguments into pthread_create cleanly (start, index, end index, comparator, and the shared list)
Handling both integers and randomly generated string while keeping the code generic

### Solution: Careful index partitioning
I created a SortArgs struct with fields for list(the shared linked list), start and end indicies for the subrange. cmp function pointer for either ints or strings.
Each thread calls list_sort(list, start, end, cmp) Because their index ranges don't overlap, no synchronization primitives were needed. 
For mergering: I pulled the second half [mid,n] into a tempoary list, called list_merge(list, right,cmp) to combine them into one fully sorted list, verified final sorted order with is_sorted and printed the results

#### Why it works: 
Threads never touch the same nodes, so no corruption without mutexes!
Generic compartor function makes the same logic work for both ints and strings
By moving the second half into a temp list, the merge step reuses the existing list_merge function safely

## Usage
To build: make all
To run ints: ./build/release/myapp int 100
To run strings: ./build/release/myapp string 100
To run the script to stress test: 
./scripts/run_many.sh