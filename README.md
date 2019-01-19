# Fast BF #

Compiles BF to C code, then uses gcc to produce an executable. This is part of a larger collection of applications designed to study the computational behavior of BF programs.

The chosen variety of BF uses 32-bit unsigned integer cells and an infinitely expandable tape in either direction. Most programs which don't abuse 8-bit cells run perfectly well.
    
------------------------------------------------

### Usage ###
    
    ./bf hello.bf hello.c

produces `hello.c` from a BF program `hello.bf`

    ./bf hello.bf -r
    
automatically **runs** the BF program as a solitary executable.

---------------

### Optimizations ###

Currently implemented:
* Converting repeated instructions such as `>>>` to `ptr += 3;`
* `[-]` to `*ptr = 0;`

---------------

### What's next? ###

* `[-]+++` to `*p = 3;`
* Instant copies: `[->>...>>+<<...<<]` = `ptr[n] = *ptr, *ptr = 0;`
