#ifndef DATATYPES_H
#define	DATATYPES_H

#ifdef	__cplusplus
extern "C" {
#endif
#define HBYTE(x) (x >> 8)
#define LBYTE(x) (x & 255)
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef const unsigned char* codePtr;
typedef unsigned char* dataPtr;

#ifdef	__cplusplus
}
#endif

#endif	/* DATATYPES_H */