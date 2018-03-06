#include <aes.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include<fcntl.h>

typedef unsigned char byte;

using namespace std;

#define MBEDTLS_CIPHER_MODE_CTR

int main(int argc, char* argv[])
{
    //set aes key
    int randomData = open("/dev/urandom", O_RDONLY);
    byte key[16];

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
    
    AES_ctx ctx;
    uint8_t* buf = new uint8_t[16];

    for (int idx = 0; idx< 16; idx++)
        buf[idx] = 0;
    AES_init_ctx(&ctx, (uint8_t*)key);
    
    AES_CTR_xcrypt_buffer(&ctx, buf, sizeof(buf)*16);

    for (int idx = 0; idx< 16; idx++)
        cout << (int)buf[idx] << endl;

    AES_CTR_xcrypt_buffer(&ctx, buf, sizeof(buf)*16);

    for (int idx = 0; idx< 16; idx++)
        cout << (int)buf[idx] << endl;

    return 0;
}
