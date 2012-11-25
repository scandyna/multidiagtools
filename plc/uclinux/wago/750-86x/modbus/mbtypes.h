/*---------------------------------------------------------------------------
Filename: mbtypes.h  -  Spezielle Datentypen für Modbus-Software
---------------------------------------------------------------------------*/

/* the following sequence will declare a character constant containing module
 * name and version in the name and in the contents. A "." in the file name
 * must be replaced by "_".
 */
#define PEP_MODNAME mbtypes_h
#define PEP_VER_ID 01a

#define PEP_VER_VARNAME_H(v,m,u) PEP_VER_##m##u##v
#define PEP_VER_VARNAME(v,m) PEP_VER_VARNAME_H(v,m,_)

#define PEP_VER_STR_H(m,v) "$PEPVERSION: " #m " " #v " $"
#define PEP_VER_STR(m,v) PEP_VER_STR_H(m,v)

#ifndef _ASMLANGUAGE
volatile static char PEP_VER_VARNAME(PEP_MODNAME,PEP_VER_ID)[]
    __attribute__ ((unused)) = PEP_VER_STR(PEP_MODNAME,PEP_VER_ID);
#endif

#undef PEP_MODNAME
#undef PEP_VER_ID
#undef PEP_VER_VARNAME_H
#undef PEP_VER_VARNAME
#undef PEP_VER_STR_H
#undef PEP_VER_STR


/*---------------------------------------------*/
/* Datentypen, wie in den WAGO-Sourcen benutzt */
/*---------------------------------------------*/

#define UINT16  unsigned short int
#define INT16   short int
#define UINT32  unsigned int
#define INT32   int
#define UINT8   unsigned char
#define INT8    char

#define __packed    //TODO??????
