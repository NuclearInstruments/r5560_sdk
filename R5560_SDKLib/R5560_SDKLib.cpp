// R5560_SDKLib.cpp: definisce le funzioni esportate per l'applicazione DLL.
//


	#include "stdafx.h"


#include "zmq.h"
#include "R5560_SDKLib.h"


#ifdef _WIN32
  /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
  /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
  #include <unistd.h> /* Needed for close() */
  #include <string.h> /* Needed for close() */
#endif

#define ErrorHandler printf


#ifdef _WIN32

#else
	#define SOCKET int
#endif

int sockInit(void);
int sockQuit(void);
int sockClose(SOCKET sock);



// Esempio di variabile esportata
R5560_SDKLIB_API int nR5560_SDKLib=0;

// Esempio di funzione esportata.
R5560_SDKLIB_API int fnR5560_SDKLib(void)
{
	return 42;
}


R5560_SDKLIB_API void * R5560_HandleAllocator()
{
	tR5560_Handle *tr;
	tr=(tR5560_Handle*)  malloc (sizeof(tR5560_Handle));
	return tr;

}
// Connect to R5560
R5560_SDKLIB_API int R5560_ConnectTCP(char *ipaddress, uint32_t port, tR5560_Handle *handle)
{
	char zmq_string[255];
	int Length;
	sockInit();
	int Csocket;
	handle->connected=0;
	Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Csocket < 0) {
		//ErrorHandler("socket creation failed.\n");
		sockClose(Csocket);
		sockQuit();
		return -1;
	}
	// COSTRUZIONE DELL’INDIRIZZO DEL SERVER
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(ipaddress); // IP del server
	sad.sin_port = htons(port); // Server port
	// CONNESSIONE AL SERVER
	printf("connecting to: %s:%d\n", ipaddress, port);
	if (connect(Csocket, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
		//ErrorHandler("Failed to connect.\n");
		sockClose(Csocket);
		sockQuit();
		return -2;
	}
	handle->Csocket=Csocket;
	handle->connected=1;
	handle->__IICBASEADDRESS=0;
	handle->__IICBASEADDRESS_STATUS=0;
	handle->socketType = LOW_LEVEL_TCP;
	
	handle->zmq = (tZMQEndpoint*) malloc(sizeof(tZMQEndpoint) * (ZMQ_ENDPOINT_COUNT+1));

	//try to conenct to zmq server 
	for (int i=0;i<ZMQ_ENDPOINT_COUNT;i++)
	{
		handle->zmq[i].zmq_context = zmq_ctx_new ();
		handle->zmq[i].zmq_pullsocket = zmq_socket (handle->zmq[i].zmq_context, ZMQ_PULL);
		int hwm=5;
		zmq_setsockopt(handle->zmq[i].zmq_pullsocket, ZMQ_RCVHWM, &hwm, sizeof(int));
		sprintf(zmq_string,"tcp://%s:%d",ipaddress, 5556+i);
		int rc = zmq_connect (handle->zmq[i].zmq_pullsocket, zmq_string);
		handle->zmq[i].recv_blocking = 1;
		if (rc==0)
			handle->zmq[i].zmq_connected=1;
		else
			handle->zmq[i].zmq_connected=0;
	}
	
	return 0;
}


R5560_SDKLIB_API int NI_CloseConnection(tR5560_Handle *handle)
{
	if (handle->connected)
	{
		sockClose(handle->Csocket);
		sockQuit();
		return 0;
	}
	else
		return -1;
}

R5560_SDKLIB_API int NI_WriteData(uint32_t *data, uint32_t count, 
										uint32_t address, 
										tR5560_Handle *handle,
										uint32_t *written_data)
{
	int Length;
	uint8_t *buffer;
	*written_data=0;
	if(handle->connected==0) return -1;
	
	buffer = (uint8_t *) malloc((10+count) * sizeof(uint32_t));
	if (buffer==NULL) return -3;

	*((uint32_t *)(buffer) + 0) = 0x80000000;
	*((uint32_t *)(buffer) + 1) = 0x00000002;
	*((uint32_t *)(buffer) + 2) = address;
	*((uint32_t *)(buffer) + 3) = count;
	memcpy(buffer + 4*4, data, count * sizeof(uint32_t));

	Length = 4*4 + count*sizeof(uint32_t);
	if (send(handle->Csocket, (char*)buffer, Length, 0) != Length) {
		free(buffer);
		return -2;
	}
	*written_data = count;
	free(buffer);
	return 0;
}
R5560_SDKLIB_API int NI_ReadData(uint32_t *data, uint32_t count, 
										uint32_t address, tR5560_Handle *handle, 
										uint32_t *read_data)
{
	int Length;
	uint8_t *buffer;
	*read_data = 0;
	if(handle->connected==0) return -1;
	
	buffer = (uint8_t *) malloc((10+count) * sizeof(uint32_t));
	if (buffer==NULL) return -3;


	*((uint32_t *)(buffer) + 0) = 0x80000000;
	*((uint32_t *)(buffer) + 1) = 0x00000003;
	*((uint32_t *)(buffer) + 2) = address;
	*((uint32_t *)(buffer) + 3) = count;

	Length = 4*4 ;
	if (send(handle->Csocket, (char*)buffer, Length, 0) != Length) {
		free(buffer);
		return -2;
	}


	int bytesRcvd;
	int totalBytesRcvd = 0;
	Length=count * 4;
	
	while (Length>0) {
		if ((bytesRcvd = recv(handle->Csocket, (char*)(buffer + totalBytesRcvd), Length, 0)) <= 0) {	
			free(buffer);
			return -3;
		}
		totalBytesRcvd += bytesRcvd; // Keep tally of total bytes
		Length -= bytesRcvd;
	}
	
	memcpy(data, buffer, count*sizeof(uint32_t));
	*read_data = count;
	free(buffer);
	return 0;
}

R5560_SDKLIB_API int NI_ReadFifo(uint32_t *data, uint32_t count, 
										uint32_t address, uint32_t fifo_status_address, BUS_MODE bus_mode, 
										uint32_t timeout_ms, tR5560_Handle *handle, 
										uint32_t *read_data)
{
	int Length;
	uint32_t valid_word;
	uint8_t *buffer;
	*read_data=0;
	if(handle->connected==0) return -1;

	buffer = (uint8_t *) malloc((10+count) * sizeof(uint32_t));
	if (buffer==NULL) return -3;


	*((uint32_t *)(buffer) + 0) = 0x80000000;
	*((uint32_t *)(buffer) + 1) = 0x00000004;
	*((uint32_t *)(buffer) + 2) = address;
	*((uint32_t *)(buffer) + 3) = count;
	*((uint32_t *)(buffer) + 4) = fifo_status_address;
	*((uint32_t *)(buffer) + 5) = timeout_ms + (bus_mode == STREAMING_BLOCKING ? 0x80000000 : 0);

	Length = 6*4 ;
	if (send(handle->Csocket, (char*)buffer, Length, 0) != Length) {
		free(buffer);
		return -2;
	}


	int bytesRcvd;
	int totalBytesRcvd = 0;
	Length=1 * 4;
	
	while (Length>0) {
		if ((bytesRcvd = recv(handle->Csocket, (char*)(buffer + totalBytesRcvd), Length, 0)) <= 0) {
			free(buffer);
			return -3;
		}
		totalBytesRcvd += bytesRcvd; // Keep tally of total bytes
		Length -= bytesRcvd;
	}
	
	memcpy(&valid_word, buffer, sizeof(uint32_t));

	totalBytesRcvd = 0;
	Length=valid_word * 4;

	if (valid_word > count)
	{
		free(buffer);
		return -4;
	}
	
	while (Length>0) {
		if ((bytesRcvd = recv(handle->Csocket, (char*)(buffer + totalBytesRcvd), Length, 0)) <= 0) {
			free(buffer);
			return -3;
		}
		totalBytesRcvd += bytesRcvd; // Keep tally of total bytes
		Length -= bytesRcvd;
	}

	memcpy(data, buffer, valid_word *sizeof(uint32_t));
	*read_data= valid_word;
	free(buffer);

	return 0;
}
R5560_SDKLIB_API int NI_WriteReg(uint32_t data, uint32_t address, tR5560_Handle *handle)
{
	int Length;
	uint8_t buffer[256];

	if(handle->connected==0) return -1;

	*((uint32_t *)(buffer) + 0) = 0x80000000;
	*((uint32_t *)(buffer) + 1) = 0x00000000;
	*((uint32_t *)(buffer) + 2) = address;
	*((uint32_t *)(buffer) + 3) = 0x00000001;
	*((uint32_t *)(buffer) + 4) = data;

	Length = 5*4 ;
	if (send(handle->Csocket, (char*)buffer, Length, 0) != Length) {
		return -2;
	}
	
	return 0;

}
R5560_SDKLIB_API int NI_ReadReg(uint32_t *data, uint32_t address, tR5560_Handle *handle)
{
	int Length;
	uint8_t buffer[256];

	if(handle->connected==0) return -1;

	*((uint32_t *)(buffer) + 0) = 0x80000000;
	*((uint32_t *)(buffer) + 1) = 0x00000001;
	*((uint32_t *)(buffer) + 2) = address;
	*((uint32_t *)(buffer) + 3) = 0x00000001;

	Length = 4*4 ;
	if (send(handle->Csocket, (char*)buffer, Length, 0) != Length) {
		return -2;
	}


	int bytesRcvd;
	int totalBytesRcvd = 0;
	Length=1 * 4;
	
	while (Length>0) {
		if ((bytesRcvd = recv(handle->Csocket, (char*)(buffer + totalBytesRcvd), Length, 0)) <= 0) {
			return -3;
		}
		totalBytesRcvd += bytesRcvd; // Keep tally of total bytes
		Length -= bytesRcvd;
	}
	
	memcpy(data, buffer, sizeof(uint32_t));
	
	return 0;
}

R5560_SDKLIB_API int NI_USB3_SetIICControllerBaseAddress(uint32_t ControlAddress, uint32_t StatusAddress, tR5560_Handle *handle)
{
	handle->__IICBASEADDRESS=ControlAddress;
	handle->__IICBASEADDRESS_STATUS=StatusAddress;
	return 0;
}





R5560_SDKLIB_API int NI_USB3_IIC_ReadData(uint8_t address, uint8_t *value, int len, uint8_t *value_read, int len_read, tR5560_Handle *handle)
{
	int i;
	/*
	if (NI_USB3_WriteReg(0, RFA_IIC_BA + 0, handle) != 0)
	return -1;
	Sleep(5);
	if (NI_USB3_WriteReg(1<<15, RFA_IIC_BA + 0, handle) != 0)
	return -1;
	Sleep(5);
	if (NI_USB3_WriteReg(0, RFA_IIC_BA + 0, handle) != 0)
	return -1;	*/
	uint32_t data_tmp;

	
	if (NI_WriteReg(1 << 8, handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;


	if (NI_WriteReg((address << 1) + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
	
	
	do 
	{
		if (NI_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
			return -1;
	} while ((data_tmp>>17)==0);


	for (i = 0; i<len; i++)
	{
		if (NI_WriteReg((value[i]) + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
			return -1;
		
		do 
		{
			if (NI_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);


	}

	if (NI_WriteReg(1 << 8, handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;

	if (NI_WriteReg((address << 1) + 1 + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
	
	do 
	{
		if (NI_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
			return -1;
	} while ((data_tmp>>17)==0);


	for (i = 0; i<len_read; i++)
	{
		if (NI_WriteReg((1 << 11) + (1 << 10), handle->__IICBASEADDRESS + 0, handle) != 0)
			return -1;
		do 
		{
			if (NI_ReadReg(&data_tmp, handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;

		} while ((data_tmp>>17)==0);
		value_read[i] = data_tmp & 0xFF;
	}

	if (NI_WriteReg((1 << 9), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;

	do 
	{
		if (NI_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
			return -1;
	} while ((data_tmp>>17)==0);



	return 0;
}


R5560_SDKLIB_API int NI_USB3_IIC_WriteData(uint8_t address, uint8_t *value, int len, tR5560_Handle *handle)
{
	int i;
	/*
	if (NI_USB3_WriteReg(0, RFA_IIC_BA + 0, handle) != 0)
	return -1;
	Sleep(5);
	if (NI_USB3_WriteReg(1<<15, RFA_IIC_BA + 0, handle) != 0)
	return -1;
	Sleep(5);
	if (NI_USB3_WriteReg(0, RFA_IIC_BA + 0, handle) != 0)
	return -1;	*/
	uint32_t data_tmp;
	if (NI_WriteReg(1 << 8, handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
		do 
		{
			if (NI_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);


	if (NI_WriteReg((address << 1) + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
		do 
		{
			if (NI_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);


	for (i = 0; i<len; i++)
	{
		if (NI_WriteReg((value[i]) + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
			return -1;
		do 
		{
			if (NI_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);

	}

	if (NI_WriteReg((1 << 9), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
		do 
		{
			if (NI_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);


	return 0;
}


R5560_SDKLIB_API int NI_DMA_Read(uint32_t dma_channel, char *buffer, uint32_t max_len, uint32_t *valid_data, tR5560_Handle *handle)
{
	if (dma_channel < ZMQ_ENDPOINT_COUNT)
	{
		if (handle->zmq[dma_channel].zmq_connected != 1) return -2;
		int flags=0;
		if (handle->zmq[dma_channel].recv_blocking != 1) flags += ZMQ_DONTWAIT;
		int size = zmq_recv(handle->zmq[dma_channel].zmq_pullsocket, buffer, max_len, flags);
		if (size > 0)
		{
			*valid_data = size;
			return 0;
		}
		else
		{
			int err;
			err = zmq_errno() ;
			*valid_data = 0;
			if (err == EAGAIN)
			{
				return 0;
			}
			else
				return -3;
		}
	}
	else
		return -1;
}

R5560_SDKLIB_API int NI_DMA_SetOptions(uint32_t dma_channel, int blocking, int timeout, int buffer_length, tR5560_Handle *handle)
{
	if (dma_channel < ZMQ_ENDPOINT_COUNT)
	{
		if (handle->zmq[dma_channel].zmq_connected != 1) return -2;

		zmq_setsockopt(handle->zmq[dma_channel].zmq_pullsocket, ZMQ_RCVHWM, &buffer_length, sizeof(int));
		zmq_setsockopt(handle->zmq[dma_channel].zmq_pullsocket, ZMQ_RCVTIMEO, &timeout, sizeof(int));
		handle->zmq[dma_channel].recv_blocking = blocking;
	}
	else
		return -1;

}



R5560_SDKLIB_API int NI_InternalWriteReg(uint32_t data, uint32_t address, tR5560_Handle *handle)
{
	int Length;
	uint8_t buffer[256];

	if(handle->connected==0) return -1;

	*((uint32_t *)(buffer) + 0) = 0x80000000;
	*((uint32_t *)(buffer) + 1) = 0x00000005;
	*((uint32_t *)(buffer) + 2) = address;
	*((uint32_t *)(buffer) + 3) = 0x00000001;
	*((uint32_t *)(buffer) + 4) = data;

	Length = 5*4 ;
	if (send(handle->Csocket, (char*)buffer, Length, 0) != Length) {
		return -2;
	}
	
	return 0;

}
R5560_SDKLIB_API int NI_InternalReadReg(uint32_t *data, uint32_t address, tR5560_Handle *handle)
{
	int Length;
	uint8_t buffer[256];

	if(handle->connected==0) return -1;

	*((uint32_t *)(buffer) + 0) = 0x80000000;
	*((uint32_t *)(buffer) + 1) = 0x00000006;
	*((uint32_t *)(buffer) + 2) = address;
	*((uint32_t *)(buffer) + 3) = 0x00000001;

	Length = 4*4 ;
	if (send(handle->Csocket, (char*)buffer, Length, 0) != Length) {
		return -2;
	}


	int bytesRcvd;
	int totalBytesRcvd = 0;
	Length=1 * 4;
	
	while (Length>0) {
		if ((bytesRcvd = recv(handle->Csocket, (char*)(buffer + totalBytesRcvd), Length, 0)) <= 0) {
			return -3;
		}
		totalBytesRcvd += bytesRcvd; // Keep tally of total bytes
		Length -= bytesRcvd;
	}
	
	memcpy(data, buffer, sizeof(uint32_t));
	
	return 0;
}


int sockInit(void)
{
  #ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(1,1), &wsa_data);
  #else
    return 0;
  #endif
}

int sockQuit(void)
{
  #ifdef _WIN32
    return WSACleanup();
  #else
    return 0;
  #endif
}

int sockClose(SOCKET sock)
{

  int status = 0;

  #ifdef _WIN32
    status = shutdown(sock, SD_BOTH);
    if (status == 0) { status = closesocket(sock); }
  #else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) { status = close(sock); }
  #endif

  return status;

}
