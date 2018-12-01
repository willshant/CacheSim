#include <cstdlib>

class Block {
public:
    bool valid;
    bool dirty;
    int lastAccessTime;
    long tag;   // changed by zx  size_t to long
    size_t recentUseIndex;
    Block() : valid(false), dirty(false), lastAccessTime(0), tag(0), recentUseIndex(0){};
};
/*
It took me 12308501 clicks (12.3085 seconds)
7458670 demand fetches for L1 cache
38238 demand misses for L1 cache
The total demand miss rate for L1 cache is 0.00512665
Instruction cache level 1:
5689551 times of instruction demand fetches
155 times of instruction demand misses
The instruction demand miss rate is 2.72429e-05
Data cache level 1:
1769119 times of data demand fetches
38083 times of data demand misses
The data demand miss rate is 0.0215265
1145629 times of data reads
34969 times of data read misses
The data read miss rate is 0.0305238
623490 times of data writes
3114 times of data write misses
The data write miss rate is 0.00499447
41337 demand fetches for L2 cache
16179 demand misses for L2 cache
The total demand miss rate for L2 cache is 0.00512665
Instruction cache level 2:
155 times of instruction demand fetches
144 times of instruction demand misses
The instruction demand miss rate is 0.929032
Data cache level 2:
41182 times of data demand fetches
16035 times of data demand misses
The data demand miss rate is 0.389369
38083 times of data reads
15880 times of data read misses
The data read miss rate is 0.416984
3099 times of data writes
155 times of data write misses
The data write miss rate is 0.0500161
The average access time for L1 is 3.0578
The average access time for L2 is 401.393
*/