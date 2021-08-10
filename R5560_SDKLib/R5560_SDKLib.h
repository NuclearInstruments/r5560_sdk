// Il seguente blocco ifdef rappresenta il modo standard di creare macro che semplificano 
// l'esportazione da una DLL. Tutti i file all'interno della DLL sono compilati con il simbolo R5560_SDKLIB_EXPORTS
// definito nella riga di comando. Questo simbolo non deve essere definito in alcun progetto
// che utilizza questa DLL. In questo modo qualsiasi altro progetto i cui file di origine includono questo file vedranno le funzioni 
// R5560_SDKLIB_API come importate da una DLL, mentre la DLL vedrà i simboli
// definiti con questa macro come esportati.



#ifdef _WIN32
    #ifdef R5560_SDKLIB_EXPORTS
        #define R5560_SDKLIB_API extern "C" __declspec(dllexport)// __declspec(dllexport)
    #else
        #define R5560_SDKLIB_API extern "C" __declspec(dllimport)
    #endif
#else
    #ifdef R5560_SDKLIB_EXPORTS
        #define R5560_SDKLIB_API __attribute__((visibility("default")))
    #else
        #define R5560_SDKLIB_API
    #endif
    
#endif

#define NO_ERROR 0
#define ERR_NO_CONNECTION -1
#define ERR_SOCKET -2
#define ERR_CONNECTION_DROP -3
#define ERR_ALLOC -4
#define ERR_OUT_OF_LIMIT -5
#define ERR_TIMEOUT -6

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <stdint.h>

#define ZMQ_ENDPOINT_COUNT 1

typedef enum 
{
	LOW_LEVEL_TCP = 0
} SOCKET_TYPE;


typedef enum 
{
	REG_ACCESS = 0,
	STREAMING_BLOCKING = 1,
	STREAMING_NONBLOCKING = 2
} BUS_MODE;


typedef struct 
{
	void *zmq_context;
	void *zmq_pullsocket;
	int zmq_connected;		
	int recv_blocking;
}tZMQEndpoint;

typedef struct {
	int Csocket;
	int connected;
	uint32_t __IICBASEADDRESS;
	uint32_t __IICBASEADDRESS_STATUS;
	SOCKET_TYPE socketType;
	tZMQEndpoint *zmq;
} tR5560_Handle;

typedef enum {
	IREG_VERSION_SW=0,
	IREG_VERSION_MANAGER=1,
	IREG_VERSION_BRIDGE=2,
	IREG_MODEL=3,
	IREG_SYS_PID=5,
	IREG_DAQ_PID=6,
	IREG_GAIN=7,
	IREG_OFFSET=8,
	IREG_SHAPER=9,
	IREG_50R=10,
	IREG_5DIV=11,
	IREG_DAQ_STATUS=12,
	IREG_DAQ_ETH=13,
	IREG_DAQ_IP=14,
	IREG_DAQ_POSITION=15,
	IREG_FW_VER=16,
	IREG_FW_BUILD=17,
	IREG_FW_STRING=18,
	IREG_REBOOT_DAQ=19,
} t_IREG;


#ifndef _WIN32 
	#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
#endif
R5560_SDKLIB_API int R5560_ConnectTCP(char *ipaddress, uint32_t port, tR5560_Handle *handle);
R5560_SDKLIB_API int NI_CloseConnection(tR5560_Handle *handle);
R5560_SDKLIB_API int NI_WriteData(uint32_t *data, uint32_t count, 
										uint32_t address, 
										tR5560_Handle *handle,
										uint32_t *written_data);
R5560_SDKLIB_API int NI_ReadData(uint32_t *data, uint32_t count, 
										uint32_t address, tR5560_Handle *handle, 
										uint32_t *read_data);
R5560_SDKLIB_API int NI_ReadFifo(uint32_t *data, uint32_t count, 
										uint32_t address, uint32_t fifo_status_address, BUS_MODE bus_mode, 
										uint32_t timeout_ms, tR5560_Handle *handle, 
										uint32_t *read_data);
R5560_SDKLIB_API int NI_WriteReg(uint32_t data, uint32_t address, tR5560_Handle *handle);
R5560_SDKLIB_API int NI_ReadReg(uint32_t *data, uint32_t address, tR5560_Handle *handle);
R5560_SDKLIB_API int NI_DMA_Read(uint32_t dma_channel, char *buffer, uint32_t max_len, uint32_t *valid_data, tR5560_Handle *handle);
R5560_SDKLIB_API int NI_DMA_SetOptions(uint32_t dma_channel, int blocking, int timeout, int buffer_length, tR5560_Handle *handle);
R5560_SDKLIB_API void * R5560_HandleAllocator();

R5560_SDKLIB_API int NI_InternalWriteReg(uint32_t data, uint32_t address, tR5560_Handle *handle);
R5560_SDKLIB_API int NI_InternalReadReg(uint32_t *data, uint32_t address, tR5560_Handle *handle);

#ifndef _WIN32
	
#ifdef __cplusplus
}
#endif
#endif
