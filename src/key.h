#ifndef SCAPILITE_KEY_H
#define SCAPILITE_KEY_H

#include "Common.h"

using namespace std;

class Key {
public:
	/*
	* Returns the name of the algorithm associated with this key.
	*/
	virtual string getAlgorithm()=0;
	virtual vector<byte> getEncoded()=0;
	virtual ~Key() {};
};

class SecretKey : Key {
private:
	vector<byte> key;
	string algorithm;

public:
	SecretKey() {};
	SecretKey(byte * keyBytes, int keyLen, string algorithm) {
		copy_byte_array_to_byte_vector(keyBytes, keyLen, this->key, 0);
		this->algorithm = algorithm;
	}
	SecretKey(const vector<byte> & key, string algorithm) {
		this->key = key;
		this->algorithm = algorithm;
	};
	string getAlgorithm() override { return algorithm; };
	vector<byte> getEncoded() override { return key; };
	virtual ~SecretKey() {};
	
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & key;
		ar & algorithm;
	}
};

class PublicKey : public Key {};
class PrivateKey : public Key {};

#endif