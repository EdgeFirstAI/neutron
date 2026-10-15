/*
 * Copyright 2022-2024 NXP
 *
 *  NXP Confidential. This software is owned or controlled by NXP and may only
 *  be used strictly in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */

#ifndef NEUTRON_DRIVER_BMM_H
#define NEUTRON_DRIVER_BMM_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int channelDensity;
    int lineDensity;
    int numNeutrons;
    bool bPingPong;
    int divisions;
} TilingResult;

typedef struct {
    int8_t *data;
    size_t size;
    size_t cap;
} Dyn8;
typedef struct {
    int32_t *data;
    size_t size;
    size_t cap;
} Dyn32;

typedef struct {
    Dyn8 *Bpacked;
    Dyn32 *lengths;
    TilingResult tilingInfo;
} PrepackOut;

typedef struct {
    bool rearrange;
    bool miniWeights;
    int weightBits;
    int groupSize;
    bool useDecodeBias;
    bool compress;
    int numMacs;
    int numNeutrons;
    int tcmSize;
    int numBanks;
} PrepackCfg;

#define IDX(i, j, cols) (((size_t)(i) * (size_t)(cols)) + (size_t)(j))

void dyn8_reserve(Dyn8 *b, size_t need);
void dyn8_append(Dyn8 *b, const int8_t *src, size_t n);
void dyn8_push(Dyn8 *b, int8_t v);
void dyn8_free(Dyn8 *b);

void dyn32_reserve(Dyn32 *b, size_t need);
void dyn32_append(Dyn32 *b, const int32_t *src, size_t n);
void dyn32_push(Dyn32 *b, int32_t v);
void dyn32_free(Dyn32 *b);

// tiling_solver(...)
TilingResult tiling_solver(int numTokens, int embeddings_in, int embeddings_out, int resNumBytes, int MACS,
                           int neutrons, int tcm_size, int tcm_banks, int weightBits, bool decodeWeights,
                           bool useDecodeBias, int groupSize);

// decimal_to_fixed_point
double decimal_to_fixed_point(double number, int integer_bits, int fraction_bits);

// outPacked/outOrganized/outCompressed：Memory is allocated inside the function using
// malloc, and the caller is responsible for freeing it
void weight_packer(const int8_t *B, int rowsB, int colsB, int channelDensity, int MACs, int weightBits,
                   int8_t **outPacked, size_t *outPackedN);

void fetch_unp_organize(const int8_t *packed, size_t packedN, int rowsB, int colsB, int channelDensity, int MACs,
                        int weightBits, int numNeutrons, int8_t **outOrganized, size_t *outOrganizedN);

// compress_weight_tensor_grouped function
int *compress_weight_tensor_grouped(int8_t *data, int data_size, int channelDensity, int numColsA, int weightBits,
                                    int num_decomp, int word_size, int buffer_size, int packet_size, bool compress,
                                    int8_t **outCompressed, size_t *outCompressedN, int *outListSize);

// pack group scales/biases
int8_t *fetch_unp_organize_group_scales_int32_int8(const int32_t *param_data, int rowsB, int colsB, int channelDensity,
                                                   int MACs, int weightBits, int numNeutrons, int groupSize, int isBias,
                                                   int divisions, int *out_size);

int16_t *fetch_unp_organize_group_scales_double_int16(const double *param_data, int rowsB, int colsB,
                                                      int channelDensity, int MACs, int weightBits, int numNeutrons,
                                                      int groupSize, int isBias, int divisions, int *out_size);

void extract_patterned_rows(const int8_t *matrix, size_t rows, size_t cols, size_t start, size_t step, size_t length,
                            size_t count, int8_t **out_ptr, size_t *out_len);

void prepack(const int8_t *B, int rb, int cb, PrepackCfg *pckCfg, PrepackOut *pckOut);
#ifdef __cplusplus
}
#endif
#endif // NEUTRON_DRIVER_BMM_H
