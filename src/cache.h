#include <vector>
#include "block.h"

using namespace std;

class Cache {
public:
    int associativity;
	int blockSize;
	int capacity;
	int nsets;
	vector<vector<Block *> *> * myCache;
	int nfetch, nread, nwrite;
	int nfetch_hit, nread_hit, nwrite_hit;
	int nfetch_miss, nread_miss, nwrite_miss;

	int currentTime;
	int strategy;   // 0 for RND, 1 for LRU
	int level;
	
    // cache();
	Cache (int A, int B, int C, int strategy, int level) {
		associativity = A;
		blockSize = B;
		capacity = C;
		nfetch = nread = nwrite = 0;
		nfetch_hit = nread_hit = nwrite_hit = 0;
		nfetch_miss = nread_miss = nwrite_miss = 0;
		currentTime = 0;
		this->strategy = strategy;
		this->level = level;
		
		nsets = capacity / (blockSize * associativity);
		
		myCache = new vector<vector<Block *> *>;
		for (int i = 0; i < nsets; i++) {
            vector<Block *> * way = new vector<Block *>;
            for(int j = 0; j < associativity; j++) {
                way->push_back(new Block());
                //myCache[i][j] = new cacheBlock();
			}
            myCache->push_back(way);
		}
	}
    // ~cache();
};
