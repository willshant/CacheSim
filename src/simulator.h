#include <vector>
#include "cache.h"

using namespace std;

#define Read 0
#define Write 1
#define Fetch 2
#define Random 0
#define RLU 1

class simulator {
public:
    vector<Cache> L1Cache;

};