/*---------------------------------------------------------------------------
Filename: mbfuns.h  -  Header for Functions for Modbus like access to data areas

Description:            Header for Functions for Modbus like access to data areas
---------------------------------------------------------------------------*/

/* the following sequence will declare a character constant containing module
 * name and version in the name and in the contents. A "." in the file name
 * must be replaced by "_".
 */
#define PEP_MODNAME mbfuns_h
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

/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Include-Dateien                                                       */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include <asm/byteorder.h> //Für Ermittlung __LITTLE_ENDIAN

/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Defines                                                               */
/*                                                                       */
/*-----------------------------------------------------------------------*/

#define FBUS_ERROR  0x01
#define KBUS_ERROR  0x02


#define NU_NULL     0
#define NU_SUCCESS  0


/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Modbus-Definitionen                                                   */
/*                                                                       */
/*-----------------------------------------------------------------------*/

/*-- Function Codes */

/*---- Class 0 */
#define READ_MULTIPLE_REGISTERS     0x03
#define WRITE_MULTIPLE_REGISTERS    0x10

/*---- Class 1 */
#define READ_COILS                  0x01
#define READ_INPUT_DISCRETES        0x02
#define READ_INPUT_REGISTERS        0x04
#define WRITE_COIL                  0x05
#define WRITE_SINGLE_REGISTER       0x06
#define READ_EXCEPTION_STATUS       0x07

/*---- Class 2 */
#define FORCE_MULTIPLE_COILS        0x0F
#define MASK_WRITE_REGISTER         0x16
#define READ_WRITE_REGISTERS        0x17

/*---- Class Vendor Specific */
#define GET_COMM_EVENT_COUNTER      0x0B

/*-- Exception Codes */

#define ILLEGAL_FUNCTION            0x01
#define ILLEGAL_DATA_ADDRESS        0x02
#define ILLEGAL_DATA_VALUE          0x03
#define SERVER_FAILURE              0x04
#define SLAVE_DEVICE_BUSY           0x06

/*---- Buffer Sizes */
//#define MODBUS_BUFFER_SIZE  1500
#define MAX_MB_BUFFER_SIZE 1500

/*---- Laengenbeschraenkungen der Funktionen */
#define READ_REGS_MAX           125
#define WRITE_REGS_MAX          100
#define FORCE_COILS_MAX         800
#define READ_COILS_MAX          2000
#define READ_COILS_BYTE_MAX     250   /* = READ_COILS_MAX  / 8 */
#define FORCE_COILS_BYTE_MAX    100   /* = FORCE_COILS_MAX / 8 */

/*---- Festgelegte Laengen in Byte */
#define MODBUS_HEADER_LENGTH                      8     /* inclusive Function-Code */
#define LENGTH_TO_UNIT_ID                         6     /* Header bis einschliesslich LengthField */
#define WRITE_MULTIPLE_REGISTERS_RESPONSE_LENGTH 12
#define WRITE_SINGLE_REGISTER_RESPONSE_LENGTH    12
#define WRITE_COIL_RESPONSE_LENGTH               12
#define FORCE_MULTIPLE_COILS_RESPONSE_LENGTH     12
#define READ_MULTIPLE_REGISTERS_LENGTH            9     /* ohne Daten */
#define READ_WRITE_REGISTERS_LENGTH               9
#define READ_COIL_LENGTH                          9
#define MASK_WRITE_REGISTER_RESPONSE_LENGTH      14
#define GET_COMM_EVENT_COUNTER_LENGTH            12
#define GET_COMM_EVENT_COUNTER_DATA_LENGTH        6

/*----- Fehler-Codes */
#define MODBUS_PROTOCOL_ERROR     -1  /* Protokoll-Fehler */
#define MODBUS_LENGTH_ERROR       -2  /* Angefragte Laenge falsch */
#define MB_WATCHDOG_EXPIRE        -3  /* Modbus Watchdog abgelaufen */

/*---- Adressbereiche, Zugriffskennungen für Tabellen */
#define UNKNOWN_ADDRESS                 0
#define PROZESS_IMAGE_ADDRESS_IN        1
#define PROZESS_IMAGE_ADDRESS_OUT       2
#define PFC_VAR_ADDRESS_IN              3
#define PFC_VAR_ADDRESS_OUT             4
#define INTERNAL_VAR_ADDRESS_IN         5
#define INTERNAL_VAR_ADDRESS_OUT        6
#define NOVRAM_ADDRESS_IN               7
#define NOVRAM_ADDRESS_OUT              8
#define PROZESS_IMAGE_ADDRESS_EXT_IN    9
#define PROZESS_IMAGE_ADDRESS_EXT_OUT   10
#define ONBOARD_ADDRESS_IN              11
#define ONBOARD_ADDRESS_OUT             12
#define FBUS_IMAGE_ADDRESS_IN           13
#define FBUS_IMAGE_ADDRESS_OUT          14

/*---- Allgemeines */
#define EXCEPTION_LENGTH        9


/*-- Strukturen */

/*---- Allgemeine Modbus Struktur      */
typedef struct
{
  UINT16 usTransactionIdentifier;
  UINT16 usProtocolIdentifier;
  UINT16 usLengthField;
  UINT8  ucUnitIdentifier; 
  UINT8  ucFunctionCode;
} MODBUS_HEADER;

/* copied from kbusioctldefs.h */
typedef struct transfer_data
{ 
  int op_offset;
  int op_size;
  char data[256];
} transfer_data;

/* copied from kbus_lib.h */
typedef struct
{
  unsigned short usDigitalInputOffset;
  unsigned short usDigitalOutputOffset;
} T_TERMINAL_OFFSET;

/*---- Funktionsspezifische Strukturen */

/*------ FC3 Read Multiple Registers */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usWordCount;
} READ_MULTIPLE_REGISTERS_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT8  ucByteCount;
  UINT8 aucRegisterValues[READ_REGS_MAX];
} READ_MULTIPLE_REGISTERS_RESPONSE;


/*------ FC16 Write Multiple Registers */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usWordCount;
  UINT8  ucByteCount;
  UINT8 aucRegisterValues[WRITE_REGS_MAX];
} WRITE_MULTIPLE_REGISTERS_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usWordCount;
} WRITE_MULTIPLE_REGISTERS_RESPONSE;


/*------ FC1 Read Coils */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usBitCount;
} READ_COILS_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT8 ucByteCount;
  UINT8  ucBitValues[READ_COILS_BYTE_MAX];
} READ_COILS_RESPONSE;


/*------ FC3 Read Input Discretes */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usBitCount;
} READ_INPUT_DISCRETES_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT8 ucByteCount;
  UINT8 ucBitValues[READ_COILS_BYTE_MAX];
} READ_INPUT_DISCRETES_RESPONSE;


/*------ FC4 Read Input Registers */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usWordCount;
} READ_INPUT_REGISTERS_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT8  ucByteCount;
  UINT8 aucRegisterValues[READ_REGS_MAX];
} READ_INPUT_REGISTERS_RESPONSE;



/*------ FC5 Write Coil */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usValue;  
} WRITE_COIL_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usValue;  
} WRITE_COIL_RESPONSE;


/*------ FC6 Write Single Register */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usRegisterValue;  
} WRITE_SINGLE_REGISTER_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usRegisterValue;  
} WRITE_SINGLE_REGISTER_RESPONSE;


/*------ FC7 Read Exception Status ( keine Struktur ) */

/* FC11 Get comm event counter */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usStatus;
  UINT16 usEventCount;
} GET_COMM_EVENT_COUNTER_RESPONSE;


/*------ FC15 Force Multiple Coils */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usBitCount;
  UINT8  ucByteCount;
  UINT8  aucData[FORCE_COILS_BYTE_MAX];
} FORCE_MULTIPLE_COILS_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usBitCount;
} FORCE_MULTIPLE_COILS_RESPONSE;


/*------ FC22 Mask Write Register */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usAndMask;
  UINT16 usOrMask;
} MASK_WRITE_REGISTER_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumber;
  UINT16 usAndMask;
  UINT16 usOrMask;
} MASK_WRITE_REGISTER_RESPONSE;


/*------ FC23 Read Write Register */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT16 usReferenceNumberRead;
  UINT16 usWordCountRead;
  UINT16 usReferenceNumberWrite;
  UINT16 usWordCountWrite;
  UINT8  ucByteCountWrite;
  UINT8 aucRegisterValues[WRITE_REGS_MAX];
} READ_WRITE_REGISTERS_REQUEST;

typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT8  ucByteCount;
  UINT8 aucRegisterValues[READ_REGS_MAX];
} READ_WRITE_REGISTERS_RESPONSE;


/*------ Exception */
typedef struct
{
  MODBUS_HEADER  stModbusHeader;
  UINT8 ucException;
} EXCEPTION;


/*---- Implementierungsabhängige Strukturen */

/*------ Modbus Statistik */
typedef struct
{
  UINT16 usSlaveDeviceFailure;
  UINT16 usBadProtocol;
  UINT16 usBadLength;
  UINT16 usBadFunction;
  UINT16 usBadAddress;
  UINT16 usBadData;
  UINT16 usTooManyRegisters;
  UINT16 usTooManyBits;
  UINT16 usModbusMessageCounter;
} T_MODBUS_STATISTIK;



/*------ Modbus Funktionstabelle */ 
typedef struct  
{
  UINT8  cFunctionCode;
  INT32 (*pFunction) (char *pcModbusPacket);
} MODBUS_FUNCTION_SELECT;

/*------ Modbus Adressen */
typedef struct
{
  UINT16 usModbusStartAddress;
  UINT16 usModbusEndAddress;
  UINT16 * pusPhysicalAddress;
  UINT16 usArtOfAddress;
} MODBUS_MEMORY_MAP;

/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Funktionsprototypen                                                   */
/*                                                                       */
/*-----------------------------------------------------------------------*/
int ProcessModbusRequest ( char *pcModbusPacket, int iBytesReceived );

INT32 ReadMultipleRegisters ( char *pcModbusPacket);
INT32 WriteMultipleRegisters ( char *pcModbusPacket);
INT32 WriteSingleRegister ( char *pcModbusPacket );
INT32 MaskWriteRegister ( char *pcModbusPacket );
INT32 ReadWriteRegisters ( char *pcModbusPacket );
INT32 GetCommEventCounters ( char *pcModbusPacket );
INT32 ForceMultipleCoils ( char *pcModbusPacket );
INT32 ReadInputBits ( char *pcModbusPacket );
INT32 WriteCoil ( char *pcModbusPacket );

UINT32 CheckAddress ( MODBUS_MEMORY_MAP *pstMemoryMap, UINT32 uiNumberOfEntries,
                      UINT16 usStartAddress, UINT16 usWordCount,
                      UINT16 **pusPhysAddress, UINT16 *pusRegisterOffset );
                      
INT32 ReturnException ( char *pcModbusPacket, UINT8 ucException );
INT32 ProcessOneModbusRequest ( char *pcModbusPacket );
void InitModbusVariables ( void );
void CopyDigitalData ( UINT16 *pusSource, UINT32 uiSourceBitOffset, UINT16 *pusDest,
                       UINT32 uiDestinationBitOffset, UINT16 usBitCount );
void MbRegisterCopy_WITH_SWAP_IF_LITTLE ( UINT16 __packed *pusDest, UINT16 __packed *pusSrc, UINT32 uiNrOfRegs );
void MbRegisterCopy ( UINT8 *, UINT8 *, UINT32);

/*----------------------- Ende der Quelldatei ---------------------------*/
