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
