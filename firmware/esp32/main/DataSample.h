//
// Created by paolo on 10/01/20.
//

#ifndef VASO2_DATASAMPLE_H
#define VASO2_DATASAMPLE_H
struct DataSample {
    uint16_t offset;
    int16_t humidity;
    int16_t temperature;
    uint16_t soil;
}  __attribute__ ((aligned (4)));
#endif //VASO2_DATASAMPLE_H
