#include <cstdlib>

class Block {
public:
    bool valid;
    bool dirty;
    size_t lastAccessTime;
    size_t tag;
    Block() : valid(false), dirty(false), lastAccessTime(0), tag(0) {};
};
