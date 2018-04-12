#include "Prg.h"

#include <iostream>
#include <random>

#define MBEDTLS_CIPHER_MODE_CTR

PrgFromAES::PrgFromAES()
{
    m_isKeySet = false;
}


SecretKey PrgFromAES::generateKey(int keySize)
{
	std::random_device rd;
	byte key[keySize/8];
	int x;

	for (int i = 0; i < keySize/(8*sizeof(x)); ++i) {
		x = rd();
		memcpy(key + i * sizeof(x), &x, sizeof(x));
	}
    vector<byte> vc(key, key + (keySize/8));
    SecretKey sk(vc,"PrgAES");
    setKey(ref(sk));
    return sk;
}

void PrgFromAES::setKey(SecretKey & secretKey)
{
    if (!m_isKeySet)
    {
        AES_init_ctx(&m_ctx, (uint8_t*)&(secretKey.getEncoded()).at(0));
    }
}

void PrgFromAES::getPRGBytes(vector<byte> & outBytes, int outOffset, int outlen)
{
    AES_CTR_xcrypt_buffer(&m_ctx, (uint8_t*)outBytes.data(), outlen);
}
