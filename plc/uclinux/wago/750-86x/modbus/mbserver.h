/*---------------------------------------------------------------------------
Filename: mbserver.h  -  Header for Modbus like access to data areas

Description:            Header of Server for Modbus like access to data areas
---------------------------------------------------------------------------*/

/* the following sequence will declare a character constant containing module
 * name and version in the name and in the contents. A "." in the file name
 * must be replaced by "_".
 */
#define PEP_MODNAME mbserver_h
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
/*   WAGO Kontakttechnik GmbH Hansastr. 27 32423 Minden(Westf.)          */
/*        Tel. +49571/887-0 Fax. +49571/887169                           */
/*                                                                       */
/*   Projekt: Feldbuskoppler 750-841                                     */
/*                                                                       */
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Datei:       ModbusServer.h                                           */
/*                                                                       */
/* Verwendung:  Header-Datei                                             */
/*                                                                       */
/* Aufgabe:     Modbus Header                                            */
/*                                                                       */
/* Zielsystem:                                                           */
/*                                                                       */
/* Ersteller:                                                            */
/*                                                                       */
/*                                                                       */
/* Aenderungshistorie                                                    */
/*                                                                       */
/* Version  Datum    Aenderung Name                                      */
/*                                                                       */
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Defines                                                               */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#define MB_NOT_CONNECTED            1   /* Modbus-Server ist nicht mit Client verbunden */
#define MB_CONNECTED                2   /* Modbus-Server ist mit Client verbunden */

#define MODBUS_PORT    502              //Original: 502
#define MAX_MODBUS_TCP_CONNECTIONS  5 /* 5 Modbus Verbindungen über TCP gleichzeitig */
#define MAX_MODBUS_UDP_CONNECTIONS  5 /* 5 Modbus Pakete über UDP gleichzeitig */

/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Typdefinitionen                                                       */
/*                                                                       */
/*-----------------------------------------------------------------------*/

/* Socket Informationen */
typedef struct _MB_SOCKET_STRUCT
{
 struct _MB_SOCKET_STRUCT   *pstNext;
 struct _MB_SOCKET_STRUCT   *pstPrev;
 int    iSocketDescriptor;
 unsigned int uiLastRequestTime;
} MB_SOCKET_STRUCT;    


/* Liste Socket Informationen */
typedef struct _MB_SOCKET_LIST
{
  MB_SOCKET_STRUCT *ModbusSocketListHead; /*  Listenanfang */
  MB_SOCKET_STRUCT *ModbusSocketListTail; /*  Listenende */
} MB_SOCKET_LIST;



/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Funktionsprototypen                                                   */
/*                                                                       */
/*-----------------------------------------------------------------------*/
//int InitModbus ( NU_MEMORY_POOL *pSysMem );
//void CloseAllModbusSockets ( void );
