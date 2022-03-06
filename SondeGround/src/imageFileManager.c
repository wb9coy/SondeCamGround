#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imageFileManager.h"
#include "packetDefs.h"
#include "utils.h"
#include "checksum.h"
#include "packetDefs.h"

static FILE *fptr[256];
static FILE *fptrSeq[256];
static int firstTime = 1;

void processImageFilePacketInit()
{
	for(int i = 0; i < 256;i++)
	{
		fptr[i]    = NULL;
		fptrSeq[i] = NULL;
	}

}

int processImageFilePacket(unsigned char * imagefilePacket, int len, int packetType)
{
	int status = 1;
    uint16_t  crc16;
	struct HABPacketImageStartType    HABPacketImageStart;
	struct HABPacketImageDataType     HABPacketImageData;
	struct HABPacketImageEndType      HABPacketImageEnd;
	struct HABPacketImageSeqStartType HABPacketImageSeqStart;
	struct HABPacketImageSeqDataType  HABPacketImageSeqData;
	struct HABPacketImageSeqEndType	  HABPacketImageSeqEnd;

	static int prevSeq = 0;

	if(firstTime)
	{
		processImageFilePacketInit();
		firstTime = 0;
	}

	switch(packetType)
	{
		case START_IMAGE:
			if(len == sizeof(HABPacketImageStart))
			{
				memcpy(&HABPacketImageStart,imagefilePacket,len);
				crc16 = crc_16((unsigned char *)&HABPacketImageStart,len - sizeof(HABPacketImageStart.crc16) - NPAR);
				//printf("Packet CRC %x Calc CRC %x\n",HABPacketImageStart.crc16,crc16);
				if(crc16 != HABPacketImageStart.crc16)
				{
					printf("RS Packet\n");
					rscode_decode((unsigned char *)&HABPacketImageStart, len);
					crc16 = crc_16((unsigned char *)&HABPacketImageStart,len - sizeof(HABPacketImageStart.crc16) - NPAR);
					if(crc16 != HABPacketImageStart.crc16)
					{
						printf("Bad HABPacketImageStart\n");
						status = 0;
					}
				}

				if(status)
				{
					for(int i = 0; i < 256;i++)
					{
						if (fptr[i])
						{
							fclose(fptr[i]);
							fptr[i] = NULL;
						}
						if (fptrSeq[i])
						{
							fclose(fptrSeq[i]);
							fptrSeq[i] = NULL;
						}
					}
					setImageFileID(HABPacketImageStart.imageFileID);
					printf("packetType = %d\n",HABPacketImageStart.packetType);
					printf("imageFileID = %d\n",getImageFileID());
					printf("fileSize = %d\n",HABPacketImageStart.fileSize);

					createImageFilePathName();
					remove(getImageFilePathName());

					printf("START_IMAGE %d\n",getImageFileID());
					fptr[HABPacketImageStart.imageFileID] = fopen(getImageFilePathName(),"wb");

					createImageSeqFilePathName();
					remove(getImageSeqFilePathName());
					HABPacketImageSeqStart.imageFileID = HABPacketImageStart.imageFileID;
					fptrSeq[HABPacketImageSeqStart.imageFileID] = fopen(getImageSeqFilePathName(),"wb");

					HABPacketImageSeqStart.packetType  = START_SEQ_IMAGE;
					HABPacketImageSeqStart.gwID        = getGWID();
					HABPacketImageSeqStart.fileSize    = HABPacketImageStart.fileSize;
					if (fptrSeq[HABPacketImageSeqStart.imageFileID])
					{
						fwrite(&HABPacketImageSeqStart, 1, sizeof(HABPacketImageSeqStart), fptrSeq[HABPacketImageSeqStart.imageFileID]);
					}

					prevSeq = 0;
				}
			}
			else
			{
				printf("ERROR invalid HABPacketImageStart\n");
				status = 0;
			}

			break;

		case IMAGE_DATA:
			if(len == sizeof(HABPacketImageData))
			{
				memcpy(&HABPacketImageData,imagefilePacket,len);
				crc16 = crc_16((unsigned char *)&HABPacketImageData,len - sizeof(HABPacketImageData.crc16));
				//printf("Packet CRC %x Calc CRC %x\n",HABPacketCallSignData.crc16,crc16);
				if(crc16 != HABPacketImageData.crc16)
				{
					printf("ERROR invalid CRC check on image data\n");
					status = 0;
				}
				else
				{
					if (fptr[HABPacketImageData.imageFileID])
					{
						if( (prevSeq + 1) != HABPacketImageData.imageSeqnum)
						{
							printf("#######################  SEQ ERROR %d %d\n",prevSeq, HABPacketImageData.imageSeqnum);
						}
						fwrite(HABPacketImageData.imageData, 1, HABPacketImageData.imageDataLen, fptr[HABPacketImageData.imageFileID]);

						printf("IMAGE_DATA Seq %d\n",HABPacketImageData.imageSeqnum);
						prevSeq = HABPacketImageData.imageSeqnum;

						HABPacketImageSeqData.imageFileID  = HABPacketImageData.imageFileID;
						if (fptrSeq[HABPacketImageSeqData.imageFileID])
						{
							HABPacketImageSeqData.packetType   = IMAGE_SEQ_DATA;
							HABPacketImageSeqData.imageSeqnum  = HABPacketImageData.imageSeqnum;
							HABPacketImageSeqData.imageDataLen = HABPacketImageData.imageDataLen;
							HABPacketImageSeqData.gwID         = getGWID();
							memcpy(HABPacketImageSeqData.imageData,HABPacketImageData.imageData,HABPacketImageData.imageDataLen);
							fwrite(&HABPacketImageSeqData, 1, sizeof(HABPacketImageSeqData), fptrSeq[HABPacketImageSeqData.imageFileID]);
						}
					}
				}
			}
			else
			{
				printf("ERROR invalid HABPacketImageData\n");
				status = 0;
			}
			break;

		case END_IMAGE:
			printf("******************* END_IMAGE *********\n");
			if(len == sizeof(HABPacketImageEnd))
			{
				memcpy(&HABPacketImageEnd,imagefilePacket,len);
				//HABPacketImageEnd.imageDataLen = 100;
				crc16 = crc_16((unsigned char *)&HABPacketImageEnd,len - sizeof(HABPacketImageEnd.crc16) - NPAR);
				printf("Packet CRC %x Calc CRC %x\n",HABPacketImageEnd.crc16,crc16);
				if(crc16 != HABPacketImageEnd.crc16)
				{
					printf("RS Packet\n");
					rscode_decode((unsigned char *)&HABPacketImageEnd, len);
					crc16 = crc_16((unsigned char *)&HABPacketImageEnd,len - sizeof(HABPacketImageEnd.crc16) - NPAR);
					//printf("Packet CRC %x Calc CRC %x\n",HABPacketImageEnd.crc16,crc16);
					if(crc16 != HABPacketImageEnd.crc16)
					{
						printf("Bad HABPacketImageEnd\n");
						status = 0;
					}
				}

				if(status)
				{
					if (fptr[HABPacketImageEnd.imageFileID])
					{
						if(HABPacketImageEnd.imageFileID != getImageFileID())
						{
							printf("ERROR processImageFilePacket HABPacketImageEnd.imageFileID != imageFileID %d",getImageFileID());
						}
						fclose(fptr[HABPacketImageEnd.imageFileID]);
						fptr[HABPacketImageEnd.imageFileID] = NULL;

						HABPacketImageSeqEnd.imageFileID = HABPacketImageEnd.imageFileID;
						if (fptrSeq[HABPacketImageSeqEnd.imageFileID])
						{
							HABPacketImageSeqEnd.packetType  = END_SEQ_IMAGE;
							HABPacketImageSeqEnd.imageFileID = HABPacketImageEnd.imageFileID;
							HABPacketImageSeqEnd.gwID        = getGWID();
							fwrite(&HABPacketImageSeqEnd, 1, sizeof(HABPacketImageSeqEnd), fptrSeq[HABPacketImageSeqEnd.imageFileID]);
							fclose(fptrSeq[HABPacketImageSeqEnd.imageFileID]);
							fptrSeq[HABPacketImageSeqEnd.imageFileID] = NULL;

							char command[MAX_COMMAND_SIZE];
							int commandStatus;

							sprintf (command, "ftp-upload -h %s:%d -u sonde --password sonde --passive -d / %s", getGatewayServerIPAddress(),getGatewayFTPPort(),getImageSeqFilePathName());
							printf("FTP Commad %s\n",command);
							commandStatus = system(command);
							printf("FTP commandStatus %d\n",commandStatus);
						}
					}
					else
					{
						printf("ERROR processImageFilePacket no file open for HABPacketImageEnd.imageFileID %d\n",HABPacketImageEnd.imageFileID);

					}
				}
			}
			else
			{
				printf("ERROR invalid HABPacketImageEnd\n");
				status = 0;
			}

			printf("END_IMAGE%d\n",len);

			break;
		default:
			printf("ERROR processImageFilePacket invalid packetType\n");
			status = 0;
	}

	return status;
}
