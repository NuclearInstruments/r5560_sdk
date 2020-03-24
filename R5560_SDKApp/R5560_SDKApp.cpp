// R5560_SDKApp.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include <stdint.h>
#include <time.h>


#include "../R5560_SDKLib/R5560_SDKLib.h"

#define IPADDR "192.168.50.150"

int _tmain(int argc, _TCHAR* argv[])
{
	tR5560_Handle handle;
	uint32_t Model;
	uint32_t BuildData;
	uint32_t data;
	uint32_t *vector_test;
	uint32_t rc;
	uint32_t old;
	char *DMA_BUFFER;
	int i=0;
	int errCount;

	vector_test = (uint32_t *) malloc(1024*sizeof(uint32_t));

	if (R5560_ConnectTCP(IPADDR, 8888, &handle)!=0)
	{
		printf("Unable to connect to: %s", IPADDR);
		return 0;
	}

	printf ("ZMQ DMA Status:\n");
	for (i=0;i<ZMQ_ENDPOINT_COUNT;i++)
		printf("DMA [%d] -> %s\n", i, handle.zmq[i].zmq_connected ? "connected": "not connected");

	printf ("\n");
 	if (NI_ReadReg(&Model, 0x03FFFFFF, &handle)!=0)
	{
		printf("Register READ failed\n");
		return 0;
	}
	if (NI_ReadReg(&BuildData, 0x03FFFFFE, &handle)!=0)
	{
		printf("Register READ failed\n");
		return 0;
	}

	printf("Device Model:       %8x\n", Model);
	printf("Firmware Build:     %8x\n", BuildData);


	printf("Write register test:             ");
	if (NI_WriteReg(0x0123ABBA, 0x03FE0000, &handle)!=0)
	{
		printf("[FAILED]\n");
		return 0;
	}
	printf("[OK]\n");

	printf("READ register test:              ");
	if (NI_ReadReg(&data, 0x03FE0000, &handle)!=0)
	{
		printf("[FAILED]\n");
		return 0;
	}
	if (data == 0x0123ABBA)
		printf("[OK]\n");
	else
		printf("[FAILED]\n");


	for (i=0;i<64;i++)
		vector_test[i]=i+1;
	
	printf("Write memory test:               ");
	if (NI_WriteData(vector_test, 64, 0x03FE0000, &handle, &rc)!=0)
	{
		printf("[FAILED]\n");
		return 0;
	}
	printf("[OK]\n");

	for (i=0;i<64;i++)
		vector_test[i]=0;

	printf("Read memory test:                ");
	if (NI_ReadData(vector_test, 64, 0x03FE0000, &handle, &rc)!=0)
	{
		printf("[FAILED]\n");
		return 0;
	}
	
	errCount=0;
	for (i=0;i<64;i++)
		if (vector_test[i]!=i+1) errCount++;

	if (errCount == 0)
		printf("[OK]\n");
	else
		printf("[FAILED]\n");


	printf("Read FIFO (BLOCKING) test:         ");
	if (NI_ReadFifo(vector_test, 1024, 0x02, 0x03, STREAMING_BLOCKING,1000, &handle, &rc)!=0)
	{
		printf("[FAILED]\n");
		return 0;
	}
	
	old=vector_test[0];
	errCount=0;
	for (i=1;i<rc;i++)
		if (vector_test[i-1]!=vector_test[i-1]) errCount++;

	if (errCount == 0)
		printf("[OK]\n");
	else
		printf("[FAILED]\n");

	printf("Total received data: %d\n", rc);

	printf ("Starting DMA test... \n");

	NI_DMA_SetOptions(0, 1, -1, 10, &handle);

	NI_WriteReg(1, 0x3, &handle);

	DMA_BUFFER = (char*) malloc(32*1024*1024*sizeof(char));
	const clock_t begin_time = clock();
	float totalbyte=0;
	for (i=0;i<10000;i++)
	{
		uint32_t size;
		float secs;
		float speed;
		int rc = NI_DMA_Read(0, DMA_BUFFER, 1024*1024*32, &size, &handle);
		secs = float( clock () - begin_time ) /  CLOCKS_PER_SEC;
		totalbyte+=size;
		speed = (totalbyte/secs)/1024.0;
		printf("Test %4d. Transfer size: %8x ... speed: %6.3fkB/s\r",i,size, speed);
		fflush(stdout);
	}
	printf ("\n\n");
	system("pause");
}


