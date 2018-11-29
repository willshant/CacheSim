#include <vector>
#include <string>
#include "cache.h"
#include <iostream>
#include <sstream>

#include <stdio.h>
using namespace std;

#define Read 0
#define Write 1
#define Fetch 2
#define Random 0
#define RLU 1

class simulator {
public:
    vector<Cache *> L1Cache;
    vector<Cache *> L2Cache;
    bool L1_unified, L2_unified;
    int nlevels;
    float L1_hitTime,L2_hitTime, DRAM_accessTime;
    int nrecords;
    simulator(int nlevels, bool L1_unified, bool L2_unified, std::string L1, std::string L2, float L1_hitTime, float L2_hitTime, float DRAM_accessTime){
        this->nlevels = nlevels;
        this->L1_unified = L1_unified;
        this->L1_hitTime = L1_hitTime;
        this->DRAM_accessTime = DRAM_accessTime;
        this->nrecords = 0;
        if(L1_unified){
            // 开一个 cache 类型的数组，长度只有1 给L1cache
            //std::stringstream test("this_is_a_test_string");
            std::stringstream temp;
            temp.str (L1);  //convert string to stringstream
            std::string configuration;
            std::vector<std::string> L1_configuration;
            while(std::getline(temp, configuration, ' '))
            {
                L1_configuration.push_back(configuration);
            }
            int L1_A = atoi(L1_configuration[0].c_str());
            int L1_B = atoi(L1_configuration[1].c_str());
            int L1_C = atoi(L1_configuration[2].c_str());
            int L1_strategy = atoi(L1_configuration[3].c_str());
            //int L1_writeScheme = atoi(L1_configuration[4].c_str());
            L1Cache.push_back(new Cache(L1_A, L1_B, L1_C, L1_strategy, 0));  // right ??
        }else{
            std::stringstream temp;
            temp.str (L1);  //convert string to stringstream
            std::string configuration;
            std::vector<std::string> L1_configuration;
            while(std::getline(temp, configuration, ' '))
            {
                L1_configuration.push_back(configuration);
            }
            int L1_I_A = atoi(L1_configuration[0].c_str());
            int L1_I_B = atoi(L1_configuration[1].c_str());
            int L1_I_C = atoi(L1_configuration[2].c_str());
            int L1_I_strategy = atoi(L1_configuration[3].c_str());
            //int L1_writeScheme = atoi(L1_configuration[4].c_str());
            int L1_D_A = atoi(L1_configuration[4].c_str());
            int L1_D_B = atoi(L1_configuration[5].c_str());
            int L1_D_C = atoi(L1_configuration[6].c_str());
            int L1_D_strategy = atoi(L1_configuration[7].c_str());
            L1Cache.push_back(new Cache(L1_I_A, L1_I_B, L1_I_C, L1_I_strategy, 0));
            L1Cache.push_back(new Cache(L1_D_A, L1_D_B, L1_D_C, L1_D_strategy, 0));
        }
        if(nlevels > 1){
            if(L2_unified){
                std::stringstream temp;
                temp.str (L2);  //convert string to stringstream
                std::string configuration;
                std::vector<std::string> L2_configuration;
                while(std::getline(temp, configuration, ' '))
                {
                    L2_configuration.push_back(configuration);
                }
                int L2_A = atoi(L2_configuration[0].c_str());
                int L2_B = atoi(L2_configuration[1].c_str());
                int L2_C = atoi(L2_configuration[2].c_str());
                int L2_strategy = atoi(L2_configuration[3].c_str());
                //int L1_writeScheme = atoi(L1_configuration[4].c_str());
                L2Cache.push_back(new Cache(L2_A, L2_B, L2_C, L2_strategy, 0));
            }else{
                std::stringstream temp;
                temp.str (L2);  //convert string to stringstream
                std::string configuration;
                std::vector<std::string> L2_configuration;
                while(std::getline(temp, configuration, ' '))
                {
                    L2_configuration.push_back(configuration);
                }
                int L2_I_A = atoi(L2_configuration[0].c_str());
                int L2_I_B = atoi(L2_configuration[1].c_str());
                int L2_I_C = atoi(L2_configuration[2].c_str());
                int L2_I_strategy = atoi(L2_configuration[3].c_str());
                //int L1_writeScheme = atoi(L1_configuration[4].c_str());
                int L2_D_A = atoi(L2_configuration[0].c_str());
                int L2_D_B = atoi(L2_configuration[1].c_str());
                int L2_D_C = atoi(L2_configuration[2].c_str());
                int L2_D_strategy = atoi(L2_configuration[3].c_str());
                L2Cache.push_back(new Cache(L2_I_A, L2_I_B, L2_I_C, L2_I_strategy, 0));
                L2Cache.push_back(new Cache(L2_D_A, L2_D_B, L2_D_C, L2_D_strategy, 0));
            }
            this->L2_unified = L2_unified;
            this->L2_hitTime = L2_hitTime;
        }
    };
    void process(std::string s){
        std::stringstream temp;
        temp.str (s);  //convert string to stringstream
        std::string element;
        std::vector<std::string> ss;
        while(std::getline(temp, element, ' '))
        {
            ss.push_back(element);
        }
        int code = atoi(ss[0].c_str());
        long address = strtol(ss[1].c_str(), NULL, 16);
        nrecords ++;
        // std::cout << nrecords << std::endl;
        if(code == Read){
            if(L1_unified){
                read(address, 0, L1Cache[0]);
            }else{
                read(address, 0, L1Cache[1]);
            }
        }else if (code == Write){
            if(L1_unified){
                write(address, 0, L1Cache[0]);
            }else{
                write(address, 0, L1Cache[1]);
            }
        }else{
            fetch(address, 0, L1Cache[0]);
        }
    };
    void fetch(long address, int level, Cache * C){
        C->nfetch++;
        long a = address / C->blockSize;
        long tag = a / C->nsets;
        long index = a % C->nsets;
        for(int col = 0 ; col < C->associativity ; col ++){
            Block * data = C->myCache->operator[]((int)index)->operator[](col);
            if(data->valid && data->tag == tag){
                C->nfetch_hit ++;
                data->lastAccessTime = C->currentTime;
                break;
            }else if(C->associativity == 1){
                C->nfetch_miss++;
                if(level == 0 && nlevels > 1){
                    fetch(address,1,L2Cache[0]);
                }
                data->tag = tag;
                data->valid = true;
                data->dirty = false;
                data->lastAccessTime = C->currentTime;
                break;
            }else if(col == C->associativity - 1){
                C->nfetch_miss++;
                int col_evicted;
                Block * data_evicted;
                if(C->strategy == Random){
                    col_evicted = getRandomIndex(C->associativity);
                    data_evicted = C->myCache->operator[]((int)index)->operator[](col_evicted);
                }else{
                    col_evicted = getLRUIndex(C->myCache->operator[]((int)index));
                    data_evicted = C->myCache->operator[]((int)index)->operator[](col_evicted);
                }
                if(level == 0 && nlevels > 1){
                    fetch(address, 1, L2Cache[0]);
                }
                data_evicted->tag = tag;
                data_evicted->valid = true;
                data_evicted->dirty = false;
                data_evicted->lastAccessTime = C->currentTime;
            }
        }
        C->currentTime++;
    };
    void read(long address, int level, Cache * C){
        C->nread ++;
        long a = address / C->blockSize;
        long tag = a / C->nsets;
        long index = a % C->nsets;
        for(int col = 0; col < C->associativity; col ++){
            Block * data = C->myCache->operator[]((int)index)->operator[](col);
            if(data->valid && data->tag == tag){
                C->nfetch_hit ++;
                data->lastAccessTime = C->currentTime;
                break;
            }else if(C->associativity == 1){
                C->nread_miss ++;
                if(data->valid && data->dirty && level == 0 && nlevels > 1){
                    long address_toWrite = (data->tag * C->nsets + index) * C->blockSize;
                    if(L2_unified){
                        write(address_toWrite, 1, L2Cache[0]);
                    }else{
                        write(address_toWrite, 1, L2Cache[1]);
                    }
                }
                if(level == 0 && nlevels > 1){
                    if(L2_unified){
                        read(address, 1, L2Cache[0]);
                    }else{
                        read(address, 1, L2Cache[1]);
                    }
                }
                data->tag = tag;
                data->valid = true;
                data->dirty = false;
                data->lastAccessTime = C->currentTime;
                break;
            }else if(col == C->associativity - 1){
                C->nread_miss++;
                int col_evicted;
                Block * data_evicted;
                if(C->strategy == Random){
                    col_evicted = getRandomIndex(C->associativity);
                    data_evicted = C->myCache->operator[]((int)index)->operator[](col_evicted);
                    if(data_evicted->valid && data_evicted->dirty && level == 0 && nlevels > 1){
                        long address_toWrite = (data_evicted->tag * C->nsets + index) * C->blockSize;
                        if(L2_unified){
                            write(address_toWrite, 1, L2Cache[0]);
                        }else{
                            write(address_toWrite, 1, L2Cache[1]);
                        }
                    }
                }else{  //LRU
                    col_evicted = getLRUIndex(C->myCache->operator[]((int)index));
                    data_evicted = C->myCache->operator[]((int)index)->operator[](col_evicted);
                    if(data_evicted->valid && data_evicted->dirty && level == 0 && nlevels > 1){
                        long address_toWrite = (data_evicted->tag * C->nsets + index) * C->blockSize;
                        if(L2_unified){
                            write(address_toWrite, 1, L2Cache[0]);
                        }else{
                            write(address_toWrite, 1, L2Cache[1]);
                        }
                    }
                }
                if(level == 0 && nlevels > 1){
                    if(L2_unified){
                        read(address, 1, L2Cache[0]);
                    }else{
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
    };
    void write(long address, int level, Cache * C){
        C->nwrite++;
        long a = address / C->blockSize;
        long tag = a / C->nsets;
        long index = a % C->nsets;
        for(int col = 0; col < C->associativity; col++){
            Block * data = C->myCache->operator[]((int)index)->operator[](col);
            // write hit
            if(data->valid && data->tag == tag){
                C->nwrite_hit++;
                data->dirty = true;
                data->lastAccessTime = C->currentTime;
                break;
                // write miss and direct-mapped
            }else if (C->associativity == 1){
                C->nwrite_miss++;
                // writeScheme is write_allocate
                if(data->valid && data->dirty && level == 0 && nlevels > 1){
                    long address_toWrite = (data->tag * C->nsets + index) * C->blockSize;
                    if(L2_unified){
                        write(address_toWrite, 1, L2Cache[0]);
                    }else{
                        write(address_toWrite, 1, L2Cache[1]);
                    }
                }
                if(level == 0 && nlevels > 1){
                    if(L2_unified){
                        read(address, 1, L2Cache[0]);
                    }else{
                        read(address, 1, L2Cache[1]);
                    }
                }
                data->tag = tag;
                data->valid = true;
                data->dirty = true;
                data->lastAccessTime = C->currentTime;
                break;
            }else if (col == C->associativity - 1){
                C->nwrite_miss++;
                int col_evicted;
                Block * data_evicted;
                if(C->strategy == Random){
                    col_evicted = getRandomIndex(C->associativity);
                    data_evicted = C->myCache->operator[]((int)index)->operator[](col_evicted);
                    if(data_evicted->valid && data_evicted->dirty && level == 0 && nlevels > 1){
                        long address_toWrite = (data_evicted->tag * C->nsets + index) * C->blockSize;
                        if(L2_unified){
                            write(address_toWrite, 1, L2Cache[0]);
                        }else{
                            write(address_toWrite, 1, L2Cache[1]);
                        }
                    }
                    if(level == 0 && nlevels > 1){
                        if(L2_unified){
                            read(address, 1, L2Cache[0]);
                        }else{
                            read(address, 1, L2Cache[1]);
                        }
                    }
                }else{
                    col_evicted = getLRUIndex(C->myCache->operator[]((int)index));
                    data_evicted = C->myCache->operator[]((int)index)->operator[](col_evicted);
                    if(data_evicted->valid && data_evicted->dirty && level == 0 && nlevels > 1){
                        long address_toWrite = (data_evicted->tag * C->nsets + index) * C->blockSize;
                        if(L2_unified){
                            write(address_toWrite, 1, L2Cache[0]);
                        }else{
                            write(address_toWrite, 1, L2Cache[1]);
                        }
                    }
                    if(level == 0 && nlevels > 1){
                        if(L2_unified){
                            read(address, 1, L2Cache[0]);
                        }else{
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
    };
    int getRandomIndex(int size){
        int index = (int) (rand() % size);
        return index;
    };
    int getLRUIndex(vector<Block *> * B){
        int LRU = INT_MAX;
        int index = 0;
        for(size_t i = 0; i < B->size(); i++){
            Block * data = B->operator[](i);
            if(data->lastAccessTime < LRU){
                LRU = data->lastAccessTime;
                index = i;
            }
        }
        return index;
    };
    void printResult(){
        int InstrnDemandFetch = L1Cache[0]->nfetch;
        int InstrnDemandMiss = L1Cache[0]->nfetch_miss;
        float InstrnDemandMissRate = InstrnDemandMiss / (float) InstrnDemandFetch;
        
        int totalDemandFetches_L1, totalDemandMiss_L1, DataDemandFetch, DataDemandMiss, DataReadDemandFetch, DataReadDemandMiss, DataWriteDemandFetch, DataWriteDemandMiss;
        float totalDemandMissRate_L1, DataDemandMissRate, DataReadDemandMissRate, DataWriteDemandMissRate;
        if(L1_unified){
            totalDemandFetches_L1 = L1Cache[0]->nfetch + L1Cache[0]->nread + L1Cache[0]->nwrite;
            totalDemandMiss_L1 = L1Cache[0]->nfetch_miss + L1Cache[0]->nread_miss + L1Cache[0]->nwrite_miss;
            totalDemandMissRate_L1 = totalDemandMiss_L1 / (float) totalDemandFetches_L1;
            
            DataDemandFetch = L1Cache[0]->nread + L1Cache[0]->nwrite;
            DataDemandMiss = L1Cache[0]->nread_miss + L1Cache[0]->nwrite_miss;
            DataDemandMissRate = DataDemandMiss / (float) DataDemandFetch;
            
            DataReadDemandFetch = L1Cache[0]->nread;
            DataReadDemandMiss = L1Cache[0]->nread_miss;
            DataReadDemandMissRate = DataReadDemandMiss / (float) DataReadDemandFetch;
            
            DataWriteDemandFetch = L1Cache[0]->nwrite;
            DataWriteDemandMiss = L1Cache[0]->nwrite_miss;
            DataWriteDemandMissRate = DataWriteDemandMiss / (float) DataWriteDemandFetch;
        }else{
            totalDemandFetches_L1 = L1Cache[0]->nfetch + L1Cache[1]->nread + L1Cache[1]->nwrite;
            totalDemandMiss_L1 = L1Cache[0]->nfetch_miss + L1Cache[1]->nread_miss + L1Cache[1]->nwrite_miss;
            totalDemandMissRate_L1 = totalDemandMiss_L1 / (float) totalDemandFetches_L1;

            DataDemandFetch = L1Cache[1]->nread + L1Cache[1]->nwrite;
            DataDemandMiss = L1Cache[1]->nread_miss + L1Cache[1]->nwrite_miss;
            DataDemandMissRate = DataDemandMiss / (float) DataDemandFetch;
            
            DataReadDemandFetch = L1Cache[1]->nread;
            DataReadDemandMiss = L1Cache[1]->nread_miss;
            DataReadDemandMissRate = DataReadDemandMiss / (float) DataReadDemandFetch;
            
            DataWriteDemandFetch = L1Cache[1]->nwrite;
            DataWriteDemandMiss = L1Cache[1]->nwrite_miss;
            DataWriteDemandMissRate = DataWriteDemandMiss / (float) DataWriteDemandFetch;
        }
        std::cout << totalDemandFetches_L1 << " demand fetches for L1 cache" << std::endl;
        std::cout << totalDemandMiss_L1 << " demand misses for L1 cache" << std::endl;
        std::cout << "The total demand miss rate for L1 cache is " << totalDemandMissRate_L1 << std::endl;
        
        std::cout << "Instruction cache level 1:" <<std::endl;
        std::cout << InstrnDemandFetch << " times of instruction demand fetches" <<std::endl;
        std::cout << InstrnDemandMiss << " times of instruction demand misses" <<std::endl;
        std::cout << "The instruction demand miss rate is " << InstrnDemandMissRate <<std::endl;

        std::cout << "Data cache level 1: " <<std::endl;
        std::cout << DataDemandFetch << " times of data demand fetches" <<std::endl;
        std::cout << DataDemandMiss << " times of data demand misses" <<std::endl;
        std::cout << "The data demand miss rate is " << DataDemandMissRate << std::endl;
        
        std::cout << DataReadDemandFetch << " times of data reads" << std::endl;
        std::cout << DataReadDemandMiss << " times of data read misses" << std::endl;
        std::cout << "The data read miss rate is " << DataReadDemandMissRate << std::endl;

        std::cout << DataWriteDemandFetch << " times of data writes" << std::endl;
        std::cout << DataWriteDemandMiss << " times of data write misses" << std::endl;
        std::cout << "The data write miss rate is " << DataWriteDemandMissRate << std::endl;

        int totalDemandFetches_L2, totalDemandMiss_L2, InstrnDemandFetch_L2, InstrnDemandMiss_L2, DataDemandFetch_L2, DataDemandMiss_L2, DataReadDemandFetch_L2, DataReadDemandMiss_L2, DataWriteDemandFetch_L2, DataWriteDemandMiss_L2;
        float totalDemandMissRate_L2, InstrnDemandMissRate_L2, DataDemandMissRate_L2, DataReadDemandMissRate_L2, DataWriteDemandMissRate_L2;
        
        if(nlevels > 1){
            InstrnDemandFetch_L2 = L2Cache[0]->nfetch;
            InstrnDemandMiss_L2 = L2Cache[0]->nfetch_miss;
            InstrnDemandMissRate_L2 = InstrnDemandMiss_L2 / (float) InstrnDemandFetch_L2;
            
            if(L2_unified){
                totalDemandFetches_L2 = L2Cache[0]->nfetch + L2Cache[0]->nread + L2Cache[0]->nwrite;
                totalDemandMiss_L2 = L2Cache[0]->nfetch_miss +  L2Cache[0]->nread_miss + L2Cache[0]->nwrite_miss;
                totalDemandMissRate_L2 = totalDemandMiss_L2 / (float) totalDemandFetches_L2;
                
                DataDemandFetch_L2 = L2Cache[0]->nread + L2Cache[0]->nwrite;
                DataDemandMiss_L2 = L2Cache[0]->nread_miss + L2Cache[0]->nwrite_miss;
                DataDemandMissRate_L2 = DataDemandMiss_L2 / (float) DataDemandFetch_L2;
                
                DataReadDemandFetch_L2 = L2Cache[0]->nread;
                DataReadDemandMiss_L2 = L2Cache[0]->nread_miss;
                DataReadDemandMissRate_L2 = DataReadDemandMiss_L2 / (float) DataReadDemandFetch_L2;
                
                DataWriteDemandFetch_L2 = L2Cache[0]->nwrite;
                DataWriteDemandMiss_L2 = L2Cache[0]->nwrite_miss;
                DataWriteDemandMissRate_L2 = DataWriteDemandMiss_L2 / (float) DataWriteDemandFetch_L2;
            }else{
                totalDemandFetches_L2 = L2Cache[0]->nfetch + L2Cache[1]->nread + L2Cache[1]->nwrite;
                totalDemandMiss_L2 = L2Cache[0]->nfetch_miss +  L2Cache[1]->nread_miss + L2Cache[1]->nwrite_miss;
                totalDemandMissRate_L2 = totalDemandMiss_L2 / (float) totalDemandFetches_L2;
                
                DataDemandFetch_L2 = L2Cache[1]->nread + L2Cache[1]->nwrite;
                DataDemandMiss_L2 = L2Cache[1]->nread_miss + L2Cache[1]->nwrite_miss;
                DataDemandMissRate_L2 = DataDemandMiss_L2 / (float) DataDemandFetch_L2;
                
                DataReadDemandFetch_L2 = L2Cache[1]->nread;
                DataReadDemandMiss_L2 = L2Cache[1]->nread_miss;
                DataReadDemandMissRate_L2 = DataReadDemandMiss_L2 / (float) DataReadDemandFetch_L2;

                DataWriteDemandFetch_L2 = L2Cache[1]->nwrite;
                DataWriteDemandMiss_L2 = L2Cache[1]->nwrite_miss;
                DataWriteDemandMissRate_L2 = DataWriteDemandMiss_L2 / (float) DataWriteDemandFetch_L2;
            }
            std::cout << totalDemandFetches_L2 << " demand fetches for L2 cache" << std::endl;
            std::cout << totalDemandMiss_L2 << " demand misses for L2 cache" << std::endl;
            std::cout << "The total demand miss rate for L2 cache is " << totalDemandMissRate_L1 << std::endl;

            std::cout << "Instruction cache level 2:" << std::endl;
            std::cout << InstrnDemandFetch_L2 << " times of instruction demand fetches" << std::endl;
            std::cout << InstrnDemandMiss_L2 << " times of instruction demand misses" << std::endl;
            std::cout << "The instruction demand miss rate is " << InstrnDemandMissRate_L2 << std::endl;

            std::cout << "Data cache level 2: " << std::endl;
            std::cout << DataDemandFetch_L2 << " times of data demand fetches" << std::endl;
            std::cout << DataDemandMiss_L2 << " times of data demand misses" << std::endl;
            std::cout << "The data demand miss rate is " << DataDemandMissRate_L2 << std::endl;

            std::cout << DataReadDemandFetch_L2 << " times of data reads" << std::endl;
            std::cout << DataReadDemandMiss_L2 << " times of data read misses" << std::endl;
            std::cout << "The data read miss rate is " << DataReadDemandMissRate_L2 << std::endl;
            
            std::cout << DataWriteDemandFetch_L2 << " times of data writes" << std::endl;
            std::cout << DataWriteDemandMiss_L2 << " times of data write misses" << std::endl;
            std::cout << "The data write miss rate is " << DataWriteDemandMissRate_L2 << std::endl;
        }
        
        int demandFetch_L1, demandMiss_L1, demandFetch_L2, demandMiss_L2;
        float demandMissRate_L1, demandMissRate_L2;
        float accessTime_L1, accessTime_L2;
        
        if(nlevels == 1){
            if(L1_unified){
                demandFetch_L1 = L1Cache[0]->nfetch + L1Cache[0]->nread + L1Cache[0]->nwrite;
                demandMiss_L1 = L1Cache[0]->nfetch_miss + L1Cache[0]->nread_miss + L1Cache[0]->nwrite_miss;
            }else{
                demandFetch_L1 = L1Cache[0]->nfetch + L1Cache[1]->nread + L1Cache[1]->nwrite;
                demandMiss_L1 = L1Cache[0]->nfetch_miss + L1Cache[1]->nread_miss + L1Cache[1]->nwrite_miss;
            }
            demandMissRate_L1 = demandMiss_L1 / (float) demandFetch_L1;
            accessTime_L1 = L1_hitTime + demandMissRate_L1 * DRAM_accessTime;
            std::cout << "The average access time for L1 is " << accessTime_L1 << std::endl;
        }else{
            if(L1_unified){
                demandFetch_L1 = L1Cache[0]->nfetch + L1Cache[0]->nread + L1Cache[0]->nwrite;
                demandMiss_L1 = L1Cache[0]->nfetch_miss + L1Cache[0]->nread_miss + L1Cache[0]->nwrite_miss;
            }else{
                demandFetch_L1 = L1Cache[0]->nfetch + L1Cache[1]->nread + L1Cache[1]->nwrite;
                demandMiss_L1 = L1Cache[0]->nfetch_miss + L1Cache[1]->nread_miss + L1Cache[1]->nwrite_miss;
            }
            if(L2_unified){
                demandFetch_L2 = L2Cache[0]->nfetch + L2Cache[0]->nread + L2Cache[0]->nwrite;
                demandMiss_L2 = L2Cache[0]->nfetch_miss + L2Cache[0]->nread_miss + L2Cache[0]->nwrite_miss;
            }else{
                demandFetch_L2 = L2Cache[0]->nfetch + L2Cache[1]->nread + L2Cache[1]->nwrite;
                demandMiss_L2 = L2Cache[0]->nfetch_miss + L2Cache[1]->nread_miss + L2Cache[1]->nwrite_miss;
            }
            demandMissRate_L1 = demandMiss_L1 / (float) demandFetch_L1;
            demandMissRate_L2 = demandMiss_L2 / (float) demandFetch_L2;
            accessTime_L2 = L2_hitTime + demandMissRate_L2 * DRAM_accessTime;
            accessTime_L1 = L1_hitTime + demandMissRate_L1 * accessTime_L2;
            std::cout << "The average access time for L1 is " << accessTime_L1 << std::endl;
            std::cout << "The average access time for L2 is " << accessTime_L2 << std::endl;

        }
    };
    /*
    //static ?? const ??
#include <time.h>
#include <sys/time.h>
    double get_wall_time(){
        struct timeval time;
        if (gettimeofday(&time,NULL)){
            //  Handle error
            return 0;
        }
        return (double)time.tv_sec + (double)time.tv_usec * .000001;
    }
    double getCpuTime(){
        return (double)clock() / CLOCKS_PER_SEC;
    }
    */
};
