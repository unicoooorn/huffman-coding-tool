//
// Created by Roman Gostilo on 23.05.2022.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifndef HUFFMAN_CODING_HUFFMAN_CODING_H
#define HUFFMAN_CODING_HUFFMAN_CODING_H

void* huffmanEncode(const unsigned char input_file_name[], const unsigned char output_file_name[]);

void huffmanDecode(const unsigned char input_file_name[], const unsigned char output_file_name[], void* key);

#endif //HUFFMAN_CODING_HUFFMAN_CODING_H
