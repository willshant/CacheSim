#include <cstdlib>

class Block {
public:
    bool valid;
    bool dirty;
    size_t tag;
    size_t recentUseIndex;
    Block() : valid(false), dirty(false), tag(0), recentUseIndex(0) {};
};