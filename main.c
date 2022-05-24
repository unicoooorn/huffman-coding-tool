#include <stdio.h>
#include "huffman.h"

int main() {
    void* key = huffmanEncode("sherlock_holmes.txt", "compressed_sherlock_holmes.txt");
    huffmanDecode("compressed_sherlock_holmes.txt", "decompressed_sherlock_holmes.txt", key);
    return 0;
}
