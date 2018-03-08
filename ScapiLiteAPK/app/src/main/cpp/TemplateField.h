//
// Created by liork on 08/03/18.
//

#ifndef SCAPILITEAPK_TEMPLATEFIELD_H
#define SCAPILITEAPK_TEMPLATEFIELD_H

#include "NTL/ZZ_p.h"
#include "NTL/ZZ.h"
#include "gmp.h"
#include "Prg.h"

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


#endif //SCAPILITEAPK_TEMPLATEFIELD_H
