#include <unistd.h>
#include <pigpio.h>
#include <stdio.h>
#include <math.h>
#include "SX1278FSK.h"


int resetChip(uint8_t gpio_n)
{
	int status = 1;

    gpioSetMode(gpio_n, PI_OUTPUT);
    gpioWrite(gpio_n, 0);
    usleep(1000000);SX1278FSK_h
    gpioWrite(gpio_n, 1);
    usleep(3000000);

	return status;
}


uint8_t readRegister(int spid, unsigned char reg)
{
    int ret;
    char rx[2], tx[2];
    tx[0]=reg;
    tx[1]=0x00;

    rx[0]=0x00;
    rx[1]=0x00;

    ret = spiXfer(spid, tx, rx, 2);
    if(ret<0)
        return ret;

    if(ret<=1)
        return -1;

    return rx[1];
}

int writeRegister(int spid, unsigned char reg, unsigned char byte)
{
    char rx[2], tx[2];
    tx[0]=(reg | 0x80);
    tx[1]=byte;

    rx[0]=0x00;
    rx[1]=0x00;

    return spiXfer(spid, tx, rx, 2);
}

int readRegisterBytes(int spid, unsigned char reg, uint8_t *buff, unsigned char size)
{
    int ret;
    char tx[257];
    char rx[257];

    memset(tx, '\0', 257);
    memset(rx, '\0', 257);
    memset(buff, '\0', size);
    tx[0]=reg;
    ret = spiXfer(spid, tx, rx, size+1);
    memcpy(buff, &rx[1], ret-1);
    return ret;
}

int writeRegisterBytes(int spid, unsigned char reg, char *buff, unsigned char size){
    char tx[257];
    char rx[257];
    memset(tx, '\0', 257);
    memset(rx, '\0', 257);

    tx[0]=(reg | 0x80);
    memcpy(&tx[1], buff, size);
    return spiXfer(spid, tx, rx, size+1);
}

/*
 * Function: Clears the IRQ flags
 *
 * Configuration registers are accessed through the SPI interface.
 * Registers are readable in all device mode including Sleep. However, they
 * should be written only in Sleep and Stand-by modes.
 *
 * Returns: Nothing
 */
int clearIRQFlags(int spid)
{
	int status = 1;

	// FSK mode flags1 register
	writeRegister(spid,REG_IRQ_FLAGS1, 0xFF);
	// FSK mode flags2 register
	writeRegister(spid, REG_IRQ_FLAGS2, 0xFF);

	return status;
}

/*
Function: Sets the module in FSK mode.
Returns:   Integer that determines if there has been any error
	state = 2  --> The command has not been executed
	state = 1  --> There has been an error while executing the command
	state = 0  --> The command has been executed with no errors
*/
uint8_t setFSK(int spid)
{
	uint8_t state = 2;
	uint8_t st0;
	//byte config1;

	writeRegister(spid,REG_OP_MODE, FSK_SLEEP_MODE);	// Sleep mode (mandatory to change mode)
	// If we are in LORA mode, above line activate Sleep mode, but does not change mode to FSK
	// as mode change is only allowed in sleep mode. Next line changes to FSK
	writeRegister(spid,REG_OP_MODE, FSK_SLEEP_MODE);

	writeRegister(spid,REG_OP_MODE, FSK_STANDBY_MODE);	// FSK standby mode

	usleep(100000);

	st0 = readRegister(spid,REG_OP_MODE);	// Reading config mode
	if( st0 == FSK_STANDBY_MODE )
	{ // FSK mode
		state = 0;
	}
	else
	{ // LoRa mode
		state = 1;
		printf("Mode is %d\n",st0 );
	}
	return state;
}


/* Function: Sets FSK bitrate
 * Returns: 0 for success, >0 in case of error
 * Parameters: bps: requested bitrate
 * (raw data rate, for Mancester encoding, the effective bitrate is bps/2)
*/

int setBitrate(int spid, float bps)
{
	// TODO: Check if FSK mode is active

	// check if bitrate is allowed allowed bitrate
	if((bps < 1200) || (bps > 300000)) {
	  return -1;
	}

	// set mode to FSK STANDBY
	writeRegister(spid, REG_OP_MODE, FSK_STANDBY_MODE);

	// set bit rate
	uint16_t bitRate = (SX127X_CRYSTAL_FREQ * 1.0) / bps;
	writeRegister(spid, REG_BITRATE_MSB, (bitRate & 0xFF00) >> 8);
	writeRegister(spid, REG_BITRATE_LSB, (bitRate & 0x00FF));

	// also set fractional part
	uint16_t fracRate = (SX127X_CRYSTAL_FREQ * 16.0) / bps - bitRate * 16 + 0.5;
	writeRegister(spid, REG_BIT_RATE_FRAC, fracRate&0x0F);
	return 1;
}
/* Function: Gets configured bitrate
 * Returns bitrate in bit/second
 */
float getBitrate(int spid)
{
	uint8_t fmsb = readRegister(spid,REG_BITRATE_MSB);
	uint8_t flsb = readRegister(spid,REG_BITRATE_LSB);
	uint8_t ffrac = readRegister(spid,REG_BIT_RATE_FRAC) & 0x0F;
	return SX127X_CRYSTAL_FREQ / ( (fmsb<<8) + flsb + ffrac / 16.0 );
}

//typedef struct rxbwset { float bw; uint8_t mant; uint8_t rxp; } st_rxbwsettings;

int setRxBandwidth(int spid,float bw)
{
	// TODO: Check if in FSK mode
	//
	if(bw<2600 || bw>250000) { return -1; /* invalid */ }

	uint8_t rxbwexp = 1;
	bw = SX127X_CRYSTAL_FREQ / bw / 8;
	while(bw>31) { rxbwexp++; bw/=2.0; }
	uint8_t rxbwmant = bw<17?0 : bw<21? 1:2;

	// set mode to FSK STANDBY
	writeRegister(spid, REG_OP_MODE, FSK_STANDBY_MODE);

	writeRegister(spid, REG_RX_BW, rxbwexp | (rxbwmant<<3));
	return 1;
}

float getRxBandwidth(int spid)
{
	uint8_t rxbw = readRegister(spid, REG_RX_BW);
	uint8_t rxbwexp = rxbw&0x07;
	uint8_t rxbwmant = (rxbw>>3)&0x03;
	rxbwmant = 16 + 4*rxbwmant;
	return SX127X_CRYSTAL_FREQ / ( rxbwmant << (rxbwexp+2));
}

int setAFCBandwidth(int spid, float bw)
{
	// TODO: Check if in FSK mode
	//
	if(bw<2600 || bw>250000) { return -1; /* invalid */ }

	uint8_t rxbwexp = 1;
	bw = SX127X_CRYSTAL_FREQ / bw / 8;
	while(bw>31) { rxbwexp++; bw/=2.0; }
	uint8_t rxbwmant = bw<17?0 : bw<21? 1:2;

	// set mode to FSK STANDBY
	writeRegister(spid,REG_OP_MODE, FSK_STANDBY_MODE);

	writeRegister(spid,REG_AFC_BW, rxbwexp | (rxbwmant<<3));
	return 1;
}

float getAFCBandwidth(int spid)
{
	uint8_t rxbw = readRegister(spid,REG_AFC_BW);
	uint8_t rxbwexp = rxbw&0x07;
	uint8_t rxbwmant = (rxbw>>3)&0x03;
	rxbwmant = 16 + 4*rxbwmant;
	return SX127X_CRYSTAL_FREQ / ( rxbwmant << (rxbwexp+2));
}


int setFrequency(int spid, float freq)
{
	int status = 1;
	int count;
    int frf, frf_revers=0;

    frf = (int)ceil((freq/(32000000.0))*524288);
    frf_revers += (int)((unsigned char)(frf>>0))<<16;
    frf_revers += (int)((unsigned char)(frf>>8))<<8;
    frf_revers += (int)((unsigned char)(frf>>16)<<0);
    count = writeRegisterBytes(spid, REG_FRF_MSB, (char *)&frf_revers, 3);

	if(count < 1)
	{
		status = 0;
	}

	return status;
}

float getFrequency(int spid)
{
	uint8_t fmsb = readRegister(spid,REG_FRF_MSB);
	uint8_t fmid = readRegister(spid,REG_FRF_MID);
	uint8_t flsb = readRegister(spid,REG_FRF_LSB);
	return ((fmsb<<16)|(fmid<<8)|flsb) * 1.0 / (1<<19) * SX127X_CRYSTAL_FREQ;
}

static int gaintab[]={-999,0,-6,-12,-24,-36,-48,-999};
int getLNAGain(int spid)
{
	int gain = (readRegister(spid, REG_LNA)>>5)&0x07;
	return gaintab[gain];
}

uint8_t setLNAGain(int spid, int gain) {
	uint8_t g=1;
	while(gain<gaintab[g] && g<6) {g ++; }
	writeRegister(spid,REG_LNA, g<<5);
	return 0;
}

uint8_t getRxConf(int spid)
{
	return readRegister(spid, REG_RX_CONFIG);
}

int setRxConf(int spid, uint8_t conf)
{
	writeRegister(spid, REG_RX_CONFIG, conf);
	return 1;
}

int setSyncConf(int spid, uint8_t conf, int len, const uint8_t *syncpattern) {
	int res=1;
	writeRegister(spid, REG_SYNC_CONFIG, conf);
	if(len>8) return -1;
	for(int i=0; i<len; i++)
	{
		printf("sync %d\n",syncpattern[i]);
		writeRegister(spid, REG_SYNC_VALUE1+i, syncpattern[i]);
	}
	return res;
}

uint8_t getSyncConf(int spid)
{
	return readRegister(spid, REG_SYNC_CONFIG);
}

uint8_t setPreambleDetect(int spid, uint8_t conf)
{
	writeRegister(spid, REG_PREAMBLE_DETECT, conf);
	return 1;
}

uint8_t getPreambleDetect(int spid)
{
	return readRegister(spid, REG_PREAMBLE_DETECT);
}

int setPacketConfig(int spid, uint8_t conf1, uint8_t conf2)
{
	int ret=1;
	writeRegister(spid, REG_PACKET_CONFIG1, conf1);
	writeRegister(spid, REG_PACKET_CONFIG2, conf2);
	return ret;
};

uint16_t getPacketConfig(int spid)
{
	uint8_t c1 = readRegister(spid, REG_PACKET_CONFIG1);
	uint8_t c2 = readRegister(spid, REG_PACKET_CONFIG2);
	return (c2<<8)|c1;
}

/*
Function: Gets the preamble length from the module.
Returns: preamble length
*/
uint16_t getPreambleLength(int spid)
{
	uint16_t p_length;

	p_length = readRegister(spid, REG_PREAMBLE_MSB_FSK);
	p_length = (p_length<<8) | readRegister(spid, REG_PREAMBLE_LSB_FSK);
	return p_length;
}

/*
Function: Sets the preamble length in the module
Returns: Integer that determines if there has been any error
state = 2  --> The command has not been executed
state = 1  --> There has been an error while executing the command
state = 0  --> The command has been executed with no errors
Parameters:
l: length value to set as preamble length.
*/
uint8_t setPreambleLength(int spid, uint16_t l)
{
	uint8_t st0;
	int8_t state = 2;

	st0 = readRegister(spid, REG_OP_MODE);	// Save the previous status

	writeRegister(spid, REG_OP_MODE, FSK_STANDBY_MODE);    // Set Standby mode to write in registers
	// Storing MSB preamble length in FSK mode
	writeRegister(spid, REG_PREAMBLE_MSB_FSK, l>>8);
	writeRegister(spid, REG_PREAMBLE_LSB_FSK, l&0xFF);

	state = 0;

	if(st0 != FSK_STANDBY_MODE) {
		writeRegister(spid, REG_OP_MODE, st0);	// Getting back to previous status
	}
	return state;
}

/*
Function: Gets the payload length from the module.
Returns: configured length; -1 on error
*/
int getPayloadLength(int spid)
{
	int length;
	length = readRegister(spid, REG_PAYLOAD_LENGTH_FSK);

	return length;
}

/*
Function: Sets the payload length from the module.
Returns: 0 for ok, otherwise error
// TODO: Larger than 255 bytes?
*/
uint8_t setPayloadLength(int spid, int len)
{

//	uint8_t conf2 = readRegister(spid, REG_PACKET_CONFIG2);
//	conf2 = (conf2 & 0xF8) | ( (len>>8)&0x7 );
//	writeRegister(spid, REG_PACKET_CONFIG2, conf2);
	writeRegister(spid, REG_PAYLOAD_LENGTH_FSK, len&0xFF);
	return 0;
}

/*
Function: Gets the current value of RSSI.
Returns: RSSI value
*/
int16_t getRSSI(int spid)
{
	int16_t RSSI;
	//int rssi_mean = 0;
	int total = 1;

	/// FSK mode
	// get mean value of RSSI
	for(int i = 0; i < total; i++)
	{
		RSSI = readRegister(spid, REG_RSSI_VALUE_FSK);
		//rssi_mean += _RSSI;
	}
	//rssi_mean = rssi_mean / total;
	//RSSI = rssi_mean;

	return RSSI;
}

/*
Function: Gets the current value of FEI (frequency error indication)
Returns: FEI value in Hz
*/
int32_t getFEI(int spid)
{
	int32_t FEI;
	int16_t regval = (readRegister(spid,REG_FEI_MSB)<<8) | readRegister(spid, REG_FEI_LSB);
	//Serial.printf("feireg: %04x\n", regval);
	FEI = (int32_t)(regval * SX127X_FSTEP);
	return FEI;
}

/*
Function: Gets the current value of AFC (automated frequency correction)
Returns: AFC value in Hz
*/
int32_t getAFC(int spid)
{
	int32_t AFC;
	int16_t regval = (readRegister(spid, REG_AFC_MSB)<<8) | readRegister(spid,REG_AFC_LSB);
	//Serial.printf("afcreg: %04x\n", regval);
	AFC = (int32_t)(regval * SX127X_FSTEP);
	return AFC;
}

uint16_t getRawAFC(int spid)
{
	return (readRegister(spid, REG_AFC_MSB)<<8) | readRegister(spid, REG_AFC_LSB);
}

void setRawAFC(int spid, uint16_t afc)
{
	writeRegister(spid, REG_AFC_MSB, afc>>8);
	writeRegister(spid, REG_AFC_LSB, afc&0xFF);
}

/*
Function: Gets the current supply limit of the power amplifier, protecting battery chemistries.
Returns: Integer that determines if there has been any error
state = 2  --> The command has not been executed
state = 1  --> There has been an error while executing the command
state = 0  --> The command has been executed with no errors
Parameters:
rate: value to compute the maximum current supply. Maximum current is 45+5*'rate' [mA]
*/
int getMaxCurrent(int spid)
{
	int value;

	value = readRegister(spid, REG_OCP);

	// extract only the OcpTrim value from the OCP register
	value &= 0x1f;
	//value &= B00011111;

	if( value <= 15 ) {
		value = (45 + (5 * value));
	} else if( value <= 27 ) {
		value = (-30 + (10 * value));
	} else {
		value = 240;
	}

	return value;
}

/*
Function: Limits the current supply of the power amplifier, protecting battery chemistries.
Returns: Integer that determines if there has been any error
state = 2  --> The command has not been executed
state = 1  --> There has been an error while executing the command
state = 0  --> The command has been executed with no errors
state = -1 --> Forbidden parameter value for this function
Parameters:
rate: value to compute the maximum current supply. Range: 0x00 to 0x1B. The
Maximum current is:
Imax = 45+5*OcpTrim [mA] 	if OcpTrim <= 15 (120 mA) /
Imax = -30+10*OcpTrim [mA] 	if 15 < OcpTrim <= 27 (130 to 240 mA)
Imax = 240mA 				for higher settings
*/
int setMaxCurrent(int spid, uint8_t rate)
{
	int status =1;
	uint8_t st0;


	// Maximum rate value = 0x1B, because maximum current supply = 240 mA
	if (rate > 0x1B)
	{
		status = 0;
	}
	else
	{
		// Enable Over Current Protection
		//rate |= B00100000;
		rate |= 0x20;

		writeRegister(spid, REG_OP_MODE, FSK_STANDBY_MODE);	// Set FSK Standby mode to write in registers
		writeRegister(spid, REG_OCP, rate);		// Modifying maximum current supply
		st0 = readRegister(spid, REG_OP_MODE);
		if(st0 != FSK_STANDBY_MODE)
		{
			status = 0;
		}
	}
	return status;
}

int set_dio_rx_mapping(int spid)
{
	int status = 1;

    //writeRegister(spid, REG_DIO_MAPPING1,  MAP1_FSK_DIO0_RXDONE | MAP1_FSK_DIO1_LEVEL | MAP1_FSK_DIO2_RXTOUT);

    writeRegister(spid, REG_DIO_MAPPING1,  0 << Dio0MappingShift );
    //writeRegister(spid, REG_DIO_MAPPING1, 0x40);
    //writeRegister(spid, REG_DIO_MAPPING1, 0x00);

    return status;
}



void setRxDoneDioISR(int gpio_n, rxDoneISR func, GFSK_ctl *modem)
{
    gpioSetMode(gpio_n, PI_INPUT);
    gpioSetISRFuncEx(gpio_n, RISING_EDGE, 0, func, (void *)modem);

}


