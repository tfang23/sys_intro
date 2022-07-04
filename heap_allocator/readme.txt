File: readme.txt
Author: Tiantian Fang
----------------------

implicit
--------
I implemented the implicit allocator according to the requirements. I use first fit for mymalloc because it has reasonable utilization and pretty fast. I tried best fit at the beginning and then figured out that first fit is better in terms of utilization and speed. I use a linear myfree because it's very fast. Performance wise, I average 2831 instructions/request and 72% utilization for the sample tests. It does well with most of the scripts except for coalesce, realloc, and inplace and it's reasonable because I didn't implement any of these for implicit. I optimized using -O3 pretty aggressively, which works well for my implicit allocator. A fun anecdote: I tried to name my variables really nicely and took me a long time!

explicit
--------
I implemented the explicit allocator according to the requirements as well. I use first fit because it has very good utilization and very fast. I also use a linear myfree and coalesces to the first right block if it's free. The realloc has 3 scenarios: 1) resize to smaller and inplace realloc 2) try to get to as many free blocks to the right as possible and inplace realloc 3) malloc to another place. I average 44 instructions/request and 78% utlization which is pretty satisfying for me. Most of my tests got over 50%. I also optimized with -O3 pretty aggressively. A fun anecdote: actually not that fun, but it took me a long time to figure out the internal fragmentation and how it should be implemented. Anyways, it eventually worked!

Tell us about your quarter in CS107!
-----------------------------------
Thank you guys for this amazing quarter! I am particularly proud of the binary bomb because I didn't expect that I would be able to solve it.


