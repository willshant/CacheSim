#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cfloat> // FLT_EPSILON

#include "cache.h"

using namespace std;

#define Read 0
#define Write 1
#define Fetch 2
#define Random 0
#define RLU 1

class Simulator {
    vector<Cache *> L1Cache;
    vector<Cache *> L2Cache;
    bool L1_unified, L2_unified;
    size_t nlevels;
public:
    Simulator (size_t nlevels, bool L1_unified, bool L2_unified, string L1, string L2) {
        this->nlevels = nlevels;
        this->L1_unified = L1_unified;
        if (L1_unified) {
            stringstream temp;
            temp.str(L1);  //convert string to stringstream
            string configuration;
            vector<string> L1_configuration;
            while (getline(temp, configuration, ' ')) {
                L1_configuration.push_back(configuration);
            }
            size_t L1_A = atoi(L1_configuration[0].c_str());
            size_t L1_B = atoi(L1_configuration[1].c_str());
            size_t L1_C = atoi(L1_configuration[2].c_str());
            size_t L1_strategy = atoi(L1_configuration[3].c_str());
            L1Cache.push_back(new Cache(L1_A, L1_B, L1_C, L1_strategy));  // right ?? -- probably...
        } else {
            stringstream temp;
            temp.str(L1);  //convert string to stringstream
            string configuration;
            vector<string> L1_configuration;
            while (getline(temp, configuration, ' ')) {
                L1_configuration.push_back(configuration);
            }
            size_t L1_I_A = atoi(L1_configuration[0].c_str());
            size_t L1_I_B = atoi(L1_configuration[1].c_str());
            size_t L1_I_C = atoi(L1_configuration[2].c_str());
            size_t L1_I_strategy = atoi(L1_configuration[3].c_str());
            size_t L1_D_A = atoi(L1_configuration[4].c_str());
            size_t L1_D_B = atoi(L1_configuration[5].c_str());
            size_t L1_D_C = atoi(L1_configuration[6].c_str());
            size_t L1_D_strategy = atoi(L1_configuration[7].c_str());
            L1Cache.push_back(new Cache(L1_I_A, L1_I_B, L1_I_C, L1_I_strategy));
            L1Cache.push_back(new Cache(L1_D_A, L1_D_B, L1_D_C, L1_D_strategy));
        }
        if (nlevels > 1) {
            if (L2_unified) {
                stringstream temp;
                temp.str (L2);  //convert string to stringstream
                string configuration;
                vector<string> L2_configuration;
                while (getline(temp, configuration, ' ')) {
                    L2_configuration.push_back(configuration);
                }
                size_t L2_A = atoi(L2_configuration[0].c_str());
                size_t L2_B = atoi(L2_configuration[1].c_str());
                size_t L2_C = atoi(L2_configuration[2].c_str());
                size_t L2_strategy = atoi(L2_configuration[3].c_str());
                L2Cache.push_back(new Cache(L2_A, L2_B, L2_C, L2_strategy));
            } else {
                stringstream temp;
                temp.str (L2);  //convert string to stringstream
                string configuration;
                vector<string> L2_configuration;
                while (getline(temp, configuration, ' ')) {
                    L2_configuration.push_back(configuration);
                }
                size_t L2_I_A = atoi(L2_configuration[0].c_str());
                size_t L2_I_B = atoi(L2_configuration[1].c_str());
                size_t L2_I_C = atoi(L2_configuration[2].c_str());
                size_t L2_I_strategy = atoi(L2_configuration[3].c_str());
                size_t L2_D_A = atoi(L2_configuration[4].c_str());
                size_t L2_D_B = atoi(L2_configuration[5].c_str());
                size_t L2_D_C = atoi(L2_configuration[6].c_str());
                size_t L2_D_strategy = atoi(L2_configuration[3].c_str());
                L2Cache.push_back(new Cache(L2_I_A, L2_I_B, L2_I_C, L2_I_strategy));
                L2Cache.push_back(new Cache(L2_D_A, L2_D_B, L2_D_C, L2_D_strategy));
            }
            this->L2_unified = L2_unified;
        }
    }
    void process(string s) {
        stringstream temp;
        temp.str (s);  //convert string to stringstream
        string element;
        vector<string> ss;
        while (getline(temp, element, ' ')) {
            ss.push_back(element);
        }
        size_t code = atoi(ss[0].c_str());
        size_t address = strtol(ss[1].c_str(), NULL, 16);
        if(code == Read) {
            if(L1_unified) {
                read(address, 0, L1Cache[0]);
            } else {
                read(address, 0, L1Cache[1]);
            }
        } else if (code == Write) {
            if(L1_unified) {
                write(address, 0, L1Cache[0]);
            } else {
                write(address, 0, L1Cache[1]);
            }
        } else {
            fetch(address, 0, L1Cache[0]);
        }
    }
    void fetch(size_t address, size_t level, Cache * C) {
        C->nfetch++;
        size_t a = address / C->blockSize;
        size_t tag = a / C->nsets;
        size_t index = a % C->nsets;
        for (size_t col = 0 ; col < C->associativity ; col ++) {
            Block * data = C->myCache->operator[]((size_t)index)->operator[](col);
            if (data->valid && data->tag == tag) {
                C->nfetch_hit ++;
                data->lastAccessTime = C->currentTime;
                break;
            } else if (C->associativity == 1) {
                C->nfetch_miss++;
                if (level == 0 && nlevels > 1) {
                    fetch(address,1,L2Cache[0]);
                }
                data->tag = tag;
                data->valid = true;
                data->dirty = false;
                data->lastAccessTime = C->currentTime;
                break;
            } else if (col == C->associativity - 1) {
                C->nfetch_miss++;
                size_t col_evicted;
                Block * data_evicted;
                if (C->strategy == Random) {
                    col_evicted = getRandomIndex(C->associativity);
                    data_evicted = C->myCache->operator[]((size_t)index)->operator[](col_evicted);
                } else {
                    col_evicted = getLRUIndex(C->myCache->operator[]((size_t)index));
                    data_evicted = C->myCache->operator[]((size_t)index)->operator[](col_evicted);
                }
                if (level == 0 && nlevels > 1) {
                    fetch(address, 1, L2Cache[0]);
                }
                data_evicted->tag = tag;
                data_evicted->valid = true;
                data_evicted->dirty = false;
                data_evicted->lastAccessTime = C->currentTime;
            }
        }
        C->currentTime++;
    }
    void read (size_t address, size_t level, Cache * C) {
        C->nread++;
        size_t a = address / C->blockSize;
        size_t tag = a / C->nsets;
        size_t index = a % C->nsets;
        for (size_t col = 0; col < C->associativity; col ++) {
            Block * data = C->myCache->operator[]((size_t)index)->operator[](col);
            if (data->valid && data->tag == tag) {
                C->nfetch_hit ++;
                data->lastAccessTime = C->currentTime;
                break;
            } else if (C->associativity == 1) {
                C->nread_miss++;
                if (data->valid && data->dirty && level == 0 && nlevels > 1) {
                    size_t address_toWrite = (data->tag * C->nsets + index) * C->blockSize;
                    if (L2_unified) {
                        write(address_toWrite, 1, L2Cache[0]);
                    } else {
                        write(address_toWrite, 1, L2Cache[1]);
                    }
                }
                if (level == 0 && nlevels > 1) {
                    if (L2_unified) { 
                        read(address, 1, L2Cache[0]);
                    } else {
                        read(address, 1, L2Cache[1]);
                    }
                }
                data->tag = tag;
                data->valid = true;
                data->dirty = false;
                data->lastAccessTime = C->currentTime;
                break;
            } else if (col == C->associativity - 1) {
                C->nread_miss++;
                size_t col_evicted;
                Block * data_evicted;
                if (C->strategy == Random) {
                    col_evicted = getRandomIndex(C->associativity);
                    data_evicted = C->myCache->operator[]((size_t)index)->operator[](col_evicted);
                    if (data_evicted->valid && data_evicted->dirty && level == 0 && nlevels > 1) {
                        size_t address_toWrite = (data_evicted->tag * C->nsets + index) * C->blockSize;
                        if (L2_unified) {
                            write(address_toWrite, 1, L2Cache[0]);
                        } else {
                            write(address_toWrite, 1, L2Cache[1]);
                        }
                    }
                } else {  //LRU
                    col_evicted = getLRUIndex(C->myCache->operator[]((size_t)index));
                    data_evicted = C->myCache->operator[]((size_t)index)->operator[](col_evicted);
                    if (data_evicted->valid && data_evicted->dirty && level == 0 && nlevels > 1) {
                        size_t address_toWrite = (data_evicted->tag * C->nsets + index) * C->blockSize;
                        if (L2_unified) {
                            write(address_toWrite, 1, L2Cache[0]);
                        } else {
                            write(address_toWrite, 1, L2Cache[1]);
                        }
                    }
                }
                if (level == 0 && nlevels > 1) {
                    if (L2_unified) {
                        read(address, 1, L2Cache[0]);
                    } else {
                        read(address, 1, L2Cache[1]);
                    }
                }
                data_evicted->tag = tag;
                data_evicted->valid = true;
                data_evicted->dirty = false;
                data_evicted->lastAccessTime = C->currentTime;
            }
        }
        C->currentTime++;
    }
    void write (size_t address, size_t level, Cache * C) {
        C->nwrite++;
        size_t a = address / C->blockSize;
        size_t tag = a / C->nsets;
        size_t index = a % C->nsets;
        for (size_t col = 0; col < C->associativity; col++) {
            Block * data = C->myCache->operator[]((size_t)index)->operator[](col);
            // write hit
            if (data->valid && data->tag == tag) {
                C->nwrite_hit++;
                data->dirty = true;
                data->lastAccessTime = C->currentTime;
                break;
                // write miss and direct-mapped
            } else if (C->associativity == 1) {
                C->nwrite_miss++;
                // writeScheme is write_allocate
                if (data->valid && data->dirty && level == 0 && nlevels > 1) {
                    size_t address_toWrite = (data->tag * C->nsets + index) * C->blockSize;
                    if (L2_unified) {
                        write(address_toWrite, 1, L2Cache[0]);
                    } else {
                        write(address_toWrite, 1, L2Cache[1]);
                    }
                }
                if (level == 0 && nlevels > 1) {
                    if (L2_unified) {
                        read(address, 1, L2Cache[0]);
                    } else {
                        read(address, 1, L2Cache[1]);
                    }
                }
                data->tag = tag;
                data->valid = true;
                data->dirty = true;
                data->lastAccessTime = C->currentTime;
                break;
            } else if (col == C->associativity - 1) {
                C->nwrite_miss++;
                size_t col_evicted;
                Block * data_evicted;
                if (C->strategy == Random) {
                    col_evicted = getRandomIndex(C->associativity);
                    data_evicted = C->myCache->operator[]((size_t)index)->operator[](col_evicted);
                    if (data_evicted->valid && data_evicted->dirty && level == 0 && nlevels > 1) {
                        size_t address_toWrite = (data_evicted->tag * C->nsets + index) * C->blockSize;
                        if (L2_unified) {
                            write(address_toWrite, 1, L2Cache[0]);
                        } else {
                            write(address_toWrite, 1, L2Cache[1]);
                        }
                    }
                    if (level == 0 && nlevels > 1) {
                        if (L2_unified) {
                            read(address, 1, L2Cache[0]);
                        } else {
                            read(address, 1, L2Cache[1]);
                        }
                    }
                } else {
                    col_evicted = getLRUIndex(C->myCache->operator[]((size_t)index));
                    data_evicted = C->myCache->operator[]((size_t)index)->operator[](col_evicted);
                    if (data_evicted->valid && data_evicted->dirty && level == 0 && nlevels > 1) {
                        size_t address_toWrite = (data_evicted->tag * C->nsets + index) * C->blockSize;
                        if (L2_unified) {
                            write(address_toWrite, 1, L2Cache[0]);
                        } else {
                            write(address_toWrite, 1, L2Cache[1]);
                        }
                    }
                    if (level == 0 && nlevels > 1) {
                        if (L2_unified) {
                            read(address, 1, L2Cache[0]);
                        } else {
                            read(address, 1, L2Cache[1]);
                        }
                    }
                }
                data_evicted->tag = tag;
                data_evicted->valid = true;
                data_evicted->dirty = true;
                data_evicted->lastAccessTime = C->currentTime;
            }
        }
    }
    size_t getRandomIndex (size_t size) {
        size_t index = (size_t) (rand() % size);
        return index;
    }
    size_t getLRUIndex (vector<Block *> * B) {
        size_t LRU = UINT_MAX;
        size_t index = 0;
        for (size_t i = 0; i < B->size(); i++) {
            Block * data = B->operator[](i);
            if (data->lastAccessTime < LRU) {
                LRU = data->lastAccessTime;
                index = i;
            }
        }
        return index;
    }
    void printResult () {
        size_t insFetchL1, insMissL1, totalFetchL1, totalMissL1, dataFetchL1, dataMissL1, readFetchL1, readMissL1, writeFetchL1, writeMissL1;
        float insMissRateL1, totalMissRateL1, dataMissRateL1, readMissRateL1, writeMissRateL1;
        
        insFetchL1 = L1Cache[0]->nfetch;
        insMissL1 = L1Cache[0]->nfetch_miss;
        insMissRateL1 = insMissL1 / ((float) insFetchL1 + FLT_EPSILON);

        if (L1_unified) {
            totalFetchL1 = L1Cache[0]->nfetch + L1Cache[0]->nread + L1Cache[0]->nwrite;
            totalMissL1 = L1Cache[0]->nfetch_miss + L1Cache[0]->nread_miss + L1Cache[0]->nwrite_miss;
            totalMissRateL1 = totalMissL1 / ((float) totalFetchL1 + FLT_EPSILON);
            
            dataFetchL1 = L1Cache[0]->nread + L1Cache[0]->nwrite;
            dataMissL1 = L1Cache[0]->nread_miss + L1Cache[0]->nwrite_miss;
            dataMissRateL1 = dataMissL1 / ((float) dataFetchL1 + FLT_EPSILON);
            
            readFetchL1 = L1Cache[0]->nread;
            readMissL1 = L1Cache[0]->nread_miss;
            readMissRateL1 = readMissL1 / ((float) readFetchL1 + FLT_EPSILON);
            
            writeFetchL1 = L1Cache[0]->nwrite;
            writeMissL1 = L1Cache[0]->nwrite_miss;
            writeMissRateL1 = writeMissL1 / ((float) writeFetchL1 + FLT_EPSILON);
        } else {
            totalFetchL1 = L1Cache[0]->nfetch + L1Cache[1]->nread + L1Cache[1]->nwrite;
            totalMissL1 = L1Cache[0]->nfetch_miss + L1Cache[1]->nread_miss + L1Cache[1]->nwrite_miss;
            totalMissRateL1 = totalMissL1 / ((float) totalFetchL1 + FLT_EPSILON);

            dataFetchL1 = L1Cache[1]->nread + L1Cache[1]->nwrite;
            dataMissL1 = L1Cache[1]->nread_miss + L1Cache[1]->nwrite_miss;
            dataMissRateL1 = dataMissL1 / ((float) dataFetchL1 + FLT_EPSILON);
            
            readFetchL1 = L1Cache[1]->nread;
            readMissL1 = L1Cache[1]->nread_miss;
            readMissRateL1 = readMissL1 / ((float) readFetchL1 + FLT_EPSILON);
            
            writeFetchL1 = L1Cache[1]->nwrite;
            writeMissL1 = L1Cache[1]->nwrite_miss;
            writeMissRateL1 = writeMissL1 / ((float) writeFetchL1 + FLT_EPSILON);
        }
        
        cout << fixed;
        cout.precision(4);

        cout << "L1 Cache" << endl;
        cout << "Metrics\t\t\t" << "total\t\t" << "Ins\t\t" << "Data\t\t" << "Read\t\t" << "Write" << endl;
        cout << "------------\t\t" << "--------\t" << "--------\t" << "--------\t" << "--------\t" << "--------" << endl;
        cout << "Demand Fetches\t\t" << totalFetchL1 << "\t\t" << insFetchL1 << "\t\t" << dataFetchL1 << "\t\t" << readFetchL1 << "\t\t" << writeFetchL1 << endl;
        cout << "Demand Misses\t\t" << totalMissL1 << "\t\t" << insMissL1 << "\t\t" << dataMissL1 << "\t\t" << readMissL1 << "\t\t" << writeMissL1 << endl;
        cout << "Demand Miss Rate\t" << totalMissRateL1 << "\t\t" << insMissRateL1 << "\t\t" << dataMissRateL1 << "\t\t" << readMissRateL1 << "\t\t" << writeMissRateL1 << endl;
        cout << endl;
        
        if (nlevels > 1) {
            size_t insFetchL2, insMissL2, totalFetchL2, totalMissL2, dataFetchL2, dataMissL2, readFetchL2, readMissL2, writeFetchL2, writeMissL2;
            float insMissRateL2, totalMissRateL2, dataMissRateL2, readMissRateL2, writeMissRateL2;
        
            insFetchL2 = L2Cache[0]->nfetch;
            insMissL2 = L2Cache[0]->nfetch_miss;
            insMissRateL2 = insMissL2 / ((float) insFetchL2 + FLT_EPSILON);
            
            if (L2_unified) {
                totalFetchL2 = L2Cache[0]->nfetch + L2Cache[0]->nread + L2Cache[0]->nwrite;
                totalMissL2 = L2Cache[0]->nfetch_miss +  L2Cache[0]->nread_miss + L2Cache[0]->nwrite_miss;
                totalMissRateL2 = totalMissL2 / ((float) totalFetchL2 + FLT_EPSILON);
                
                dataFetchL2 = L2Cache[0]->nread + L2Cache[0]->nwrite;
                dataMissL2 = L2Cache[0]->nread_miss + L2Cache[0]->nwrite_miss;
                dataMissRateL2 = dataMissL2 / ((float) dataFetchL2 + FLT_EPSILON);
                
                readFetchL2 = L2Cache[0]->nread;
                readMissL2 = L2Cache[0]->nread_miss;
                readMissRateL2 = readMissL2 / ((float) readFetchL2 + FLT_EPSILON);
                
                writeFetchL2 = L2Cache[0]->nwrite;
                writeMissL2 = L2Cache[0]->nwrite_miss;
                writeMissRateL2 = writeMissL2 / ((float) writeFetchL2 + FLT_EPSILON);
            } else {
                totalFetchL2 = L2Cache[0]->nfetch + L2Cache[1]->nread + L2Cache[1]->nwrite;
                totalMissL2 = L2Cache[0]->nfetch_miss +  L2Cache[1]->nread_miss + L2Cache[1]->nwrite_miss;
                totalMissRateL2 = totalMissL2 / ((float) totalFetchL2 + FLT_EPSILON);
                
                dataFetchL2 = L2Cache[1]->nread + L2Cache[1]->nwrite;
                dataMissL2 = L2Cache[1]->nread_miss + L2Cache[1]->nwrite_miss;
                dataMissRateL2 = dataMissL2 / ((float) dataFetchL2 + FLT_EPSILON);
                
                readFetchL2 = L2Cache[1]->nread;
                readMissL2 = L2Cache[1]->nread_miss;
                readMissRateL2 = readMissL2 / ((float) readFetchL2 + FLT_EPSILON);

                writeFetchL2 = L2Cache[1]->nwrite;
                writeMissL2 = L2Cache[1]->nwrite_miss;
                writeMissRateL2 = writeMissL2 / ((float) writeFetchL2 + FLT_EPSILON);
            }

            cout << "L2 Cache" << endl;
            cout << "Metrics\t\t\t" << "total\t\t" << "Ins\t\t" << "Data\t\t" << "Read\t\t" << "Write" << endl;
            cout << "------------\t\t" << "--------\t" << "--------\t" << "--------\t" << "--------\t" << "--------" << endl;
            cout << "Demand Fetches\t\t" << totalFetchL2 << "\t\t" << insFetchL2 << "\t\t" << dataFetchL2 << "\t\t" << readFetchL2 << "\t\t" << writeFetchL2 << endl;
            cout << "Demand Misses\t\t" << totalMissL2 << "\t\t" << insMissL2 << "\t\t" << dataMissL2 << "\t\t" << readMissL2 << "\t\t" << writeMissL2 << endl;
            cout << "Demand Miss Rate\t" << totalMissRateL2 << "\t\t" << insMissRateL2 << "\t\t" << dataMissRateL2 << "\t\t" << readMissRateL2 << "\t\t" << writeMissRateL2 << endl;
            cout << endl;
            
        }
    }
};
