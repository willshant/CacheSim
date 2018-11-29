#include <iostream>
#include <fstream>
#include <string>

#include "simulator.h"


int main() {
    
    std::cout << "Hello Easy C++ project!" << std::endl;
    // simulator cs(1, true, true, "1 32 8192 0", "", 0, 0, 0);
    simulator cs(2, false, true, "2 64 32768 0 2 64 32768 0", "8 64 524288 1", 0, 0, 0);
    ifstream file;
    file.open("/Users/will/Documents/550/LAB/CacheSimulator/DineroFull.din.txt");
    char line[10];
    int cnt = 0;
    while(file.getline(line, 10)){
        cs.process(string(line));
        if (cnt++ % 1000 == 0)
            cout << cnt / 1000 << "k completed" << endl;
    }
    file.close();
    cs.printResult();
    
}
