
#ifndef SX1278FSK_h
#define SX1278FSK_h

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


/******************************************************************************
 * Definitions & Declarations
 *****************************************************************************/

#define SX127X_CRYSTAL_FREQ 32000000
#define SX127X_FSTEP (SX127X_CRYSTAL_FREQ*1.0/(1<<19))

#define SX1278FSK_debug_mode 0

//! MACROS //
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)  // read a bit
#define bitSet(value, bit) ((value) |= (1UL << (bit)))    // set bit to '1'
#define bitClear(value, bit) ((value) &= ~(1UL << (bit))) // set bit to '0'


//! REGISTERS //
//							FSK	Commun	LORA
#define        REG_FIFO        					0x00
#define        REG_OP_MODE        				0x01
#define        REG_BITRATE_MSB    			0x02
#define        REG_BITRATE_LSB    			0x03
#define        REG_FDEV_MSB   				0x04
#define        REG_FDEV_LSB    				0x05
#define        REG_FRF_MSB    					0x06
#define        REG_FRF_MID    					0x07
#define        REG_FRF_LSB    					0x08
#define        REG_PA_CONFIG    				0x09
#define        REG_PA_RAMP    					0x0A
#define        REG_OCP    					0x0B
#define        REG_LNA    					0x0C
#define        REG_RX_CONFIG    			0x0D
#define        REG_FIFO_ADDR_PTR  					0x0D
#define        REG_RSSI_CONFIG   			0x0E
#define        REG_FIFO_TX_BASE_ADDR 		    			0x0E
#define        REG_RSSI_COLLISION    			0x0F
#define        REG_FIFO_RX_BASE_ADDR   					0x0F
#define        REG_RSSI_THRESH    			0x10
#define        REG_FIFO_RX_CURRENT_ADDR   				0x10
#define        REG_RSSI_VALUE_FSK	    		0x11
#define        REG_IRQ_FLAGS_MASK    					0x11
#define        REG_RX_BW		    		0x12
#define        REG_IRQ_FLAGS	    					0x12
#define        REG_AFC_BW		    		0x13
#define        REG_RX_NB_BYTES	    					0x13
#define        REG_OOK_PEAK	    			0x14
#define        REG_RX_HEADER_CNT_VALUE_MSB  				0x14
#define        REG_OOK_FIX	    			0x15
#define        REG_RX_HEADER_CNT_VALUE_LSB  				0x15
#define        REG_OOK_AVG	 			0x16
#define        REG_RX_PACKET_CNT_VALUE_MSB  				0x16
#define        REG_RX_PACKET_CNT_VALUE_LSB  				0x17
#define        REG_MODEM_STAT	  					0x18
#define        REG_PKT_SNR_VALUE	  				0x19
#define        REG_AFC_FEI	  			0x1A
#define        REG_PKT_RSSI_VALUE	  				0x1A
#define        REG_AFC_MSB	  			0x1B
#define        REG_RSSI_VALUE_LORA	  				0x1B
#define        REG_AFC_LSB	  			0x1C
#define        REG_HOP_CHANNEL	  					0x1C
#define        REG_FEI_MSB	  			0x1D
#define        REG_MODEM_CONFIG1	 		 		0x1D
#define        REG_FEI_LSB	  			0x1E
#define        REG_MODEM_CONFIG2	  				0x1E
#define        REG_PREAMBLE_DETECT  			0x1F
#define        REG_SYMB_TIMEOUT_LSB  					0x1F
#define        REG_RX_TIMEOUT1	  			0x20
#define        REG_PREAMBLE_MSB_LORA  					0x20
#define        REG_RX_TIMEOUT2	  			0x21
#define        REG_PREAMBLE_LSB_LORA  					0x21
#define        REG_RX_TIMEOUT3	 			0x22
#define        REG_PAYLOAD_LENGTH_LORA			 		0x22
#define        REG_RX_DELAY	 			0x23
#define        REG_MAX_PAYLOAD_LENGTH 					0x23
#define        REG_OSC		 			0x24
#define        REG_HOP_PERIOD	  					0x24
#define        REG_PREAMBLE_MSB_FSK 			0x25
#define        REG_FIFO_RX_BYTE_ADDR 					0x25
#define        REG_PREAMBLE_LSB_FSK 			0x26
#define        REG_MODEM_CONFIG3	 		 		0x26
#define        REG_SYNC_CONFIG	  			0x27
#define        REG_SYNC_VALUE1	 			0x28
#define	       REG_LORA_FEI_MSB						0x28
#define        REG_SYNC_VALUE2	  			0x29
#define	       REG_LORA_FEI_MID						0x29
#define        REG_SYNC_VALUE3	  			0x2A
#define	       REG_LORA_FEI_LSB						0x2A
#define        REG_SYNC_VALUE4	  			0x2B
#define        REG_SYNC_VALUE5	  			0x2C
#define	       REG_RSSI_WIDEBAND					0x2C
#define        REG_SYNC_VALUE6	  			0x2D
#define        REG_SYNC_VALUE7	  			0x2E
#define        REG_SYNC_VALUE8	  			0x2F
#define        REG_PACKET_CONFIG1	  		0x30
#define        REG_PACKET_CONFIG2	  		0x31
#define        REG_DETECT_OPTIMIZE	  				0x31
#define        REG_PAYLOAD_LENGTH_FSK			0x32
#define        REG_NODE_ADRS	  			0x33
#define        REG_INVERT_IQ						0x33
#define        REG_BROADCAST_ADRS	 		0x34
#define        REG_FIFO_THRESH	  			0x35
#define        REG_SEQ_CONFIG1	  			0x36
#define        REG_SEQ_CONFIG2	  			0x37
#define        REG_TIMER_RESOL	  			0x38
#define        REG_TIMER1_COEF	  			0x39
#define        REG_SYNC_WORD						0x39
#define        REG_TIMER2_COEF	  			0x3A
#define        REG_IMAGE_CAL	  			0x3B
#define        REG_TEMP		  			0x3C
#define        REG_LOW_BAT	  			0x3D
#define        REG_IRQ_FLAGS1	  			0x3E
#define        REG_IRQ_FLAGS2	  			0x3F
#define        REG_DIO_MAPPING1	  				0x40
#define        REG_DIO_MAPPING2	  				0x41
#define        REG_VERSION	  				0x42
#define        REG_PLL_HOP	  			0x44
#define        REG_TCXO		  				0x4B
#define        REG_PA_DAC	  				0x4D
#define        REG_FORMER_TEMP	  				0x5B
#define        REG_BIT_RATE_FRAC			0x5D
#define        REG_AGC_REF	  				0x61
#define        REG_AGC_THRESH1		  			0x62
#define        REG_AGC_THRESH2		  			0x63
#define        REG_AGC_THRESH3	  				0x64
#define        REG_PLL			  			0x70
#define        IRQ_RXDONE                   0x40

//                                  MAP1:D0D1D2D3
#define MAP1_FSK_DIO0_RXDONE    0x00  // 00------
#define MAP1_FSK_DIO0_TXDONE    0x00  // 00------
#define MAP1_FSK_DIO1_LEVEL     0x00  // --00----
#define MAP1_FSK_DIO1_EMPTY     0x10  // --01----
#define MAP1_FSK_DIO1_FULL      0x20  // --10----
#define MAP1_FSK_DIO1_NOP       0x30  // --11----
#define MAP1_FSK_DIO2_TXNOP     0x04  // ----01--
#define MAP1_FSK_DIO2_RXTOUT    0x08  // ----10--

#define	Dio0MappingShift 6
#define Dio1MappingShift 4
#define	Dio2MappingShift 2
#define	Dio3MappingShift 0

#define FSK_SLEEP_MODE   0x00
#define FSK_STANDBY_MODE 0x01
#define FSK_TX_MODE      0x03
#define FSK_RX_MODE      0x05

typedef void (*rxDoneISR)(int gpio_n, int level, uint32_t tick, void *userdata);

typedef struct
{
	int    spid;
    float freq;
    float freqofs;
    uint8_t resetGpioN;//raspberry GPIO pin connected to RESET pin of the chip
    uint8_t dio0GpioN;//raspberry GPIO pin connected to DIO0 pin of the chip to detect TX and Rx done events.
    float bps;
    int agcbw;
    int rxbw;
    unsigned int preambleLen;
} FSKModemConfigType;

typedef void (*UserRxDoneCallback);

typedef struct{
    UserRxDoneCallback callback;
} GFSK_Rx;

typedef struct
{
	uint8_t spiCS;//SPI CS number
    int spid;//SPI file descriptor
    FSKModemConfigType eth;//ethereum config structure
    GFSK_Rx rx;//rx structure
} GFSK_ctl;


int resetChip(uint8_t gpio_n);

uint8_t readRegister(int spid, unsigned char reg);


int writeRegister(int spid, unsigned char reg, unsigned char byte);


int readRegisterBytes(int spid, unsigned char reg, uint8_t *buff, unsigned char size);


int writeRegisterBytes(int spid, unsigned char reg, char *buff, unsigned char size);


// Clear IRQ flags
int clearIRQFlags(int spid);

// Activate FSK mode (return 0 on success, 1 otherwise)
uint8_t setFSK(int spid);

// Configures bitrate register (closest approximation to requested bitrate)
int setBitrate(int spid, float bps);
float getBitrate(int spid);

// Configures RX bandwidth (next largest supported bandwith if exact value not possible)
int setRxBandwidth(int spid, float bps);
float getRxBandwidth(int spid);

// Configures AFC bandwidth (next largest supported bandwith if exact value not possible)
int setAFCBandwidth(int spid, float bps);
float getAFCBandwidth(int spid);

// Configures RX frequency (closest approximation to requested frequency)
int setFrequency(int spid, float freq);
float getFrequency(int spid);

int getLNAGain(int spid);
uint8_t setLNAGain(int spid, int gain);

uint8_t getRxConf(int spid);
int setRxConf(int spid, uint8_t conf);

int setSyncConf(int spid, uint8_t conf, int len, const uint8_t *syncpattern);
uint8_t getSyncConf(int spid);

uint8_t setPreambleDetect(int spid, uint8_t conf);
uint8_t getPreambleDetect(int spid);

int setPacketConfig(int spid, uint8_t conf1, uint8_t conf2);
uint16_t getPacketConfig(int spid);

// Get configured preamble length (used for TX only?)
uint16_t getPreambleLength(int spid);

// Sets the preamble length.
uint8_t setPreambleLength(int spid, uint16_t l);

// Gets the payload length (expected length for receive)
int getPayloadLength(int spid);
uint8_t setPayloadLength(int spid, int len);

// Get current RSSI value
int16_t getRSSI(int spid);

// Get current FEI (frequency error indication) value
int32_t getFEI(int spid);

// Get current AFC value
int32_t getAFC(int spid);
uint16_t getRawAFC(int spid);
void setRawAFC(int spid, uint16_t afc);

// Get the maximum current supply by the module.
int getMaxCurrent(int spid);

// Set the maximum current supply by the module.
int setMaxCurrent(int spid, uint8_t rate);

int set_dio_rx_mapping(int spid);

// Put the module in reception mode.
//return '0' on success, '1' otherwise
uint8_t receive(GFSK_ctl *modem);

void setRxDoneDioISR(int gpio_n, rxDoneISR func, GFSK_ctl *modem);


#endif
