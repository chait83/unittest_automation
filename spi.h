#ifndef V_3_QT_SPI_H
#define V_3_QT_SPI_H

#define _GNU_SOURCE 1
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
//#include <pthread.h>

#include <poll.h>

#define CRC16_INIT_VALUE	0xffff
#define CRC16_XOR_VALUE		0x0000
#define GPIO1_8 		8 
#define GPIO4_11		107
#define GPIO4_12		108
#define GPIO3_19		83
#define ON                      1
#define OFF                     0
#define MAX_CHANNEL_NO		10


#define SET_MODE                0x03

#define ARISTOS 1


#define MAX_BYTE_VALUE 256

#define TRUE				1
#define FALSE				0

/* IO Card Number's*/
#define IO_CARD_ZERO		0
#define IO_CARD_ONE			1
#define IO_CARD_TWO			2
#define IO_CARD_THREE		3
#define IO_CARD_FOUR		4
#define IO_CARD_FIVE		5
#define IO_CARD_SIX			6
#define IO_CARD_SEVEN		7
#define IO_CARD_SEVEN		7
#define IO_CARD_SEVEN		7
#define IO_CARD_EIGHT		8
#define IO_CARD_NINE		9

/* Channel Number's*/

#define CH_NUM_ZERO				0
#define CH_NUM_ONE				1
#define CH_NUM_TWO				2
#define CH_NUM_THREE			3
#define CH_NUM_FOUR				4
#define CH_NUM_FIVE				5
#define CH_NUM_SIX				6
#define CH_NUM_SEVEN			7
#define Ch_ALL					8

/* Get life History Mode */
#define LIFE_HIS_MODE_1			1
#define LIFE_HIS_MODE_2			2
#define LIFE_HIS_MODE_3			3


/* Error code list */

#define ERROR_NO_DATA			0x26
#define ERROR_PARTIAL_DATA		0x25
#define ERROR_TIMED_OUT			0x48
#define ERROR_IN_LIST			0x49
#define GET_ERR_CODE			0X0A

/* Funtion Code common to all Board */

#define RESET					0X01
#define GET_BOARD_TYPE			0X02
#define	GET_BOARD_ID			0X03
#define GET_TIMESTAMP			0X04
#define GET_LIFE_HISTORY		0X05
#define UPDATE_STATICS			0X06
#define GET_VERSION				0X07
#define REPORT_ERRORS			0X0A
#define RESET_BUFFER_TS			0X12
#define RD_BD_CAPABILITY		0x15

/* Board Type*/

#define BOARD_AI				0x00
#define BOARD_AR				0x01
#define BOARD_DIO				0x02
#define BOARD_AO				0x03
#define BOARD_PI				0x04


/*  Slot Type  */

#define SLOT_A					0
#define SLOT_B					1
#define SLOT_C					2
#define SLOT_D					3
#define SLOT_E					4
#define SLOT_F					5
#define SLOT_G					6
#define SLOT_H					7
#define SLOT_I					8



#define MAX_SLOT_NUM			9
#define NO_DATA_AVAIL			0

#define GPIO_TOGGLE(PIN, N) (PIN ^= (0<<N))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

typedef struct _slot_detect
{
	int slot_A;
	int slot_B;
	int slot_C;
	int slot_D;
	int slot_E;
	int slot_F;
	int slot_G;
	int slot_H;
	int slot_I;
	
}Slot_Detect;

typedef struct _slot_name
{
	char slot_A;
	char slot_B;
	char slot_C;
	char slot_D;
	char slot_E;
	char slot_F;
	char slot_G;
	char slot_H;
	char slot_I;
	/* data */
}Slot_Name;


typedef struct _board_type
{
	uint8_t Board_AI;
	uint8_t Board_AR;
	uint8_t Board_DIO;
	uint8_t Board_AO;
	uint8_t Board_PI;
}Board_Type;

typedef struct _rx_fun_header
{
    uint8_t fun_code;
    uint8_t error_code;
    uint8_t byte_high;
    uint8_t byte_low;
    uint16_t numofdata;
    /* data */
}rx_function_header;

typedef struct _AI_CardBoard_data
{	
	uint8_t ID_code;
	uint8_t numOfChannels;
	uint8_t version[6];

}AI_CardBoard_Data;

typedef struct _AR_CardBoard_data
{	
	uint8_t ID_code;
	uint8_t numOfChannels;
	uint8_t version[6];

}AR_CardBoard_Data;

typedef struct _AO_CardBoard_data
{	
	uint8_t ID_code;
	uint8_t numOfChannels;
	uint8_t version[6];

}AO_CardBoard_Data;

typedef struct _PI_CardBoard_data
{	
	uint8_t ID_code;
	uint8_t numOfChannels;
	uint8_t version[6];

}PI_CardBoard_Data;

typedef struct _DIO_CardBoard_data
{	
	uint8_t ID_code;
	uint8_t numOfChannels;
	uint8_t version[6];

}DIO_CardBoard_Data;

typedef struct _board_capability
{
	uint8_t b_type;
	uint8_t b_revision;
	uint8_t ch_available;
	uint16_t ch_enable;

}Board_Capability;

typedef struct _board_data
{
	uint8_t board_reg_id;
	uint8_t get_version[6];
	Board_Capability *bd_capability;
}BOARD_DATA;

typedef struct _slot_id_card
{
	int slot_number;
	uint8_t board_type;
	uint8_t phy_num_channel;
	BOARD_DATA *board_data;

}CARD_SLOT;

typedef struct  _AI_Board_info
{
    /* data */
    uint8_t version[2];
    uint8_t setupInfo[8];
    uint8_t rangeInfo[8];
}AI_Card_Info;

typedef struct _AI_Channel_info
{
    uint8_t enable;
    uint8_t raw_reading;
    uint8_t ack_frq;
    uint8_t field_cal;
    uint8_t active_burnout;
}AI_Channel_info;

typedef enum
{
	DEV_XS_MINITREND = 0,	///< Minitrend, 5.5" TFT with 16 Channels
	DEV_XS_MULTIPLUS,	///< MultiTrend Plus, 12.1" TFT with 48 Channels
	DEV_XS_EZTREND,		///< XSeries QXe Device Type updates duplicate
	DEV_ARISTOS_MINITREND,	///< Minitrend, 5.5" TFT with 16 Channels
	DEV_ARISTOS_MULTIPLUS,	///< MultiTrend Plus, 12.1" TFT with 48 Channels
	DEV_ARISTOS_EZTREND,	///< ARISTOS QXe Device Type updates
	DEV_PC_MINI,		///< PC BuildS of system for runtime checking
	DEV_PC_MULTI,
	DEV_PC_EZTREND,
	DEV_PC_SCREEN_DESIGNER,	///< Screen designer build - requires us to test the
	DEV_PC_TTR6SETUP,
	///< layout configuration to determine the screen type
	DEV_TEST,		///< Test equipment
	DEV_SCR_MINITREND,	//DRG2 12.1".The value of this enum is 12.Once reaches count 99, Please add new enum at the end.
	DEV_UNKNOWN=99		///< Unknown device
}
T_DEV_TYPE;


              /*  Function Specific to Execution */

int hex_to_dec(uint8_t *hex_value);
int execute_Test(int fd);
int custom_execute_Test(int fd);
int cleanup(int fd);
int get_Rx_Header(uint8_t fun_code, uint8_t *x_rx_buf, int len);
int get_Data_from_Rx(uint8_t *x_rx_buf, int len);
int fill_FunCode_Data(uint8_t *TxData, uint8_t fun_code, uint8_t len);
int error_code_action(int fd, uint8_t fun_code, uint8_t flag_err_code, uint32_t IO_num, uint32_t channel_num, uint8_t channel_count);
int test_select_option(int fd);
int execute_Test_buffer(int fd, uint8_t *buff,  int len);
                  
              /*  Function for IO Card Specific Operation  */

void IOCard_Selection(int IOCardindex);
// int scanning_IO_Port(int fd); --- used in v2.2 and before version
int DetectSlot(int fd);
int IO_CardTest_CTAB(int fd, uint32_t IO_num, uint8_t fun_code);
int IO_Card_Test(int fd);
int reset_CircularBuff(int fd, uint32_t IO_num, int time_stamp);
int SlotDetect(int i);


             /*  Function Related to GPIO's Operation      */

int get_gpio(int gpio, int * value);
int set_gpio_value(int gpio, int value);
int gpio_export(int gpio, int direction);
int set_gpio_direction(int gpio, int direction);


				/*  Function Related to SPI Communication 	  */

void spi_Write_Read(int fd, uint8_t *buff, int size);
int spi_Read(int fd, uint8_t const *rx,  size_t len);
void spi_Write(int fd, uint8_t const *tx,  size_t len);
int SPI_init(int fd);
int spi_gpio_init();
int irq_init();

				 /*  Function for All common board     */

int reset_Board(int fd, int IO_num);
int get_Life_History(int fd, uint8_t IO_num, uint8_t mode);


				/*   Function Related to CRC Operation      */

unsigned short CRC16_BlockChecksum( const void *data, int length );
void CRC16_FinishChecksum( unsigned short *crcvalue );
void CRC16_UpdateChecksum( unsigned short *crcvalue, const void *data, int length );
void CRC16_InitChecksum( unsigned short *crcvalue );

                 /*  Function Related to Analogue Channel   */

int config_AI_RawMode(int fd, int IO_num, int chanel_num);
int set_AI_ACQ_Freq(int fd, int IO_num);
int ack_AI_GoodBlock(int fd, int IO_num, uint8_t channel_num);
int read_AI_Channel_Input(int fd, int IO_num, int chanel_num);
int read_AI_Analogue_block(int fd, int IO_num, uint8_t channel_num);
int read_AI_Channel_Config(int fd, int IO_num, int channel_num);
int read_AI_Board_Config(int fd, int IO_num, AI_Card_Info *AIboardinfo;);
uint8_t read_AI_BlockData_Partial(int fd, int IO_num, uint8_t channel_num);

int write_AI_Channel_Config(int fd, int IO_num, int channel_num, uint8_t setup_info, uint8_t range_info);
int write_AI_Board_Config(int fd, int IO_num, uint8_t *setup_info, uint8_t *range_info);
int all_Partial_Data(int fd, int IO_num, uint8_t channel_num, uint8_t ch_count);
int processing_AI_BlockData();
int reset_AI_Global_Data(void);
int execute_AI_Test(int fd, int slot_num, uint8_t channels_num, int Acq_rate);
int GetAICardData(int fd, int slot_num, AI_Card_Info *AICardinfo);
int GetAIChannelData(int fd, int slot_num, int channel_num, AI_Card_Info *AICardinfo, AI_Channel_info *AIChannelinfo);
int ReadAIAnalogueBlock(int fd, int IO_num, uint8_t channel_num, uint8_t *data_ptr, uint8_t *error_code);
int readAIFactoryCal(int fd, int slot_num, uint8_t input_pair, uint8_t gain);
int readAIData(uint8_t ch_num, uint8_t *data_ptr, int d_acq_value);
int GetPlottingCounter(AI_Channel_info *AIChannelinfo, int m_frq, int max_frq,int *counter, long *delay);


				/*  Function Related to Digital I/O Card */


int write_DIO_Board_Config(int fd, int IO_num);  // 6d
int read_DIO_Board_Config(int fd, int IO_num);	//6e
int write_DIO_Pin_Output(int fd, int IO_num, uint8_t byte_04, uint8_t byte_05, uint8_t byte_06);		//6b
int read_DIO_Pin_Current_state(int fd, int IO_num);	//6a
int ack_DIO_Data_Received();		//69
int read_DIO_AllInput_stored(int fd, int IO_num);		//68



#endif