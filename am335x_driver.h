#ifndef _AM335X_DRIVER_H_
#define _AM335X_DRIVER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>  
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>
#include <termios.h>
//#include <pthread.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/rtc.h>
#include <linux/i2c.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#include<linux/input.h>
#include <errno.h>
#include <pthread.h>
#include <sys/prctl.h>

//#define ATS_TEST
/*
define gpio register 
*/
#define GPIO0_CLOCK_CTRL    0x408
#define GPIO1_CLOCK_CTRL    0x0ac
#define GPIO2_CLOCK_CTRL    0x0b0
#define GPIO3_CLOCK_CTRL	0x0b4
#define CM_PER      		0x44e00000
#define GPIO0_REG           0x44e07000
#define GPIO1_REG			0x4804c000
#define GPIO2_REG			0x481ac000
#define GPIO3_REG			0x481ae000
#define GPIO_IRQSTATUS_0    0x02C
#define GPIO_IRQSTATUS_1	0x030
#define GPIO_IRQSTATUS_SET_0	0x034
#define GPIO_IRQSTATUS_SET_1	0x038
#define GPIO_CTRL				0x130
#define GPIO_OE					0x134
#define GPIO_DATAIN				0x138
#define GPIO_DATAOUT			0x13C
#define GPIO_LEVELDETECT0		0x140
#define GPIO_LEVELDETECT1		0x144
#define GPIO_RISINGDETECT		0x148
#define GPIO_FALLINGDETECT		0x14C

#define BLOCK_SIZE  0x100000
#define AM335X_BOARD_VERSION    0x13
#define BOOL  int
#define TRUE  1
#define FALSE 0
#define AM335X_SENSOR_RUN_OK    1
#define AM335X_SENSOR_RUN_ERR   2
#define AM335X_SENSOR_PRE_HEAT  3
#define AM335X_SENSOR_NOT_INIT  4

#define AM335X_SENSOR_TRY_TIMES 5

#define AM335X_SENSOR_TYPE_WINSEN_ZE03    1
#define AM335X_SENSOR_TYPE_WINSEN_ZE10    2
#define AM335X_SENSOR_TYPE_SINGOAN_SGA700 3

#define AM335X_SENSOR_TYPE_WINSEN_ZH03    1
#define AM335X_SENSOR_TYPE_NOVA_SDS       2
#define AM335X_SENSOR_TYPE_AMPHENOL       3
#define AM335X_SENSOR_TYPE_PMS5003        4
#define AM335X_SENSOR_TYPE_WXA            5
#define AM335X_SENSOR_TYPE_FRT            6

#define AM335X_SENSOR_PRE_HEAT_TIME  30//10

#define AM335X_SENSOR_SO2       0
#define AM335X_SENSOR_NO2       1
#define AM335X_SENSOR_CO        2
#define AM335X_SENSOR_O3        3
#define AM335X_SENSOR_TVOC      4
#define AM335X_SENSOR_GAS_NUMS  5

#define AM335X_SENSOR_PM        0
#define AM335X_SENSOR_PMS_NUMS  1

#define AM335X_SENSOR_ATOMS       0
#define AM335X_SENSOR_ATOMS_NUMS  1 

#define AM335X_SENSOR_TVOCS      0
#define AM335X_SENSOR_TVOC_NUMS  1 

#define AM335X_SENSOR_MAX_NUMS  (AM335X_SENSOR_GAS_NUMS + AM335X_SENSOR_PMS_NUMS + AM335X_SENSOR_ATOMS_NUMS + AM335X_SENSOR_TVOC_NUMS)

#define AM335X_SENSOR_DATA_SO2       0
#define AM335X_SENSOR_DATA_NO2       1
#define AM335X_SENSOR_DATA_CO        2
#define AM335X_SENSOR_DATA_O3        3

#define AM335X_SENSOR_DATA_PM100     4
#define AM335X_SENSOR_DATA_PM025     5
#define AM335X_SENSOR_DATA_PM010     6

#define AM335X_SENSOR_DATA_CF_PM010  7
#define AM335X_SENSOR_DATA_CF_PM025  8
#define AM335X_SENSOR_DATA_CF_PM100  9
#define AM335X_SENSOR_DATA_03_NUM    10
#define AM335X_SENSOR_DATA_05_NUM    11
#define AM335X_SENSOR_DATA_10_NUM    12
#define AM335X_SENSOR_DATA_25_NUM    13
#define AM335X_SENSOR_DATA_50_NUM    14
#define AM335X_SENSOR_DATA_100_NUM   15

#define AM335X_SENSOR_DATA_TEMPERATURE  16    //è„¦è„—éœ²è„ 
#define AM335X_SENSOR_DATA_HUMIDITY     17   //è„¢é™‹éœ²è„ 
#define AM335X_SENSOR_DATA_AIR_PRESSURE 18   //è„é…¶è„©é¹¿
#define AM335X_SENSOR_DATA_WIND_SPEED   19    //è·¯èŽ½è„£è„µ
#define AM335X_SENSOR_DATA_WIND_DIRECTION 20   //è·¯èŽ½è„§è²Œ
#define AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL  21

#define AM335X_SENSOR_DATA_TVOC      22

/* FRT ×îÐ¡·çÏò ×î´ó·çÏò Æ½¾ù·çÏò  ×îÐ¡·çËÙ ×î´ó·çËÙ Æ½¾ù·çËÙ */
#define AM335X_SENSOR_DATA_MIN_WIND_SPEED 23
#define AM335X_SENSOR_DATA_MAX_WIND_SPEED 24
#define AM335X_SENSOR_DATA_MIN_WIND_DIRECTION 25
#define AM335X_SENSOR_DATA_MAX_WIND_DIRECTION 26

#define AM335X_SENSOR_DATA_MAX_NUMS  27

#define AM335X_FRT_MIN_WIND_DIRECTION    0
#define AM335X_FRT_MAX_WIND_DIRECTION   1
#define AM335X_FRT_AVE_WIND_DIRECTION   2
#define AM335X_FRT_MIN_WIND_SPEED          3
#define AM335X_FRT_MAX_WIND_SPEED         4
#define AM335X_FRT_AVE_WIND_SPEED         5
#define AM335X_FRT_TEMPERATURE                6
#define AM335X_FRT_HUMIDITURY                  7
#define AM335X_FRT_PRESSURE                      8
#define AM335X_FRT_RAINFALL                      9

#define SENSOR_NUM    7
#define S_SO2         0
#define S_NO2         1
#define S_CO          2
#define S_O3          3
#define S_TVOC        4
#define S_PM          5
#define S_ATMOS       6

//#define AM335X_SENSOR_DATA_ATMOS_MAX_NUMS  6

#define AM335X_SENSOR_DATA_MAX_SIZE  (60 * 60 * 24)    //ç©ºé—´< 32M

#define AM335X_SENSOR_DATA_ID        0xAA55
#define AM335X_4G_NET_INFO_ID        0xAAAA
#define AM335X_SO2_ID                0xAA00
//#define AM335X_ATMOS_SENSOR_DATA_ID      0xAA00
#define WATCH_DOG_ID 	0xAA66
#define WATCH_DOG_ID_MAX_NUMS  4

#define AM335X_TPS65217_ADDRESS      0x24

#define AM335X_EEPROM_ADDRESS        0x50
#define AM335X_EEPROM_PAGESIZE       0x10
#define AM335X_EEPROM_TOTALSIZE      0x100

#define AM335X_LCD_HEIGHT            128 /* bit */
#define AM335X_LCD_WIDTH             128 /* bit */
#define AM335X_LCD_CMD               2

#define AM335X_ASCII_HEIGHT          16  /* bit */
#define AM335X_ASCII_WIDTH           16  /* bit */

#if  (AM335X_BOARD_VERSION == 0x10)

#define AM335X_GPIO_MAX_PIN          5

#define AM335X_GPIO_SEN_SET          (32 * 1 + 0)
#define AM335X_GPIO_SEN_RST          (32 * 1 + 1)
#define AM335X_GPIO_CHR_DIS          (32 * 1 + 2)
#define AM335X_GPIO_LCD_DIS          (32 * 1 + 3)
#define AM335X_GPIO_LCD_EXT          (32 * 1 + 4)

#elif(AM335X_BOARD_VERSION == 0x11)

#define AM335X_GPIO_MAX_PIN          10

#define AM335X_GPIO_SEN_RST          (32 * 0 + 23)
#define AM335X_GPIO_SEN_SET          (32 * 0 + 26)
#define AM335X_GPIO_E5V_ENB          (32 * 0 + 27)
#define AM335X_GPIO_MMC_RST          (32 * 1 + 12)
#define AM335X_GPIO_LCD_EXT          (32 * 1 + 17)
#define AM335X_GPIO_LCD_DIS          (32 * 1 + 18)
#define AM335X_GPIO_CHR_DIS          (32 * 1 + 21)
#define AM335X_GPIO_EEP_WRP          (32 * 1 + 25)
#define AM335X_GPIO_HUB_RST          (32 * 1 + 26)
#define AM335X_GPIO_USB_ENB          (32 * 1 + 27)

#define AM335X_GPIO_SLF_RST          (32 * 0 + 22)
#define AM335X_GPIO_PCI_RST          (32 * 1 + 15)
#define AM335X_GPIO_4GM_PWR          (32 * 1 + 22)

#define AM335X_GPIO_DOR_ALM          (32 * 1 + 14)
#define AM335X_GPIO_PWR_SAT          (32 * 1 + 19)
#define AM335X_GPIO_SFD_ALM          (32 * 1 + 24)

#elif(AM335X_BOARD_VERSION == 0x12)

#define AM335X_GPIO_MAX_PIN          8

#define AM335X_GPIO_E5V_ENB          (32 * 0 + 27)
#define AM335X_GPIO_MMC_RST          (32 * 1 + 12)
#define AM335X_GPIO_LCD_EXT          (32 * 1 + 17)
#define AM335X_GPIO_LCD_DIS          (32 * 1 + 18)
#define AM335X_GPIO_CHR_DIS          (32 * 1 + 21)
#define AM335X_GPIO_EEP_WRP          (32 * 1 + 25)
#define AM335X_GPIO_HUB_RST          (32 * 1 + 26)
#define AM335X_GPIO_EXT_CTL          (32 * 2 +  2)

#define AM335X_GPIO_SLF_RST          (32 * 0 + 22)
#define AM335X_GPIO_PCI_RST          (32 * 1 + 15)
#define AM335X_GPIO_4GM_PWR          (32 * 1 + 22)

#define AM335X_GPIO_USB_SAT          (32 * 0 + 23)
#define AM335X_GPIO_E5V_SAT          (32 * 0 + 26)
#define AM335X_GPIO_DOR_ALM          (32 * 1 + 14)
#define AM335X_GPIO_PWR_SAT          (32 * 1 + 19)
#define AM335X_GPIO_SFD_ALM          (32 * 1 + 24)

#else

#define AM335X_GPIO_MAX_PIN          10

#define AM335X_GPIO_E5V_ENB          (32 * 0 + 27)
#define AM335X_GPIO_GAS_ENB          (32 * 1 + 27)
#define AM335X_GPIO_MMC_RST          (32 * 1 + 12)
#define AM335X_GPIO_LCD_EXT          (32 * 1 + 17)
#define AM335X_GPIO_LCD_DIS          (32 * 1 + 18)
#define AM335X_GPIO_CHR_DIS          (32 * 1 + 21)
#define AM335X_GPIO_EEP_WRP          (32 * 1 + 25)
#define AM335X_GPIO_HUB_RST          (32 * 1 + 26)
#define AM335X_GPIO_EXT_CTL          (32 * 0 + 23)
#define AM335X_GPIO_USB_ENB          (32 * 0 + 18)

#define AM335X_GPIO_SLF_RST          (32 * 0 + 22)
#define AM335X_GPIO_PCI_RST          (32 * 1 + 15)
#define AM335X_GPIO_4GM_PWR          (32 * 1 + 22)

#define AM335X_GPIO_USB_SAT          (32 * 2 + 2)
#define AM335X_GPIO_E5V_SAT          (32 * 0 + 26)
#define AM335X_GPIO_DOR_ALM          (32 * 1 + 14)
#define AM335X_GPIO_PWR_SAT          (32 * 1 + 19)
#define AM335X_GPIO_SFD_ALM          (32 * 1 + 24)

#endif

#define AM335X_GPIO_LED0             (32 * 2 + 22)
#define AM335X_GPIO_LED1             (32 * 2 + 23)
#define AM335X_GPIO_LED2             (32 * 2 + 24)

#define AM335X_4G_MODULE_FIBOCOM_L810   0
#define AM335X_4G_MODULE_FIBOCOM_L710   1
#define AM335X_4G_MODULE_HUAWEI_ME909S  2
#define AM335X_4G_MODULE_LONGSUNG_U9300 3
#define AM335X_ATOM_MODULE_ZM 3

#define AM335X_4G_MODULE_NONE           0
#define AM335X_4G_MODULE_CMCC           1
#define AM335X_4G_MODULE_CUCC           2

#define AM335X_4G_MODULE_INIT           0
#define AM335X_4G_MODULE_CHECK          1
#define AM335X_4G_MODULE_NET_CONNECT    2
#define AM335X_4G_MODULE_NET_ENABLE     3
#define AM335X_4G_MODULE_NET_CONFIG     4
#define AM335X_4G_MODULE_NET_CHECK      5

#define AM335X_4G_MODULE_RET_OK         0
#define AM335X_4G_MODULE_RET_CMD_ERROR  1
#define AM335X_4G_MODULE_RET_REG_ERROR  2
#define AM335X_4G_MODULE_RET_CON_ERROR  3
#define AM335X_4G_MODULE_RET_IP_CHANGE  4

#if(AM335X_BOARD_VERSION != 0x10)

#define AM335X_4G_MODULE_FIBOCOM_L710_ACM_SYS   "/sys/bus/usb/devices/2-1/2-1:1.4"
#define AM335X_4G_MODULE_FIBOCOM_L710_NET_SYS   "/sys/bus/usb/devices/2-1/2-1:1.0/net"

#define AM335X_4G_MODULE_FIBOCOM_L810_ACM_SYS   "/sys/bus/usb/devices/2-1/2-1:1.4/tty"
#define AM335X_4G_MODULE_FIBOCOM_L810_NET_SYS   "/sys/bus/usb/devices/2-1/2-1:1.6/net"

#define AM335X_4G_MODULE_HUAWEI_ME909S_ACM_SYS  "/sys/bus/usb/devices/2-1/2-1:2.2"
#define AM335X_4G_MODULE_HUAWEI_ME909S_NET_SYS  "/sys/bus/usb/devices/2-1/2-1:2.0/net"

#define AM335X_4G_MODULE_LONGSUNG_U9300_ACM_SYS  "/sys/bus/usb/devices/2-1/2-1:1.2"
#define AM335X_4G_MODULE_LONGSUNG_U9300_NET_SYS  "/sys/bus/usb/devices/2-1/2-1:1.4/net"

#else

#define AM335X_4G_MODULE_FIBOCOM_L710_ACM_SYS   "/sys/bus/usb/devices/1-1/1-1:1.4"
#define AM335X_4G_MODULE_FIBOCOM_L710_NET_SYS   "/sys/bus/usb/devices/1-1/1-1:1.0/net"

#define AM335X_4G_MODULE_FIBOCOM_L810_ACM_SYS   "/sys/bus/usb/devices/1-1/1-1:1.4/tty"
#define AM335X_4G_MODULE_FIBOCOM_L810_NET_SYS   "/sys/bus/usb/devices/1-1/1-1:1.6/net"
#endif
#define AM335X_TEST_TRY_COUNT 3
//define the share mem id
#define POWER_LOST_MEM_ID                    0x0001
#define ARRESTER_MEM_ID                      0x0001
#define DOOR_SYSTEM_MEM_ID                   0x0001
#define BOARD_SELF_CHECK_MEM_ID              0x0004
#define HUMIDITY_MEM_ID                      0x0002
#define TEMP_MEM_ID                          0x0002
#define CELL_MEM_ID                          0x0002
#define LINK_MEM_ID                          0x0008
#define POWER_USB_MEM_ID                     0x0001
#define POWER_5V_MEM_ID                      0x0001
#define ALARM_MEM_ID                         0x0001

//define the alarm id 
#define ALARM_POWER_LOST_ID                    0x0001
#define ALARM_ARRESTER_ID                      0x0002
#define ALARM_DOOR_SYSTEM_ID                   0x0003
#define ALARM_BOARD_SELF_CHECK_ID              0x0004
#define ALARM_HUMIDITY_ID                      0x0005
#define ALARM_TEMP_ID                          0x0006
#define ALARM_CELL_ID                          0x0007
#define ALARM_LINK_ID                          0x0008
#define ALARM_POWER_USB_ID                     0x0009
#define ALARM_POWER_5V_ID                      0x000a
#define ALARM_FAN_ID                           0x000b
#if 1
//define the device file systerm
#define ALARM_POWER_USB_SYS                     "/dev/input/event0"
#define ALARM_POWER_5V_SYS                      "/dev/input/event0"
#define AM335X_POWER_LOST_SYS                   "/dev/input/event0"
#define AM335X_ARRESTER_SYS                     "/dev/input/event0"
#define AM335X_DOOR_SYSTEM_SYS                  "/dev/input/event0"
#define AM335X_HUMIDITY_SYS                     "/sys/class/hwmon/hwmon0/device/humidity"

#define AM335X_TEMPERATURE_SYS                  "/sys/class/hwmon/hwmon0/device/temperature"

#define AM335X_VOLTAGE_SYS                      "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#endif
//define 
#define AM335X_4G_MODULE_TRY_COUNT      5

#define AM335X_PROCESS_SO2_ID           0
#define AM335X_PROCESS_NO2_ID           1
#define AM335X_PROCESS_CO_ID            2
#define AM335X_PROCESS_O3_ID            3
#define AM335X_PROCESS_TVOC_ID          4
#define AM335X_PROCESS_PM_ID            5
#define AM335X_PROCESS_ATS_ID           6
#define AM335X_PROCESS_ATOMS_ID         7
#define AM335X_PROCESS_4G_ID            8
#if 0
#define AM335X_PROCESS_POWER_LOST_ID    6
#define AM335X_PROCESS_ARRESTER_ID      7
#define AM335X_PROCESS_DOOR_SYSTEM_ID   8
#define AM335X_PROCESS_BOARD_ID         9
#define AM335X_PROCESS_HUMIDITY_ID      10
#define AM335X_PROCESS_TEMP_ID          11
#define AM335X_PROCESS_CELL_ID          12
#define AM335X_PROCESS_LINK_ID          13
#define AM335X_PROCESS_POWER_USB_ID     14
#define AM335X_PROCESS_POWER_5V_ID      15
#endif

#define AM335X_PROCESS_MAX_NUMS         9

/* #define AM335X_RAW_DATA_OUTPUT */

#define AM335X_RAW_DATA_FILE_SIZE       (16 * 1024 * 1024)

/* #define AM335X_RAW_DATA_PRINT */

#define AM335X_SYSLOG_PRINT

#define AM335X_SYSLOG_BUF_SIZE          1024

#ifdef AM335X_SYSLOG_PRINT
#define am335x_openlog(log)             openlog(log, LOG_CONS | LOG_PID, 0)
#define am335x_closelog(log)            closelog()
#define am335x_printlog(info, fmt...)   syslog(LOG_USER | LOG_DEBUG, info, ##fmt)
#else
#define am335x_openlog(log)
#define am335x_closelog(log)
#define am335x_printlog(fmt...)         printf(info, ##fmt)
#endif

#define XR_USB_SERIAL_IOC_MAGIC       	'v'

#define XR_USB_SERIAL_GET_REG           _IOWR(XR_USB_SERIAL_IOC_MAGIC, 1, int)
#define XR_USB_SERIAL_SET_REG           _IOWR(XR_USB_SERIAL_IOC_MAGIC, 2, int)
#define XR_USB_SERIAL_SET_ADDRESS_MATCH _IO(XR_USB_SERIAL_IOC_MAGIC, 3)
#define XR_USB_SERIAL_SET_PRECISE_FLAGS _IO(XR_USB_SERIAL_IOC_MAGIC, 4)
#define XR_USB_SERIAL_TEST_MODE         _IO(XR_USB_SERIAL_IOC_MAGIC, 5)
#define XR_USB_SERIAL_LOOPBACK          _IO(XR_USB_SERIAL_IOC_MAGIC, 6)

/*********************waring value**************************/
#define ONLINE_FAULT_WARNING    -1000
#define POWER_LOST_WARNING      -1001
#define ARRESTER_WARNING        -1002
#define DOOR_SYSTEM_WARNING     -1003
#define BOARD_FAULT             -1004
#define SO2_RS                  -2000
#define NO2_RS                  -2001
#define CO_RS                   -2002
#define O3_RS                   -2003
#define PM_RS                   -2004
#define HUMIDITY_WARNING        -3000
#define TEMP_WARNING            -3001
#define CELL_WARNING            -3002
#define LINK_WARNING            -3003
#define POWER_USB_FAULT         -3004
#define POWER_5V_FAULT          -3005
#define MEMORY_WARNING          -3006
#define FLASH_WARNING           -3007
/***********************************************************/

#define   SO2_PTHREAD_ERROR     -4001
#define   NO2_PTHREAD_ERROR     -4002
#define   CO_PTHREAD_ERROR      -4003
#define   O3_PTHREAD_ERROR      -4004
#define   PM_PTHREAD_ERROR      -4005
#define   ATMOS_PTHREAD_ERROR   -4006
#define   TVOC_PTHREAD_ERROR    -4007
#define   NET_4G_PTHREAD_ERROR  -4008
//threshold value
#define TEMP_THRESHOLD_VALUE           50
#define HUMIDITY_THRESHOLD_VALUE       50
#define VOLTAGE_VALUE                  20


//define alarm status
#define AM335X_ALARM_NUM 14

struct am335x_board_info_t {
	unsigned int magic;
	char         name[8];
	char         version[4];
	char         machn_sn[12];
	char         board_sn[32];
	char         mac_addr[2][6];
    char         padding[8];
    char         sysmn[16];
	char         machine_sn[32];
	char         no2_sn[21];
    char         so2_sn[21];
    char         co_sn[21];
    char         o3_sn[21];
    char         pm_sn[21];//pm_sn[9];
};

struct sensor_para_t{

	int stype;
	int gtype;
	int value;
};

struct alarm_para_t{

    int mem_id;
    int alarm_id;
    char *dev;
};
//矫正参数定义
struct fact_info_t {    
   
    int fact_a_pm25;
    int fact_b_pm25;
    
    int fact_a_pm10;
    int fact_b_pm10;
    
    int fact_a_so2;
    int fact_b_so2;
    
    int fact_a_no2;
    int fact_b_no2;
    
    int fact_a_o3;
    int fact_b_o3;
    
    int fact_a_co;
    int fact_b_co;
    
    int fact_a_voc;
    int fact_b_voc;
};

typedef struct
{
	unsigned int CurCnt, LastCnt;
	int taskID;
} STRUCT_WATCH_DOG;
struct am335x_pthread_return_t {
int so2_pth_return_val;
int no2_pth_return_val;
int co_pth_return_val;
int o3_pth_return_val;
int tvoc_pth_return_val;
int pm_pth_return_val;
int atmos_pth_return_val;
int net_pth_return_val;
int defalut_none;        /*\B4\AB\B8\D0\C6\F7\C0\E0\D0Ͳ\BBƥ\C5\E4*/
};
struct am335x_sensor_data_t {
    //volatile unsigned int value;
    volatile float value;
    volatile unsigned int time;
};

struct am335x_atmos_sensor_data_t {
    volatile float value;
    volatile unsigned int time;
};

struct am335x_sensor_dev_t {
    unsigned char gas_name[32];
    unsigned int  gas_code;
    unsigned char uart_dev[32];
    unsigned char uart_code[2];
    unsigned int  uart_speed;
    unsigned int  uart_length;
    unsigned int  uart_timeout;
};

struct am335x_sensor_mem_t {
    volatile unsigned int types;
    volatile unsigned int flags;
    volatile unsigned int count;
    volatile unsigned int index;
    struct am335x_sensor_data_t data[AM335X_SENSOR_DATA_MAX_SIZE];
};
struct am335x_so2_base_value_mem_t{
    int value; /*0,2,10,20,30,40,50,60,70,80,90,99*/
	int flag;  /*0 ::\B2\BB\B5\F7\D5\FB\A3\BB 1: \B5\F7\D5\FB*/
	int type;/*so2,no2,co,o3*/
	int retur;	
	int so2_b_value;
	int read_b_type;
	int read_b_value_flag;
	int read_b_value_retur;
};

struct am335x_atoms_sensor_mem_t{
    volatile unsigned int types;
    volatile unsigned int flags;
    volatile unsigned int count;
    volatile unsigned int index;
    struct am335x_atmos_sensor_data_t data[AM335X_SENSOR_DATA_MAX_SIZE];
};
struct am335x_4gnet_info_t {
    volatile int  status;
    volatile int  counter;
    volatile int  g_signal;//ä¿¡å·å¼ºåº¦æ ‡è¯†
    volatile char ip[16];
};
/*alarm struct*/
struct am335x_ats_alarm_info_t {
	volatile int  code;
};

#if 0
struct am335x_power_lost_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_power_5v_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_power_usb_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_link_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_cell_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_temperature_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_arrester_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_door_system_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_board_alarm_info_t {
    volatile int  type;
    volatile int  code;
};

struct am335x_humidity_alarm_info_t {
    volatile int  type;
    volatile int  code;
};
#endif

struct am335x_alarm_info_t {
	volatile int  power_lost_code;
	volatile int  arrester_code;
	volatile int  door_systerm_code;
	volatile int  board_code;
	volatile int  humidity_code;
	volatile int  temperature_code;
	volatile int  cell_code;
	volatile int  link_code;
	volatile int  power_usb_code;
	volatile int  power_5v_code;
	volatile int  signal_code;
};

extern struct am335x_sensor_dev_t am335x_sensor_ze03_dev[AM335X_SENSOR_GAS_NUMS];
extern struct am335x_sensor_dev_t am335x_sensor_ze10_dev[AM335X_SENSOR_GAS_NUMS];
extern struct am335x_sensor_dev_t am335x_sensor_sga700_dev[AM335X_SENSOR_GAS_NUMS];

extern struct am335x_sensor_dev_t am335x_sensor_zh03_dev[AM335X_SENSOR_PMS_NUMS];
extern struct am335x_sensor_dev_t am335x_sensor_sds_dev[AM335X_SENSOR_PMS_NUMS];

extern int am335x_drv_init(int gas_type, int pms_type,int atom_ptype, int *res);
extern int am335x_set_time(struct tm *time);
extern int am335x_get_time(struct tm *time);
extern int am335x_eeprom_write(int chip, int address, char *buffer, int len);
extern int am335x_eeprom_read(int chip, int address, char *buffer, int len);
extern int am335x_tps65127_reg_write(char reg, char value);
extern int am335x_tps65127_reg_read(char reg, char *value);
extern int am335x_lcd_display();
extern int am335x_lcd_clear();
extern int am335x_lcd_drawASC(int x, int y, char c);
extern int am335x_gpio_write(int pin, int value);
extern int am335x_gpio_read(int pin);
extern int am335x_gpio_show();
extern void* am335x_gas_sensor_process(void *arg);
extern void* am335x_pms_sensor_process(void *arg);
extern void* am335x_atoms_sensor_process(void *arg_t);
extern int am335x_sensor_flag_read(int item);
extern int am335x_sensor_data_read(int item, struct am335x_sensor_data_t *data, int len, unsigned int start, unsigned int end);
extern int am335x_4g_module_status_read(char *ip);
extern int am335x_4g_module_status_show();
extern int am335x_4g_module_process();
extern int am335x_4g_module_test(int type, int flag);
extern void* am335x_4gnet_init();
extern int am335x_open_dog(int fd);
extern int am335x_feed_dog(int watch_dog_fd, int sleep_time);
extern void am335x_close_dog(int watch_dog_fd);
extern void* am335x_alarm_process(void* alarm_t);
//extern int am335x_check_alarm_status(int share_mem_id,int alarm_id, const char *dev);
extern void syslog_printf(char *format, ...);
extern int find_frt_value(char *cmd, char *cmd2, char *result, int index, char *value);
extern float get_frt_value(char *value);
extern int am335x_gas_sensor_read_id(int type, int sensor, char *sId);
extern int am335x_4g_module_power(int on);

extern int am335x_ddr_test();
extern int am335x_led_test();
extern int am335x_lcd_test();
extern int am335x_eeprom_test();
extern int am335x_uart_test(char* uart);
extern int am335x_extern_uart_test(char* uart);
extern int am335x_extern_uart_process(char* uart);
extern int am335x_gas_sensor_test(int type, int sensor);
extern int am335x_pms_sensor_test(int type, int sensor);
extern int am335x_pms_sensor_test2(int type, int sensor);

extern int am335x_rtc_test();
extern int am335x_emmc_test();
extern int am335x_watchdog_test();
extern int am335x_4gnet_init_test();
extern int am335x_4g_module_test(int type, int flag);
extern int am335x_atmos_sensor_test(int type, int sensor);
extern int am335x_hih6130_read(void);
extern int am335x_set_sga700_so2_value(int fd,int value, int flag,struct am335x_so2_base_value_mem_t* so2);
extern int am335x_set_sga700_para(int type, int value);
extern int am335x_read_sga700_so2_base_value(int fd, int type,struct am335x_so2_base_value_mem_t *am335x_so2_mem);
extern int am335x_set_read_sga700_gas_base_value_para(int type, int *b_value);
extern int am335x_set_base_value_test(char *dev, int value);

extern    char         pm_type[2];
extern    char		   pm_id[21];
    
extern    char         so2_type[2];
extern    char         so2_id[6];
    
extern    char         no2_type[2];
extern    char         no2_id[6];
    
extern    char         o3_type[2];
extern    char         o3_id[6];
    
extern    char         co_type[2];
extern    char         co_id[6];
    
extern    char         voc_type[2];
extern    char         voc_id[6];
    
extern    char         met_type[2];
extern    char         met_id[10];

extern int dog_fd;
//int ats_flag;
//extern char *sig_ptr;
//char *sig_buffer;
//extern int sig_value;
//extern int g_signal;
extern int check_flag;
extern int net_status;
extern int so2_fd;
extern int g_pthread_exit;
extern struct am335x_4gnet_info_t *am335x_4gnet_info_test;
#ifdef  __cplusplus
}
#endif

#endif
