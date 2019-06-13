#include "Common.h"


/**
* Copies all byte from source vector to dest starting from some index in dest.
* Assuming dest is already initialized.
*/
void copy_byte_vector_to_byte_array(const vector<byte> &source_vector, byte * dest, int beginIndex) {
	for (auto it = source_vector.begin(); it != source_vector.end(); ++it) {
		int index = distance(source_vector.begin(), it) + beginIndex;
		dest[index] = *it;
	}
}

void copy_byte_array_to_byte_vector(const byte* src, int src_len, vector<byte>& target_vector, int beginIndex)
{
	if ((int) target_vector.size() < beginIndex + src_len)
		target_vector.resize(beginIndex + src_len);
	memcpy(target_vector.data() + beginIndex, src, src_len);
}