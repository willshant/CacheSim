#include <vector>
#include "block.h"

using namespace std;

class Cache {
public:
    size_t associativity;
	size_t blockSize;
	size_t capacity;
	size_t nsets;
	vector<vector<Block *> *> * myCache;
	size_t nfetch, nread, nwrite;
	size_t nfetch_hit, nread_hit, nwrite_hit;
	size_t nfetch_miss, nread_miss, nwrite_miss;

	size_t currentTime;
	size_t strategy;   // 0 for RND, 1 for LRU
	
	Cache (size_t A, size_t B, size_t C, size_t strategy) {
		associativity = A;
		blockSize = B;
		capacity = C;
		nfetch = nread = nwrite = 0;
		nfetch_hit = nread_hit = nwrite_hit = 0;
		nfetch_miss = nread_miss = nwrite_miss = 0;
		currentTime = 0;
		this->strategy = strategy;
		
		nsets = capacity / (blockSize * associativity);
		
		myCache = new vector<vector<Block *> *>;
		for (size_t i = 0; i < nsets; i++) {
            vector<Block *> * way = new vector<Block *>;
            for(size_t j = 0; j < associativity; j++) {
                way->push_back(new Block());
			}
            myCache->push_back(way);
		}
	}
    // ~cache(); // need to free memory
};
