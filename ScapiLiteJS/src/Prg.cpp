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
//    int randomData = open("/dev/urandom", O_RDONLY);
//    byte key[keySize];
//
//    if (randomData < 0)
//    {
//        cerr << "Cannot open /dev/urandom" << endl;
//    }
//    else
//    {
//
//        ssize_t result = read(randomData, key, sizeof key);
//        if (result < 0)
//        {
//            cerr << "Error while reading from /dev/urandom" << endl;
//        }
//    }
	std::random_device rd;
	byte key[keySize/8]; // todo: tell lior to fix his code
	int x;

//	for (int i = 0; i < keySize/(8*sizeof(x)); ++i) {
//		x = rd();
//		memcpy(key + i * sizeof(x), &x, sizeof(x));
//	}

	for (int i = 0; i < keySize/8; ++i) {
		key[i] = 0x1F;
	}

    vector<byte> vc(key, key + (keySize/8));
    SecretKey sk(vc,"PrgAES");
    setKey(ref(sk)); // todo: tell Lior to fix his code
//    m_isKeySet = true;
    return sk;
}

void PrgFromAES::setKey(SecretKey & secretKey)
{
    if (!m_isKeySet)
    {
    	// todo: revert it to be AES_init_ctx
        //AES_init_ctx(&m_ctx, (uint8_t*)&(secretKey.getEncoded()).at(0));
    	uint8_t iv[16] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25};
    	AES_init_ctx_iv(&m_ctx, (uint8_t*)&(secretKey.getEncoded()).at(0), iv);
        m_isKeySet = true;
    }
}

void PrgFromAES::getPRGBytes(vector<byte> & outBytes, int outOffset, int outlen)
{
    AES_CTR_xcrypt_buffer(&m_ctx, (uint8_t*)outBytes.data(), outlen);
}
