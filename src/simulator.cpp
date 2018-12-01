#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include "simulator.h"

int main() {
    std::cout << "Cache Simulator" << std::endl;
    // simulator cs(1, true, true, "1 32 8192 0", "", 1, 10, 1000);
    // simulator cs(2, false, true, "2 64 32768 0 2 64 32768 0", "8 64 524288 1", 1, 10, 1000);
    simulator cs(2, false, false, "1 64 8192 1 4 64 16384 1", "8 64 32768 1 16 64 65536 1", 1, 10, 1000);
    ifstream file;
    file.open("/Users/will/Documents/550/LAB/CacheSimulator/DineroFull.din.txt");
    char line[10];
    int cnt = 0;
    clock_t t;
    t = clock();
    cout << "Calculating..." << endl;
    while(file.getline(line, 10)){
        cs.process(string(line));
        if (cnt++ % 1000 == 0)
            cout << cnt / 1000 + 1 << "k completed" << endl;
    }       
    t = clock() - t;
    cout << "It took me " << t << " clicks (" << ((float)t)/CLOCKS_PER_SEC << " seconds)" << endl;
    file.close();
    cs.printResult();
}
