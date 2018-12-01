#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include "simulator.h"

int main () {
    cout << "----------------------------------------Cache Simulator----------------------------------------" << endl << endl;
    // Simulator cs(1, true, true, "1 32 8192 0", "");
    // Simulator cs(2, false, true, "1 32 8192 0 2 32 8192 0", "8 32 32768 0");
    Simulator cs(2, false, false, "1 64 8192 1 4 64 16384 1", "8 64 32768 1 16 64 65536 1");
    clock_t t;
    t = clock();
    ifstream file;
    file.open("/Users/will/Documents/550/LAB/CacheSimulator/DineroFull.din.txt");
    char line[10];
    cout << "Calculating..." << endl << endl;
    while (file.getline(line, 10)) {
        cs.process(string(line));
    }
    file.close();       
    t = clock() - t;
    cs.printResult();
    cout << "It took me " << ((float)t)/CLOCKS_PER_SEC << " seconds." << endl;
    return 0;
}
