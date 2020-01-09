//
// Created by paolo on 08/01/20.
//

#ifndef VASO2_DATACHUNKFLASH_H
#define VASO2_DATACHUNKFLASH_H

#include <memory>
#include <nvs.h>
#include "DataChunk.h"

class DataChunkFlash {
public:
    void init();
    ~DataChunkFlash();
    size_t getChunkSize(const char * chunkName);
    std::unique_ptr<DataChunk> readChunk(const char * chunkName);
    void write(const char * chunkName, const DataChunk * dataChunk );
    bool existChunk(char string[20]);

    uint32_t getU32(const char *key);
    void setU32(const char * key, uint32_t value);
private:
    nvs_handle handle;
};


#endif //VASO2_DATACHUNKFLASH_H
