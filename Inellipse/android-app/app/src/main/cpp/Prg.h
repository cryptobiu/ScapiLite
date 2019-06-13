#ifndef SCAPILITE_PRG_H
#define SCAPILITE_PRG_H

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include<fcntl.h>
#include "key.h"
#include "aes.h"


using namespace std;

class PseudorandomGenerator
{

protected:
    bool m_isKeySet = false;
public:
	/**
	* Sets the secret key for this prg.
	* The key can be changed at any time.
	*/
	virtual void setKey(SecretKey & secretKey)=0;
	
	/**
	* An object trying to use an instance of prg needs to check if it has already been initialized with a key.
	* @return true if the object was initialized by calling the function setKey.
	*/
	virtual bool isKeySet()=0;
	
	
	/**
	* Generates a secret key to initialize this prg object.
	* @param keySize is the required secret key size in bits
	* @return the generated secret key
	*/
	virtual SecretKey generateKey(int keySize=128)=0;
	
	/**
	* Streams the prg bytes.
	* @param outBytes - output bytes. The result of streaming the bytes.
	* @param outOffset - output offset
	* @param outlen - the required output length
	*/
	virtual void getPRGBytes(vector<byte> & outBytes, int outOffset, int outlen)=0;
};

class PrgFromAES : public PseudorandomGenerator
{
private:
    // Counter used for key generation.
    AES_ctx m_ctx;
public:
    PrgFromAES();
//
//	//move assignment
//	PrgFromAES& operator=(PrgFromAES&& other);
//
//	//copy assignment - not allowed to prevent unneccessary copy of arrays.
//	PrgFromAES& operator=(PrgFromAES& other) = delete;
//
//	//move constructor
//	PrgFromAES(PrgFromAES&& old);
//	//copy constructor - not allowed to prevent unneccessary copy of arrays.
//	PrgFromAES(PrgFromAES& other) = delete;
//
//	~PrgFromAES();

    /**
	* This function does the following.
	* - Calls OpenSSL init function to create the key schedule. 
	* - Performs the encryption to fill in the cypherbits array
	*  @param secretKey - the new secret key for the aes to set.
	*/
	void setKey(SecretKey & secretKey) override;
	bool isKeySet() override { return m_isKeySet; };
	SecretKey generateKey(int keySize) override;

	/**
	* Fill the out vector with random bytes. This bytes are set to used and will not be used again
	* @param outBytes - output random bytes pre-generated by the prg 
	* @param outOffset - output offset
	* @param outlen - the required output length
	*/
	void getPRGBytes(vector<byte> & outBytes, int outOffset, int outLen) override;
};

#endif