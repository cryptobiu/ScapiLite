//
// Created by liork on 12/03/18.
//

#ifndef SCAPILITEAPK_MERSENNE_H
#define SCAPILITEAPK_MERSENNE_H

#include <gmp.h>
#include <sstream>

#include "NTL/ZZ.h"
#include "NTL/ZZ_p.h"

#include "Prg.h"

using namespace std;
using namespace NTL;

class ZpMersenneIntElement {

//private:
public: //TODO return to private after tesing

    static const unsigned int p = 2147483647;
    unsigned int elem;

public:

    ZpMersenneIntElement(){elem = 0;};
    ZpMersenneIntElement(int elem)
    {
        this->elem = elem;
        if(this->elem<p){
            return;
        }
        this->elem -=p;
        if(this->elem<p){
            return;
        }
        this->elem -=p;
    }

    ZpMersenneIntElement& operator=(const ZpMersenneIntElement& other){elem = other.elem; return *this;};
    bool operator!=(const ZpMersenneIntElement& other){ return !(other.elem == elem); };

    ZpMersenneIntElement operator+(const ZpMersenneIntElement& f2)
    {
        ZpMersenneIntElement answer;

        answer.elem = (elem + f2.elem);

        if(answer.elem>=p)
            answer.elem-=p;

        return answer;
    }
    ZpMersenneIntElement operator-(const ZpMersenneIntElement& f2)
    {
        ZpMersenneIntElement answer;

        int temp =  (int)elem - (int)f2.elem;

        if(temp<0){
            answer.elem = temp + p;
        }
        else{
            answer.elem = temp;
        }

        return answer;
    }
    ZpMersenneIntElement operator/(const ZpMersenneIntElement& f2)
    {
        //code taken from NTL for the function XGCD
        int a = f2.elem;
        int b = p;
        long s;

        int  u, v, q, r;
        long u0, v0, u1, v1, u2, v2;

        int aneg = 0, bneg = 0;

        if (a < 0) {
            if (a < -NTL_MAX_LONG) Error("XGCD: integer overflow");
            a = -a;
            aneg = 1;
        }

        if (b < 0) {
            if (b < -NTL_MAX_LONG) Error("XGCD: integer overflow");
            b = -b;
            bneg = 1;
        }

        u1=1; v1=0;
        u2=0; v2=1;
        u = a; v = b;

        while (v != 0) {
            q = u / v;
            r = u % v;
            u = v;
            v = r;
            u0 = u2;
            v0 = v2;
            u2 =  u1 - q*u2;
            v2 = v1- q*v2;
            u1 = u0;
            v1 = v0;
        }

        if (aneg)
            u1 = -u1;


        s = u1;

        if (s < 0)
            s =  s + p;

        ZpMersenneIntElement inverse(s);

        return inverse* (*this);
    }

    ZpMersenneIntElement operator*(const ZpMersenneIntElement& f2)
    {
        ZpMersenneIntElement answer;

        long multLong = (long)elem * (long) f2.elem;

        //get the bottom 31 bit
        unsigned int bottom = multLong & p;

        //get the top 31 bits
        unsigned int top = (multLong>>31);

        answer.elem = bottom + top;

        //maximim the value of 2p-2
        if(answer.elem>=p)
            answer.elem-=p;

        //return ZpMersenneIntElement((bottom + top) %p);
        return answer;
    }

    ZpMersenneIntElement& operator+=(const ZpMersenneIntElement& f2){ elem = (f2.elem + elem) %p; return *this;};
    ZpMersenneIntElement& operator*=(const ZpMersenneIntElement& f2)
    {
        long multLong = (long)elem * (long) f2.elem;

        //get the bottom 31 bit
        unsigned int bottom = multLong & p;

        //get the top 31 bits
        unsigned int top = (multLong>>31) ;

        elem = bottom + top;

        //maximim the value of 2p-2
        if(elem>=p)
            elem-=p;

        return *this;
    }

};

inline ::ostream& operator<<(::ostream& s, const ZpMersenneIntElement& a){ return s << a.elem; };


template <class FieldType>
class TemplateField {
private:

    PrgFromAES prg;
    long fieldParam;
    int elementSizeInBytes;
    int elementSizeInBits;
    FieldType* m_ZERO;
    FieldType* m_ONE;
public:


    /**
     * the function create a field by:
     * generate the irreducible polynomial x^8 + x^4 + x^3 + x + 1 to work with
     * init the field with the newly generated polynomial
     */
    TemplateField(long fieldParam);

    /**
     * return the field
     */

    string elementToString(const FieldType &element);
    FieldType stringToElement(const string &str);


    void elementToBytes(unsigned char* output,FieldType &element);

    FieldType bytesToElement(unsigned char* elemenetInBytes);
    void elementVectorToByteVector(vector<FieldType> &elementVector, vector<byte> &byteVector);

    FieldType* GetZero();
    FieldType* GetOne();

    int getElementSizeInBytes(){ return elementSizeInBytes;}
    int getElementSizeInBits(){ return elementSizeInBits;}
    /*
     * The i-th field element. The ordering is arbitrary, *except* that
     * the 0-th field element must be the neutral w.r.t. addition, and the
     * 1-st field element must be the neutral w.r.t. multiplication.
     */
    FieldType GetElement(long b);
    FieldType Random();
    ~TemplateField();

};

template <class FieldType>
string TemplateField<FieldType>::elementToString(const FieldType& element)
{
    ostringstream stream;
    stream << element;
    string str =  stream.str();
    return str;
}


template <class FieldType>
FieldType TemplateField<FieldType>::stringToElement(const string &str) {

    FieldType element;

    istringstream iss(str);
    iss >> element;

    return element;
}



/**
 * A random random field element, uniform distribution
 */
template <class FieldType>
FieldType TemplateField<FieldType>::Random() {
    unsigned long b;
    if(elementSizeInBytes<=4)
    {
        prg.generateKey(128);
        vector<byte> v(32);
        prg.getPRGBytes(v ,0, 32);
        memccpy(&b, &v, 0, sizeof(b));
    }
    return GetElement(b);
}

template <class FieldType>
FieldType* TemplateField<FieldType>::GetZero()
{
    return m_ZERO;
}

template <class FieldType>
FieldType* TemplateField<FieldType>::GetOne()
{
    return m_ONE;
}


template <class FieldType>
TemplateField<FieldType>::~TemplateField() {
    delete m_ZERO;
    delete m_ONE;
}

#endif //SCAPILITEAPK_MERSENNE_H
