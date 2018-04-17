#ifndef SCAPILITE_COMMON_H
#define SCAPILITE_COMMON_H

#include <iostream>
#include <cstring> // for memcpy
#include <string>
#include <vector>
#include <memory>

typedef unsigned char byte;

using namespace std;

void copy_byte_vector_to_byte_array(const vector<byte> &source_vector, byte * dest, int beginIndex);
void copy_byte_array_to_byte_vector(const byte* src, int src_len, vector<byte>& target_vector, int beginIndex);

#endif
