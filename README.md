# Project 1

- Name: Laura Loughmiller
- Email: laurajones2@u.boisestate.edu
- Class: 452-001

## Known Bugs or Issues

No known Bugs or Issues

## Experience

Well it's done. Hopefully I didn't booby trap p2. Here is a quick break down of what cost me the most time.

### Problem: I needed to test allocation-failure branches
but lab.c was compiled in a separate translation unit, so my test-side #define ALLOC=... didn’t affect it. Including lab.c in tests caused duplicate symbols because the Makefile already compiled it.

### Solution: hooks
In lab.h: define AllocFn/FreeFn and extern AllocFn lab_alloc_fn; extern FreeFn lab_free_fn;, then
     #define ALLOC(sz)   (lab_alloc_fn ? lab_alloc_fn(sz) : malloc(sz))
     #define DESTROY(p)  do { if (lab_free_fn) lab_free_fn(p); else free(p); } while (0)
In lab.c: define the globals once:
     AllocFn lab_alloc_fn = NULL;
     FreeFn  lab_free_fn  = NULL;
In tests (setUp): point hooks to the test doubles:
    lab_alloc_fn = test_alloc;
    lab_free_fn  = test_destroy;

#### Why it works: No Makefile changes. 
lab.c still compiles normally, but at runtime the tests can inject a failing allocator. In production, hooks are NULL → real malloc/free.

Used a smoke test to confirm the hook is hit (alloc count ≥ 2 in list_create), then forced failures (alloc_fail_after = 1/2) to cover error paths in list_create, list_append, and list_insert.