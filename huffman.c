//
// Created by Roman Gostilo on 23.05.2022.
//
#include "huffman.h"


// node structure in binary huffman tree
struct Node {
    struct Node *left;
    struct Node *right;
    struct Node *next;
    int frequency;
    unsigned char symbol;
    unsigned char is_symbol;
};

// node constructor
struct Node *newNode(struct Node *left, struct Node *right) {
    struct Node *t = malloc(sizeof(struct Node));
    t->left = left;
    t->right = right;
    t->next = NULL;
    t->is_symbol = 0;
    return t;
}

// counting letter and symbols entries
int *countSymbolsEntries(FILE *file_to_scan) {
    int *entries_count = malloc(256 * sizeof(int));
    memset(entries_count, 0, 256 * sizeof(int));
    fseek(file_to_scan, 0L, SEEK_END);
    long length = ftell(file_to_scan);
    fseek(file_to_scan, 0, SEEK_SET);
    for (int i = 0; i < length; ++i) {
        unsigned char curr_symbol = fgetc(file_to_scan);
        entries_count[(unsigned char) curr_symbol]++;
    }
    return entries_count;
}

// finding index of the largest index and assigning int_max
int findLeastFrequentSymbol(int const *array) {
    int min = INT32_MAX;
    int index = -1;
    for (int i = 0; i < 256; ++i) {
        if (array[i] != 0 && array[i] != INT32_MAX && array[i] < min) {
            min = array[i];
            index = i;
        }
    }
    return index;
}

// adding node to an end of a priority queue based on a linked list
void add2List(struct Node *head, unsigned char symbol, int frequency) {
    struct Node *new_node = malloc(sizeof(struct Node));
    new_node->symbol = symbol;
    new_node->is_symbol = 1;
    new_node->frequency = frequency;
    new_node->next = NULL;

    struct Node *curr_node = head;
    while (curr_node->next) {
        curr_node = curr_node->next;
    }
    curr_node->next = new_node;
    new_node->next = NULL;
}

// merge two elements of a priority queue with the lowest priorities
int mergeRarestPair(struct Node *priority_queue) {
    if (!priority_queue->next->next) {
        return 0;
    }
    // connecting two entities via a connecting node and extracting them
    struct Node *connecting_node = newNode(priority_queue->next, priority_queue->next->next);
    priority_queue->next = priority_queue->next->next->next;
    connecting_node->frequency = connecting_node->left->frequency + connecting_node->right->frequency;
    connecting_node->left->next = NULL;
    connecting_node->right->next = NULL;
    connecting_node->is_symbol = 0;

    // inserting a new node into a priority queue
    struct Node *curr_node = priority_queue;
    while (curr_node->next) {
        if (curr_node->frequency <= connecting_node->frequency
            && connecting_node->frequency <= curr_node->next->frequency) {
            connecting_node->next = curr_node->next;
            curr_node->next = connecting_node;
            return 1;
        }
        curr_node = curr_node->next;
    }
    curr_node->next = connecting_node;
    connecting_node->next = NULL;
    return 1;
}

// building a huffman tree out of priority queue
struct Node *buildHuffmanTreeOnPriorityQueue(struct Node *priority_queue) {
    int has_been_merged = mergeRarestPair(priority_queue);
    while (has_been_merged) {
        has_been_merged = mergeRarestPair(priority_queue);
    }
    return priority_queue->next;
}

// building a priority queue of symbols with respect to the number of entries
struct Node *buildPriorityQueueOnArray(int *frequencies) {
    // symbolic node contains symbols
    // connecting node connects symbolic node and the rest subtree
    struct Node *priority_queue = malloc(sizeof(struct Node));
    priority_queue->frequency = -1;
    priority_queue->is_symbol = 0;
    priority_queue->next = NULL;

    int curr_symbol_index = findLeastFrequentSymbol(frequencies);
    while (curr_symbol_index != -1) {
        add2List(priority_queue, (unsigned char) curr_symbol_index,
                 frequencies[curr_symbol_index]);
        frequencies[curr_symbol_index] = INT32_MAX;
        curr_symbol_index = findLeastFrequentSymbol(frequencies);
    }
    // now a connecting_node is a head of a priority queue
    return priority_queue;
}

// writing a single bit into char using bitwise operations,
// then printing the whole char when filled
void writeSingleBit(int bit, FILE *output_file) {
    static unsigned char buffer = 0x0;
    static unsigned char buffer_bits_written_count = -1;
    buffer_bits_written_count++;
    buffer_bits_written_count %= 8;
    if (bit == 0) {
        buffer = buffer ^ (0 << (7 - (buffer_bits_written_count)));
    } else if (bit == 1){
        buffer = buffer ^ (1 << (7 - (buffer_bits_written_count)));
    }
    else if (bit == -1){
        fprintf(output_file, "%c", buffer);
        buffer = 0x0;
        buffer_bits_written_count = 0;
    }
    if (buffer_bits_written_count == 7) {
        fprintf(output_file, "%c", buffer);
        buffer = 0x0;
    }
}

// print letter with prebuilt huffman trie
void writeSingleLetterToFile(const unsigned char symbol, unsigned char *dictionary[256], FILE *output_file) {
    int letter_id = 0;
    while (dictionary[symbol][letter_id] != '-') {
        if (dictionary[symbol][letter_id] == '1') {
            writeSingleBit(1, output_file);
        } else if (dictionary[symbol][letter_id] == '0') {
            writeSingleBit(0, output_file);
        }
        letter_id++;
    }
}

// writing to output file with prebuilt huffman tree
void write2File(FILE *input_file, FILE *output_file, unsigned char *dictionary[256]) {
    fseek(input_file, 0L, SEEK_END);
    long length = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    for (int i = 0; i < length; ++i) {
        writeSingleLetterToFile(fgetc(input_file), dictionary, output_file);
    }
    writeSingleBit(-1, output_file);
}

// generating dictionary based on huffman tree using DFS
void generateDictionary(int level, unsigned char *curr_code, unsigned char *dictionary[256], struct Node *huffman_tree_root) {
    if (!huffman_tree_root) {
        return;
    }
    if (huffman_tree_root->left) {
        curr_code[level] = '0';
        generateDictionary(level + 1, curr_code, dictionary, huffman_tree_root->left);
        curr_code[level] = '-';
    }
    if (huffman_tree_root->right) {
        curr_code[level] = '1';
        generateDictionary(level + 1, curr_code, dictionary, huffman_tree_root->right);
        curr_code[level] = '-';
    }
    if (huffman_tree_root->is_symbol) {
        strncpy(dictionary[huffman_tree_root->symbol], curr_code, 100);
    }
}

// huffman encoding, returns a pointer to huffman tree, which works as a key for decoding
void *huffmanEncode(const unsigned char input_file_name[], const unsigned char output_file_name[]) {
    FILE *input_file = fopen(input_file_name, "rb");
    if (!input_file)
        return NULL;

    int *entries_count = countSymbolsEntries(input_file);
    struct Node *priority_queue = buildPriorityQueueOnArray(entries_count);
    struct Node *huffman_tree = buildHuffmanTreeOnPriorityQueue(priority_queue);

    unsigned char *code = malloc(sizeof(unsigned char) * 100);
    memset(code, '-', 100 * sizeof(unsigned char));
    unsigned char *dictionary[256];
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = malloc(100 * sizeof(unsigned char));
        memset(dictionary[i], '-', 100 * sizeof(unsigned char));
    }
    generateDictionary(0, code, dictionary, huffman_tree);


    FILE *output_file = fopen(output_file_name, "wb+");

    write2File(input_file, output_file, dictionary);

    fclose(input_file);
    fclose(output_file);

    // freeing memory
    free(code);
    code = NULL;
    for (int i = 0; i < 256; ++i) {
        free(dictionary[i]);
        dictionary[i] = NULL;
    }
    free(entries_count);
    entries_count = NULL;
    free(priority_queue);
    priority_queue = NULL;

    return huffman_tree;
}

void decodeBinWithHuffmanTree(struct Node *huffman_tree, FILE *input_file, FILE *output_file) {
    fseek(input_file, 0L, SEEK_END);
    long length = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    struct Node* curr_node = huffman_tree;
    for (int i = 0; i < length; ++i) {
        unsigned char curr_symbol = fgetc(input_file);
        for (int bit_index = 0; bit_index < 8; ++bit_index) {
            if (curr_node->is_symbol){
                fprintf(output_file, "%c", curr_node->symbol);
                curr_node = huffman_tree;
            };
            char curr_bit = curr_symbol & 1 << (7 - bit_index);
            if (curr_bit){
                curr_node = curr_node->right;
            }
            else{
                curr_node = curr_node->left;
            }
        }
    }
}

// freeing memory allocated for huffman tree
void freeHuffmanTree(struct Node* huffman_tree){
    if (!huffman_tree){
        return;
    }
    freeHuffmanTree(huffman_tree->left);
    freeHuffmanTree(huffman_tree->right);
    if (huffman_tree->left)
        free(huffman_tree->left);
    if(huffman_tree->right)
        free(huffman_tree->right);
}


void huffmanDecode(const unsigned char input_file_name[], const unsigned char output_file_name[], void *key) {
    FILE *input_file = fopen(input_file_name, "rb");
    FILE *output_file = fopen(output_file_name, "w");

    struct Node *huffman_tree = key;
    decodeBinWithHuffmanTree(huffman_tree, input_file, output_file);
    freeHuffmanTree(huffman_tree);
    huffman_tree = NULL;

    fclose(input_file);
    fclose(output_file);
}