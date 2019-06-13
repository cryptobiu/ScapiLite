//
// Created by liork on 12/03/18.
//

#ifndef SCAPILITEAPK_GF2_8LOOKUPTABLE_H
#define SCAPILITEAPK_GF2_8LOOKUPTABLE_H


#include <NTL/GF2E.h>
#include <NTL/GF2X.h>
#include <NTL/ZZ_p.h>
#include <NTL/GF2XFactoring.h>
#include "Common.h"


class GF2_8LookupTable {

public:
    byte elem;
    static byte multTable[256][256];
    static void initTable();
    GF2_8LookupTable(){elem = 0;};
    GF2_8LookupTable(unsigned int elem): elem(elem){};

    GF2_8LookupTable& operator=(const GF2_8LookupTable& other){elem = other.elem; return *this;};
    bool operator!=(const GF2_8LookupTable& other){ return !(other.elem == elem); };

    GF2_8LookupTable operator+(const GF2_8LookupTable& f2){return GF2_8LookupTable(f2.elem ^ elem);};
    GF2_8LookupTable operator-(const GF2_8LookupTable& f2){return GF2_8LookupTable(f2.elem ^ elem);};
    GF2_8LookupTable operator/(const GF2_8LookupTable& f2);
    GF2_8LookupTable operator*(const GF2_8LookupTable& f2){return GF2_8LookupTable( multTable[elem][f2.elem]);};
    GF2_8LookupTable& operator*=(const GF2_8LookupTable& f2){ elem = multTable[elem][f2.elem] ; return *this;};
    GF2_8LookupTable& operator+=(const GF2_8LookupTable& f2){ elem = (f2.elem ^ elem) ; return *this;};
};

inline ::ostream& operator<<(::ostream& s, const GF2_8LookupTable& a){ return s << (unsigned int)a.elem; };

#endif //SCAPILITEAPK_GF2_8LOOKUPTABLE_H
