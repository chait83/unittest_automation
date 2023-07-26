/*
In this file inputting some more api related to analog input
and keeping all the printf and log msg into the conditional compilation.

API : int config_AI_RawMode(int fd, int IO_num, int chanel_num);

*/


#include "v3_qt_spi.h"

#define SIGTX 44
#define REGISTER_UAPP _IO('R', 'g')

rx_function_header *rx_header;


Slot_Detect *detect_slot = NULL;
CARD_SLOT *slot_info[9] = {0,};
Slot_Name *slot_name;


uint8_t x_rx_buf[256] = {0,};
uint8_t rx_pData[256] = {0,};

int fd;
int sig_fd;

uint8_t TX_pBuffer[12];
uint16_t num_of_data;
uint8_t Tx_pBuffer[24];

/* Flags for checking execution */
uint8_t flag_fun_code = 0;
uint8_t flag_err_code = 0;
uint8_t flag_irq_spi_rdy = 0;


//unsigned short real_data_ch0 = 0;
long long AI_data_ch0 = 0;
long long AI_data_ch1 = 0;
long long AI_data_ch2 = 0;
long long AI_data_ch3 = 0;
long long AI_data_ch4 = 0;
long long AI_data_ch5 = 0;
long long AI_data_ch6 = 0;
long long AI_data_ch7 = 0;


long AI_ch0_TS = 0;
long AI_ch1_TS = 0;
long AI_ch2_TS = 0;
long AI_ch3_TS = 0;
long AI_ch4_TS = 0;
long AI_ch5_TS = 0;
long AI_ch6_TS = 0;
long AI_ch7_TS = 0;

/* AI channel block data newest reading */

uint8_t AI_CH_Block_Timestamp[2];
uint8_t AI_ch0_Bdata[2] = {0,};
uint8_t AI_ch1_Bdata[2] = {0,};
uint8_t AI_ch2_Bdata[2] = {0,};
uint8_t AI_ch3_Bdata[2] = {0,};
uint8_t AI_ch4_Bdata[2] = {0,};
uint8_t AI_ch5_Bdata[2] = {0,};
uint8_t AI_ch6_Bdata[2] = {0,};
uint8_t AI_ch7_Bdata[2] = {0,};

uint8_t AI_ch0_TS_data[2] = {0,};
uint8_t AI_ch1_TS_data[2] = {0,};
uint8_t AI_ch2_TS_data[2] = {0,};
uint8_t AI_ch3_TS_data[2] = {0,};
uint8_t AI_ch4_TS_data[2] = {0,};
uint8_t AI_ch5_TS_data[2] = {0,};
uint8_t AI_ch6_TS_data[2] = {0,};
uint8_t AI_ch7_TS_data[2] = {0,};

int time_ms = 0;
struct timespec start, end, x_start, x_end;
uint64_t delta_us;
int counter[8];
int d_acq_value = 0;


int LOG_LEVEL_1 = 1;
int LOG_LEVEL_2 = 1;

unsigned short time_stamp_ch0;

static const char *device = "/dev/spidev1.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 3000000;
static uint16_t delay;
int verbose = 0;

#define SPI_LOG_0   0
#define SPI_LOG_1   1
#define SPI_LOG_2   2

typedef struct  _data_ptr
    {
        int fd;
        int slot_num;
        int channel_num;
        int time_in_ms;
        /* data */
    }data_ptr;


unsigned short crctable[256]= {
0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

/********  GPIO_CARD_SELECTION  ************** */

int set_gpio_direction(int gpio, int direction)
{
	FILE * pFile;
	char *gpio_path = NULL;
	int ret;

	ret = asprintf(&gpio_path, "/sys/class/gpio/gpio%d/direction",gpio);
	if (ret < 0)
	{
		free(gpio_path);
		gpio_path = NULL;
	}

	if (access(gpio_path, F_OK ) == 0)
	{
		pFile=fopen(gpio_path, "wr");
		if (pFile==NULL) {
			printf("File open error %s\r\n",__func__);
			free(gpio_path);
			gpio_path = NULL;
		}
		else{
			if (direction == 1)
				ret = fwrite("in",1,2,pFile);
			if (direction == 0)
				ret = fwrite("out",1,3,pFile);
			if (ret < 0 )
				printf("Direction set failed \r\n");
			else
				printf("Direction Set Success \r\n");
				
			fclose(pFile);
		}
	}
	return ret;
}

int gpio_export(int gpio, int direction)
{
        FILE * pFile;
        int ret =0;
        char *gpio_path = NULL;
        char *gpio_buf = NULL;
        ret = asprintf(&gpio_buf,"%d",gpio);
        if(ret < 0)
        {
                free(gpio_buf);
                gpio_buf = NULL;
        }
        ret = asprintf(&gpio_path,"/sys/class/gpio/gpio%d", gpio);
        if(ret < 0)
        {
		free(gpio_buf);
                gpio_buf = NULL;

        }
        if (access(gpio_path, F_OK ) == 0)
        {
                ret=-1;
        }
        else
        {
                pFile=fopen("/sys/class/gpio/export", "wr");
                if (pFile==NULL)
                {
                        printf("File open error %s\r\n",__func__);
			free(gpio_buf);
                	gpio_buf = NULL;
			return -1;
                }
                ret = fwrite( gpio_buf,1, strlen(gpio_buf) ,pFile);
                if (ret < 0)
                {
                        fclose(pFile);
                }
                else
                {
                        fclose(pFile);
                        set_gpio_direction(gpio, direction );
                }
        }
        return ret;
}

int set_gpio_value(int gpio, int value)
{
	int ret;
	FILE *pFile = NULL;
	char *gpio_path = NULL;
	char *gpio_buf = NULL;

	ret = asprintf(&gpio_buf,"%d",value);
	if (ret < 0)
	{
		ret = -1;
		free(gpio_path);
		gpio_path = NULL;

	}

	asprintf(&gpio_path, "/sys/class/gpio/gpio%d/value",gpio);
	if (ret < 0)
	{
		ret = -1;
		free(gpio_path);
		gpio_path = NULL;
	}

	if (access(gpio_path, F_OK ) == 0)
	{

	}
	else
	{
		gpio_export(gpio, 0);
	}
	if (access(gpio_path, F_OK ) == 0)
	{
		pFile=fopen(gpio_path, "wr");
		if (pFile==NULL) {
			printf("File open error  %s\r\n",__func__);
			free(gpio_path);
			gpio_path = NULL;

		}

		ret = fwrite( gpio_buf,1,strlen( gpio_buf ),pFile);
		if (ret < 0 )
		{
			fclose(pFile);
			free(gpio_path);
			gpio_path = NULL;

		}

		fclose(pFile);
	}
	return ret;
}

int get_gpio(int gpio, int * value)
{
	int ret =0;
	FILE * pFile;
	char gpio_buf[10] = {0};
	char *gpio_path = NULL;

	ret = asprintf(&gpio_path, "/sys/class/gpio/gpio%d/value",gpio);
	if (ret < 0)
	{
		free(gpio_path);
		gpio_path = NULL;
	}
	if (access(gpio_path, F_OK ) == 0)
	{
		pFile=fopen(gpio_path, "r");
		if (pFile==NULL) {
			printf("File open error %s\r\n",__func__);
			free(gpio_path);
			gpio_path = NULL;
		}

		ret = fread(gpio_buf, 1, sizeof(gpio_buf), pFile);
		if (ret == strlen(gpio_buf))
		{
			*value = atoi(gpio_buf);
		}

		fclose(pFile);

	}
	return ret;
}


void  IOCard_Selection(int IOCardindex)
{

	switch(IOCardindex)
	{
	case 0:
		set_gpio_value(GPIO1_8, OFF);
		set_gpio_value(GPIO4_12, OFF);
		set_gpio_value(GPIO4_11, OFF);
		set_gpio_value(GPIO3_19, OFF);

		break;
	case 1:
		set_gpio_value(GPIO1_8, OFF);
		set_gpio_value(GPIO4_12, OFF);
		set_gpio_value(GPIO4_11, OFF);
		set_gpio_value(GPIO3_19, ON);/*write 1*/

		break;
	case 2:
		set_gpio_value(GPIO1_8, OFF);
		set_gpio_value(GPIO4_12, OFF);
		set_gpio_value(GPIO4_11, ON);/*write 1*/
		set_gpio_value(GPIO3_19, OFF);

		break;
	case 3:
		set_gpio_value(GPIO1_8, OFF);
		set_gpio_value(GPIO4_12, OFF);
		set_gpio_value(GPIO4_11, ON);/*write 1*/
		set_gpio_value(GPIO3_19, ON);/*write 1*/

		break;
	case 4:
		set_gpio_value(GPIO1_8, OFF);
		set_gpio_value(GPIO4_12, ON);/*write 1*/
		set_gpio_value(GPIO4_11, OFF);
		set_gpio_value(GPIO3_19, OFF);

		break;
	case 5:
		set_gpio_value(GPIO1_8, OFF);
		set_gpio_value(GPIO4_12, ON);/*write 1*/
		set_gpio_value(GPIO4_11, OFF);
		set_gpio_value(GPIO3_19, ON);/*write 1*/

		break;
	case 6:
		set_gpio_value(GPIO1_8, OFF);
		set_gpio_value(GPIO4_12, ON);/*write 1*/
		set_gpio_value(GPIO4_11, ON);/*write 1*/
		set_gpio_value(GPIO3_19, OFF);

		break;
	case 7:
		set_gpio_value(GPIO1_8, OFF);
		set_gpio_value(GPIO4_12, ON);/*write 1*/
		set_gpio_value(GPIO4_11, ON);/*write 1*/
		set_gpio_value(GPIO3_19, ON);/*write 1*/

		break;
	case 8:
		set_gpio_value(GPIO1_8, ON);/*write 1*/
		set_gpio_value(GPIO4_12, OFF);
		set_gpio_value(GPIO4_11, OFF);
		set_gpio_value(GPIO3_19, OFF);

		break;
	case 9:
		set_gpio_value(GPIO1_8, ON);/*write 1*/
		set_gpio_value(GPIO4_12, OFF);
		set_gpio_value(GPIO4_11, OFF);
		set_gpio_value(GPIO3_19, ON);
    
    case 10:
		set_gpio_value(GPIO1_8, ON);/*write 1*/
		set_gpio_value(GPIO4_12, OFF);
		set_gpio_value(GPIO4_11, ON);
		set_gpio_value(GPIO3_19, OFF);
    
    case 11:
		set_gpio_value(GPIO1_8, ON);/*write 1*/
		set_gpio_value(GPIO4_12, OFF);
		set_gpio_value(GPIO4_11, ON);
		set_gpio_value(GPIO3_19, ON);
    
    case 12:
		set_gpio_value(GPIO1_8, ON);/*write 1*/
		set_gpio_value(GPIO4_12, ON);
		set_gpio_value(GPIO4_11, OFF);
		set_gpio_value(GPIO3_19, OFF);
    
    case 13:
		set_gpio_value(GPIO1_8, ON);/*write 1*/
		set_gpio_value(GPIO4_12, ON);
		set_gpio_value(GPIO4_11, OFF);
		set_gpio_value(GPIO3_19, ON);

    case 14:
		set_gpio_value(GPIO1_8, ON);/*write 1*/
		set_gpio_value(GPIO4_12, ON);
		set_gpio_value(GPIO4_11, ON);
		set_gpio_value(GPIO3_19, OFF);

    case 15:
		set_gpio_value(GPIO1_8, ON);/*write 1*/
		set_gpio_value(GPIO4_12, ON);
		set_gpio_value(GPIO4_11, ON);
		set_gpio_value(GPIO3_19, ON);

		break;
	default:
		break;
	}
}

/**************************************************/


/********************  CRC_CALCULATION  ************************/

void CRC16_InitChecksum( unsigned short *crcvalue )
{
	*crcvalue = CRC16_INIT_VALUE;
}

void CRC16_UpdateChecksum( unsigned short *crcvalue, const void *data, int length )
{
	unsigned short crc;
	const unsigned char *buf = (const unsigned char *) data;
	crc = *crcvalue;
	while( length-- )
	{
		crc = ( crc << 8 ) ^ crctable[ ( crc >> 8 ) ^ *buf++ ];
	}
	*crcvalue = crc;
}

void CRC16_FinishChecksum( unsigned short *crcvalue )
{
	*crcvalue ^= CRC16_XOR_VALUE;
}

unsigned short CRC16_BlockChecksum( const void *data, int length )
{
	unsigned short crc;
	CRC16_InitChecksum( &crc );
	CRC16_UpdateChecksum( &crc, data, length );
	CRC16_FinishChecksum( &crc );
	return crc;
}

static void hex_dump(const void *src, size_t length, size_t line_size,
		     char *prefix)
{
	int i = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" |");
			while (line < address) {
				c = *line++;
				printf("%c", (c < 32 || c > 126) ? '.' : c);
			}
			printf("|\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}

int hex_to_dec(uint8_t *hex_value)
{
	int y = 0;
   	int decimal = 0;
   	int x, i;
   	for(i = strlen(hex_value) - 1 ; i >= 0 ; --i)
	{
      	if(hex_value[i] >= '0' && hex_value[i] <= '9')
		{
         	x = hex_value[i] - '0';
      	}
      	else
		{
        	x = hex_value[i] - 'A' + 10;
     	}
      	decimal = decimal + x * pow(16 , y); 
   	}
   
   return decimal;
    
}

void spi_Write(int fd, uint8_t const *tx,  size_t len)
{
	int ret;
    
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .len = len,
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };

        if (mode & SPI_TX_QUAD)
                tr.tx_nbits = 4;
        else if (mode & SPI_TX_DUAL)
                tr.tx_nbits = 2;
        if (mode & SPI_RX_QUAD)
                tr.rx_nbits = 4;
        else if (mode & SPI_RX_DUAL)
                tr.rx_nbits = 2;
        if (!(mode & SPI_LOOP)) {
                if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
                        tr.rx_buf = 0;
                else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
                        tr.tx_buf = 0;
        }


        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        
        if (ret < 1)
                pabort("can't send spi message");
    if(verbose == 1){
      hex_dump(tx, len, 32, "TX");
    }
}


int spi_Read(int fd, uint8_t const *rx,  size_t len)
{

	int ret;

        struct spi_ioc_transfer Rx = {
                .rx_buf = (unsigned long)rx,
                .len = len,
                .delay_usecs = delay,
        
        };

	if (mode & SPI_RX_QUAD)
                Rx.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
                Rx.rx_nbits = 2;
	
	if (!(mode & SPI_LOOP)) {
                if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
                        Rx.rx_buf = 0;
                else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
                        Rx.tx_buf = 0;
        }
        
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &Rx);
    flag_irq_spi_rdy = 0;

    if(verbose == 1)
        hex_dump(rx, len, 32, "RX");

    return 0;
}

void spi_Write_Read(int fd, uint8_t *buff, int size)
{

    int ret;
	int len;
	struct spi_ioc_transfer xfer[2];
	uint8_t rx_buff[size], *bp;

	memset(xfer, 0, sizeof xfer);

	xfer[0].tx_buf = (unsigned long)buff;
	len = size;
	xfer[0].len = size;

	xfer[1].rx_buf = (unsigned long)rx_buff;
	xfer[1].len = size;


        ret = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);

        for (bp = rx_buff; len; len--)
	{
		printf("%02x ", *bp++);

	}
	printf("\n");

}


int fill_FunCode_Data(uint8_t *TxData, uint8_t fun_code, uint8_t len)
{
	int result, i;
	
	for (i = 0; i < len; i++)
        {
                TxData[i] = 0xFF;
        }
	TxData[0x0] = fun_code;
	TxData[0x1] = ~(fun_code);
	TxData[0x2] = 0x00;
	TxData[0x3] = 0x00;

	result = CRC16_BlockChecksum(&TxData[0], 4);

	TxData[0x4] = result >> 8;
	TxData[0x5] = result & 0xFF;
	
	return 1;

}
  
int get_Data_from_Rx(uint8_t *x_rx_buf, int len)
{
    int result = 0, k = 0;
    uint16_t i = 0;
    uint16_t num_data = 0x0000;
    
    num_data =  (rx_header-> byte_high) << 8;
    num_data = num_data | (rx_header -> byte_low);
    num_data = num_data + 2;

    rx_header -> numofdata = num_data;
   

    memset(rx_pData, 0, sizeof(rx_pData));

    for (i = 0; i < num_data; i++)
    {
        rx_pData[k] = x_rx_buf[6+k];
        k++;
    }
    //printf("value of k %d\n", k);

    return 1;

}

int  get_Rx_Header(uint8_t fun_code, uint8_t *x_rx_buf, int len)
{
	
    rx_header = (rx_function_header *)malloc(sizeof(rx_function_header));

	rx_header -> fun_code = x_rx_buf[2];
    rx_header -> error_code = x_rx_buf[3];
    rx_header -> byte_high = x_rx_buf[4];
    rx_header -> byte_low = x_rx_buf[5];

	if(fun_code == rx_header->fun_code)
	{
		flag_fun_code = 1;
	}
	else
		flag_fun_code = -1;

	if (rx_header->error_code > 0x00)
	{
		flag_err_code = rx_header -> error_code;
	}

    if ((flag_fun_code == 1) && (flag_err_code == 0))
	{
        if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
        printf("fun_code : %X error_code : %X\n", rx_header->fun_code, rx_header->error_code);
		printf("high_byte: %2X low_byte : %2X\n", rx_header->byte_high, rx_header->byte_low);
        }
        return 1;
    }
	
	return -1;

}

void signal_handler(int sig)
{
    //printf("Interrupt occured %d\n", sig);
    flag_irq_spi_rdy = 1;
    return ;
    //exit(1);
}

int irq_init()
{
    signal(SIGTX, signal_handler);

    printf("PID: %d\n", getpid());

    sig_fd = open("/dev/spi_irq_sig", O_RDONLY);

    if(sig_fd < 0)
    {
        perror("Could not open device file\n");
        return -1;
    }

    if (ioctl(sig_fd, REGISTER_UAPP, NULL))
    {
        perror("Error Registering app\n");
        close(sig_fd);
        return -1;
    }

    return 0;
}

int reset_AI_Global_Data(void)
{
    AI_data_ch0 = 0;
    AI_data_ch1 = 0;
 AI_data_ch2 = 0;
 AI_data_ch3 = 0;
 AI_data_ch4 = 0;
 AI_data_ch5 = 0;
 AI_data_ch6 = 0;
 AI_data_ch7 = 0;

 AI_ch0_TS = 0;
 AI_ch1_TS = 0;
 AI_ch2_TS = 0;
 AI_ch3_TS = 0;
 AI_ch4_TS = 0;
 AI_ch5_TS = 0;
 AI_ch6_TS = 0;
 AI_ch7_TS = 0;

memset(AI_ch0_Bdata, 0, sizeof(AI_ch0_Bdata));
memset(AI_ch1_Bdata, 0, sizeof(AI_ch1_Bdata));
memset(AI_ch2_Bdata, 0, sizeof(AI_ch2_Bdata));
memset(AI_ch3_Bdata, 0, sizeof(AI_ch3_Bdata));
memset(AI_ch4_Bdata, 0, sizeof(AI_ch4_Bdata));
memset(AI_ch5_Bdata, 0, sizeof(AI_ch5_Bdata));
memset(AI_ch6_Bdata, 0, sizeof(AI_ch6_Bdata));
memset(AI_ch7_Bdata, 0, sizeof(AI_ch7_Bdata));

memset(AI_ch0_TS_data, 0, sizeof(AI_ch0_TS_data));
memset(AI_ch1_TS_data, 0, sizeof(AI_ch1_TS_data));
memset(AI_ch2_TS_data, 0, sizeof(AI_ch2_TS_data));
memset(AI_ch3_TS_data, 0, sizeof(AI_ch3_TS_data));
memset(AI_ch4_TS_data, 0, sizeof(AI_ch4_TS_data));
memset(AI_ch5_TS_data, 0, sizeof(AI_ch5_TS_data));
memset(AI_ch6_TS_data, 0, sizeof(AI_ch6_TS_data));
memset(AI_ch7_TS_data, 0, sizeof(AI_ch7_TS_data));

return 1;
}

int processing_AI_BlockData()
{
    /*
    unsigned short real_data;
    uint8_t i = 0x00;
    uint8_t count;

    count = rx_pData[7];
    
    for (i = 0; i < count; i++)
    {
        memcpy(&real_data, &rx_pData[i], 2);
        printf("%X\t%d", rx_pData[i], real_data);

        i++;
    }
*/

return 1;

}

int all_Partial_Data(int fd, int IO_num, uint8_t channel_num, uint8_t ch_count)
{
    int result = 0;
    uint8_t read_result;
    int i = 0;
    uint8_t card_data[4], ch_num;
    
    ch_num = channel_num;
   
    while(i < 10 )
    {
        result = ack_AI_GoodBlock(fd, IO_num, ch_num);
        read_result = read_AI_BlockData_Partial(fd, IO_num, ch_num);
        
        if ((read_result == 0) || (read_result == 0x01))
        {
            memcpy(&card_data[0], &rx_pData[0], 4);
            result = processing_AI_BlockData();

            if (card_data[3])
            {
                ch_num = card_data[3];
            }

            if (card_data[3] == 0)
            {
                break;
            }
        }

        i++;
    }

    return 1;

}

int error_code_action(int fd, uint8_t fun_code, uint8_t flag_err_code, uint32_t IO_num,
                      uint32_t channel_num, uint8_t channel_count)
{
    int result = 0;
    uint8_t read_result;
    
    switch (flag_err_code)
    {

        case ERROR_NO_DATA:
            printf("There is no reading to return on channel\n");
            return -1;

        case ERROR_PARTIAL_DATA:
        {

           // result = all_Partial_Data(fd, IO_num, channel_num, channel_count);
            return result;
        }
        case ERROR_TIMED_OUT:

            break;

        case ERROR_IN_LIST:
            result = IO_CardTest_CTAB(fd, IO_num, 0x0a);
            break;
           
        default:
            printf("Error Code! is not defined in Error list \n");

    }

    return result;

}

int custom_execute_Test(int fd)
{
	int result, pin_val = -1;
    int len;

    len = sizeof(Tx_pBuffer);

	memset(x_rx_buf, 0, sizeof(x_rx_buf));
    flag_irq_spi_rdy = 0;
    spi_Write(fd, Tx_pBuffer, sizeof(Tx_pBuffer));
    flag_irq_spi_rdy = 0;
    spi_Write(fd, Tx_pBuffer, sizeof(Tx_pBuffer));
   
   usleep(50*1000);

    for (int i = 0; i<15; i++)
    {
        if (flag_irq_spi_rdy == 1)
        {
            printf("\n");
            spi_Read(fd, x_rx_buf, sizeof(x_rx_buf));
            flag_irq_spi_rdy = 0;
		    return 1;
        }
        usleep(500);
    }
     return -1;
}

int execute_Test(int fd)
{
	int result, pin_val = -1;
 
	memset(x_rx_buf, 0, sizeof(x_rx_buf));
    flag_irq_spi_rdy = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    spi_Write(fd, TX_pBuffer, sizeof(TX_pBuffer));
    flag_irq_spi_rdy = 0;
    spi_Write(fd, TX_pBuffer, sizeof(TX_pBuffer));
   
    if (time_ms > 0)
        usleep(time_ms * 1000);
    
    
    for (int i = 0; i<15; i++)
    {
        if (flag_irq_spi_rdy == 1)
        {
            //printf("\n");
            spi_Read(fd, x_rx_buf, sizeof(x_rx_buf));
            flag_irq_spi_rdy = 0;
            clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		    return 1;
        }
        usleep(500);
       
    }
     return -1;
}

int execute_Test_buffer(int fd, uint8_t *buff, int len)
{
	int result, pin_val = -1;
    int i;

	memset(x_rx_buf, 0, sizeof(x_rx_buf));
    flag_irq_spi_rdy = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    spi_Write(fd, buff, len);
    flag_irq_spi_rdy = 0;
    spi_Write(fd, buff, len);
   
    if (time_ms > 0)
        usleep(time_ms * 1000);
    
    for (int i = 0; i<15; i++)
    {
        if (flag_irq_spi_rdy == 1)
        {
            //printf("\n");
            spi_Read(fd, x_rx_buf, sizeof(x_rx_buf));
            flag_irq_spi_rdy = 0;
            clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		    return 1;
        }
        usleep(500);
    }
     return -1;
}

int IO_CardTest_CTAB(int fd, uint32_t IO_num, uint8_t fun_code)
{
    int result, i = 0;
    int pin_val = -1;
    uint8_t tx_pBuffer[12];

    uint8_t board_type, board_revision, channels_available;
    uint16_t channels_enabled;
    uint16_t crc;



    /************ IO Card Selection *************/
    IOCard_Selection(IO_num);

    /*******************  Preparing Buffer to send **********/
	result = fill_FunCode_Data(TX_pBuffer, fun_code, sizeof(TX_pBuffer));
		
    /*********************   sending Data ********************************/

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 == 1)
                printf("\nCan not execute the function code\n");
            return -1;
        }

        if (LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
            printf("RX Header : %2X %2X %2X %2X\n", rx_header->fun_code, rx_header->error_code,
                    rx_header->byte_high, rx_header->byte_low);
            hex_dump(rx_pData, rx_header -> byte_low, 32, "Data : ");
        }

        if (LOG_LEVEL_1 == 2 || LOG_LEVEL_1 == 5)
        {
            printf("\n\n");
            printf("        Board Type :: %d\n", (board_type = rx_pData[0]));
            printf("    Board Revision :: %0.2X\n", (board_revision = rx_pData[1]));
            printf("Channels Available :: %0.2X\n", (channels_available = rx_pData[2]));
            channels_enabled = rx_pData[3] << 8;
            channels_enabled = channels_enabled | rx_pData[4];
            printf("  Channels Enabled :: %0.4X\n", channels_enabled);
            crc = rx_pData[rx_header->byte_low] << 8;
            crc = crc | rx_pData[rx_header->byte_low + 1];
            printf("               CRC :: %0.4X\n", crc);
        }
    return result;

}

int reset_CircularBuff(int fd, uint32_t IO_num, int time_stamp)
{
    int result, i = 0, len;
    int pin_val = -1;
    uint8_t tx_pBuffer[12];

    /************ IO Card Selection *************/
    IOCard_Selection(IO_num);

    /*******************  Preparing Buffer to send **********/
	len = sizeof(TX_pBuffer);

	for (i=0; i<len; i++)
	{
		TX_pBuffer[i] = 0xFF;
	}

    if(time_stamp == 0)
    {
	TX_pBuffer[0x00] = 0x12;
    TX_pBuffer[0x01] = ~(0x12);
    TX_pBuffer[0x02] = 0 / MAX_BYTE_VALUE;
    TX_pBuffer[0x03] = 0 % MAX_BYTE_VALUE;
    result = CRC16_BlockChecksum(&TX_pBuffer[0], 4);
    TX_pBuffer[0x04] = result >> 8;
    TX_pBuffer[0x05] = result & 0xFF;
    }

    if(time_stamp == 1)
    {
	TX_pBuffer[0x00] = 0x12;
    TX_pBuffer[0x01] = ~(0x12);
    TX_pBuffer[0x02] = 0 / MAX_BYTE_VALUE;
    TX_pBuffer[0x03] = 1 % MAX_BYTE_VALUE;
    TX_pBuffer[0x04] = 1;
    result = CRC16_BlockChecksum(&TX_pBuffer[0], 5);
    TX_pBuffer[0x05] = result >> 8;
    TX_pBuffer[0x06] = result & 0xFF;
    }

    /*********************   sending Data ********************************/

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 = 1)
                printf("\nCan not execute the function code\n");
            return -1;
        }
        
        if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
            printf("RX Header : %2X %2X %2X %2X\n", rx_header->fun_code, rx_header->error_code,
                rx_header->byte_high, rx_header->byte_low);
            hex_dump(rx_pData, rx_header -> byte_low, 32, "Data : ");
        }
    return result;
}


int SPI_init(int fd)
{
    uint8_t x_rx_buff[100];

    for (int i = 0; i<100; i++)
    {
	    x_rx_buff[i] = 0xFF;
    }

    spi_Read(fd, x_rx_buff, sizeof(x_rx_buff));
    for (int i =0; i<(60*100) ; i++)
    {
	    usleep(1);
    }
    return 1;
}

int get_Life_History(int fd, uint8_t IO_num, uint8_t mode)
{
	
	int i=0, len;
	int result;
    uint8_t board_type;
    uint8_t mode_type;

	len = sizeof(TX_pBuffer);

	for (i=0; i<len; i++)
	{
		TX_pBuffer[i] = 0xFF;
	}

	TX_pBuffer[0x00] = 0x05;
    TX_pBuffer[0x01] = ~(0x05);
    TX_pBuffer[0x02] = 0 / MAX_BYTE_VALUE;
    TX_pBuffer[0x03] = 1 % MAX_BYTE_VALUE;
	TX_pBuffer[0x04] = mode;

    result = CRC16_BlockChecksum(&TX_pBuffer[0], 5);

    TX_pBuffer[0x05] = result >> 8;
    TX_pBuffer[0x06] = result & 0xFF;

	IOCard_Selection(IO_num);

	for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

    if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 == 1)
                printf("\n Failed to execute the function code\n");
            return -1;
        }
    board_type = rx_pData[0];
    mode_type = rx_pData[1];

    printf("Board Type : %2X  Mode_type : %2X", board_type, mode_type);


	if (result == 1)
	{
		printf("\n Successfull on IO card : %d\n\n", IO_num);
		return 1;
	}

	return -1;

}

int reset_Board(int fd, int IO_num)
{

        int result, len, i;

        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = 0x01;
        TX_pBuffer[0x01] = ~(0x01);
        TX_pBuffer[0x02] = 0x00;
        TX_pBuffer[0x03] = 0x00;

        result = CRC16_BlockChecksum(&TX_pBuffer[0], 4);

        TX_pBuffer[0x04] = result >> 8;
        TX_pBuffer[0x05] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 == 1)
                printf("\n Failed to execute the function code\n");
            return -1;
        }

        if (result == 1)
        {
                printf("Successfull on IO card : %d\n\n", IO_num);
                return 1;
        }

        return -1;


}

int read_AI_Board_Config(int fd, int IO_num, AI_Card_Info *AICardinfo)
{
		int result, len, i;
        

        if (slot_info[IO_num]->board_type != 0x00)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = 0x41;
        TX_pBuffer[0x01] = ~(0x41);
        TX_pBuffer[0x02] = 0x00;
        TX_pBuffer[0x03] = 0x00;

        result = CRC16_BlockChecksum(&TX_pBuffer[0], 4);

        TX_pBuffer[0x04] = result >> 8;
        TX_pBuffer[0x05] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 ==1)
                printf("\n Failed to execute the function code\n");
            return -1;
        }

        memcpy(&AICardinfo->version[0], &rx_pData[0], 2);
        memcpy(&AICardinfo->setupInfo[0], &rx_pData[2], 8);
        memcpy(&AICardinfo->rangeInfo[0], &rx_pData[10], 8);

        if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
        hex_dump(&AICardinfo->version[0], 2, 2, "Version");
        hex_dump(&AICardinfo->setupInfo[0], 8, 8, "Setup Info");
        hex_dump(&AICardinfo->rangeInfo[0], 8, 8, "Range Info");
        }
        if (result == 1)
        {
                printf("Successfull on IO card : %d\n\n", IO_num);
                return 1;
        }

        return -1;

}

int read_AI_Channel_Config(int fd, int IO_num, int channel_num)
{

	    int result, len, i = 0;

        uint8_t rx_chanel_num;
        uint8_t chanel_config[2];

       if (slot_info[IO_num]->board_type != 0x00)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = 0x3f;
        TX_pBuffer[0x01] = ~(0x3f);
        TX_pBuffer[0x02] = 0 / MAX_BYTE_VALUE;
        TX_pBuffer[0x03] = 1 % MAX_BYTE_VALUE;
	    TX_pBuffer[0x04] = channel_num;
        
	    result = CRC16_BlockChecksum(&TX_pBuffer[0], 5);

        TX_pBuffer[0x05] = result >> 8;
        TX_pBuffer[0x06] = result & 0xFF;


        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 == 1)
            printf("\n Failed to execute the function code\n");
            return -1;
        }

        rx_chanel_num = rx_pData[0];
        memcpy(chanel_config, &rx_pData[1], 2);

        if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
            printf("Chanel Num : %d\n", rx_chanel_num);
            hex_dump(chanel_config, 2, 2, "Channel Configuration :");
        }

        if (result == 1)
        {
                printf("Successfull on IO card : %d && Channel : %d\n\n", IO_num, channel_num);
                return 1;
        }

        return -1;


}

int config_AI_RawMode(int fd, int IO_num, int chanel_num)
{
    int result, len, i = 0;
    uint8_t rx_chanel_num;
    uint8_t raw_state;

    if (slot_info[IO_num]->board_type != 0x00)
    {
        printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
        return -1;
    }
    
    len = sizeof(TX_pBuffer);

    for (i=0; i<len; i++)
    {
            TX_pBuffer[i] = 0xFF;
    }

    TX_pBuffer[0x00] = 0x39;
    TX_pBuffer[0x01] = ~(0x39);
    TX_pBuffer[0x02] = 0 / MAX_BYTE_VALUE;
    TX_pBuffer[0x03] = 2 % MAX_BYTE_VALUE;
    TX_pBuffer[0x04] = chanel_num;
    TX_pBuffer[0x05] = 0x00;

    result = CRC16_BlockChecksum(&TX_pBuffer[0], 6);

    TX_pBuffer[0x06] = result >> 8;
    TX_pBuffer[0x07] = result & 0xFF;

    IOCard_Selection(IO_num);

    for (i = 0; i < 3; i++)
    {
        result = execute_Test(fd);
        if (result == 1)
        {
            result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
            flag_fun_code = 0;
            flag_err_code = 0;

			if (result == 1)
            {
                result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                break;
            }
        }
    }

    if (result < 0)
    {
        flag_fun_code = 0;
        flag_err_code = 0;
        if (LOG_LEVEL_2 == 1)
        printf("\n Failed to execute the function code\n");
        return -1;
    }

    rx_chanel_num = rx_pData[0];
    raw_state = rx_pData[1];

    if (LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
    {
        printf("Chanel Num : %d\n", rx_chanel_num);
        printf("Raw state it : %2X\n", raw_state);
    }

    if (result == 1)
    {
            printf("Successfull on IO card : %d && Channel : %d\n\n", IO_num, chanel_num);
            return 1;
    }

    return -1;
}

int set_AI_ACQ_Freq(int fd, int IO_num)
{
    int result, len, i = 0;
    uint32_t ch_ACFQ;
    uint16_t time_stamp;

    if (slot_info[IO_num]->board_type != 0x00)
    {
        printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
        return -1;
    }

    len = sizeof(TX_pBuffer);

    for (i=0; i<len; i++)
    {
            TX_pBuffer[i] = 0xFF;
    }

    TX_pBuffer[0x00] = 0x3d;
    TX_pBuffer[0x01] = ~(0x3d);
    TX_pBuffer[0x02] = 0 / MAX_BYTE_VALUE;
    TX_pBuffer[0x03] = 4 % MAX_BYTE_VALUE;
    TX_pBuffer[0x04] = 0x44;
    TX_pBuffer[0x05] = 0x44;
    TX_pBuffer[0x06] = 0x44;
    TX_pBuffer[0x07] = 0x44;

    result = CRC16_BlockChecksum(&TX_pBuffer[0], 8);

    TX_pBuffer[0x08] = result >> 8;
    TX_pBuffer[0x09] = result & 0xFF;

    IOCard_Selection(IO_num);

    for (i = 0; i < 3; i++)
    {
        result = execute_Test(fd);
        if (result == 1)
        {
            result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
            flag_fun_code = 0;
            flag_err_code = 0;

			if (result == 1)
            {
                result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                break;
            }
        }
    }

    if (result < 0)
    {
        flag_fun_code = 0;
        flag_err_code = 0;
        if (LOG_LEVEL_2 == 1)
            printf("\n Failed to execute the function code\n");
        return -1;
    }

    memcpy(&ch_ACFQ, &rx_pData[0], 4);
    memcpy(&time_stamp, &rx_pData[4], 2);

    if (LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
    {
        printf("Chanel frequency : %2X\n", ch_ACFQ);
        printf("Time Stamp : %2X\n", time_stamp);
    }

    if (result == 1)
    {
            printf("Successfull on IO card : %d && Channel : \n\n", IO_num);
            return 1;
    }

    return -1;
}

int ReadAIAnalogueBlock(int fd, int IO_num, uint8_t channel_num, uint8_t *data_ptr, uint8_t *error_code)
{
        int result, len, i, j, ch_count = 0, num_data, k = 0;
        uint8_t rx_funCode, tx_funCode;
        uint8_t card_data[4];
        uint8_t tmp_ch_num;
        uint8_t *ptr_rx_pData = NULL;

        uint8_t CH_NUM;
        uint8_t time_stamp[2] = {0,};
        uint8_t num_of_data = 0x00; 
        uint8_t data_read[255] = {0,};
       
        if (slot_info[IO_num]->board_type != 0x00)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        tmp_ch_num = channel_num;

    /*  Counting the number of Channel */
        while (tmp_ch_num)
        {
            ch_count += tmp_ch_num & 1;
            tmp_ch_num >>= 1 ;
        }

    /*  Filling the function code and data in Tx Buffer */
        tx_funCode = 0x33;

        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x01;
        TX_pBuffer[0x04] = channel_num;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 5);
      
      	TX_pBuffer[0x05] =  result >> 8;
        TX_pBuffer[0x06] = result & 0xFF;

    /*     Selection of IO Card     */
        IOCard_Selection(IO_num);

    /*  Executing the test for SPI TX and SPI RX */

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    break;
                }

                /* Error Code Action  */
                if ((flag_err_code == 0x25) || (flag_err_code == 0x26))
                {
                    *error_code = flag_err_code;
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    flag_err_code = 0;
                    flag_fun_code = 0;
                    result = 1;
                    break;
                }
               
                flag_err_code = 0;
                flag_fun_code = 0;
            }
        }
    
    /*   Validating the result  */
        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if(LOG_LEVEL_2 == 1)
                printf("\n Failed to execute the function code ::: %s\n", __FUNCTION__);
            return -1;
        }

        memcpy(data_ptr, &rx_pData[0], rx_header->numofdata);

        if (LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
            hex_dump(data_ptr, rx_header->numofdata, 32, "Data Read : ");
        }
}

int read_AI_Analogue_block(int fd, int IO_num, uint8_t channel_num)
{
	    int result, len, i, j, ch_count = 0, num_data, k = 0;

        uint8_t rx_funCode, tx_funCode, error_code = 0x00;
        uint8_t card_data[4];
        uint8_t tmp_ch_num;
        uint8_t *ptr_rx_pData = NULL;

        uint8_t CH_NUM;
        uint8_t time_stamp[2] = {0,};
        uint8_t num_of_data = 0x00; 
        uint8_t data_read[255] = {0,};
       
        if (slot_info[IO_num]->board_type != 0x00)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        tmp_ch_num = channel_num;

    /*  Counting the number of Channel */
        while (tmp_ch_num)
        {
            ch_count += tmp_ch_num & 1;
            tmp_ch_num >>= 1 ;
        }

    /*  Filling the function code and data in Tx Buffer */
        tx_funCode = 0x33;

        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x01;
        TX_pBuffer[0x04] = channel_num;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 5);
      
      	TX_pBuffer[0x05] =  result >> 8;
        TX_pBuffer[0x06] = result & 0xFF;

    /*     Selection of IO Card     */
        IOCard_Selection(IO_num);

    /*  Executing the test for SPI TX and SPI RX */

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    break;
                }

                /* Error Code Action  */
                if ((flag_err_code == 0x25) || (flag_err_code == 0x26))
                {
                    error_code = flag_err_code;
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    flag_err_code = 0;
                    flag_fun_code = 0;
                    result = 1;
                    break;
                }
               
                flag_err_code = 0;
                flag_fun_code = 0;
            }
        }
    
    /*   Validating the result  */
        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if(LOG_LEVEL_2 == 1)
                printf("\n Failed to execute the function code ::: %s\n", __FUNCTION__);
            return -1;
        }
    /*  Validating the channel sent data*/
        memcpy(&card_data[0], &rx_pData[0], 4);

        if (card_data[3] == 0)
        {
            goto print_end;         // need to modified according to the error code
        }
       
    /*   Filtering the data from received Buffer */

        memset(data_read, 0, sizeof(data_read));
        ptr_rx_pData = &rx_pData[4];
        
        for (i = 0; i < ch_count; i++)
        {
            CH_NUM = *ptr_rx_pData;
            
            //printf("ch num %0.2X ponter data %0.2X\n",CH_NUM, *ptr_rx_pData);
            
            if (CH_NUM > 7)
            {
                goto print_end;
            }
            
            ptr_rx_pData++;
            memcpy(&time_stamp[0], ptr_rx_pData, 2);
            ptr_rx_pData++;
            ptr_rx_pData++;
            num_of_data = *ptr_rx_pData;
            num_data = num_of_data * 2;
            ptr_rx_pData++;
            j = 0;
            for (k = 0; k < num_data; k++)
            {
                data_read[k] = *ptr_rx_pData;
                ptr_rx_pData++;
                j++;
            }

            switch (CH_NUM)
            {
            case 0:
                memcpy(&AI_ch0_Bdata, &data_read[num_data - 2], 2);
		        memcpy(&AI_ch0_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch0, &AI_ch0_Bdata[0], 2);
                /*AI_data_ch0 = AI_ch0_Bdata[0] << 8;
                AI_data_ch0 = AI_data_ch0 | AI_ch0_Bdata[1];*/
                AI_ch0_TS = AI_ch0_TS_data[0] << 8;
                AI_ch0_TS = AI_ch0_TS | AI_ch0_TS_data[1];
                break;
            case 1:
                memcpy(&AI_ch1_Bdata[0], &data_read[num_data - 2], 2);
		        memcpy(&AI_ch1_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch1, &AI_ch1_Bdata[0], 2);
                /*AI_data_ch1 = AI_ch1_Bdata[0] << 8;
                AI_data_ch1 = AI_data_ch1 | AI_ch1_Bdata[1];*/
                AI_ch1_TS = AI_ch1_TS_data[0] << 8;
                AI_ch1_TS = AI_ch1_TS | AI_ch1_TS_data[1];
                break;
            case 2:
                memcpy(&AI_ch2_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch2_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch2, &AI_ch2_Bdata[0], 2);
                /*AI_data_ch2 = AI_ch2_Bdata[0] << 8;
                AI_data_ch2 = AI_data_ch2 | AI_ch2_Bdata[1];*/
                AI_ch2_TS = AI_ch2_TS_data[0] << 8;
                AI_ch2_TS = AI_ch2_TS | AI_ch2_TS_data[1];
                break;
            case 3:
                memcpy(&AI_ch3_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch3_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch3, &AI_ch3_Bdata[0], 2);
                /*AI_data_ch3 = AI_ch3_Bdata[0] << 8;
                AI_data_ch3 = AI_data_ch3 | AI_ch3_Bdata[1];*/
                AI_ch3_TS = AI_ch3_TS_data[0] << 8;
                AI_ch3_TS = AI_ch3_TS | AI_ch3_TS_data[1];
                break;
            case 4:
                memcpy(&AI_ch4_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch4_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch4, &AI_ch4_Bdata[0], 2);
                /*AI_data_ch4 = AI_ch4_Bdata[0] << 8;
                AI_data_ch4 = AI_data_ch4 | AI_ch4_Bdata[1];*/
                AI_ch4_TS = AI_ch4_TS_data[0] << 8;
                AI_ch4_TS = AI_ch4_TS | AI_ch4_TS_data[1];
                break;
            case 5:
                memcpy(&AI_ch5_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch5_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch5, &AI_ch5_Bdata[0], 2);
                /*AI_data_ch5 = AI_ch5_Bdata[0] << 8;
                AI_data_ch5 = AI_data_ch5 | AI_ch5_Bdata[1];*/
                AI_ch5_TS = AI_ch5_TS_data[0] << 8;
                AI_ch5_TS = AI_ch5_TS | AI_ch5_TS_data[1];
                break;
            case 6:
                memcpy(&AI_ch6_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch6_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch6, &AI_ch6_Bdata[0], 2);
                /*AI_data_ch6 = AI_ch6_Bdata[0] << 8;
                AI_data_ch6 = AI_data_ch6 | AI_ch6_Bdata[1];*/
                AI_ch6_TS = AI_ch6_TS_data[0] << 8;
                AI_ch6_TS = AI_ch6_TS | AI_ch6_TS_data[1];
                break;
            case 7:
                memcpy(&AI_ch7_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch7_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch7, &AI_ch7_Bdata[0], 2);
                /*AI_data_ch7 = AI_ch7_Bdata[0] << 8;
                AI_data_ch7 = AI_data_ch7 | AI_ch7_Bdata[1];*/
                AI_ch7_TS = AI_ch7_TS_data[0] << 8;
                AI_ch7_TS = AI_ch7_TS | AI_ch7_TS_data[1];
                break;
            default:
                break;
            }

        /*  If selectd  LOG LEVEL to print data    */
            if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
            {
                printf("\n\n");
                hex_dump(&card_data, 4, 4, "Card Data : ");
                printf("Channel Number : %d\n", CH_NUM);
                printf("Number of Data : %d\n", num_data);
                hex_dump(&data_read, num_data, 32, "Data Read : ");
                printf("\n\n");
            }
              

        } 

        /*   Right Channel with error code '25' or '26'*/
        if (error_code > 0x00)
        {
            result = error_code_action(fd, tx_funCode, error_code, IO_num, card_data[3], channel_num);
            if (result != 1)
               return result;
        }

print_end:
        if (!(ptr_rx_pData == NULL))
            //free(ptr_rx_pData);
        if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
            hex_dump(&rx_pData[4], 96, 32, "rx_pData : ");
            printf("Successfull on IO card : %d\n", IO_num);
            return 1;
        }

        return -1;

}

int read_AI_Channel_Input(int fd, int IO_num, int chanel_num)
{
        int result, len, i;
        uint8_t rx_chanel_num;
        unsigned short tp = 0;
        unsigned short op = 0;

        if (slot_info[IO_num]->board_type != 0x00)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = 0x32;
        TX_pBuffer[0x01] = ~(0x32);
        TX_pBuffer[0x02] = 0x00;
        TX_pBuffer[0x03] = 0x01;
        TX_pBuffer[0x04] = chanel_num;

        result = CRC16_BlockChecksum(&TX_pBuffer[0], 5);

        TX_pBuffer[0x05] =  result >> 8;
        TX_pBuffer[0x06] = result & 0xFF;


        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 = 1)
                printf("\nCan not execute the function code\n");
            return -1;
        }

        rx_chanel_num = rx_pData[0];

        //printf("ch %0.2X\n", rx_chanel_num);

        switch (rx_chanel_num)
        {
            case 0x00:
            {
		        memcpy(&AI_ch0_Bdata[0], &rx_pData[3], 2);
		        memcpy(&AI_ch0_TS_data[0], &rx_pData[1], 2);
                AI_data_ch0 = AI_ch0_Bdata[0] << 8;
                AI_data_ch0 = AI_data_ch0 | AI_ch0_Bdata[1];
                AI_ch0_TS = AI_ch0_TS_data[0] << 8;
                AI_ch0_TS = AI_ch0_TS | AI_ch0_TS_data[1];
                return 1;
            }

            case 0x01:
            {
		        memcpy(&AI_ch1_Bdata[0], &rx_pData[3], 2);
		        memcpy(&AI_ch1_TS_data[0], &rx_pData[1], 2);
                AI_data_ch1 = AI_ch1_Bdata[0] << 8;
                AI_data_ch1 = AI_data_ch1 | AI_ch1_Bdata[1];
                AI_ch1_TS = AI_ch1_TS_data[0] << 8;
                AI_ch1_TS = AI_ch1_TS | AI_ch1_TS_data[1];
                //printf("%0.2X %0.2X\n", AI_ch1_Bdata[0], AI_ch1_TS_data[0]);
                return 1;
            }

            case 0x02:
            {
		        memcpy(&AI_ch2_Bdata[0], &rx_pData[3], 2);
		        memcpy(&AI_ch2_TS_data[0], &rx_pData[1], 2);
                AI_data_ch2 = AI_ch2_Bdata[0] << 8;
                AI_data_ch2 = AI_data_ch2 | AI_ch2_Bdata[1];
                AI_ch2_TS = AI_ch2_TS_data[0] << 8;
                AI_ch2_TS = AI_ch2_TS | AI_ch2_TS_data[1];
                return 1;
            }

            case 0x03:
            {
		        memcpy(&AI_ch3_Bdata[0], &rx_pData[3], 2);
		        memcpy(&AI_ch3_TS_data[0], &rx_pData[1], 2);
                AI_data_ch3 = AI_ch3_Bdata[0] << 8;
                AI_data_ch3 = AI_data_ch3 | AI_ch3_Bdata[1];
                AI_ch3_TS = AI_ch3_TS_data[0] << 8;
                AI_ch3_TS = AI_ch3_TS | AI_ch3_TS_data[1];
  
                return 1;
            }

            case 0x04:
            {
		        memcpy(&AI_ch4_Bdata[0], &rx_pData[3], 2);
		        memcpy(&AI_ch4_TS_data[0], &rx_pData[1], 2);
                AI_data_ch4 = AI_ch4_Bdata[0] << 8;
                AI_data_ch4 = AI_data_ch4 | AI_ch4_Bdata[1];
                AI_ch4_TS = AI_ch4_TS_data[0] << 8;
                AI_ch4_TS = AI_ch4_TS | AI_ch4_TS_data[1];
                return 1;
            }

            case 0x05:
            {
		        memcpy(&AI_ch5_Bdata[0], &rx_pData[3], 2);
		        memcpy(&AI_ch5_TS_data[0], &rx_pData[1], 2);
                AI_data_ch5 = AI_ch5_Bdata[0] << 8;
                AI_data_ch5 = AI_data_ch5 | AI_ch5_Bdata[1];
                AI_ch5_TS = AI_ch5_TS_data[0] << 8;
                AI_ch5_TS = AI_ch5_TS | AI_ch5_TS_data[1];
                return 1;
            }

            case 0x06:
            {
		        memcpy(&AI_ch6_Bdata[0], &rx_pData[3], 2);
		        memcpy(&AI_ch6_TS_data[0], &rx_pData[1], 2);
                AI_data_ch6 = AI_ch6_Bdata[0] << 8;
                AI_data_ch6 = AI_data_ch6 | AI_ch6_Bdata[1];
                AI_ch6_TS = AI_ch6_TS_data[0] << 8;
                AI_ch6_TS = AI_ch6_TS | AI_ch6_TS_data[1];
                return 1;
            }

            case 0x07:
            {
		        memcpy(&AI_ch7_Bdata[0], &rx_pData[3], 2);
		        memcpy(&AI_ch7_TS_data[0], &rx_pData[1], 2);
                AI_data_ch7 = AI_ch7_Bdata[0] << 8;
                AI_data_ch7 = AI_data_ch7 | AI_ch7_Bdata[1];
                AI_ch7_TS = AI_ch7_TS_data[0] << 8;
                AI_ch7_TS = AI_ch7_TS | AI_ch7_TS_data[1];
                return 1;
            }
            default :
                break;
        }
        
        if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
            printf("AI Channel Number : %2X\n", rx_pData[0]);
	        printf("time stamp in hex: %X %X, value in hex: %X %X\n", 
                    rx_pData[1], rx_pData[2], rx_pData[3], rx_pData[4]);
        }

        if (rx_chanel_num == chanel_num)
        {
            return 1;
        }
        return -1;

}

int write_AI_Channel_Config(int fd, int IO_num, int channel_num, uint8_t setup_info, uint8_t range_info)
{
        int result, len, i;
        int rx_channel_num;
        uint8_t error_code;
        uint8_t tx_funCode;
        
        if (slot_info[IO_num]->board_type != 0x00)
        {
            printf("Executing Wrong Function Code : function - %s\n", __FUNCTION__);
            return -1;
        }

        tx_funCode = 0x3e;
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x03;
        TX_pBuffer[0x04] = channel_num;
        TX_pBuffer[0x05] = setup_info;
        TX_pBuffer[0x06] = range_info;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 7);
      
      	TX_pBuffer[0x07] =  result >> 8;
        TX_pBuffer[0x08] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    break;
                }

                /* Error Code Action  */
                if ((flag_err_code == 0x49) || (flag_err_code == 0x26))
                {
                    error_code = flag_err_code;
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    flag_err_code = 0;
                    flag_fun_code = 0;
                    result = 1;
                    break;
                }
                
                flag_err_code = 0;
                flag_fun_code = 0;
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 == 1)
                printf("\n Failed to execute the function code\n");
            return -1;
        }
        
        rx_channel_num = rx_pData[0];

        if (error_code)
        {
            result = error_code_action(fd, tx_funCode, error_code, IO_num, channel_num, 0);
        }

        if ((rx_channel_num == channel_num) && (result == 1))
        {
                printf("Successfull on IO card : %d && Channel : %d\n\n", IO_num, channel_num);
                return 1;
        }

        return -1;
}

int readAIFactoryCal(int fd, int IO_num, uint8_t input_pair, uint8_t gain)
{
        int result, len, i;
        uint8_t rx_chanel_num;
        
    /*    if (slot_info[IO_num]->board_type != 0x00)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }
*/
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = 0x37;
        TX_pBuffer[0x01] = ~(0x37);
        TX_pBuffer[0x02] = 0x00;
        TX_pBuffer[0x03] = 0x02;
        TX_pBuffer[0x04] = input_pair;
        TX_pBuffer[0x05] = gain;

        result = CRC16_BlockChecksum(&TX_pBuffer[0], 6);

        TX_pBuffer[0x06] =  result >> 8;
        TX_pBuffer[0x07] = result & 0xFF;


        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 = 1)
                printf("\nCan not execute the function code\n");
            return -1;
        }

        hex_dump(rx_pData, rx_header -> byte_low, 32, "Data : ");

        return -1;
}

int write_AI_Board_Config(int fd, int IO_num, uint8_t *setup_info, uint8_t *range_info)
{
        int result, len, i, j, k;
        int rx_channel_num;
       
        
        if (slot_info[IO_num]->board_type != 0x00)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }
        
        len = sizeof(Tx_pBuffer);
    
        for (i=0; i<len; i++)
        {
                Tx_pBuffer[i] = 0xFF;
        }

        Tx_pBuffer[0x00] = 0x40;
        Tx_pBuffer[0x01] = ~(0x40);
        Tx_pBuffer[0x02] = 0x00;  
        Tx_pBuffer[0x03] = 0x16;

        j = 4;
        k = 12;
        for (i = 0; i < 8; i++)
        {
            Tx_pBuffer[j] = setup_info[i];
            Tx_pBuffer[k] = range_info[i];
            j++;
            k++;
        }

		result = CRC16_BlockChecksum(&Tx_pBuffer[0], 19);
      
      	Tx_pBuffer[20] =  result >> 8;
        Tx_pBuffer[21] = result & 0xFF;

        printf("%02X  %02X\n", Tx_pBuffer[20], Tx_pBuffer[21]);

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = custom_execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 == 1)
                printf("\n Failed to execute the function code\n");
            return -1;
        }
        
        if (LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
        {
            printf("RX Header : %2X %2X %2X %2X\n", rx_header->fun_code, rx_header->error_code,
                    rx_header->byte_high, rx_header->byte_low);
            hex_dump(rx_pData, rx_header -> byte_low, 32, "Data : ");
        }

        if (result == 1)
        {
                printf("Successfull on IO card : %d\n", IO_num);
                return 1;
        }

        return -1;

}

int ack_AI_GoodBlock(int fd, int IO_num, uint8_t channel_num)
{
        int result, len, i;
        uint8_t tx_funCode;
        uint8_t rx_channel_num;
        uint8_t user_channel;

        tx_funCode = 0x34;
        
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        switch(channel_num)
        {
            case 0x00:
                user_channel = 0x01;
                break;
            case 0x01:
                user_channel = 0x02;
                break;
            case 0x02:
                user_channel = 0x04;
                break;
            case 0x04:
                user_channel = 0x08;
                break;
            case 0x08:
                user_channel = 0x10;
                break;
            case 0x10:
                user_channel = 0x20;
                break;
            case 0x20:
                user_channel = 0x40;
                break;
            case 0x40:
                user_channel = 0x80;
                break;
            default:
                break;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x01;
        TX_pBuffer[0x04] = user_channel;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 5);
      
      	TX_pBuffer[0x05] =  result >> 8;
        TX_pBuffer[0x06] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                flag_fun_code = 0;
                flag_err_code = 0;

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            printf("\n Failed to execute the function code\n");
            return -1;
        }

        rx_channel_num = rx_pData[0];

        if (rx_channel_num == channel_num)
        {
            return 1;
        }

    return -1;

}

uint8_t read_AI_BlockData_Partial(int fd, int IO_num, uint8_t channel_num)
{
        int result, len, i;
        uint8_t tx_funCode;
        uint8_t card_data[4];
        uint8_t error_code = 0;
        uint8_t user_channel;

        tx_funCode = 0x33;
        
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        switch(channel_num)
        {
            case 0x00:
                user_channel = 0x01;
                break;
            case 0x01:
                user_channel = 0x02;
                break;
            case 0x02:
                user_channel = 0x04;
                break;
            case 0x04:
                user_channel = 0x08;
                break;
            case 0x08:
                user_channel = 0x10;
                break;
            case 0x10:
                user_channel = 0x20;
                break;
            case 0x20:
                user_channel = 0x40;
                break;
            case 0x40:
                user_channel = 0x80;
                break;
            default:
                break;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x01;
        TX_pBuffer[0x04] = user_channel;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 5);
      
      	TX_pBuffer[0x05] =  result >> 8;
        TX_pBuffer[0x06] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));
                
                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    
                    break;
                }

                if ((flag_err_code == 0x25) || (flag_err_code == 0x26))
                {
                    error_code = flag_err_code;
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));
                    flag_err_code = 0;
                    flag_fun_code = 0;
                    result = 1;
                    break;
                }
                flag_fun_code = 0;
                flag_err_code = 0;
            }
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            printf("\n Failed to execute the function code\n");
            return -1;
        }

        memcpy(&card_data[0], &rx_pData[0], 4);

        if(verbose == 1)
        {
            hex_dump(&card_data, 4, 4, "Card Data : ");
            hex_dump(&rx_pData[4], 200, 32, "rx_pData : ");
        }
        if (result == 1)
        {
            return error_code;
        }

    return -1;
}

int write_DIO_Board_Config(int fd, int IO_num)
{
        int result, len, i;
        uint16_t rx_crc;
        uint8_t *dio_tx_buff;
        uint8_t tx_funCode;
       
       
        if (slot_info[IO_num]->board_type != BOARD_DIO)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        len = 24;
        dio_tx_buff = (uint8_t *)calloc(len , sizeof(uint8_t));
       // memset(dio_tx_buff, 0, sizeof(dio_tx_buff));
        if (dio_tx_buff == NULL)
        {
            printf("Memory Allocation Fail in %s()\n", __FUNCTION__);
            return -1;
        }

        tx_funCode = 0x6d;

        for (i=0; i<len; i++)
        {
                dio_tx_buff[i] = 0xFF;
        }

        dio_tx_buff[0x00] = tx_funCode;
        dio_tx_buff[0x01] = ~(tx_funCode);
        dio_tx_buff[0x02] = 0x00;  
        dio_tx_buff[0x03] = 0x0F;

        // IO digital enable mask
        dio_tx_buff[4] = 0xFF;
        dio_tx_buff[5] = 0xFF;

        // OUTPUT pulse mask
        dio_tx_buff[6] = 0x00;
        dio_tx_buff[7] = 0x00;

        // IO Selection mask
        dio_tx_buff[8] = 0xFF;
        dio_tx_buff[9] = 0xFF;
        // fial safe
        dio_tx_buff[10] = 0x00;
        dio_tx_buff[11] = 0x00;



        for (i = 12; i<=18; i++)
        {
            dio_tx_buff[i] = 0x05;
        }

		result = CRC16_BlockChecksum(&dio_tx_buff[0], 19);
      
      	dio_tx_buff[19] =  result >> 8;
        dio_tx_buff[20] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test_buffer(fd, dio_tx_buff, len);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));  
                    flag_err_code = 0;
                    flag_fun_code = 0; 
                    break;
                }
            }
        }

        if (flag_err_code != 0)
        {
           // result = error_code_action(fd, tx_funCode, flag_err_code, IO_num, NO_DATA_AVAIL, 0);
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 ==1 )
                printf("\n Failed to execute the function code\n");
            return -1;
        }
        
        if(LOG_LEVEL_1 == 3 || LOG_LEVEL_1 == 5)
        {
            hex_dump(&rx_pData, 6, 6, "Rx Data : ");
            
        }

        if (result == 1)
        {
            return 1;
        }
        free(dio_tx_buff);
        return -1;

}

int write_DIO_Pin_Output(int fd, int IO_num, uint8_t byte_04, uint8_t byte_05, uint8_t byte_06)
{
        int result, len;
        uint8_t i, tx_funCode;
       
        if (slot_info[IO_num]->board_type != BOARD_DIO)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        tx_funCode = 0x6b;
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x03;
        TX_pBuffer[0x04] = byte_04;
        TX_pBuffer[0x05] = byte_05;
        TX_pBuffer[0x06] = byte_06;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 7);
      
      	TX_pBuffer[0x07] =  result >> 8;
        TX_pBuffer[0x08] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));  
                    flag_err_code = 0;
                    flag_fun_code = 0; 
                    break;
                }
            }
        }

        if (flag_err_code != 0)
        {
           // result = error_code_action(fd, tx_funCode, flag_err_code, IO_num, NO_DATA_AVAIL, 0);
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 ==1 )
                printf("\n Failed to execute the function code\n");
            return -1;
        }
        
        if(LOG_LEVEL_1 == 3 || LOG_LEVEL_1 == 5)
        {
            hex_dump(&x_rx_buf, 6, 6, "Rx Data : ");
            
        }

        if (result == 1)
            return 1;
        
        return -1;

}

int read_DIO_Pin_Current_state(int fd, int IO_num)
{
        int result, len;
        uint16_t rx_crc;
        uint16_t num_of_data;
        uint8_t i, tx_funCode;
        uint16_t time_stamp;
        uint16_t data;
        uint16_t t_stamp;
        uint16_t r_data;
        uint16_t crc;
       
        if (slot_info[IO_num]->board_type != BOARD_DIO)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        tx_funCode = 0x6a;
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x00;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 4);
      
      	TX_pBuffer[0x04] =  result >> 8;
        TX_pBuffer[0x05] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));  
                    flag_err_code = 0;
                    flag_fun_code = 0; 
                    break;
                }
            }
        }

        if (flag_err_code != 0)
        {
           // result = error_code_action(fd, tx_funCode, flag_err_code, IO_num, NO_DATA_AVAIL, 0);
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 ==1 )
                printf("\n Failed to execute the function code\n");
            return -1;
        }
        
        time_stamp = rx_pData[0];
        time_stamp = time_stamp << 8;
        time_stamp = time_stamp | rx_pData[1];

        data = rx_pData[2];
        data = data << 8;
        data = data | rx_pData[3];

        memcpy(&crc, &rx_pData[4], 2);
        
        if(LOG_LEVEL_1 == 3 || LOG_LEVEL_1 == 5)
        {
            hex_dump(&rx_pData, 6, 6, "Rx Data : ");
            printf("Time stamp : %0.2X \t Data : %0.2X\n", time_stamp, data);
            printf("CRC : %0.2X\n", crc);
            
        }

        if (result == 1)
            return 1;
        
        return -1;
}

int read_DIO_AllInput_stored(int fd, int IO_num)
{
        int result, len;
        uint16_t rx_crc;
        uint16_t num_of_data;
        uint8_t i, tx_funCode;
        uint16_t time_stamp;
        uint16_t data;
        uint16_t t_stamp;
        uint16_t r_data;
        uint16_t crc;
       
        if (slot_info[IO_num]->board_type != BOARD_DIO)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        tx_funCode = 0x68;
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x00;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 4);
      
      	TX_pBuffer[0x04] =  result >> 8;
        TX_pBuffer[0x05] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));  
                    flag_err_code = 0;
                    flag_fun_code = 0; 
                    break;
                }
            }
        }

        if (flag_err_code != 0)
        {
           // result = error_code_action(fd, tx_funCode, flag_err_code, IO_num, NO_DATA_AVAIL, 0);
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 ==1 )
                printf("\n Failed to execute the function code\n");
            return -1;
        }

        num_of_data = rx_header->byte_high;
        num_of_data = num_of_data << 8;
        num_of_data = num_of_data | rx_header->byte_low;

        printf("\n");
        for (i = 0; i < num_of_data; i=i+4)
        {
            time_stamp = rx_pData[i];
            time_stamp = time_stamp << 8;
            time_stamp = time_stamp | rx_pData[i+1];

            data = rx_pData[i+2];
            data = data << 8;
            data = data | rx_pData[i+3];

            printf("Time stamp : %0.2X \t Data : %0.2X\n", time_stamp, data);

        }

        memcpy(&crc, &rx_pData[num_of_data], 2);
       

        
        if(LOG_LEVEL_1 == 3 || LOG_LEVEL_1 == 5)
        {
            printf("CRC : %0.2X\n", crc);
            hex_dump(&rx_pData, num_of_data+2, 32, "Rx Data : ");
            
        }

        if (result == 1)
            return 1;
        
        return -1;
}

int read_DIO_Board_Config(int fd, int IO_num)
{
        int result, len, i;
        uint16_t version;
        uint8_t DigIOsetup[40];
        uint8_t PulseINsetup[5];
        uint8_t spare[3];
        uint16_t rx_crc;
        uint8_t tx_funCode;
       
        if (slot_info[IO_num]->board_type != BOARD_DIO)
        {
            printf("Executing Wrong Function Code : function() - %s\n", __FUNCTION__);
            return -1;
        }

        tx_funCode = 0x6e;
        len = sizeof(TX_pBuffer);

        for (i=0; i<len; i++)
        {
                TX_pBuffer[i] = 0xFF;
        }

        TX_pBuffer[0x00] = tx_funCode;
        TX_pBuffer[0x01] = ~(tx_funCode);
        TX_pBuffer[0x02] = 0x00;  
        TX_pBuffer[0x03] = 0x00;

		result = CRC16_BlockChecksum(&TX_pBuffer[0], 4);
      
      	TX_pBuffer[0x04] =  result >> 8;
        TX_pBuffer[0x05] = result & 0xFF;

        IOCard_Selection(IO_num);

        for (i = 0; i < 3; i++)
        {
            result = execute_Test(fd);
            if (result == 1)
            {
                result = get_Rx_Header(TX_pBuffer[0], x_rx_buf, sizeof(x_rx_buf));

                if (result == 1)
                {
                    result = get_Data_from_Rx(x_rx_buf, sizeof(x_rx_buf));  
                    flag_err_code = 0;
                    flag_fun_code = 0; 
                    break;
                }
            }
        }

        if (flag_err_code != 0)
        {
           // result = error_code_action(fd, tx_funCode, flag_err_code, IO_num, NO_DATA_AVAIL, 0);
        }

        if (result < 0)
        {
            flag_fun_code = 0;
            flag_err_code = 0;
            if (LOG_LEVEL_2 ==1 )
                printf("\n Failed to execute the function code\n");
            return -1;
        }
        
        memcpy(&version, &rx_pData[0], 2);
        memcpy(&DigIOsetup[0], &rx_pData[2], 41);
        memcpy(&PulseINsetup[0], &rx_pData[42], 5);
        memcpy(&spare[0], &rx_pData[47], 3);
        memcpy(&rx_crc, &rx_pData[50], 2);
        
        if(LOG_LEVEL_1 == 3 || LOG_LEVEL_1 == 5)
        {
            printf("\n\n");
            hex_dump(&version, 2, 2, "Version : ");
            hex_dump(DigIOsetup, 40, 32, "Digital IO Setup : ");
            hex_dump(PulseINsetup, 5, 5, "Pulse Input setup : ");
            hex_dump(spare, 3, 3, "Spare : ");
            hex_dump(&rx_crc, 2, 2, "Rx Data CRC : ");
        }

        if (result == 1)
            return 1;
        
        return -1;
}

int SlotDetect(int i)
{

    switch (i)
    {
    case 0:
        detect_slot->slot_A = SLOT_A;
        slot_name->slot_A = 'A';
        break;
    case 1:
        detect_slot->slot_B = SLOT_B;
        slot_name->slot_B = 'B';
        break;
    case 2:
        detect_slot->slot_C = SLOT_C;
        slot_name->slot_C = 'C';
        break;
    case 3:
        detect_slot->slot_D = SLOT_D;
        slot_name->slot_D = 'D';
        break;
    case 4:
        detect_slot->slot_E = SLOT_E;
        slot_name->slot_E = 'E';
        break;
    case 5:
        detect_slot->slot_F = SLOT_F;
        slot_name->slot_F = 'F';
        break;
    case 6:
        detect_slot->slot_G = SLOT_G;
        slot_name->slot_G = 'G';
        break;
    case 7:
        detect_slot->slot_H = SLOT_H;
        slot_name->slot_H = 'H';
        break;
    case 8:
        detect_slot->slot_I = SLOT_I;
        slot_name->slot_I = 'I';
        break;
    
    default:
        break;
    }

    return 1;
}

int DetectSlot(int fd)
{
    int result, i = 0;
    uint8_t b_type = 0xFF;
    uint8_t b_ID;
    uint8_t channel_present;
    int slot_num;

    detect_slot = (Slot_Detect *)malloc(sizeof(Slot_Detect));
   
    if (detect_slot == NULL)
    {
        printf("Memory Allocation Failure -- in fun %s()\n", __FUNCTION__);
        exit(0);
    }
    memset(detect_slot, 0, sizeof(detect_slot));

    slot_name = (Slot_Name *)malloc(sizeof(Slot_Name));
   
    if (slot_name == NULL)
    {
        printf("Memory Allocation Failure for Slot_Name-- in fun %s()\n", __FUNCTION__);
        exit(0);
    }
    memset(slot_name, 0, sizeof(slot_name));

    for (int i = 0; i < 9; i++)
    {
        slot_info[i] = (CARD_SLOT *)malloc(sizeof(CARD_SLOT));
        if (slot_info[i] == NULL)
        {
            printf("Memory Allocation Failure -- in fun %s()\n", __FUNCTION__);
            exit(0);
        }
        memset(slot_info[i], 0, sizeof(slot_info[i]));

        slot_info[i]->board_data = (BOARD_DATA *)malloc(sizeof(BOARD_DATA));
        if (slot_info[i]->board_data == NULL)
        {
            printf("Memory Allocation Failure -- in fun %s()\n", __FUNCTION__);
            exit(0);
        }
        memset(slot_info[i]->board_data, 0, sizeof(slot_info[i]->board_data));
        
        slot_info[i]->board_data->bd_capability = (Board_Capability *)malloc(sizeof(Board_Capability));
        
        if (slot_info[i]->board_data->bd_capability == NULL)
        {
            printf("Memory Allocation Failure -- in fun %s()\n", __FUNCTION__);
            exit(0);
        }
        memset(slot_info[i]->board_data->bd_capability, 0, sizeof(slot_info[i]->board_data->bd_capability));

    }
    
    for (i = 0; i < 9; i++)
    {
        result = IO_CardTest_CTAB(fd, i, GET_ERR_CODE);
       
        slot_info[i]->slot_number = 15;
        slot_info[i]->board_type = b_type;
        result = IO_CardTest_CTAB(fd, i, GET_BOARD_TYPE);
        if (result == 1)
        {
            b_type = rx_pData[0];
            channel_present = rx_pData[1];

            slot_info[i]->slot_number = i;
            slot_info[i]->board_type = b_type;
            slot_info[i]->phy_num_channel = channel_present;

            result = SlotDetect(i);

            result = IO_CardTest_CTAB(fd, i, GET_BOARD_ID);
            if (result == 1)
            {
                b_ID = rx_pData[5];
                slot_info[i]->board_data->board_reg_id = b_ID;
            }
            else{
                printf("Failed to Run GetBoardID -- in fun::%s()\n", __FUNCTION__);
            }

            result = IO_CardTest_CTAB(fd, i, GET_VERSION);

            if(result == 1)
            {
                memcpy(&(slot_info[i]->board_data->get_version), &rx_pData[0], 6);
            }
            else{
                printf("Failed to Run GetBoardVersion -- in fun::%s()\n", __FUNCTION__);
            }

            result = IO_CardTest_CTAB(fd, i, RD_BD_CAPABILITY);
            
            if(result == 1)
            {
                slot_info[i]->board_data->bd_capability->b_type = rx_pData[0];
                slot_info[i]->board_data->bd_capability->b_revision = rx_pData[1];
                slot_info[i]->board_data->bd_capability->ch_available = rx_pData[2];
                memcpy(&(slot_info[i]->board_data->bd_capability->ch_enable), &rx_pData[3], 2);

            }
            else{
                printf("Failed to Run GetBoardCapability -- in fun::%s()\n", __FUNCTION__);
            }
            
        }
        b_type = 0xff;

    }

    if (LOG_LEVEL_1 == 3)
    {
        printf("\n\n");
        printf("Detected Slots and Card are : \n\n");

        for (i = 0; i<9; i++)
        {
            slot_num = slot_info[i]->slot_number;
            if (slot_num >= 0 && slot_num <= 8)
            {
                printf("%d.slot %c -- ", i, i+65);
                switch (slot_info[i]->board_type)
                {
                case 0x00:
                    printf("Detected : Analogue Input (AI)\n");
                    break;
                case 0x01:
                    printf("Detected : Alarm & Relay (AR)\n");
                    break;
                case 0x02:
                    printf("Detected : Digital I/O (DO)\n");
                    break;
                case 0x03:
                    printf("Detected : Analogue OUTPUT (AO)\n");
                    break;
                case 0x04:
                    printf("Detected : Pulse Input (PI)\n");
                    break;
                default:
                    break;
                }
            }
        }
        printf("\n\n");
    }

    return 1;
}

int spi_gpio_init() // qt_IO_Card_Test()
{
	int ret = 0, result , i = 0, k = 0;
    char fileName[20];
    char cmd[32];
	
    /****************  SPI communication process ********************/
	
	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	mode = SET_MODE;
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
	

	/* Enable Verbose */

	verbose = 1;
    LOG_LEVEL_2 = 1;
    LOG_LEVEL_1 = 1;

	return ret;
}

int cleanup (int fd)
{
    int i = 0;
    int slot_num;
   
    for (i = 0; i < 9; i++)
    {
        slot_num = slot_info[i]->slot_number;
        if (slot_num >= 0 && slot_num <= 8)
        {
            free(slot_info[i]->board_data->bd_capability);
            free(slot_info[i]->board_data);
            free(slot_info[i]);
        }
    }

    if (detect_slot)
        free(detect_slot);
    if (slot_name);
        free(slot_name);
    
    close(fd);
    free(rx_header);
    close(sig_fd);   

}

int GetAICardData(int fd, int slot_num, AI_Card_Info *AICardinfo)
{
    int result; 

    result = read_AI_Board_Config(fd, slot_num, AICardinfo);

    return 0;

}

int GetAIChannelData(int fd, int slot_num, int channel_num, AI_Card_Info *AICardinfo, AI_Channel_info *AIChannelinfo)
{
    uint8_t chk_acq = 0x1C, temp;


    AIChannelinfo->enable = AICardinfo->setupInfo[channel_num] & (1 << 0);
    AIChannelinfo->raw_reading = AICardinfo->setupInfo[channel_num] & (1 << 1);

    temp = AICardinfo->setupInfo[channel_num] & chk_acq;
    temp >>= 2;
    AIChannelinfo->ack_frq =  temp;

    temp = AICardinfo->setupInfo[channel_num] & 0x20;
    temp >>= 5;
    AIChannelinfo->field_cal = temp;

    temp = AICardinfo->setupInfo[channel_num] & 0xC0;
    temp >>= 6;
    AIChannelinfo->active_burnout = temp;

    if (LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
    {
        printf("Enable :: %X\n", AIChannelinfo->enable);
        printf("Raw_Read :: %X\n", AIChannelinfo->raw_reading);
        printf("ACQ Frq:: %X\n", AIChannelinfo->ack_frq);
        printf("Field Calib:: %X\n", AIChannelinfo->field_cal);
        printf("\n");
        
    }

    return 1;
}

int execute_AI_Test(int fd, int slot_num, uint8_t ch_num, int Acq_rate)
{
    int result, user_input;
    int timer_value = 0;
    int ch_freq_count, count = 0;
    uint64_t n_time;

    user_input = Acq_rate;

    if (user_input == 1)
        {
       // result = read_AI_Board_Config(fd, slot_num);

        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_ZERO, 0x11, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_ONE, 0x11, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_TWO, 0x11, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_THREE, 0x11, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_FOUR, 0x11, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_FIVE, 0x11, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_SIX, 0x11, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_SEVEN, 0x11, 0x25); 
        result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
        //result = read_AI_Board_Config(fd, slot_num);
        result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
        timer_value = 20 * 1000;
        ch_freq_count = 1;
        printf("\n\nwait for 10 sec\n");
        sleep(10);
        }
        else if (user_input == 2)
        {
       // result = read_AI_Board_Config(fd, slot_num);

        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_ZERO, 0x0D, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_ONE, 0x0D, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_TWO, 0x0D, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_THREE, 0x0D, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_FOUR, 0x0D, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_FIVE, 0x0D, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_SIX, 0x0D, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_SEVEN, 0x0D, 0x25); 
        result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
        //result = read_AI_Board_Config(fd, slot_num);
        result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
        timer_value = 100 * 100;
        printf("\n\nwait for 10 sec\n");
        ch_freq_count = 2;
        sleep(10);

        }
        else if (user_input == 3)
        {
        //result = read_AI_Board_Config(fd, slot_num);

        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_ZERO, 0x09, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_ONE, 0x09, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_TWO, 0x09, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_THREE, 0x09, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_FOUR, 0x09, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_FIVE, 0x09, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_SIX, 0x09, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_SEVEN, 0x09, 0x25); 
        result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
       // result = read_AI_Board_Config(fd, slot_num);
        result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
        timer_value = 200 *1000;
        ch_freq_count = 4;
        printf("\n\nwait for 10 sec\n");
        sleep(10);

        }
        else if (user_input == 4)
        {
        //result = read_AI_Board_Config(fd, slot_num);

        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_ZERO, 0x05, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_ONE, 0x05, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_TWO, 0x05, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_THREE, 0x05, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_FOUR, 0x05, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_FIVE, 0x05, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_SIX, 0x05, 0x25); // 25 57
        result = write_AI_Channel_Config(fd, slot_num, CH_NUM_SEVEN, 0x05, 0x25); 
        result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
        //result = read_AI_Board_Config(fd, slot_num);
        result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
        timer_value = 500 * 1000;
        ch_freq_count = 6;
        printf("\n\nwait for 10 sec\n");
        sleep(10);
        }

    for (int i = 1; i >= 1; i++)
    {
        
        result = reset_AI_Global_Data();

        if (count >= ch_freq_count){
            result = reset_CircularBuff(fd, slot_num, 0);
            count = 0;
        }
     
        clock_gettime(CLOCK_MONOTONIC_RAW, &x_start);
        usleep(timer_value);
        result = read_AI_Analogue_block(fd, slot_num, ch_num);
        clock_gettime(CLOCK_MONOTONIC_RAW, &x_end);
        
        n_time = (x_end.tv_sec - x_start.tv_sec) * 1000000 + (x_end.tv_nsec - x_start.tv_nsec) / 1000;
        //result = read_AI_Channel_Input(fd, IO_CARD_NINE, CH_NUM_ZERO);
        //result = read_AI_Channel_Input(fd, IO_CARD_NINE, CH_NUM_ONE);
        delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;

        printf("%lld\t%0.2X\t %lld\t%0.2X\t %lld\t%0.2X\t %lld\t%0.2X\t %lld\t %lld\t %lld\t %lld\n", 
        AI_data_ch0, AI_ch0_TS, AI_data_ch1, AI_ch1_TS, AI_data_ch2, AI_ch2_TS, AI_data_ch3, AI_ch3_TS, AI_data_ch4, AI_data_ch5, AI_data_ch6, AI_data_ch7);
        printf(" spi w/r: %lld\n", delta_us);
        printf(" acq time:  %lld\n", n_time);

        time_ms = 0;
        count++;
       
    }
return 1;

}

int readAIData(uint8_t ch_num, uint8_t *data_ptr, int d_acq_value)
{
    uint8_t card_data[4];
    uint8_t data_read[255] = {0,};
    uint8_t *ptr_rx_pData = NULL;
    uint8_t CH_NUM;
    uint8_t tmp_ch_num;
    int len, i, j, ch_count = 0, num_data, k = 0;

    uint8_t time_stamp[2] = {0,};

    tmp_ch_num = ch_num;

    /*  Counting the number of Channel */
    while (tmp_ch_num)
    {
        ch_count += tmp_ch_num & 1;
        tmp_ch_num >>= 1 ;
    }

/*  Validating the channel sent data*/
    memcpy(&card_data[0], &rx_pData[0], 4);

    if (card_data[3] == 0)
    {
        
        goto end;         // need to modified according to the error code
    }
       
    /*   Filtering the data from received Buffer */

    memset(data_read, 0, sizeof(data_read));
    ptr_rx_pData = &rx_pData[4];

    for (i = 0; i < ch_count; i++)
        {
            CH_NUM = *ptr_rx_pData;
            
            //printf("ch num %0.2X ponter data %0.2X\n",CH_NUM, *ptr_rx_pData);
            
            if (CH_NUM > 7)
            {
                goto end;
            }
            
            ptr_rx_pData++;
            memcpy(&time_stamp[0], ptr_rx_pData, 2);
            ptr_rx_pData++;
            ptr_rx_pData++;
            num_of_data = *ptr_rx_pData;
            num_data = num_of_data * 2;
            ptr_rx_pData++;
            j = 0;
            for (k = 0; k < num_data; k++)
            {
                data_read[k] = *ptr_rx_pData;
                ptr_rx_pData++;
                j++;
            }

            switch (CH_NUM)
            {
            case 0:
                if ((d_acq_value % counter[CH_NUM]) == 0){
                memcpy(&AI_ch0_Bdata, &data_read[num_data - 2], 2);
		        memcpy(&AI_ch0_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch0, &AI_ch0_Bdata[0], 2);
                /*AI_data_ch0 = AI_ch0_Bdata[0] << 8;
                AI_data_ch0 = AI_data_ch0 | AI_ch0_Bdata[1];*/
                AI_ch0_TS = AI_ch0_TS_data[0] << 8;
                AI_ch0_TS = AI_ch0_TS | AI_ch0_TS_data[1];
                }
                break;
            case 1:
                if ((d_acq_value % counter[CH_NUM]) == 0){
                memcpy(&AI_ch1_Bdata[0], &data_read[num_data - 2], 2);
		        memcpy(&AI_ch1_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch1, &AI_ch1_Bdata[0], 2);
                /*AI_data_ch1 = AI_ch1_Bdata[0] << 8;
                AI_data_ch1 = AI_data_ch1 | AI_ch1_Bdata[1];*/
                AI_ch1_TS = AI_ch1_TS_data[0] << 8;
                AI_ch1_TS = AI_ch1_TS | AI_ch1_TS_data[1];
                }
                break;
            case 2:
                if ((d_acq_value % counter[CH_NUM]) == 0){
                memcpy(&AI_ch2_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch2_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch2, &AI_ch2_Bdata[0], 2);
                /*AI_data_ch2 = AI_ch2_Bdata[0] << 8;
                AI_data_ch2 = AI_data_ch2 | AI_ch2_Bdata[1];*/
                AI_ch2_TS = AI_ch2_TS_data[0] << 8;
                AI_ch2_TS = AI_ch2_TS | AI_ch2_TS_data[1];
                }
                break;
            case 3:
                if ((d_acq_value % counter[CH_NUM]) == 0){
                memcpy(&AI_ch3_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch3_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch3, &AI_ch3_Bdata[0], 2);
                /*AI_data_ch3 = AI_ch3_Bdata[0] << 8;
                AI_data_ch3 = AI_data_ch3 | AI_ch3_Bdata[1];*/
                AI_ch3_TS = AI_ch3_TS_data[0] << 8;
                AI_ch3_TS = AI_ch3_TS | AI_ch3_TS_data[1];
                }
                break;
            case 4:
                if ((d_acq_value % counter[CH_NUM]) == 0){
                memcpy(&AI_ch4_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch4_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch4, &AI_ch4_Bdata[0], 2);
                /*AI_data_ch4 = AI_ch4_Bdata[0] << 8;
                AI_data_ch4 = AI_data_ch4 | AI_ch4_Bdata[1];*/
                AI_ch4_TS = AI_ch4_TS_data[0] << 8;
                AI_ch4_TS = AI_ch4_TS | AI_ch4_TS_data[1];
                }
                break;
            case 5:
                if ((d_acq_value % counter[CH_NUM]) == 0){
                memcpy(&AI_ch5_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch5_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch5, &AI_ch5_Bdata[0], 2);
                /*AI_data_ch5 = AI_ch5_Bdata[0] << 8;
                AI_data_ch5 = AI_data_ch5 | AI_ch5_Bdata[1];*/
                AI_ch5_TS = AI_ch5_TS_data[0] << 8;
                AI_ch5_TS = AI_ch5_TS | AI_ch5_TS_data[1];
                }
                break;
            case 6:
                if ((d_acq_value % counter[CH_NUM]) == 0){
                memcpy(&AI_ch6_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch6_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch6, &AI_ch6_Bdata[0], 2);
                /*AI_data_ch6 = AI_ch6_Bdata[0] << 8;
                AI_data_ch6 = AI_data_ch6 | AI_ch6_Bdata[1];*/
                AI_ch6_TS = AI_ch6_TS_data[0] << 8;
                AI_ch6_TS = AI_ch6_TS | AI_ch6_TS_data[1];
                }
                break;
            case 7:
                if ((d_acq_value % counter[CH_NUM]) == 0){
                memcpy(&AI_ch7_Bdata[0], &data_read[num_data - 2], 2);
                memcpy(&AI_ch7_TS_data[0], &time_stamp[0], 2);
                memcpy(&AI_data_ch7, &AI_ch7_Bdata[0], 2);
                /*AI_data_ch7 = AI_ch7_Bdata[0] << 8;
                AI_data_ch7 = AI_data_ch7 | AI_ch7_Bdata[1];*/
                AI_ch7_TS = AI_ch7_TS_data[0] << 8;
                AI_ch7_TS = AI_ch7_TS | AI_ch7_TS_data[1];
                }
                break;
            default:
                break;
            }

        /*  If selectd  LOG LEVEL to print data    */
            if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
            {
                printf("\n\n");
                hex_dump(&card_data, 4, 4, "Card Data : ");
                printf("Channel Number : %d\n", CH_NUM);
                printf("Number of Data : %d\n", num_data);
                hex_dump(&data_read, num_data, 32, "Data Read : ");
                printf("\n\n");
            }
        }

end:
    if (!(ptr_rx_pData == NULL))
            //free(ptr_rx_pData);
    if(LOG_LEVEL_1 == 1 || LOG_LEVEL_1 == 5)
    {
        hex_dump(&rx_pData[4], 96, 32, "rx_pData : ");
        //printf("Successfull on IO card : %d\n", IO_num);
        return 1;
    }

    return -1;

}

int GetPlottingCounter(AI_Channel_info *AIChannelinfo, int min_frq, int max_frq, int *counter, long *delay)
{
    int max_acq, min_acq;

    switch(max_frq)
    {
        case 1:
            max_acq = 500;
            *delay = 500;
            break;
        case 2:
            max_acq = 200;
            *delay = 200;
            break;
        case 3:
            max_acq = 100;
            *delay = 100;
            break;
        case 4:
            max_acq = 20;
            *delay = 20;
            break;
        default:
            break;
    }

    switch(min_frq)
    {
        case 1:
            min_acq = 500;
            
            break;
        case 2:
            min_acq = 200;
            
            break;
        case 3:
            min_acq = 100;
           
            break;
        case 4:
            min_acq = 20;
            
            break;
        default:
            break;
    }



//printf("max_frq%d\n", max_frq);
//printf("%X\n", AIChannelinfo->ack_frq);
    switch (AIChannelinfo->ack_frq)
    {
        case 1:
            *counter = 500/max_acq;
            break;
        case 2:
            *counter = 200/max_acq;
            break;
        case 3:
            *counter = 100/max_acq;
            break;
        case 4:
            *counter = 20/max_acq;
            break;
        default:
            break;

    }

}

int test_select_option(int fd)
{
    int result;
    int user_input, log, count;
    int ch_freq_count;
    uint8_t ch_num, num_of_ch;
    int operation;
    int slot_num;
    uint64_t n_time;
    uint8_t DI_ch_value;
    int slot_AI;
    uint8_t gain_info;
    uint8_t pair_info;

    AI_Card_Info *AICardinfo;
    AI_Channel_info *AIChannelinfo[8];
    uint8_t *data_ptr, error_code;
    uint8_t card_data[4];
    uint8_t data_read[255] = {0,};
    uint8_t *ptr_rx_pData = NULL;
    uint8_t CH_NUM;
    uint8_t tmp_ch_num;
    int len, i, j, ch_count = 0, num_data, k = 0;
    uint8_t ix = 0x00, temp;
    
    int max_frq, min_frq;
    int s_rate[8] = {0,}, temp_s_rate[8] = {0,};
    int channels, c_num;
    long delay;
    int chavail_num;


    printf("\n\n");
    printf("Select the Individual Operation \n\n");

    printf("1. Scan the Card and Slot\n");
    printf("2. Get the Card and Channel Details\n");
    printf("3. Select the Slot for Scanning\n");
    printf("4. Start scaning the slot by default setting\n");
    printf("5. Read Factory Calibration for AI Card\n\n");
    
    printf("Enter the Operation from above :: ");
    scanf(" %d", &operation);

    if (operation == 1)
    {
        LOG_LEVEL_1 = 3;
        LOG_LEVEL_2 = 0;
        verbose = 0;
        result = DetectSlot(fd);
        return 1;
    }

    if (operation == 2)
    {
        LOG_LEVEL_1 = 3;
        LOG_LEVEL_2 = 0;
        verbose = 0;
       
        result = DetectSlot(fd);

        printf("Select the Card from the given slot number ::\n\n");
        printf("Enter the Slot number :: ");
        scanf(" %d", &slot_num);
        verbose = 0;
        LOG_LEVEL_2 = 0;
        LOG_LEVEL_1 = 2;
        result = IO_CardTest_CTAB(fd, slot_num, 0x15);

    }

    if (operation == 3)
    {
        LOG_LEVEL_1 = 3;
        LOG_LEVEL_2 = 0;
        verbose = 0;
        result = DetectSlot(fd);
        printf("Select the Slot Number From Above Menu\n\n");
        printf("Enter the Slot number :: ");
        scanf(" %d", &slot_num);

        if (slot_info[slot_num]->board_type == BOARD_AI)
        {
            chavail_num = slot_info[slot_num]->board_data->bd_capability->ch_available;
            for(i = 0; i < chavail_num; i++)
            {
                result = write_AI_Channel_Config(fd, slot_num, i, 0x05, 0x25); // 25 57
            }
            
            printf ("Select the Number of Channel to Scan ::: \nAvailable Number of channels are ::: %d\n",
                    slot_info[slot_num]->board_data->bd_capability->ch_available);
           
            printf("Enter Number of Channel want to plot :: ");
            scanf(" %d", &channels);
            printf("Give channel Number '0' -- '7' and Frequency\n ");
            printf("1. 20 ms\n 2. 100 ms\n 3. 200ms \n 4. 500ms\n");

            for (i = 0; i<channels; i++)
            {
                printf("Enter Channel Number :: ");
                scanf(" %d", &c_num);

                temp = 1 << c_num;
                ch_num = ch_num | temp;

                printf("Input freq : ");
                scanf(" %d", &s_rate[c_num]);
                printf("\n");
                temp = 0x00;
            }
printf("ch_num Val %X\n", ch_num);
            printf("\nlog level  1|0 : ");
            scanf(" %d", &log);

            printf("Configuring the Frquency wait.........\n");
            LOG_LEVEL_1 = 1;
            LOG_LEVEL_2 = 1;
            verbose = 1;
            for (i = 0; i < 8; i++)
            {
                if (s_rate[i] > 0)
                {
                    switch (s_rate[i])
                    {
                        case 1:
                            result = write_AI_Channel_Config(fd, slot_num, i, 0x11, 0x25); // 25 57
                            result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
                            break;
                        case 2:
                            result = write_AI_Channel_Config(fd, slot_num, i, 0x0D, 0x25); // 25 57
                            result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
                            break;
                        case 3:
                            result = write_AI_Channel_Config(fd, slot_num, i, 0x09, 0x25); // 25 57
                            result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
                            break;
                        case 4:
                            result = write_AI_Channel_Config(fd, slot_num, i, 0x05, 0x25); // 25 57
                            result = IO_CardTest_CTAB(fd, slot_num, 0x0a);
                            break;
                        default:
                            break;

                    }
                }
                    
            }

            num_of_ch = slot_info[slot_num]->board_data->bd_capability->ch_available;

            AICardinfo = (AI_Card_Info *)malloc(sizeof(AI_Card_Info));
            if (AICardinfo == NULL)
            {
                printf("Memory Allocation Failure -- in fun %s()\n", __FUNCTION__);
                exit(0);
            }
            memset(AICardinfo, 0, sizeof(AICardinfo));

          
            result = GetAICardData(fd, slot_num, AICardinfo);
            LOG_LEVEL_1 == 1;
           
            for (int i = 0; i < chavail_num ; i++)
            {
                AIChannelinfo[i] = (AI_Channel_info *)malloc(sizeof(AI_Channel_info));
                if (AIChannelinfo[i] == NULL)
                {
                printf("Memory Allocation Failure -- in fun %s()\n", __FUNCTION__);
                exit(0);
                }
                memset(AIChannelinfo[i], 0, sizeof(AIChannelinfo[i]));
                result = GetAIChannelData(fd, slot_num, i, AICardinfo, AIChannelinfo[i]);
            }

            min_frq=max_frq=AIChannelinfo[0]->ack_frq;
            for(i=1; i < chavail_num; i++)
            {
                if(min_frq > AIChannelinfo[i]->ack_frq)
		            min_frq=AIChannelinfo[i]->ack_frq;   
		        if(max_frq < AIChannelinfo[i]->ack_frq)
		            max_frq=AIChannelinfo[i]->ack_frq;       
            }
            printf("min %d, max %d\n", min_frq, max_frq);

            for (i = 0; i < chavail_num; i++)
            {
                result = GetPlottingCounter(AIChannelinfo[i], min_frq, max_frq, &counter[i], &delay);
                printf("%d\t",counter[i]);
            }
            printf("\n");
            printf("delay is %ld\n", delay);
        

            // this will filter the highest frequency for creating delay
            min_frq=max_frq=counter[0];
            for(i=1; i<8; i++)
            {
                if(min_frq > counter[i])
		            min_frq=counter[i];   
		        if(max_frq < counter[i])
		            max_frq=counter[i];       
            }

            if (log == 1)
            {
                verbose = 1;
                LOG_LEVEL_2 = 1;
                LOG_LEVEL_1 = 1;
            }
            else if(log == 0)
            {
                verbose = 0;
                LOG_LEVEL_2 = 0;
                LOG_LEVEL_1 = 0;
            }

            //result = execute_AI_Test(fd, slot_num, ch_num, user_input);

            data_ptr = (uint8_t *)malloc(256*sizeof(uint8_t));
            if ( data_ptr == NULL)
            {
                printf("Memory Allocation Failure -- in fun %s()\n", __FUNCTION__);
                exit(0);
            }
            memset( data_ptr, 0, sizeof(uint8_t));

            /*  Counting the number of Channel */
            tmp_ch_num = ch_num;
            while (tmp_ch_num)
            {
                ch_count += tmp_ch_num & 1;
                tmp_ch_num >>= 1 ;
            }
            
            delay = delay * 1000;
            ch_freq_count = 10;
            printf("ch frq count %d\n",ch_freq_count);
            count = 0;
            i = 1;
            
            printf("Wait for 5 Sec\n");
            sleep(5);
            result = reset_CircularBuff(fd, slot_num, 0);
            //usleep(1);
            while (1)
            {
                
                count++;

                result = reset_AI_Global_Data();

                if (count >= ch_freq_count){
                result = reset_CircularBuff(fd, slot_num, 0);
                usleep(delay);
                count = 0;
                }
                error_code = 0x00;
                result =  ReadAIAnalogueBlock(fd, slot_num, ch_num, data_ptr, &error_code);
               
                d_acq_value++;
                if (error_code == 0x00)
                {
                    result = readAIData(ch_num, data_ptr, d_acq_value);
                }

                printf("G_counter : %d  %lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\n", d_acq_value, AI_data_ch0, AI_data_ch1, AI_data_ch2,AI_data_ch3,
                        AI_data_ch4, AI_data_ch5, AI_data_ch6,AI_data_ch7);
                
                if (d_acq_value >= 5000)
                {
                    d_acq_value = 0;
                    
                } 
                printf("\n\n");
                usleep(delay);

         /*       else if (error_code == 0x25)
                {
                    memcpy(&card_data[0], &rx_pData[0], 4);
                    if (card_data[3] == 0)
                    {
                        tmp_ch_num = ch_num;
                        
                        while (tmp_ch_num)
                        {
                            ch_count += tmp_ch_num & 1;
                            tmp_ch_num >>= 1 ;
                        }
                        for (i = 0; i < ch_count; i++)
                        {
                            result = ack_AI_GoodBlock(fd, slot_num, 0x01);
                            result = ReadAIAnalogueBlock(fd, slot_num, 0x01, data_ptr, &error_code);
                                 // need to modified according to the error code
                        }
                    }
                    
                    memset(data_read, 0, sizeof(data_read));
                    ptr_rx_pData = &rx_pData[4];

                    for (ix = 0; ix < card_data[3]; ix++)
                    {
                        CH_NUM = *ptr_rx_pData;
            
                    //printf("ch num %0.2X ponter data %0.2X\n",CH_NUM, *ptr_rx_pData);
            
                        if (CH_NUM > 7)
                        {
                            goto print_end;
                        }
            
                        ptr_rx_pData++;
                        memcpy(&time_stamp[0], ptr_rx_pData, 2);
                        ptr_rx_pData++;
                        ptr_rx_pData++;
                        num_of_data = *ptr_rx_pData;
                        num_data = num_of_data * 2;
                        ptr_rx_pData++;
                        j = 0;
                        for (k = 0; k < num_data; k++)
                        {
                            data_read[k] = *ptr_rx_pData;
                            ptr_rx_pData++;
                            j++;
                        }

                    }

                    result = ack_AI_GoodBlock(fd, slot_num, CH_NUM);

                    while (CH_NUM)

                    for (ix = CH_NUM; ix < card_data[3]; ix++)
                    {
                        result = ReadAIAnalogueBlock(fd, slot_num, CH_NUM, data_ptr, &error_code);
                    }
*/
                    



            }
        }

        if (slot_info[slot_num]->board_type == BOARD_DIO)
        {
            
            printf("\n");
            printf("1.Read DIO Board Configuation\n");
            printf("2.Read DIO Pin Current State\n");
            printf("3.Read DIO All Input Stored\n");
            printf("4.Write DIO Configuration\n");
            printf("5.Write Digital Output\n\n");

            printf("Select Option :: ");
            scanf(" %d",&user_input);
            if (user_input == 1)
            {
                LOG_LEVEL_1 = 3;
                LOG_LEVEL_2 = 0;
                verbose = 0;
                result = read_DIO_Board_Config(fd, slot_num);

            }
            if (user_input == 2)
            {
                LOG_LEVEL_1 = 3;
                LOG_LEVEL_2 = 0;
                verbose = 0;
                result = read_DIO_Pin_Current_state(fd, slot_num);
            }
            if (user_input == 3)
            {
                result = read_DIO_AllInput_stored(fd, slot_num);
            }

            if (user_input == 4)
            {
                verbose = 1;
                result = write_DIO_Board_Config(fd, slot_num);
            }
            if (user_input == 5)
            {
                printf("Enter value to write on Channel :: ");
                scanf(" %X", &DI_ch_value);
                verbose = 1;
                printf("di %X\n", DI_ch_value);
                result = write_DIO_Pin_Output(fd, slot_num, 0x00, 0x00, 0x00);
            }

        }

    }

    if (operation == 4)
    {
        LOG_LEVEL_1 = 3;
        LOG_LEVEL_2 = 0;
        verbose = 0;
        result = DetectSlot(fd);

        for (int i = 0; i<9; i++)
        {
           
            if (slot_info[i]->board_type == BOARD_AI)
            {
                slot_AI = slot_info[i]->slot_number;
                printf("%d", i);
                //break;
            }
        }

        ch_num = 0xff;
        user_input = 0;
        //result = execute_AI_Test(fd, slot_AI, ch_num, user_input);
    }

    if (operation == 5)
    {
        LOG_LEVEL_1 = 3;
        LOG_LEVEL_2 = 0;
        verbose = 0;
        result = DetectSlot(fd);
        printf("Detected Cards are above: \n");
        printf("Enter AI Card slot Num :: ");
        scanf(" %d",&slot_num);
        /*printf("\nEnter pair Value :: ");
        scanf(" %d", &pair_info);
        printf("\nEnter Gain Info in Hex format(0x00) :: ");
        scanf(" %d", &gain_info);
        LOG_LEVEL_1 = 0;
        LOG_LEVEL_2 = 0;
        verbose = 1;
        printf(" %X,%X\n", pair_info, gain_info);
        result = readAIFactoryCal(fd, slot_num, pair_info, gain_info);*/
        verbose = 1;
        result = readAIFactoryCal(fd, slot_num, 0x10, 0x04);
    }

    return 1;
}

int temp_test_main()
{
    time_t r_time;
    struct  tm* curr_time;
    uint32_t milli_sec = 0;
    int trigger = 1, count =0;
    clock_t x_start, x_stop, x_diff;


    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);



    time(&r_time);
    curr_time = localtime(&r_time);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf("%02d:%02d:%02d \t %ld\n\n", curr_time->tm_hour, curr_time->tm_min,
            curr_time->tm_sec, milli_sec);

    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("  %lld \n", delta_us);
    printf ( "Current local time and date: %s", asctime (curr_time) );


    return 0;
}

int user_selection_based_main()
{

    int result;
    
    result = spi_gpio_init();
    result = irq_init();
    result = SPI_init(fd);
    //result = test_main();
    result = test_select_option(fd);
   // cleanup(fd);
    return 1;

}



int main()
{
    int result;
    result = user_selection_based_main();
    

    return 0;
}