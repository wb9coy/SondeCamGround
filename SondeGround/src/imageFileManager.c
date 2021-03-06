#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imageFileManager.h"
#include "packetDefs.h"
#include "utils.h"
#include "checksum.h"
#include "packetDefs.h"
#include "gps.h"

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
	struct GWPacketImageSeqEndType	  GWPacketImageSeqEnd;

	static int prevSeq = 0;
	static FILE *fptr[256];
	static FILE *fptrSeq[256];
	static int firstTime = 1;

	if(firstTime)
	{
		for(int i = 0; i < 256;i++)
		{
			fptr[i]    = NULL;
			fptrSeq[i] = NULL;
		}
		firstTime = 0;
	}

	switch(packetType)
	{
		case START_IMAGE:
			printf("START_IMAGE File Cleanup Begin\n");
			prevSeq = -1;

			printf("START_IMAGE File Cleanup End\n");

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
						fptr[i]    = NULL;
						fptrSeq[i] = NULL;
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
				if(HABPacketImageData.imageDataLen <=  MAX_IMG_BUF_LEN)
				{
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
							if( HABPacketImageData.imageSeqnum > prevSeq)
							{
								fwrite(HABPacketImageData.imageData, 1, HABPacketImageData.imageDataLen, fptr[HABPacketImageData.imageFileID]);
							}
							else
							{
								printf("ERROR file for file_id %d not open\n",HABPacketImageData.imageFileID);
							}
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
								if( ftell(fptrSeq[HABPacketImageSeqData.imageFileID]) >=0)
								{
									fwrite(&HABPacketImageSeqData, 1, sizeof(HABPacketImageSeqData), fptrSeq[HABPacketImageSeqData.imageFileID]);
								}
								else
								{
									printf("ERROR seq file for file_id %d not open\n",HABPacketImageData.imageFileID);
								}

							}
						}
					}
				}
				else
				{
					printf("ERROR invalid imageDataLen\n");
					status = 0;
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
					if(HABPacketImageEnd.imageFileID != getImageFileID())
					{
						printf("ERROR processImageFilePacket HABPacketImageEnd.imageFileID != imageFileID %d",getImageFileID());
					}
					else
					{
						if (fptr[HABPacketImageEnd.imageFileID])
					    {
							fclose(fptr[HABPacketImageEnd.imageFileID]);
						    for(int i = 0; i < 256;i++)
						    {
						    	fptr[i]    = NULL;
						    }
					    }
					    else
					    {
					    	printf("ERROR fptr no file open for HABPacketImageEnd.imageFileID %d\n",HABPacketImageEnd.imageFileID);
					    }

					    HABPacketImageSeqEnd.imageFileID = HABPacketImageEnd.imageFileID;
					    if (fptrSeq[HABPacketImageSeqEnd.imageFileID])
					    {
							memset(&GWPacketImageSeqEnd,'\0',sizeof(GWPacketImageSeqEnd));
					    	GWPacketImageSeqEnd.packetType  = END_SEQ_IMAGE;
					    	GWPacketImageSeqEnd.imageFileID = HABPacketImageEnd.imageFileID;
					    	GWPacketImageSeqEnd.gwID        = getGWID();
					    	getLastGGA(GWPacketImageSeqEnd.GGASentence);
					    	getLastRMC(GWPacketImageSeqEnd.RMCSentence);
						    fwrite(&GWPacketImageSeqEnd, 1, sizeof(GWPacketImageSeqEnd), fptrSeq[GWPacketImageSeqEnd.imageFileID]);
						    fclose(fptrSeq[HABPacketImageSeqEnd.imageFileID]);
						    for(int i = 0; i < 256;i++)
						    {
							    fptrSeq[i] = NULL;
						    }

						    char command[MAX_COMMAND_SIZE];
						    int commandStatus;

						    sprintf (command, "ftp-upload -h %s:%d -u sonde --password sonde --passive -d / %s", getGatewayServerIPAddress(),getGatewayFTPPort(),getImageSeqFilePathName());
						    printf("FTP Commad %s\n",command);
						    commandStatus = system(command);
						    printf("FTP commandStatus %d\n",commandStatus);
					    }
					    else
					    {
						   printf("ERROR fptrSeq no file open for HABPacketImageEnd.imageFileID %d\n",HABPacketImageEnd.imageFileID);
					    }
					}
				}
			}
			else
			{
				printf("ERROR invalid HABPacketImageEnd\n");
				status = 0;
			}

			printf("END_IMAGE %d\n",HABPacketImageEnd.imageFileID);

			break;
		default:
			printf("ERROR processImageFilePacket invalid packetType\n");
			status = 0;
	}

	return status;
}
