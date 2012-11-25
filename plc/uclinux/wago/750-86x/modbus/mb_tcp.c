/*---------------------------------------------------------------------------
Filename: mbserver.c  -  Server for Modbus like access to data areas

Description:            Server for Modbus like access to data areas
Actual Revision:        $Revision: 1.2 $
---------------------------------------------------------------------------*/

/* the following sequence will declare a character constant containing module
 * name and version in the name and in the contents. A "." in the file name
 * must be replaced by "_".
 */
#define PEP_MODNAME mbserver_c
#define PEP_VER_ID 01a

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

#include <sched.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "sys/socket.h"
#include "unistd.h"
#include "arpa/ftp.h"
#include "arpa/inet.h"
#include <fcntl.h>

#include <net/if.h>
#include <linux/tcp.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>

#include "mbtypes.h"
#include "mbserver.h"
#include "mbfuns.h"
#include "kbusapi.h"
/*--------------------------------------------------------------------------*/
/*                                                                          */
/* modulinterne Defines                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define TIMEOUT_SEC 100 /* 100 Sekunden Verbindungs Timeout */
#define TIMEOUT_USEC 0
/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Externe Variablen                                                     */
/*                                                                       */
/*-----------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Globale Variablen                                                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
char acMbTcpBuffer[MAX_MB_BUFFER_SIZE]; /* Modbus TCP Buffer */
int iCntMBTCPConnections = 0;
int iCntMBUDPConnections = 0;
long lMBConnection = 0;
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
void mbTCPServerWork(int sFd, char *address, unsigned short port);
void mbTCPServerTask(void);


long CheckModbusHeader(char *pcModbusPacket)
{
  MODBUS_HEADER *pstModbusHeader;

  pstModbusHeader = (MODBUS_HEADER *) pcModbusPacket;
  if (__cpu_to_be16(pstModbusHeader->usProtocolIdentifier) != 0) /* Protokoll ID ueberpruefen
                                                                   * (muss immer 0 sein) */
  {
    return (-1);
  }
  if (__cpu_to_be16(pstModbusHeader->usLengthField) > 0xFF) /* Laenge nicht > 255 */
  {
    return (-1);
  }
  return ((long) __cpu_to_be16(pstModbusHeader->usLengthField));
}


static UINT16 MacId[3]; 
UINT16* getMAC()
{
    return MacId;
}


void mbTCPServerWork(int sFd, char *address, unsigned short port)
{
  long iConnectionStatus, iStatus;
  long iBytesReceived, iBytesSent, iBytesToSend = 0, iPacketLength;
  int iInvalidHeaders = 0;
  fd_set setsFd;
  struct timeval timeout;

  /* read MAC-ID */
  {
      struct ifreq ifr;
      UINT16 localMacId[3];
      memset(&ifr,0,sizeof(struct ifreq));
      strcpy(ifr.ifr_name, "eth0");
      ioctl(sFd, SIOCGIFHWADDR, &ifr);
      memcpy(localMacId, ifr.ifr_hwaddr.sa_data, 6);
      MacId[0] = (UINT16) (__cpu_to_be16(localMacId[0]));
      MacId[1] = (UINT16) (__cpu_to_be16(localMacId[1]));
      MacId[2] = (UINT16) (__cpu_to_be16(localMacId[2]));
  }

  //printf ("WAGO Modbus TCP server work task HAS connection to Address %s, port %d.\n", address, port);

  iConnectionStatus = MB_CONNECTED;
  while (iConnectionStatus == MB_CONNECTED)
  {
    /* Verbindung zeitlich ueberwachen */
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;
    FD_ZERO(&setsFd);
    FD_SET(sFd, &setsFd);

    iStatus = select(sFd + 1, &setsFd, NULL, NULL, &timeout);
    if (iStatus == 0)
    { /* Timeout */
      iStatus = close(sFd);
      printf("Timeout waiting for first block at socket %d. Closing it.\n", sFd);
      perror("Timeout waiting for first block.");
      iConnectionStatus = MB_NOT_CONNECTED;
    }
    else
    {
      iBytesReceived = read(sFd, &acMbTcpBuffer[0], LENGTH_TO_UNIT_ID);
      if (iBytesReceived <= 0)
      {
	  close(sFd);
	  iConnectionStatus = MB_NOT_CONNECTED;
      }
      else if (iBytesReceived < 6)
      { /* Ungueltige Telegrammlaenge */
        /* LOGERR(iStatus, ERR_TRACE, NU_NET_ERR, 0, 0); */
	if (iInvalidHeaders > 25) 
	{
	    iStatus = close(sFd);
	    iConnectionStatus = MB_NOT_CONNECTED;
	    printf("Too many invalid Modbus-Headers at socket %d. Closing it.\n", sFd);
	    perror("Too many invalid Modbus-Headers.");
	}
	iInvalidHeaders ++;
      }
      else
      { /* Modbus Header korrekt empfangen. Modbus Header auf Fehler pruefen */
	iInvalidHeaders = 0;
        iPacketLength = CheckModbusHeader(&acMbTcpBuffer[0]);
        if (iPacketLength > 0) /* Kein Fehler im Header */
        { /* Verbindung zeitlich ueberwachen */
          timeout.tv_sec = 10;
          timeout.tv_usec = 0;
          FD_ZERO(&setsFd);
          FD_SET(sFd, &setsFd);
          iStatus = select(sFd + 1, &setsFd, NULL, NULL, &timeout);
          if (iStatus == 0)
          { /* Timeout */
            iStatus = close(sFd);
            printf("Timeout waiting for next block at socket %d. Closing it.\n", sFd);
            perror("Timeout waiting for next block.");
            iConnectionStatus = MB_NOT_CONNECTED;
          }
          else
          { /* Restliche Daten des Modbus Requests empfangen */
            iBytesReceived = read(sFd, &acMbTcpBuffer[LENGTH_TO_UNIT_ID], (unsigned short) iPacketLength);
            if (iBytesReceived == iPacketLength)
            { /* Länge korrekt */
              /* Datenzugriff frei. Modbus Protokoll bearbeiten */
              iBytesToSend = ProcessOneModbusRequest(&acMbTcpBuffer[0]);
            }
            else
            {
              iBytesToSend = ReturnException ( &acMbTcpBuffer[0], ILLEGAL_DATA_VALUE );
            }
            if (iBytesToSend > 0)
            { /* Antwort senden */
              iBytesSent = write(sFd, &acMbTcpBuffer[0], (unsigned short) iBytesToSend);
              if (iBytesSent != iBytesToSend)
              { /* Fehler beim senden der Daten. Verbindung schliessen */
                printf("Error when sending via socket %d. Closing it.\n", sFd);
                perror("Error when sending.");
                iStatus = close(sFd);
                iConnectionStatus = MB_NOT_CONNECTED;
              }
            }
            else
            { /* Nichts zu senden, da Fehler beim Verarbeiten der Daten. Verbindung
               * schliessen */
              printf("Error when processing request. Closing socket %d.\n", sFd);
              perror("Error when processing request.");
              iStatus = close(sFd);
              iConnectionStatus = MB_NOT_CONNECTED;
            }
          }
        }
        else
        { /* Fehler im Modbus Header festgestellt */
          printf("Error in Modbus Header. Closing socket %d.\n", sFd);
          perror("Error in Modbus HeaderError in Modbus Header.");
          iStatus = close(sFd);
          iConnectionStatus = MB_NOT_CONNECTED;
        }
      } /* Ende Modbus Header korrekt empfangen */
    }
  } /* Ende while */
  exit(0);
}


void mbTCPServerTask(void)
{
  struct sockaddr_in serverAddr;
  struct sockaddr_in clientAddr;
  int sockAddrSize, sFd, iStatus, i=1;
  int newFd = 0;
  pid_t PID;  /* process id */
  
  /* Socket fuer den Datenaustausch anlegen */
  sFd = socket(PF_INET, SOCK_STREAM, 0);
  if (sFd < 0)
  {
    printf("Socket failed.\n");
    perror("Modbus TCP server task opening of Master socket failed.");
  }
  else
  {
    iStatus = setsockopt(sFd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
    if (iStatus < 0)
    {
      printf("setsockopt failed.\n");
      perror("setsockopt failed.");
    }
    
    i=1;
    setsockopt(sFd, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
    
    sockAddrSize = sizeof(struct sockaddr_in);
    bzero((char *) &serverAddr, sockAddrSize);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(MODBUS_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    iStatus = bind(sFd, (struct sockaddr *) & serverAddr, sockAddrSize);
    if (iStatus < 0)
    {
      printf("bind failed.\n");
      perror("Modbus TCP server task bind failed.");
    }
    else
    {
      iStatus = listen(sFd, MAX_MODBUS_TCP_CONNECTIONS);
      if (iStatus == -1)
      {
        printf("listen failed.\n");
        perror("Modbus TCP server task listening failed.");
      }
      else
      {
        printf ("Modbus TCP server ready.\n");
        /**********************************/
        /* Solange accept erfolgreich ist */
        /**********************************/
        while (1)
        {
          newFd = accept(sFd, (struct sockaddr *) & clientAddr, &sockAddrSize);
          if (newFd < 0) /* accept gibt beim Fehler -1 zurueck */
          {
            printf("accept failed.\n");
            perror("Modbus TCP server task accepting failed.");
          }
          else
          {
            if ( ( PID = vfork()) == -1 )
            {
		close (newFd);
            }
            else if ( PID == 0 ) /* child */
            {
//              struct sched_param p;
//              sched_getparam(PID, &p);
//              sched_setscheduler(PID, SCHED_RR, &p);
              /* Aufgerufene Task Schliesst Socket selbst: */
              mbTCPServerWork(newFd, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
//              close (newFd);
            }
            else 
            {
	      wait(&iStatus);
              close (newFd);
	    }
          }
        } /* while (1) */
        printf("Closing Master Socket = %d.\n", sFd);
        close(sFd);
      }
    }
  }
}


int main(void)
{
  if (0 != KbusOpen()) 
  {
    printf("Couldn't open kbus device file. KBus kernel module loaded?\n");
  }
  else
  {
    InitModbusVariables();
    readKbusConfig();
    mbTCPServerTask();
  } 
  return (0);
}
