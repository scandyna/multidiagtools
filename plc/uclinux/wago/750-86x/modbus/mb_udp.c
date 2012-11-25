/*---------------------------------------------------------------------------
Filename: modbus_udp.c  -  Server for Modbus like access to data areas

Description:            Server for Modbus like access to data areas
---------------------------------------------------------------------------*/

/* the following sequence will declare a character constant containing module
 * name and version in the name and in the contents. A "." in the file name
 * must be replaced by "_".
 */
#define PEP_MODNAME mbserver_c
#define PEP_VER_EPIID 01a

#define PEP_VER_VARNAME_H(v,m,u) PEP_VER_##m##u##v
#define PEP_VER_VARNAME(v,m) PEP_VER_VARNAME_H(v,m,_)

#define PEP_VER_STR_H(m,v) "$PEPVERSION: " #m " " #v " $"
#define PEP_VER_STR(m,v) PEP_VER_STR_H(m,v)

#ifndef _ASMLANGUAGE
volatile static char PEP_VER_VARNAME(PEP_MODNAME, PEP_VER_ID)[]
     __attribute__((unused)) = PEP_VER_STR(PEP_MODNAME, PEP_VER_ID);
#endif

#undef PEP_MODNAME
#undef PEP_VER_ID
#undef PEP_VER_VARNAME_H
#undef PEP_VER_VARNAME
#undef PEP_VER_STR_H
#undef PEP_VER_STR

/*--------------------------------------------------------------------------*/

/*                                                                          */
/* Include-Dateien                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include "sys/socket.h"
#include "unistd.h"
#include "arpa/ftp.h"
#include "arpa/inet.h"
#include <fcntl.h>

#include "mbtypes.h"
#include "mbserver.h"
#include "mbfuns.h"
/*
#include <kbus_lib.h>
*/
#include <errno.h>

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* modulinterne Defines                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Externe Variablen                                                     */
/*                                                                       */
/*-----------------------------------------------------------------------*/
extern int errno;

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Globale Variablen                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
char acMbUdpBuffer[MAX_MB_BUFFER_SIZE]; /* Modbus UDP Buffer */

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Modulinterne Variablen                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Modulinterne Funktionen                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Funktion:    CheckModbusHeader                                        */
/*                                                                       */
/* Aufgabe:     Ueberpruefung des Modbus-Headers                         */
/*                                                                       */
/* Eingang:     Zeiger auf Modbus-Header                                 */
/*                                                                       */
/* Ausgabe:     -                                                        */
/*                                                                       */
/* Rueckgabe:   Laenge des Paketes oder -1                               */
/*                                                                       */
/*-----------------------------------------------------------------------*/
long CheckModbusHeader(char *pcModbusPacket)
{
  MODBUS_HEADER *pstModbusHeader;

  pstModbusHeader = (MODBUS_HEADER *) pcModbusPacket;
  if (SWAP_IF_LITTLE(pstModbusHeader->usProtocolIdentifier) != 0) /* Protokoll ID ueberpruefen
                                                                   * (muss immer 0 sein) */
  {
    return (-1);
  }
  if (SWAP_IF_LITTLE(pstModbusHeader->usLengthField) > 0xFF) /* Laenge nicht > 255 */
  {
    return (-1);
  }
  return ((long) SWAP_IF_LITTLE(pstModbusHeader->usLengthField));
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Funktion:                                                                */
/*                                                                          */
/* Aufgabe:                                                                 */
/*                                                                          */
/* Eingang:     -                                                           */
/*                                                                          */
/* Ausgabe:     -                                                           */
/*                                                                          */
/* Rueckgabe:   -                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void mbUDPServerTask(void)
{
  struct sockaddr_in serverAddr;
  struct sockaddr_in clientAddr;
  long iBytesReceived, iBytesSent, iBytesToSend = 0, iPacketLength;
  int sockAddrSize, sFd, iStatus;
  int clientLen = sizeof clientAddr;

  /* Socket fuer den Datenaustausch anlegen */
  sFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sFd < 0)
  {
    printf("Socket failed : %s\n", strerror(errno));
  }
  else
  {
    sockAddrSize = sizeof(struct sockaddr_in);
    bzero((char *) &serverAddr, sockAddrSize);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(MODBUS_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    iStatus = bind(sFd, (struct sockaddr *) & serverAddr, sockAddrSize);
    if (iStatus < 0)
    {
      printf("Bind failed : %s\n", strerror(errno));
      perror("Modbus UDP server task bind failed.");
    }
    else
    {
      printf ("Modbus UDP server ready.\n");
      for (;;) /* loop */
      {
        /* Pakete emphangen */
        iBytesReceived = recvfrom (sFd, acMbUdpBuffer, sizeof(char[MAX_MB_BUFFER_SIZE]), 0,
                                   (struct sockaddr *) &clientAddr, &clientLen );
        if (iBytesReceived < 6 ) /* Fehler beim receive */
        {
          printf("Receive failed : %s\n", strerror(errno));
        }
        else /* Receive in Ordnung */
        {
          /* Modbus Header prüfen */
          iPacketLength = CheckModbusHeader ( &acMbUdpBuffer[0] );
          if (iBytesReceived == (iPacketLength + 6))
          {
            iBytesToSend = ProcessOneModbusRequest(&acMbUdpBuffer[0]);
          }
          if (iBytesToSend > 0 )
          {
            iBytesSent = sendto (sFd, acMbUdpBuffer, iBytesToSend, 0,
                                 (struct sockaddr *) &clientAddr, clientLen );
            if ( iBytesSent < 0 )
            {
              printf("Send failed : %s\n", strerror(errno));
            }
          }
        }
      }
    }
  }
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Funktion:                                                                */
/*                                                                          */
/* Aufgabe:                                                                 */
/*                                                                          */
/* Eingang:     -                                                           */
/*                                                                          */
/* Ausgabe:     -                                                           */
/*                                                                          */
/* Rueckgabe:   -                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int main(void)
{
/*
  if (0 != kbus_open())
  {
    printf("Couldn't open kbus device file\n");
  }
  else
  {
    kbus_run(1);
*/
    InitModbusVariables();
    mbUDPServerTask();
/*  } */
  return (0);
}
