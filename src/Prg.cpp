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
    int randomData = open("/dev/urandom", O_RDONLY);
    byte key[keySize/8];

    if (randomData < 0)
    {
        cerr << "Cannot open /dev/urandom" << endl;
    }
    else
    {

        ssize_t result = read(randomData, key, sizeof key);
        if (result < 0)
        {
            cerr << "Error while reading from /dev/urandom" << endl;
        }
    }

    SecretKey sk(vc,"PrgAES");
    setKey(ref(sk));
    //m_isKeySet = true;
    return sk;
}

void PrgFromAES::setKey(SecretKey & secretKey)
{
    if (!m_isKeySet)
    {
        AES_init_ctx(&m_ctx, (uint8_t*)&(secretKey.getEncoded()).at(0));
	m_isKeySet = true;
    }
}

void PrgFromAES::getPRGBytes(vector<byte> & outBytes, int outOffset, int outlen)
{
    AES_CTR_xcrypt_buffer(&m_ctx, (uint8_t*)outBytes.data(), outlen);
}
