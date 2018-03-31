#include <pthread.h>
#include "am335x_driver.h"
//#include "mysem.h"

int sig_value;
int g_signal = 0;
int check_flag = 0;
int net_status;
int g_pthread_exit = 0;
int so2_fd;
sem_t sem;

int dog_fd;
char *sig_ptr;
char *test;
int g_4g_type;
int log_prt = 0;
char sensor_id[6];
int g_time_flag = 1;
BOOL so2_base_value_flag = TRUE;
struct sensor_para_t sType[SENSOR_NUM];
struct am335x_pthread_return_t g_pthread_ret;
char         pm_type[2];
char		 pm_id[21];

char         so2_type[2];
char         so2_id[6];

char         no2_type[2];
char         no2_id[6];

char         o3_type[2];
char         o3_id[6];

char         co_type[2]; 
char         co_id[6];

char         voc_type[2];
char         voc_id[6];

char         met_type[2];
char         met_id[10];

struct am335x_4gnet_info_t *am335x_4gnet_info_test;
struct timeval time0, time1;
unsigned long int timer;
#if  (AM335X_BOARD_VERSION == 0x10)

const int gpio_pin[AM335X_GPIO_MAX_PIN] = {
        AM335X_GPIO_SEN_SET,
        AM335X_GPIO_SEN_RST,
        AM335X_GPIO_CHR_DIS,
        AM335X_GPIO_LCD_DIS,
        AM335X_GPIO_LCD_EXT
};

const int gpio_dir[AM335X_GPIO_MAX_PIN] = {
        0,
        0,
        0,
        0,
        0
};

const int gpio_val[AM335X_GPIO_MAX_PIN] = {
        1,
        1,
        1,
        1,
        0
};

#elif(AM335X_BOARD_VERSION == 0x11)

const int   gpio_pin[AM335X_GPIO_MAX_PIN] = {
        AM335X_GPIO_SEN_RST,
        AM335X_GPIO_SEN_SET,
        AM335X_GPIO_E5V_ENB,
        AM335X_GPIO_MMC_RST,
        AM335X_GPIO_LCD_EXT,
        AM335X_GPIO_LCD_DIS,
        AM335X_GPIO_CHR_DIS,
        AM335X_GPIO_EEP_WRP,
        AM335X_GPIO_HUB_RST,
        AM335X_GPIO_USB_ENB
};

const int gpio_dir[AM335X_GPIO_MAX_PIN] = {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
};

const int gpio_val[AM335X_GPIO_MAX_PIN] = {
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        1,
        1,
        1
};

#elif(AM335X_BOARD_VERSION == 0x12)

const int   gpio_pin[AM335X_GPIO_MAX_PIN] = {
        AM335X_GPIO_E5V_ENB,
        AM335X_GPIO_MMC_RST,
        AM335X_GPIO_LCD_EXT,
        AM335X_GPIO_LCD_DIS,
        AM335X_GPIO_CHR_DIS,
        AM335X_GPIO_EEP_WRP,
        AM335X_GPIO_HUB_RST,
        AM335X_GPIO_EXT_CTL
};

const int gpio_dir[AM335X_GPIO_MAX_PIN] = {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
};

const int gpio_val[AM335X_GPIO_MAX_PIN] = {
        1,
        1,
        0,
        1,
        0,
        1,
        1,
        1
};

#else 

const int gpio_pin[AM335X_GPIO_MAX_PIN] = {
        AM335X_GPIO_E5V_ENB,
        AM335X_GPIO_GAS_ENB,    
        AM335X_GPIO_MMC_RST,
        AM335X_GPIO_LCD_EXT,
        AM335X_GPIO_LCD_DIS,
        AM335X_GPIO_CHR_DIS,
        AM335X_GPIO_EEP_WRP,
        AM335X_GPIO_HUB_RST,
        AM335X_GPIO_EXT_CTL,
        AM335X_GPIO_USB_ENB
};

const int gpio_dir[AM335X_GPIO_MAX_PIN] = {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
};

const int gpio_val[AM335X_GPIO_MAX_PIN] = {
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        1,
        1,
        1
};
#endif

struct am335x_sensor_dev_t am335x_sensor_ze03_dev[AM335X_SENSOR_GAS_NUMS] = {
        {"SO2", 0x00, "/dev/ttyS1",  0xFF, 0x86, 9600, 9,  5},
        {"NO2", 0x00, "/dev/ttyS2",  0xFF, 0x86, 9600, 9,  5},
        {"CO",  0x00, "/dev/ttyS3",  0xFF, 0x86, 9600, 9,  5},
        {"O3",  0x00, "/dev/ttyS5",  0xFF, 0x86, 9600, 9,  5}
};

struct am335x_sensor_dev_t am335x_sensor_ze10_dev[AM335X_SENSOR_GAS_NUMS] = {
        {"SO2", 0x00, "/dev/ttyS1",  0xFF, 0x2B, 9600, 9,  5},
        {"NO2", 0x00, "/dev/ttyS2",  0xFF, 0x2C, 9600, 9,  5},
        {"CO",  0x00, "/dev/ttyS3",  0xFF, 0x04, 9600, 9,  5},
        {"O3",  0x00, "/dev/ttyS5",  0xFF, 0x2A, 9600, 9,  5}
};

struct am335x_sensor_dev_t am335x_sensor_sga700_dev[AM335X_SENSOR_GAS_NUMS] = {
        {"SO2", 0x0A, "/dev/ttyS1",  0xFF, 0xFF, 9600, 18,  5},
        {"NO2", 0x16, "/dev/ttyS2",  0xFF, 0xFF, 9600, 18,  5},
        {"CO",  0x02, "/dev/ttyS3",  0xFF, 0xFF, 9600, 18,  5},
        {"O3",  0x08, "/dev/ttyS5",  0xFF, 0xFF, 9600, 18,  5},
        {"TVOC",  0x27, "/dev/ttyXRUSB3",  0xFF, 0xFF, 9600, 18,  5}
};

struct am335x_sensor_dev_t am335x_sensor_zh03_dev[AM335X_SENSOR_PMS_NUMS] = {
        {"PM", 0x00, "/dev/ttyS4",  0x42, 0x4D, 9600,   24, 5}
};

struct am335x_sensor_dev_t am335x_sensor_sds_dev[AM335X_SENSOR_PMS_NUMS] = {
        {"PM", 0x00, "/dev/ttyS4",  0xAA, 0xC0, 9600,   10, 5}
};

struct am335x_sensor_dev_t am335x_sensor_amp_dev[AM335X_SENSOR_PMS_NUMS] = {
        {"PM", 0x00, "/dev/ttyS4",  0x5A, 0xA5, 115200, 60, 5}
};

struct am335x_sensor_dev_t am335x_sensor_pms5003_dev[AM335X_SENSOR_PMS_NUMS] = {
        {"PM", 0x00, "/dev/ttyS4",  0x42, 0x4d, 9600, 32, 5}
};

struct am335x_sensor_dev_t am335x_sensor_atoms_dev[AM335X_SENSOR_ATOMS_NUMS]={
        {"ATOMS", 0x00, "/dev/ttyXRUSB0", 0x86, 0x00, 9600, 30, 5}
}; 

struct am335x_sensor_dev_t am335x_sensor_atomsFrt_dev[AM335X_SENSOR_ATOMS_NUMS]={
        {"FRT", 0x00, "/dev/ttyXRUSB2", 0x00, 0x00, 19200, 103, 5}
};


char  lcd_buffer[AM335X_LCD_HEIGHT + 1][AM335X_LCD_WIDTH / 8 + AM335X_LCD_CMD];

char  ascii_16x16_table[] = 
{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x03, 0x80, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x07, 0xC0, 0x02, 0x80, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x20, 0x06, 0x60, 0x06, 0x60, 0x0F, 0xF8, 0x04, 0x40, 0x04, 0x40,
        0x04, 0x40, 0x1F, 0xF0, 0x0C, 0xC0, 0x0C, 0xC0, 0x08, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x80, 0x07, 0xE0, 0x0F, 0xA0, 0x0D, 0x80, 0x0D, 0x80, 0x0F, 0x80, 0x07, 0x80,
        0x03, 0xC0, 0x01, 0xE0, 0x01, 0xE0, 0x01, 0xE0, 0x0D, 0xE0, 0x07, 0xC0, 0x01, 0x80, 0x01, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x1C, 0x30, 0x36, 0x20, 0x22, 0x60, 0x22, 0x40, 0x36, 0xC0, 0x1D, 0x80,
        0x01, 0xB8, 0x03, 0x6C, 0x02, 0x44, 0x06, 0x44, 0x04, 0x6C, 0x0C, 0x38, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x0F, 0xC0, 0x0C, 0xC0, 0x0E, 0xCC, 0x0F, 0xCC, 0x0F, 0x8C,
        0x1D, 0xCC, 0x38, 0x78, 0x30, 0x38, 0x30, 0x38, 0x38, 0xFC, 0x1F, 0xCF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x03, 0x80, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00,
        0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0x80, 0x01, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x80,
        0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x03, 0x00, 0x03, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x0F, 0xE0, 0x03, 0x80, 0x06, 0xC0, 0x06, 0xC0,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x0F, 0xE0, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xE0, 0x00, 0xC0, 0x01, 0xC0, 0x01, 0xC0, 0x01, 0x80,
        0x03, 0x80, 0x03, 0x80, 0x03, 0x00, 0x07, 0x00, 0x07, 0x00, 0x06, 0x00, 0x0E, 0x00, 0x0C, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x06, 0x60, 0x0E, 0x70, 0x0E, 0x30, 0x0C, 0x30, 0x0C, 0x30,
        0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0E, 0x70, 0x06, 0x60, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x07, 0x80, 0x0D, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x06, 0x70, 0x00, 0x30, 0x00, 0x30, 0x00, 0x70, 0x00, 0x70,
        0x00, 0xE0, 0x01, 0xC0, 0x03, 0x80, 0x07, 0x00, 0x0E, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x0C, 0xE0, 0x00, 0x60, 0x00, 0x60, 0x00, 0xC0, 0x07, 0xC0,
        0x00, 0xE0, 0x00, 0x70, 0x00, 0x30, 0x00, 0x70, 0x0C, 0xE0, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x01, 0xE0, 0x01, 0x60, 0x03, 0x60, 0x06, 0x60, 0x06, 0x60,
        0x0C, 0x60, 0x18, 0x60, 0x1F, 0xF8, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x07, 0xE0,
        0x00, 0xF0, 0x00, 0x30, 0x00, 0x30, 0x00, 0x70, 0x04, 0xF0, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x07, 0x00, 0x06, 0x00, 0x0E, 0x00, 0x0C, 0x00, 0x0F, 0xE0,
        0x0E, 0x70, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x06, 0x70, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x1F, 0xE0, 0x00, 0x60, 0x00, 0xC0, 0x00, 0xC0, 0x01, 0xC0, 0x01, 0x80,
        0x01, 0x80, 0x03, 0x80, 0x03, 0x00, 0x03, 0x00, 0x07, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x07, 0xE0, 0x06, 0x60, 0x06, 0x60, 0x07, 0xE0, 0x03, 0xC0,
        0x07, 0xE0, 0x0E, 0x70, 0x0C, 0x30, 0x0C, 0x30, 0x0E, 0x70, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x0E, 0x60, 0x0C, 0x70, 0x0C, 0x30, 0x0C, 0x30, 0x0E, 0x70,
        0x07, 0xF0, 0x00, 0x30, 0x00, 0x70, 0x00, 0x60, 0x04, 0xE0, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0xE0,
        0x0F, 0x80, 0x0E, 0x00, 0x0F, 0x80, 0x01, 0xE0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0,
        0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x07, 0x80,
        0x01, 0xF0, 0x00, 0x70, 0x01, 0xF0, 0x07, 0x80, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x06, 0x70, 0x00, 0x30, 0x00, 0x70, 0x00, 0x60, 0x00, 0xC0,
        0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0, 0x38, 0x70, 0x60, 0x18, 0x47, 0xEC, 0xC6, 0xEC, 0x8C, 0x64,
        0x88, 0x44, 0x88, 0x44, 0x88, 0xCC, 0xCD, 0xE8, 0xC7, 0x78, 0x60, 0x00, 0x38, 0x60, 0x1F, 0xC0,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0xC0, 0x03, 0xC0, 0x07, 0xC0, 0x06, 0x60, 0x0E, 0x60,
        0x0C, 0x60, 0x0F, 0xF0, 0x18, 0x30, 0x18, 0x30, 0x38, 0x38, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x06, 0x70, 0x06, 0x30, 0x06, 0x30, 0x06, 0x60, 0x07, 0xE0,
        0x06, 0x30, 0x06, 0x18, 0x06, 0x18, 0x06, 0x18, 0x06, 0x38, 0x07, 0xF0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x07, 0x30, 0x0C, 0x00, 0x1C, 0x00, 0x18, 0x00, 0x18, 0x00,
        0x18, 0x00, 0x18, 0x00, 0x1C, 0x00, 0x0C, 0x00, 0x0F, 0x30, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0, 0x0C, 0x70, 0x0C, 0x18, 0x0C, 0x1C, 0x0C, 0x0C, 0x0C, 0x0C,
        0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1C, 0x0C, 0x18, 0x0C, 0x70, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xF0, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x07, 0xE0,
        0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x07, 0xF0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x07, 0xE0,
        0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0xF0, 0x07, 0x18, 0x0C, 0x00, 0x1C, 0x00, 0x18, 0x00, 0x18, 0x00,
        0x18, 0x78, 0x18, 0x18, 0x1C, 0x18, 0x0C, 0x18, 0x0F, 0x38, 0x03, 0xF8, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0F, 0xF8,
        0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0xC0, 0x00, 0xC0, 0x00, 0xC0, 0x00, 0xC0, 0x00, 0xC0,
        0x00, 0xC0, 0x00, 0xC0, 0x00, 0xC0, 0x00, 0xC0, 0x01, 0xC0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x06, 0x38, 0x06, 0x30, 0x06, 0x60, 0x06, 0xC0, 0x07, 0x80, 0x07, 0x00,
        0x07, 0x80, 0x07, 0xC0, 0x06, 0xC0, 0x06, 0x60, 0x06, 0x30, 0x06, 0x38, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
        0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x38, 0x0C, 0x38, 0x0C, 0x38, 0x0C, 0x2C, 0x1C, 0x2C, 0x14, 0x26, 0x34,
        0x26, 0x24, 0x23, 0x64, 0x23, 0x64, 0x21, 0xC4, 0x21, 0xC4, 0x21, 0x84, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x1E, 0x18, 0x1E, 0x18, 0x1F, 0x18, 0x1F, 0x18, 0x1B, 0x98, 0x19, 0x98,
        0x19, 0xD8, 0x18, 0xD8, 0x18, 0xF8, 0x18, 0x78, 0x18, 0x38, 0x18, 0x38, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x0E, 0x78, 0x18, 0x18, 0x38, 0x1C, 0x30, 0x0C, 0x30, 0x0C,
        0x30, 0x0C, 0x30, 0x0C, 0x38, 0x1C, 0x18, 0x18, 0x1E, 0x70, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xF0, 0x06, 0x38, 0x06, 0x18, 0x06, 0x18, 0x06, 0x18, 0x06, 0x38,
        0x07, 0xE0, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x0E, 0x78, 0x18, 0x18, 0x38, 0x1C, 0x30, 0x0C, 0x30, 0x0C,
        0x30, 0x0C, 0x30, 0x0C, 0x38, 0x1C, 0x18, 0x18, 0x1E, 0x70, 0x0F, 0xE0, 0x01, 0xC0, 0x00, 0xE0,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x06, 0x70, 0x06, 0x30, 0x06, 0x30, 0x06, 0x30, 0x06, 0x70,
        0x07, 0xC0, 0x06, 0x60, 0x06, 0x60, 0x06, 0x30, 0x06, 0x30, 0x06, 0x38, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x0E, 0x20, 0x0C, 0x00, 0x0C, 0x00, 0x0E, 0x00, 0x07, 0x80,
        0x03, 0xE0, 0x00, 0xF0, 0x00, 0x70, 0x00, 0x30, 0x0C, 0x70, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
        0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x38, 0x0E, 0x70, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x30, 0x18, 0x38, 0x38, 0x18, 0x30, 0x18, 0x70, 0x1C, 0x60, 0x0C, 0x60,
        0x0C, 0xE0, 0x0E, 0xC0, 0x07, 0xC0, 0x07, 0x80, 0x07, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xC1, 0x83, 0xE3, 0xC7, 0xE3, 0xC7, 0x63, 0xC6, 0x67, 0xE6, 0x66, 0x66,
        0x76, 0x6E, 0x36, 0x6C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x1C, 0x38, 0x0C, 0x30, 0x0E, 0x70, 0x06, 0x60, 0x03, 0xC0, 0x03, 0xC0,
        0x03, 0xC0, 0x03, 0xC0, 0x06, 0x60, 0x0E, 0x70, 0x0C, 0x30, 0x1C, 0x38, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x1C, 0x38, 0x1C, 0x38, 0x0E, 0x70, 0x0E, 0x70, 0x07, 0x60, 0x07, 0xE0,
        0x03, 0xC0, 0x03, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x30, 0x00, 0x30, 0x00, 0x60, 0x00, 0xC0, 0x01, 0x80,
        0x01, 0x80, 0x03, 0x00, 0x06, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x04, 0x00, 0x06, 0x00, 0x02, 0x00, 0x03, 0x00,
        0x03, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x80, 0x00, 0xC0, 0x00, 0x40, 0x00, 0x60, 0x00, 0x60,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x01, 0xC0, 0x03, 0xC0, 0x03, 0x60, 0x06, 0x60, 0x04, 0x30,
        0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x04, 0xE0, 0x00, 0x60,
        0x03, 0xE0, 0x0F, 0xE0, 0x0C, 0x60, 0x0C, 0x60, 0x0E, 0xE0, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0D, 0xF0, 0x0F, 0x78, 0x0E, 0x38,
        0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0E, 0x38, 0x0E, 0x70, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x03, 0xA0, 0x07, 0x00,
        0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x07, 0x00, 0x03, 0x20, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x07, 0xB0, 0x06, 0x70, 0x0E, 0x30,
        0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0E, 0x70, 0x0E, 0x70, 0x07, 0xB0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x06, 0x70, 0x0E, 0x30,
        0x0C, 0x30, 0x0F, 0xF0, 0x0C, 0x00, 0x0E, 0x00, 0x07, 0x20, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xE0, 0x01, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x03, 0xE0, 0x01, 0x80, 0x01, 0x80,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF0, 0x06, 0x70, 0x0E, 0x30,
        0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0E, 0x70, 0x0E, 0x70, 0x07, 0xB0, 0x00, 0x30, 0x00, 0x70,
        0x00, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0D, 0xE0, 0x0E, 0x70, 0x0E, 0x30,
        0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
        0x00, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x70, 0x0C, 0xE0, 0x0D, 0xC0,
        0x0F, 0x80, 0x0F, 0x00, 0x0D, 0x80, 0x0D, 0xC0, 0x0C, 0xE0, 0x0C, 0x70, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0x78, 0x73, 0xDC, 0x61, 0x8C,
        0x61, 0x8C, 0x61, 0x8C, 0x61, 0x8C, 0x61, 0x8C, 0x61, 0x8C, 0x61, 0x8C, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0xE0, 0x0E, 0x70, 0x0E, 0x30,
        0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x07, 0x70, 0x0E, 0x38,
        0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0E, 0x38, 0x07, 0x70, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0xF0, 0x0F, 0x38, 0x0E, 0x38,
        0x0C, 0x18, 0x0C, 0x18, 0x0C, 0x18, 0x0E, 0x38, 0x0E, 0x70, 0x0F, 0xE0, 0x0C, 0x00, 0x0C, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xB0, 0x06, 0x70, 0x0E, 0x30,
        0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0E, 0x70, 0x0E, 0x70, 0x07, 0xB0, 0x00, 0x30, 0x00, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x03, 0x80, 0x03, 0x80,
        0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x07, 0x20, 0x06, 0x00,
        0x07, 0x00, 0x03, 0xC0, 0x01, 0xE0, 0x00, 0x60, 0x04, 0xE0, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x03, 0xE0, 0x01, 0x80, 0x01, 0x80,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0xC0, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30,
        0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0E, 0x70, 0x07, 0xB0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x30, 0x0E, 0x70, 0x06, 0x60,
        0x06, 0x60, 0x06, 0x60, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x04, 0x73, 0x8C, 0x33, 0x8C,
        0x33, 0xCC, 0x33, 0xC8, 0x1E, 0x58, 0x1E, 0x78, 0x1E, 0x78, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x70, 0x06, 0x60, 0x07, 0xE0,
        0x03, 0xC0, 0x01, 0x80, 0x03, 0xC0, 0x07, 0xE0, 0x06, 0x60, 0x0E, 0x70, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x30, 0x0E, 0x70, 0x06, 0x60,
        0x06, 0x60, 0x06, 0xE0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0x80, 0x01, 0x80, 0x01, 0x80, 0x03, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x60, 0x00, 0xE0,
        0x00, 0xC0, 0x01, 0x80, 0x03, 0x00, 0x07, 0x00, 0x06, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x80,
        0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x03, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0F, 0x90, 0x09, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* #################################################################################################### 
   API
####################################################################################################### */

/************************************************************
Name: syslog_print
Func: print driver log to /var/log/messages
Para:
char *format - output format string
 *************************************************************/
void syslog_printf(char *format, ...)
{
    char buffer[AM335X_SYSLOG_BUF_SIZE] = {0};

	/*定义一个指向个数可变的参数列表指针*/
	va_list args;
    int result;

	/*使args指针指向format可变参数列表*/
    va_start(args, format);

    result = vsnprintf(buffer, AM335X_SYSLOG_BUF_SIZE, format, args);

	/*清空参数列表*/
    va_end (args);

    openlog("am335x driver", LOG_CONS | LOG_PID, 0);

    if((result < 0) ||  (result > AM335X_SYSLOG_BUF_SIZE))
    {
            syslog(LOG_USER | LOG_DEBUG, "syslog_printf format failed: %d", result);
    }
    else
    {
            syslog(LOG_USER | LOG_DEBUG, "%s", buffer);
    }

    closelog();
}



/*************************************************************
Name: am335x_set_time
Func: set system time and rtc time
Para:
struct tm *time - time struct
 *************************************************************/
#if 1
int am335x_set_time(struct tm *time)
{
        struct rtc_time rtc_tm;
        struct timeval tv; 
        int rtc;
        int err;

        am335x_openlog("am335x driver");

        rtc = open("/dev/rtc0", O_RDONLY);

        if(rtc < 0)
        {
                am335x_printlog("am335x_set_time: /dev/rtc0 open failed\r\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        time->tm_year -= 1900;
        time->tm_mon  -= 1;

        rtc_tm.tm_year  = time->tm_year;
        rtc_tm.tm_mon   = time->tm_mon;
        rtc_tm.tm_mday  = time->tm_mday;
        rtc_tm.tm_hour  = time->tm_hour;
        rtc_tm.tm_min   = time->tm_min;
        rtc_tm.tm_sec   = time->tm_sec;
        rtc_tm.tm_isdst = time->tm_isdst = 0;

        tv.tv_sec  = mktime(time);
        tv.tv_usec = 0;

        err = settimeofday(&tv, NULL);

        if(err < 0)
        {
                close(rtc);

                am335x_printlog("am335x_set_time: settimeofday return error, err = %d\r\n", err);
                am335x_closelog("am335x driver");

                return -2;
        }

        err = ioctl(rtc, RTC_SET_TIME, &rtc_tm);

        if(err < 0)
        {
                close(rtc);

                am335x_printlog("am335x_set_time: /dev/rtc0 set time failed, err = %d\r\n", err);
                am335x_closelog("am335x driver");

                return -3;
        }

        close(rtc);

        am335x_closelog("am335x driver");

        return 0;
}
#endif

#if 0
int am335x_set_time()
{
        struct rtc_time rtc_tm;
        struct timeval tv; 
        int rtc;
        int err;

        am335x_openlog("am335x driver");

        rtc = open("/dev/rtc0", O_RDONLY);

        if(rtc < 0)
        {
                am335x_printlog("am335x_set_time: /dev/rtc0 open failed\r\n");
                am335x_closelog("am335x driver");

                return -1;
        }


        err = settimeofday(&tv, NULL);

        if(err < 0)
        {
                close(rtc);

                am335x_printlog("am335x_set_time: settimeofday return error, err = %d\r\n", err);
                am335x_closelog("am335x driver");

                return -2;
        }

        err = ioctl(rtc, RTC_SET_TIME, &rtc_tm);

        if(err < 0)
        {
                close(rtc);

                am335x_printlog("am335x_set_time: /dev/rtc0 set time failed, err = %d\r\n", err);
                am335x_closelog("am335x driver");

                return -3;
        }

        close(rtc);

        am335x_closelog("am335x driver");

        return 0;
}
#endif


/************************************************************
Name: am335x_get_time
Func: get system time
Para:
struct tm *time - time struct
 *************************************************************/
int am335x_get_time(struct tm *time)
{
        struct timeval tv;
        time_t now;

        am335x_openlog("am335x driver");

        gettimeofday(&tv, NULL);

        now = tv.tv_sec;

        if(NULL== gmtime_r(&now, time))
        {
                am335x_printlog("am335x_get_time: gmtime_r return NULL\r\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        time->tm_year += 1900;
        time->tm_mon  += 1;

        am335x_closelog("am335x driver");

        return 0;
}
int am335x_hih6130_read(void)
{
        int fd,i;
        char buffer[100];
        am335x_openlog("am335x driver");
        fd = open("/dev/i2c-0", O_RDONLY);
        if(fd < 0)
        {
                am335x_printlog("am335x_hih6130_read: /dev/i2c-0 open failed\r\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        ioctl(fd, I2C_TENBIT, 0);

        if(ioctl(fd, I2C_SLAVE_FORCE,  0x27) < 0)
        {
                close (fd);

                am335x_printlog("am335x_hih6130_read: /dev/i2c-0 set chip addr failed\r\n");
                am335x_closelog("am335x driver");

                return -4;
        }

        read(fd, buffer, 100);
        for(i = 0; i < 100; i++)
        {
                //printf("test ------------------- value is %x \n",buffer[i]);
        }
        return 0;
}
/************************************************************
Name: am335x_eeprom_write
Func: write eeprom
Para:
int   chip    - 0,1,2,3
int   address - 0 ~ 255
char *buffer  - write data buffer
buffer[0] should be reserve for driver
int   len     - write data length
 *************************************************************/
int am335x_eeprom_write(int chip, int address, char *buffer, int len)
{
        char page[AM335X_EEPROM_PAGESIZE + 1];
        int  total_len;
        int  wr_len;
        int  fd;

        if(((address + len) > AM335X_EEPROM_TOTALSIZE) || (buffer == NULL))
        {
                return -1;
        }

        am335x_openlog("am335x driver");

        fd = open("/dev/i2c-0", O_RDWR);

        if(fd < 0)
        {
                am335x_printlog("am335x_eeprom_write: /dev/i2c-0 open failed\r\n");
                am335x_closelog("am335x driver");

                return -2;
        }

#if(AM335X_BOARD_VERSION != 0x10)
        if(am335x_gpio_write(AM335X_GPIO_EEP_WRP, 0) < 0)
        {
                am335x_printlog("am335x_eerprom_write: disable eeprom write protect failed\r\n");
                am335x_closelog("am335x driver");

                return -3;
        }
#endif

        ioctl(fd, I2C_TENBIT, 0);

        if(ioctl(fd, I2C_SLAVE_FORCE,  AM335X_EEPROM_ADDRESS + chip) < 0)
        {
                close (fd);

                am335x_printlog("am335x_eerprom_write: /dev/i2c-0 set chip addr failed\r\n");
                am335x_closelog("am335x driver");

                return -4;
        }

        total_len = 0;

        while(total_len < len)
        {
                wr_len = AM335X_EEPROM_PAGESIZE - (address % AM335X_EEPROM_PAGESIZE);

                if(wr_len > (len - total_len))
                {
                        wr_len = len - total_len;
                }

                page[0] = address;

                memcpy(&page[1], &buffer[total_len], wr_len);

                if(write(fd, page, wr_len + 1) < 0)
                {
                        close(fd);

                        am335x_printlog("am335x_eeprom_write: /dev/i2c-0 write data failed\r\n");
                        am335x_closelog("am335x driver");

                        return -5;
                }

                address   += wr_len;
                total_len += wr_len;

                usleep(10000);
        }

#if(AM335X_BOARD_VERSION != 0x10)
        am335x_gpio_write(AM335X_GPIO_EEP_WRP, 1); 
#endif

        close(fd);

        am335x_closelog("am335x driver");

        return 0;
}

/************************************************************
Name: am335x_eeprom_read
Func: read eeprom
Para:
int   chip    - 0,1,2,3
int   address - 0 ~ 255
char *buffer  - read data buffer
buffer[0] should be reserve for driver
int   len     - read data length
 *************************************************************/
int am335x_eeprom_read(int chip, int address, char *buffer, int len)
{
        char page[AM335X_EEPROM_PAGESIZE + 1];
        int  fd;

        if(((address + len) > AM335X_EEPROM_TOTALSIZE) || (buffer == NULL))
        {
                return -1;
        }

        am335x_openlog("am335x driver");

        fd = open("/dev/i2c-0", O_RDWR);

        if(fd < 0)
        {
                am335x_printlog("am335x_eeprom_read: /dev/i2c-0 open failed\r\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        page[0] = address % 256;

        ioctl(fd, I2C_TENBIT, 0);

        if(ioctl(fd, I2C_SLAVE_FORCE,  AM335X_EEPROM_ADDRESS + chip) < 0)
        {
                close (fd);

                am335x_printlog("am335x_eerprom_read: /dev/i2c-0 set chip addr failed\r\n");
                am335x_closelog("am335x driver");

                return -3;
        }

        if(write(fd, page, 1) < 0)
        {
                close(fd);

                am335x_printlog("am335x_eeprom_read: /dev/i2c-0 write addr failed\r\n");
                am335x_closelog("am335x driver");

                return -4;
        }

        if(read(fd, buffer, len) < 0)
        {
                close(fd);

                am335x_printlog("am335x_eeprom_read: /dev/i2c-0 read data failed\r\n");
                am335x_closelog("am335x driver");

                return -5;
        }

        close(fd);

        am335x_closelog("am335x driver");

        return 0;
}

/************************************************************
Name: am335x_tps65127_reg_write
Func: write tps65217 reg
Para:
char   reg    - 0 ~ 255
char   value  - 0 ~ 255
 *************************************************************/
int am335x_tps65127_reg_write(char reg, char value)
{
        char buffer[2];
        int  fd;

        am335x_openlog("am335x driver");

        fd = open("/dev/i2c-0", O_RDWR);

        if(fd < 0)
        {
                am335x_printlog("am335x_tps65127_reg_write: /dev/i2c-0 open failed\r\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        ioctl(fd, I2C_TENBIT, 0);

        if(ioctl(fd, I2C_SLAVE_FORCE,  AM335X_TPS65217_ADDRESS) < 0)
        {
                close(fd);

                am335x_printlog("am335x_tps65127_reg_write: /dev/i2c-0 set chip addr failed\r\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        buffer[0] = reg;
        buffer[1] = value;

        if(write(fd, buffer, 2) < 0)
        {
                close(fd);

                am335x_printlog("am335x_tps65127_reg_write: /dev/i2c-0 write data failed\r\n");
                am335x_closelog("am335x driver");

                return -3;
        }

        close(fd);

        am335x_closelog("am335x driver");

        return 0;
}

/************************************************************
Name: am335x_tps65127_reg_read
Func: read tps65217 reg
Para:
char   reg    - 0 ~ 255
char*  value  - 0 ~ 255
 *************************************************************/
int am335x_tps65127_reg_read(char reg, char *value)
{
        int fd;

        am335x_openlog("am335x driver");

        fd = open("/dev/i2c-0", O_RDWR);

        if(fd < 0)
        {
                am335x_printlog("am335x_tps65217_reg_read: /dev/i2c-0 open failed\r\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        ioctl(fd, I2C_TENBIT, 0);

        if(ioctl(fd, I2C_SLAVE_FORCE,  AM335X_TPS65217_ADDRESS) < 0)
        {
                close (fd);

                am335x_printlog("am335x_tps65217_reg_read: /dev/i2c-0 set chip addr failed\r\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        if(write(fd, &reg, 1) < 0)
        {
                close(fd);

                am335x_printlog("am335x_tps65217_reg_read: /dev/i2c-0 write addr failed\r\n");
                am335x_closelog("am335x driver");

                return -3;
        }

        if(read(fd, value, 1) < 0)
        {
                close(fd);

                am335x_printlog("am335x_tps65217_reg_read: /dev/i2c-0 read data failed\r\n");
                am335x_closelog("am335x driver");

                return -4;
        }

        close(fd);

        am335x_closelog("am335x driver");

        return 0;

}

/************************************************************
Name: am335x_bit_invert
Func: msb to lsb, lsb to msb
Para:
char c - char to inverted
 *************************************************************/
char am335x_bit_invert(char c)
{
        c = (c & 0xaa) >> 1 | (c & 0x55) << 1;
        c = (c & 0xcc) >> 2 | (c & 0x33) << 2;
        c = (c & 0xf0) >> 4 | (c & 0x0f) << 4;

        return c;
}

/************************************************************
Name: am335x_lcd_display
Func: make LCD to display its memory data
Para:
none
 *************************************************************/
int am335x_lcd_display()
{
        int mode;
        int fd;
        int i;

        am335x_openlog("am335x driver");

        fd = open("/dev/ls013b7dh", O_WRONLY);

        if(fd < 0)
        {
                am335x_printlog("am335x_lcd_display: /dev/ls013b7dh open failed\e\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        mode = SPI_CS_HIGH;

        if(ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0)
        {
                close(fd);

                am335x_printlog("am335x_lcd_dispaly: ioctl set spi mode failed\e\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        for(i = 0; i < AM335X_LCD_HEIGHT; i++)
        {
                lcd_buffer[i][0] = 0x00;
                lcd_buffer[i][1] = am335x_bit_invert(i + 1);
        }

        lcd_buffer[0][0] = 0x80;

        lcd_buffer[AM335X_LCD_HEIGHT][0] = 0x00;
        lcd_buffer[AM335X_LCD_HEIGHT][1] = 0x00;

        if(write(fd, lcd_buffer, AM335X_LCD_HEIGHT * (AM335X_LCD_WIDTH / 8 + AM335X_LCD_CMD) + 2) < 0)
        {
                close(fd);

                am335x_printlog("am335x_lcd_display: /dev/ls013b7dh update failed\r\n");
                am335x_closelog("am335x driver");

                return -3;
        }

        if(write(fd, lcd_buffer[AM335X_LCD_HEIGHT], AM335X_LCD_CMD) < 0)
        {
                close(fd);

                am335x_printlog("am335x_lcd_display: /dev/ls013b7dh display failed\r\n");
                am335x_closelog("am335x driver");

                return -4;
        }

        close(fd);

        am335x_closelog("am335x driver");

        return 0;
}

/************************************************************
Name: am335x_lcd_drawASC
Func: draw an ascii char on LCD, after this function called,
am335x_lcd_display should be called to display
Para:
int  x - 0 ~ 7
int  y - 0 ~ 127
char c - char to display
 *************************************************************/
int am335x_lcd_drawASC(int x, int y, char c)
{
        int nx,ny;
        int pos;
        int i,j;

        if((x < 0) || (x > (AM335X_LCD_WIDTH / AM335X_ASCII_WIDTH - 1)) || (y < 0) || (y > (AM335X_LCD_HEIGHT / AM335X_ASCII_HEIGHT - 1)))
        {
                return -1;
        }

        if((c < 0x20) || (c > 0x7f))
        {
                return -2;
        }

        pos = (c - 0x20) * AM335X_ASCII_WIDTH * AM335X_ASCII_HEIGHT / 8;

        for(i = 0; i < AM335X_ASCII_HEIGHT; i++)
        {
                for(j = 0; j < AM335X_ASCII_WIDTH / 8; j++)
                {
                        ny = (AM335X_LCD_HEIGHT - 1) - y * AM335X_ASCII_HEIGHT - i;
                        nx = AM335X_LCD_CMD + ((AM335X_LCD_WIDTH - 1) - x * AM335X_ASCII_WIDTH) / 8 - j;

                        lcd_buffer[ny][nx] = am335x_bit_invert(~ascii_16x16_table[pos + i * AM335X_ASCII_WIDTH / 8 + j]);
                }
        }

        return 0;
}

/************************************************************
Name: am335x_lcd_drawCHS
Func: draw an unicode char on LCD, after this function called,
am335x_lcd_display should be called to display
Para:
int   x    - TBD
int   y    - 0 ~ 127
char *code - unicode to display
 *************************************************************/
int am335x_lcd_drawCHS(int x, int y, char *code)
{
        return 0;
}

/************************************************************
Name: am335x_lcd_clear
Func: clear LCD to white
Para:
none
 *************************************************************/
int am335x_lcd_clear()
{
        int mode;
        int fd;
        int i;

        am335x_openlog("am335x driver");

        fd = open("/dev/ls013b7dh", O_WRONLY);

        if(fd < 0)
        {
                am335x_printlog("am335x_lcd_clear: /dev/ls013b7dh open failed\e\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        mode = SPI_CS_HIGH;

        if(ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0)
        {
                close(fd);

                am335x_printlog("am335x_lcd_clear: ioctl set spi mode failed\e\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        memset(lcd_buffer, 0xff, sizeof(lcd_buffer));

        lcd_buffer[AM335X_LCD_HEIGHT][0] = 0x20;
        lcd_buffer[AM335X_LCD_HEIGHT][1] = 0x00;

        if(write(fd, lcd_buffer[AM335X_LCD_HEIGHT], AM335X_LCD_CMD) < 0)
        {
                close(fd);

                am335x_printlog("am335x_lcd_clear: /dev/ls013b7dh write failed\r\n");
                am335x_closelog("am335x driver");

                return -3;
        }

        close(fd);

        am335x_closelog("am335x driver");

        return 0; 
}

/************************************************************
Name: am335x_gpio_write
Func: gpio pin write
Para:
int pin   - GPIO BANK0 is 0 ~ 31, GPIO BANK1 is 32 ~ 63 ...
int value - 0 or 1
 *************************************************************/
int am335x_gpio_write(int pin, int value)
{
        char *gpio_regs;
        int mem_fd;
        int gpio;
        int bit;

        am335x_openlog("am335x driver");

        mem_fd = open("/dev/mem", O_RDWR|O_SYNC);

        if(mem_fd < 0)
        {
                am335x_printlog("am335x_gpio_write: /dev/mem open failed\r\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        gpio = pin / 32;
        bit  = pin % 32;

        if(gpio == 0)
        {
                gpio_regs = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x44e07000);
        }
        else if(gpio == 1)
        {
                gpio_regs = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x4804c000);
        }
        else if(gpio == 2)
        {
                gpio_regs = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x481ac000);
        }
        else
        {
                close(mem_fd);

                am335x_printlog("am335x_gpio_write: gpio pin number wrong\r\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        if(gpio_regs == (char*)MAP_FAILED)
        {
                close(mem_fd);

                am335x_printlog("am335x_gpio_write: mmap gpio%d regs failed\r\n", gpio);
                am335x_closelog("am335x driver");

                return -3;
        }

        if(value != 0)
        {
                *(volatile unsigned int*)(gpio_regs + 0x194) = (1 << bit);
        }
        else
        {
                *(volatile unsigned int*)(gpio_regs + 0x190) = (1 << bit);
        }

        munmap(gpio_regs, 0x1000);

        close(mem_fd);

        return 0;
}

/************************************************************
Name: am335x_gpio_read
Func: gpio pin read
Para:
int pin   - GPIO BANK0 is 0 ~ 31, GPIO BANK1 is 32 ~ 63 ...
 *************************************************************/
int am335x_gpio_read(int pin)
{
        char *gpio_regs;
        int mem_fd;
        int value;
        int gpio;
        int bit;

        am335x_openlog("am335x driver");

        mem_fd = open("/dev/mem", O_RDWR|O_SYNC);

        if(mem_fd < 0)
        {
                am335x_printlog("am335x_gpio_write: /dev/mem open failed\r\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        gpio = pin / 32;
        bit  = pin % 32;

        if(gpio == 0)
        {
                gpio_regs = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x44e07000);
        }
        else if(gpio == 1)
        {
                gpio_regs = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x4804c000);
        }
        else if(gpio == 2)
        {
                gpio_regs = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x481ac000);
        }
        else
        {
                close(mem_fd);

                am335x_printlog("am335x_gpio_write: gpio pin number wrong\r\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        if(gpio_regs == (char*)MAP_FAILED)
        {
                close(mem_fd);

                am335x_printlog("am335x_gpio_write: mmap gpio%d regs failed\r\n", gpio);
                am335x_closelog("am335x driver");

                return -3;
        }

        if(*(volatile unsigned int*)(gpio_regs + 0x134) & (1 << bit))
        {
                value = *(volatile unsigned int*)(gpio_regs + 0x138);
        }
        else
        {
                value = *(volatile unsigned int*)(gpio_regs + 0x13C);
        }

        munmap(gpio_regs, 0x1000);

        close(mem_fd);

        return ((value >> bit) & 1);
}

/************************************************************
Name: am335x_gpio_show
Func: gpio show
Para:
none
 *************************************************************/
int am335x_gpio_show()
{
        char *gpio_regs[4];
        char *ctrl_regs;
        int mem_fd;
        int gpio;
        int bit;

        mem_fd = open("/dev/mem", O_RDWR|O_SYNC);

        if(mem_fd < 0)
        {
                printf("am335x_gpio_show: /dev/mem open failed\r\n");

                return -1;
        }

        ctrl_regs = (char*)mmap(0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, CM_PER);

        if(ctrl_regs == (char*)MAP_FAILED)
        {
                printf("am335x_gpio_show: mmap ctrl regs failed\r\n");

                close(mem_fd);

                return -2;

        }

        printf("CTRL : GPIO0 CLOCK CTRL     = %08x\r\n", *(volatile unsigned int*)(ctrl_regs + GPIO0_CLOCK_CTRL));
        printf("CTRL : GPIO1 CLOCK CTRL     = %08x\r\n", *(volatile unsigned int*)(ctrl_regs + GPIO1_CLOCK_CTRL));
        printf("CTRL : GPIO2 CLOCK CTRL     = %08x\r\n", *(volatile unsigned int*)(ctrl_regs + GPIO2_CLOCK_CTRL));
        printf("CTRL : GPIO3 CLOCK CTRL     = %08x\r\n", *(volatile unsigned int*)(ctrl_regs + GPIO3_CLOCK_CTRL));
        printf("\n\n");

        bit = *(volatile unsigned int*)(ctrl_regs + GPIO0_CLOCK_CTRL);
        if((bit & 0x00000003) != 0x00000002)
        {
                *(volatile unsigned int*)(ctrl_regs + GPIO0_CLOCK_CTRL) |= 0x00000002;
        }

        bit = *(volatile unsigned int*)(ctrl_regs + GPIO1_CLOCK_CTRL);
        if((bit & 0x00000003) != 0x00000002)
        {
                *(volatile unsigned int*)(ctrl_regs + GPIO1_CLOCK_CTRL) |= 0x00000002;
        }

        bit = *(volatile unsigned int*)(ctrl_regs + GPIO2_CLOCK_CTRL);
        if((bit & 0x00000003) != 0x00000002)
        {
                *(volatile unsigned int*)(ctrl_regs + GPIO2_CLOCK_CTRL) |= 0x00000002;
        }

        bit = *(volatile unsigned int*)(ctrl_regs + GPIO3_CLOCK_CTRL);
        if((bit & 0x00000003) != 0x00000002)
        {
                *(volatile unsigned int*)(ctrl_regs + GPIO3_CLOCK_CTRL) |= 0x00000002;
        }

        munmap(ctrl_regs, 0x20000);

        gpio_regs[0] = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, GPIO0_REG);

        if(gpio_regs[0] == (char*)MAP_FAILED)
        {
                printf("am335x_gpio_show: mmap gpio0 regs failed\r\n");

                close(mem_fd);

                return -3;
        }

        gpio_regs[1] = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, GPIO1_REG);

        if(gpio_regs[1] == (char*)MAP_FAILED)
        {
                printf("am335x_gpio_show: mmap gpio1 regs failed\r\n");

                munmap(gpio_regs[0], 0x1000);

                close(mem_fd);

                return -4;
        }

        gpio_regs[2] = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, GPIO2_REG);

        if(gpio_regs[2] == (char*)MAP_FAILED)
        {
                printf("am335x_gpio_show: mmap gpio2 regs failed\r\n");

                munmap(gpio_regs[0], 0x1000);

                munmap(gpio_regs[1], 0x1000);

                close(mem_fd);

                return -5;
        }

        gpio_regs[3] = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, GPIO3_REG);

        if(gpio_regs[3] == (char*)MAP_FAILED)
        {
                printf("am335x_gpio_show: mmap gpio2 regs failed\r\n");

                munmap(gpio_regs[0], 0x1000);

                munmap(gpio_regs[1], 0x1000);

                munmap(gpio_regs[2], 0x1000);

                close(mem_fd);

                return -6;
        }

        for(gpio = 0; gpio < 4; gpio++)
        {
                printf("GPIO%d: GPIO_IRQSTATUS_0     = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_IRQSTATUS_0));
                printf("GPIO%d: GPIO_IRQSTATUS_1     = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_IRQSTATUS_1));
                printf("GPIO%d: GPIO_IRQSTATUS_SET_0 = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_IRQSTATUS_SET_0));
                printf("GPIO%d: GPIO_IRQSTATUS_SET_1 = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_IRQSTATUS_SET_1));
                printf("GPIO%d: GPIO_CTRL            = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_CTRL));
                printf("GPIO%d: GPIO_OE              = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_OE));
                printf("GPIO%d: GPIO_DATAIN          = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_DATAIN));
                printf("GPIO%d: GPIO_DATAOUT         = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_DATAOUT));
                printf("GPIO%d: GPIO_LEVELDETECT0    = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_LEVELDETECT0));
                printf("GPIO%d: GPIO_LEVELDETECT1    = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_LEVELDETECT1));
                printf("GPIO%d: GPIO_RISINGDETECT    = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_RISINGDETECT));
                printf("GPIO%d: GPIO_FALLINGDETECT   = %08x\r\n", gpio, *(volatile unsigned int*)(gpio_regs[gpio] + GPIO_FALLINGDETECT));
                printf("\r\n\r\n");
        }

        munmap(gpio_regs[0], 0x1000);
        munmap(gpio_regs[1], 0x1000);
        munmap(gpio_regs[2], 0x1000);
        munmap(gpio_regs[3], 0x1000);

        close(mem_fd);

        return 0;
}

/* #################################################################################################### 
   INTER API
####################################################################################################### */

/************************************************************
Name: am335x_sensor_checksum8bit
Func: calculate sensor data 8 bit checksum
Para:
unsigned char *buffer - data
unsigned int   len    - length
 *************************************************************/
unsigned char am335x_sensor_checksum8bit(unsigned char *buffer, unsigned int len)
{
        unsigned char sum;
        int i;

        sum = 0;

        for(i = 0; i < len; i++)
        {
                sum += buffer[i];        
        }

        return sum;
}

/************************************************************
Name: am335x_sensor_checksum16bit
Func: calculate sensor data 16 bit checksum
Para:
unsigned char *buffer - data
unsigned int len - length
 *************************************************************/
unsigned short am335x_sensor_checksum16bit(unsigned char *buffer, unsigned int len)
{
        return 0;
}

/************************************************************
Name: am335x_bcd2dec
Func: BCD to DEC
Para:
unsigned char *buffer - data
unsigned int len - length
 *************************************************************/
unsigned int am335x_bcd2dec(unsigned char *buffer, unsigned int len)
{
        unsigned int i, sum = 0;

        for(i = 0; i < len; i++)
        {
                sum = sum * 100 + ((buffer[i] & 0xF0) >> 4) * 10 + (buffer[i] & 0x0F);
        }

        return sum;
}

/************************************************************
Name: am335x_uart_set_opt
Func: set serial options
Para:
int  fd      - uart file descripter
int  speed   - baud rate
int  bits    - 7 or 8
char parity  - 'E' - even, 'O' - odd, 'N' - none
int  stop    - 1 or 2
int  bytes   - how many bytes received when read return 
int  timeout - how many deciseconds passed when read return
 *************************************************************/
int am335x_uart_set_opt(int fd, int speed, int bits, char parity, int stop, int bytes, int timeout)
{
		/*termios :操作终端接口结构体*/
		struct termios newtio, oldtio;

		/*tcgetattr :获取终端参数函数*/
        if(tcgetattr(fd, &oldtio) != 0)
        {
                am335x_printlog("am335x_uart_set_opt: tcgetattr return error\r\n");

                return -1;
        }

        memset(&newtio, 0, sizeof(newtio));

		/*c_cflag : 控制模式标志*/
		/*CLOCAL :忽略调制解调器线路状态*/
		/*CREAD  : 使用接收器*/
		/*CSIZE   : 字符长度*/
        newtio.c_cflag  |= CLOCAL | CREAD;
        newtio.c_cflag  &= ~CSIZE;/*用数据位掩码清空数据位设置*/

        switch(bits)
        {
        		/*设置数据位*/
                case 7:
                        newtio.c_cflag |= CS7;
                        break;

                case 8:
                        newtio.c_cflag |= CS8;
                        break;

                default:
                        newtio.c_cflag |= CS8;
        }

        switch(parity)
        {
                case 'O':
                case 'o':
                        newtio.c_cflag |= PARENB;/*使用奇偶校验*/
                        newtio.c_cflag |= PARODD;/*对输入使用奇偶校验，对输出使用偶校验*/
                        newtio.c_iflag |= (INPCK | ISTRIP);/*允许输入奇偶校验，去除字符的第8个比特*/
                        break;

                case 'E':
                case 'e':
                        newtio.c_iflag |= (INPCK | ISTRIP);
                        newtio.c_cflag |= PARENB;
                        newtio.c_cflag &= ~PARODD;
                        break;

                case 'N':
                case 'n':
                        newtio.c_cflag &= ~PARENB;
                        break;

                default:
                        newtio.c_cflag &= ~PARENB;
        }

		/*设置波特率*/
        switch(speed)
        {
                case 2400:
                        cfsetispeed(&newtio, B2400);
                        cfsetospeed(&newtio, B2400);
                        break;

                case 4800:
                        cfsetispeed(&newtio, B4800);
                        cfsetospeed(&newtio, B4800);
                        break;

                case 9600:
                        cfsetispeed(&newtio, B9600);
                        cfsetospeed(&newtio, B9600);
                        break;

                case 19200:
                        cfsetispeed(&newtio, B19200);
                        cfsetospeed(&newtio, B19200);
                        break;

                case 38400:
                        cfsetispeed(&newtio, B38400);
                        cfsetospeed(&newtio, B38400);
                        break;

                case 57600:
                        cfsetispeed(&newtio, B57600);
                        cfsetospeed(&newtio, B57600);
                        break;

                case 115200:
                        cfsetispeed(&newtio, B115200);
                        cfsetospeed(&newtio, B115200);
                        break;

                default:
                        cfsetispeed(&newtio, B9600);
                        cfsetospeed(&newtio, B9600);
        }

		/*设置停止位*/
        switch(stop)
        {
                case 1:
                        newtio.c_cflag &= ~CSTOPB;
                        break;

                case 2:
                        newtio.c_cflag |= CSTOPB;
                        break;

                default:
                        newtio.c_cflag &= ~CSTOPB;
        }

		/*设置read一次读多少字节*/
        newtio.c_cc[VMIN]  = bytes;

		/*设置read超时时间*/
        newtio.c_cc[VTIME] = timeout;

		/*清除终端未完成的输入输出请求及数据*/
        tcflush(fd, TCIFLUSH);

		/*设置串口参数*/
        if((tcsetattr(fd, TCSANOW, &newtio)) != 0)
        {
                am335x_printlog("am335x_uart_set_opt: tcsetattr return error\r\n");

                return -2;
        }

        return 0;
}

/************************************************************
Name  :am335x_set_sga700_para
Func  :设置调整SO2基线值参数接口
Para  : value   - 取值范围为 0,2,10,20,30,40,50,60,70,80,90,99；
		type    - so2 no2 co o3
return: 返回0 ：调整so2基线值成功；
返回-1：参数 value 取值错误
返回-2：调整基线值命令失败
返回-3：调零命令发送失败
 *************************************************************/
int am335x_set_sga700_para(int type, int value)
{
        int shmid;
        struct am335x_so2_base_value_mem_t *am335x_so2_mem;
        shmid = shmget(AM335X_SO2_ID, sizeof(struct am335x_so2_base_value_mem_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("shmid Share Memory Get Failed\r\n");
                am335x_closelog("am335x driver");
        }

        am335x_so2_mem = (struct am335x_so2_base_value_mem_t*)shmat(shmid, 0, 0);

        if(am335x_so2_mem == (struct am335x_so2_base_value_mem_t*)(-1))
        { 
                am335x_printlog("am335x_so2_mem Share Memory Link Failed\r\n");
                am335x_closelog("am335x driver");				
        }

        memset((char*)(am335x_so2_mem), 0x01, sizeof(struct am335x_so2_base_value_mem_t));
        while(1)
        {       
                am335x_so2_mem->type  = type;
                am335x_so2_mem->value = value;
                am335x_so2_mem->flag  = 1;

                if(am335x_so2_mem->retur != 0x01010101)
                {
                        break;
                }
                sleep(1);
        }

        if(am335x_so2_mem->retur == -1)
        {
                am335x_so2_mem->flag  = 0;
                shmdt((char*)am335x_so2_mem);
                return -1;
        }
        else if(am335x_so2_mem->retur == -2)
        {
                am335x_so2_mem->flag  = 0;
                shmdt((char*)am335x_so2_mem);
                return -2;
        } 
        else if(am335x_so2_mem->retur == -3)
        {
                am335x_so2_mem->flag  = 0;
                shmdt((char*)am335x_so2_mem);
                return -3;
        } 
        else if(am335x_so2_mem->retur == 0)
        {
                am335x_so2_mem->flag  = 0;
                shmdt((char*)am335x_so2_mem);
                return 0;
        }    

}

/***********************************************************
Name  :am335x_set_sga700_para
Func  :读SGA700传感器基线值
Para  : fd      - 串口文件描述符
		type    - so2 no2 co o3
		struct am335x_so2_base_value_mem_t *am335x_so2_mem 共享内存结构指针
return: 返回0 ：读取基准值成功；
        返回-1：读取基准值错误；
***********************************************************/
int am335x_read_sga700_so2_base_value(int fd, int type, struct am335x_so2_base_value_mem_t *am335x_so2_mem)
{
		int i,len,ret;
        char buffer[256]; 
        if(1 == (am335x_so2_mem->read_b_value_flag))
        {
		    memset(buffer, 0, sizeof(buffer));
		    
			buffer[0]  = 0xFF;
			buffer[1]  = 0xFF;
			buffer[2]  = 0x01;
			buffer[3]  = 0xFE;
			buffer[4]  = 0x05;
			buffer[5]  = 0x01;
			buffer[6]  = 0x00;
			buffer[7]  = 0x8f;
			buffer[8]  = 0x03;
			buffer[9]  = 0x93;
			buffer[10] = 0xdd;
		
			write(fd, buffer, 11);
		    memset(buffer, 0, sizeof(buffer));
		    len = read(fd, buffer, sizeof(buffer));
		    for(i = 0; i < len; i++)
		    {
		            printf("RECV read so2 base value CMD: buffer[%d] = %x\n",i,buffer[i]);
		    }

		    if(0xA1 == buffer[5])
		    {
		    		switch(buffer[11])
		    		{
		    			case 0:
		    				am335x_so2_mem->so2_b_value = 0;
		    			break;
	 
		     			case 0x02:
		    				am335x_so2_mem->so2_b_value = 2;
		    			break;       			
	 
		     			case 0x10:
		    				am335x_so2_mem->so2_b_value = 10;
		    			break;
		    			
		    			case 0x20:
		    				am335x_so2_mem->so2_b_value = 20;
		    			break;
		    			
		    			case 0x30:
		    				am335x_so2_mem->so2_b_value = 30;
		    			break;   
		    			
		     			case 0x40:
		    				am335x_so2_mem->so2_b_value = 40;
		    			break; 
		    			
		    			case 0x50:
		    				am335x_so2_mem->so2_b_value = 50;
		    			break; 
		    			
		    			case 0x60:
		    				am335x_so2_mem->so2_b_value = 60;
		    			break;
		    			
		    			case 0x70:
		    				if(0x05 == buffer[12])
		    				{
		    					am335x_so2_mem->so2_b_value = 70;//70
		    				}
		    				else if(0x15 == buffer[12])
		    				{
		    					am335x_so2_mem->so2_b_value = 80;//80
		    				}
		    			break; 
		    			
		    			case 0x90:
		    				am335x_so2_mem->so2_b_value = 90;
		    			break;
		    			 
		    			case 0x99:
		    				am335x_so2_mem->so2_b_value = 99;
		    			break; 
		    			
		    			default:
		    				printf("system error\n");        			        			        			
		    			         			        		
		    		}       		
		    }
		    else
		    {
		    	am335x_so2_mem->read_b_value_retur  = -1;
		    	return -1; /*search cmd error*/
		    }
		 }
		am335x_so2_mem->read_b_value_retur  = 0;
        return 0;
}

/************************************************************
Name  :am335x_set_sga700_para
Func  :读SGA700传感器基线值
Para  : type    - so2 no2 co o3
		b_value - 指向读取基准值的指针
return: 返回0 ：设置参数成功，*b_value为读取的基准值；
        返回-1：读取基准值失败，读取基准值失败；
************************************************************/
int am335x_set_read_sga700_gas_base_value_para(int type, int *b_value)
{
        int shmid;
        struct am335x_so2_base_value_mem_t *am335x_so2_mem;
        shmid = shmget(AM335X_SO2_ID, sizeof(struct am335x_so2_base_value_mem_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("shmid Share Memory Get Failed\r\n");
                am335x_closelog("am335x driver");
        }

        am335x_so2_mem = (struct am335x_so2_base_value_mem_t*)shmat(shmid, 0, 0);

        if(am335x_so2_mem == (struct am335x_so2_base_value_mem_t*)(-1))
        { 
                am335x_printlog("am335x_so2_mem Share Memory Link Failed\r\n");
                am335x_closelog("am335x driver");				
        }

        memset((char*)(am335x_so2_mem), 0x01, sizeof(struct am335x_so2_base_value_mem_t));
        while(1)
        {       
                am335x_so2_mem->read_b_type  = type;
                am335x_so2_mem->read_b_value_flag  = 1;

                if(am335x_so2_mem->read_b_value_retur != 0x01010101)
                {
                        break;
                }
                sleep(1);
        }

        if(am335x_so2_mem->read_b_value_retur == -1)
        {
                am335x_so2_mem->read_b_value_flag  = 0;
                *b_value = am335x_so2_mem->so2_b_value;
                shmdt((char*)am335x_so2_mem);
                return -1;
        }
        else if(am335x_so2_mem->read_b_value_retur == 0)
        {
                am335x_so2_mem->read_b_value_flag  = 0;
                *b_value = am335x_so2_mem->so2_b_value;
                shmdt((char*)am335x_so2_mem);
                return 0;
        }    
	
}
/************************************************************
Name  :am335x_set_sga700_so2_value
Func  :调整SO2基线值
Para  : value   - 取值范围为 0,2,10,20,30,40,50,60,70,80,90,99；
return: 返回0 ：调整so2基线值成功；
返回-1：参数 value 取值错误
返回-2：调整基线值命令失败
返回-3：调零命令发送失败
 *************************************************************/
int am335x_set_sga700_so2_value(int fd,int value, int flag, struct am335x_so2_base_value_mem_t * am335x_so2_base_ret )
{
        int i,len,ret;
        char buffer[256]; 
        memset(buffer, 0, sizeof(buffer));
        if(flag == 1)/*change*/
        {
                buffer[0]  = 0xFF;
                buffer[1]  = 0xFF;
                buffer[2]  = 0x01;
                buffer[3]  = 0xFE;
                buffer[4]  = 0x08;
                buffer[5]  = 0x02;
                buffer[6]  = 0x00;
                buffer[7]  = 0x8f;
                buffer[8]  = 0x03;
                buffer[9]  = 0x01;
                buffer[10] = 0x00;

                switch(value)
                {
                        case 0:
                                buffer[11] = 0x00;
                                buffer[12] = 0x95;
                                break;

                        case 2:
                                buffer[11] = 0x02;
                                buffer[12] = 0x97;
                                break;

                        case 10:
                                buffer[11] = 0x10;
                                buffer[12] = 0xA5;
                                break;

                        case 20:
                                buffer[11] = 0x20;
                                buffer[12] = 0xB5;
                                break;

                        case 30:
                                buffer[11] = 0x30;
                                buffer[12] = 0xC5;
                                break;

                        case 40:
                                buffer[11] = 0x40;
                                buffer[12] = 0xD5;
                                break;

                        case 50:
                                buffer[11] = 0x50;
                                buffer[12] = 0xE5;
                                break;

                        case 60:
                                buffer[11] = 0x60;
                                buffer[12] = 0xF5;
                                break;

                        case 70:
                                buffer[11] = 0x70;
                                buffer[12] = 0x05;
                                break;

                        case 80:
                                buffer[11] = 0x70;
                                buffer[12] = 0x15;
                                break;

                        case 90:
                                buffer[11] = 0x90;
                                buffer[12] = 0x25;
                                break;

                        case 99:
                                buffer[11] = 0x99;
                                buffer[12] = 0x2E;
                                break;

                        default:
                                printf("value is not permit, please input 0,2,10,20,30,40,50,60,70,80,90,99\n");

                                return -1; /*数据输入出错*/
                }
                buffer[13] = 0xdd;

                write(fd, buffer, 14);
                memset(buffer, 0, sizeof(buffer));
                len = read(fd, buffer, sizeof(buffer));
                for(i = 0; i < len; i++)
                {
                        printf("RECV adjust so2 base value CMD: buffer[%d] = %x\n",i,buffer[i]);
                }

                if(buffer[5] == 0xA2)
                {
                        memset(buffer, 0, sizeof(buffer));

                        buffer[0]  = 0xFF;
                        buffer[1]  = 0xFF;
                        buffer[2]  = 0x01;
                        buffer[3]  = 0xFE;
                        buffer[4]  = 0x02;
                        buffer[5]  = 0x05;
                        buffer[6]  = 0x05;
                        buffer[7]  = 0xdd;
                        write(fd, buffer, 8);
                        memset(buffer, 0, sizeof(buffer));
                        len = read(fd, buffer, sizeof(buffer));
                        for(i = 0; i < len; i++)
                        {
                                printf("RECV clear so2 value to zero CMD: buffer[%d] = %x\n",i,buffer[i]);
                        }

                        if(buffer[5] == 0xA5)
                        {
                                printf("send so2 adjust value to 0 cmd success\n");
                                memset(buffer, 0, sizeof(buffer));
                                am335x_so2_base_ret->retur = 0;
                                am335x_so2_base_ret->flag = 0;
                                return 0;
                        }
                        else
                        {
                                printf("send so2 adjust value to 0 cmd failed\n");
                                memset(buffer, 0, sizeof(buffer));
                                am335x_so2_base_ret->retur = -2;
                                am335x_so2_base_ret->flag = 0;
                                return -2; /*调整出错*/
                        }
                }
                else
                {
                        printf("send so2 adjust base value cmd failed");
                        am335x_so2_base_ret->retur = -3;
                        am335x_so2_base_ret->flag = 0;
                        return -3;
                }

        }
        else if(flag == 0)/*not change*/
        {
                am335x_so2_base_ret->retur = -1;
                am335x_so2_base_ret->flag = 0;
                printf("not change\n");
                return -4;
        }

}



/************************************************************
Name: am335x_gas_sensor_process
Func: gas sensor process
Para:
none
 *************************************************************/
void* am335x_gas_sensor_process(void *arg_t)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        struct am335x_sensor_mem_t *am335x_sensor_mem;
        struct am335x_so2_base_value_mem_t *am335x_so2_mem;		
        struct sensor_para_t *arg = (struct sensor_para_t *)arg_t;
        unsigned char filename[32];
        unsigned char buffer[256];
        unsigned int  value;
        float         voc_val;
        struct timeval tv;
        fd_set rfds;
        FILE *fptr;
        int  index;
        int  shmid, bvalue_shmid;
        int  ret,ret_flag;
        int  len;
        int  fd, i  = 0;

        am335x_openlog("am335x driver");
        switch(arg->gtype)
        {
                case AM335X_SENSOR_SO2:
                        index    = AM335X_SENSOR_DATA_SO2;
                        prctl(PR_SET_NAME, "so2_pthread");
                        break;

                case AM335X_SENSOR_NO2:
                        index    = AM335X_SENSOR_DATA_NO2;
                        prctl(PR_SET_NAME, "no2_pthread");
                        break;

                case AM335X_SENSOR_CO:
                        index    = AM335X_SENSOR_DATA_CO;
                        prctl(PR_SET_NAME, "co_pthread");
                        break;

                case AM335X_SENSOR_O3:
                        index    = AM335X_SENSOR_DATA_O3;
                        prctl(PR_SET_NAME, "o3_pthread");
                        break;

                case AM335X_SENSOR_TVOC:
                        index    = AM335X_SENSOR_DATA_TVOC;
                        prctl(PR_SET_NAME, "tvoc_pthread");
                        break;

                default:
                        am335x_printlog("GAS: Gas Type Error\r\n");
                        am335x_closelog("am335x driver");
						g_pthread_exit = -1;
						pthread_exit(0);
        }


        switch(arg->stype)
        {
                case AM335X_SENSOR_TYPE_WINSEN_ZE03:
                        am335x_sensor_dev = &am335x_sensor_ze03_dev[arg->gtype];
                        break;

                case AM335X_SENSOR_TYPE_WINSEN_ZE10:
                        am335x_sensor_dev = &am335x_sensor_ze10_dev[arg->gtype];
                        break;

                case AM335X_SENSOR_TYPE_SINGOAN_SGA700:
                        am335x_sensor_dev = &am335x_sensor_sga700_dev[arg->gtype];
                        break;

                default:
                        am335x_printlog("GAS: Product Type Error\r\n");
                        am335x_closelog("am335x driver");
						g_pthread_exit = -2;
						pthread_exit(0);
        }

        shmid = shmget(AM335X_SENSOR_DATA_ID, AM335X_SENSOR_DATA_MAX_NUMS * sizeof(struct am335x_sensor_mem_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("%s: Share Memory Get Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -3;
				pthread_exit(0);
        }

        am335x_sensor_mem = (struct am335x_sensor_mem_t*)shmat(shmid, 0, 0);

        if(am335x_sensor_mem == (struct am335x_sensor_mem_t*)(-1))
        { 
                am335x_printlog("%s: Share Memory Link Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -4;
				pthread_exit(0);
        }

        memset((char*)(&am335x_sensor_mem[index]), 0, sizeof(struct am335x_sensor_mem_t));

        bvalue_shmid = shmget(AM335X_SO2_ID, sizeof(struct am335x_so2_base_value_mem_t), IPC_CREAT | 0666);

        if(bvalue_shmid < 0)
        {
                am335x_printlog("bvalue_shmid Share Memory Get Failed\r\n");
                am335x_closelog("am335x driver");
                printf("create bvalue_shmid failed\n");
				g_pthread_exit = -5;
				pthread_exit(0);
        }else
        {
                //printf("create bvalue_shmid success\n");
        }

        am335x_so2_mem = (struct am335x_so2_base_value_mem_t*)shmat(bvalue_shmid, 0, 0);

        if(am335x_so2_mem == (struct am335x_so2_base_value_mem_t*)(-1))
        { 
                am335x_printlog("am335x_so2_mem Share Memory Link Failed\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -6;
				pthread_exit(0);
        }
        else
        {
                //printf("create am335x_so2_mem success\n");
        }
        memset((char*)(&am335x_so2_mem[0]), 0x01, sizeof(struct am335x_so2_base_value_mem_t));
        am335x_sensor_mem[index].types = arg->gtype;
        am335x_sensor_mem[index].flags = AM335X_SENSOR_PRE_HEAT;

        sleep(AM335X_SENSOR_PRE_HEAT_TIME);

        am335x_sensor_mem[index].flags = AM335X_SENSOR_PRE_HEAT;

#ifdef AM335X_RAW_DATA_OUTPUT
        sprintf(filename, "/var/log/%s.raw", am335x_sensor_dev->gas_name);

        fptr = fopen(filename, "r+");

        if(fptr == NULL)
        {
                fptr = fopen(filename, "w+");
        }
        else
        {
                if(ftell(fptr) > AM335X_RAW_DATA_FILE_SIZE)
                {
                        fseek(fptr, 0, SEEK_SET);
                }
                else
                {
                        fseek(fptr, 0, SEEK_END);
                }
        }
#endif

        fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

        if(fd < 0)
        {
                shmdt((char*)am335x_sensor_mem);
                shmdt((char*)am335x_so2_mem);

                am335x_printlog("%s: Uart Open Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -7;
				pthread_exit(0);
        } 
#if 0       
        if(strcmp(am335x_sensor_dev->gas_name, "SO2") == 0)
        {
                so2_fd = dup(fd);
        }
#endif

        fcntl(fd, F_SETFL, 0);

        am335x_uart_set_opt(fd, am335x_sensor_dev->uart_speed, 8, 'N', 1, 
                        am335x_sensor_dev->uart_length, am335x_sensor_dev->uart_timeout);

        switch(arg->stype)
        {
                case AM335X_SENSOR_TYPE_WINSEN_ZE03:
                        buffer[0] = 0xFF;
                        buffer[1] = 0x01;
                        buffer[2] = 0x78;
                        buffer[3] = 0x03;
                        buffer[4] = 0x00;
                        buffer[5] = 0x00;
                        buffer[6] = 0x00;
                        buffer[7] = 0x00;
                        buffer[8] = 0x84;

                        write(fd, buffer, 9);

                        break;

                case AM335X_SENSOR_TYPE_WINSEN_ZE10:
                        buffer[0] = 0xFF;
                        buffer[1] = 0x01;
                        buffer[2] = 0x78;
                        buffer[3] = 0x40;
                        buffer[4] = 0x00;
                        buffer[5] = 0x00;
                        buffer[6] = 0x00;
                        buffer[7] = 0x00;
                        buffer[8] = 0x47;

                        write(fd, buffer, 9);

                        break;
#if 0
                case AM335X_SENSOR_TYPE_SINGOAN_SGA700:
                        if(gtype != AM335X_SENSOR_NO2)
                        {
                                break;
                        }
#if 0
                        buffer[0]  = 0xFF;
                        buffer[1]  = 0xFF;
                        buffer[2]  = 0x01;
                        buffer[3]  = 0x01;
                        buffer[4]  = 0x08;
                        buffer[5]  = 0x02;
                        buffer[6]  = 0x00;
                        buffer[7]  = 0x83;
                        buffer[8]  = 0x03;
                        buffer[9]  = 0x0F;
                        buffer[10] = 0xFA;
                        buffer[11] = 0x10;
                        buffer[12] = 0xA1;
                        buffer[13] = 0xDD;

                        write(fd, buffer, 14);
#else
                        buffer[0]  = 0xFF;
                        buffer[1]  = 0xFF;
                        buffer[2]  = 0x01;
                        buffer[3]  = 0x01;
                        buffer[4]  = 0x02;
                        buffer[5]  = 0x07;
                        buffer[6]  = 0x07;
                        buffer[7]  = 0xDD;

                        write(fd, buffer, 8);
#endif

                        break;
#endif
        }

        while(1)
        {
                if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == (arg->stype))
                {
#if 0                 
                        if(gtype == AM335X_SENSOR_DATA_TVOC)
                        {
                                sleep(2);
                        }
                        else
                        {
                                sleep(1);
                        }
#endif
#if 0
                        if((arg->gtype) == (am335x_so2_mem->type))
                        {
                                am335x_set_sga700_so2_value(fd,am335x_so2_mem->value, am335x_so2_mem->flag,am335x_so2_mem);
                        }
                        
                        if((arg->gtype) == (am335x_so2_mem->read_b_type))
                        {
                        		am335x_read_sga700_so2_base_value(fd, 0, am335x_so2_mem);
                        }
#endif

#if 1
                        if(arg->gtype == AM335X_SENSOR_DATA_SO2)
                        {

                                gettimeofday(&time1, NULL);
                                timer = (time1.tv_sec - time0.tv_sec);	
                                printf("timer count  %ld\n", timer);
                                if((timer >= 40)&& (g_time_flag == 1))
                                {
                                        buffer[0]  = 0xFF;
                                        buffer[1]  = 0xFF;
                                        buffer[2]  = 0x01;
                                        buffer[3]  = 0xFE;
                                        buffer[4]  = 0x08;
                                        buffer[5]  = 0x02;
                                        buffer[6]  = 0x00;
                                        buffer[7]  = 0x8f;
                                        buffer[8]  = 0x03;
                                        buffer[9]  = 0x01;
                                        buffer[10] = 0x00;
                                        
										switch(arg->value)
										{
												case 0:
														buffer[11] = 0x00;
														buffer[12] = 0x95;
														break;
										
												case 2:
														buffer[11] = 0x02;
														buffer[12] = 0x97;
														break;
										
												case 10:
														buffer[11] = 0x10;
														buffer[12] = 0xA5;
														break;
										
												case 20:
														buffer[11] = 0x20;
														buffer[12] = 0xB5;
														break;
										
												case 30:
														buffer[11] = 0x30;
														buffer[12] = 0xC5;
														break;
										
												case 40:
														buffer[11] = 0x40;
														buffer[12] = 0xD5;
														break;
										
												case 50:
														buffer[11] = 0x50;
														buffer[12] = 0xE5;
														break;
										
												case 60:
														buffer[11] = 0x60;
														buffer[12] = 0xF5;
														break;
										
												case 70:
														buffer[11] = 0x70;
														buffer[12] = 0x05;
														break;
										
												case 80:
														buffer[11] = 0x70;
														buffer[12] = 0x15;
														break;
										
												case 90:
														buffer[11] = 0x90;
														buffer[12] = 0x25;
														break;
										
												case 99:
														buffer[11] = 0x99;
														buffer[12] = 0x2E;
														break;
										
												default:
														printf("value is not permit, please input 0,2,10,20,30,40,50,60,70,80,90,99\n");
										
														//return -1; /*数据输入出错*/
										}
                                        
                                        buffer[13] = 0xdd;

                                        write(fd, buffer, 14);
                                        memset(buffer, 0, sizeof(buffer));
                                        len = read(fd, buffer, sizeof(buffer));
                                        for(i = 0; i < len; i++)
                                        {
                                                printf("RECV adjust so2 base value CMD: buffer[%d] = %x\n",i,buffer[i]);
                                        }
                                        memset(buffer, 0, sizeof(buffer));

                                        buffer[0]  = 0xFF;
                                        buffer[1]  = 0xFF;
                                        buffer[2]  = 0x01;
                                        buffer[3]  = 0xFE;
                                        buffer[4]  = 0x02;
                                        buffer[5]  = 0x05;
                                        buffer[6]  = 0x05;
                                        buffer[7]  = 0xdd;
                                        write(fd, buffer, 8);
                                        memset(buffer, 0, sizeof(buffer));
                                        len = read(fd, buffer, sizeof(buffer));
                                        for(i = 0; i < len; i++)
                                        {
                                                g_time_flag = 0;
                                                printf("RECV clear so2 value to zero CMD: buffer[%d] = %x\n",i,buffer[i]);
                                        }

										if(buffer[5] == 0xA5)
										{
											if(strcmp(am335x_sensor_dev->uart_dev,"/dev/ttyS1") == 0)
											{
												printf("adjust so2 base value success\n");
												//close(fd);
										
											}
											else if(strcmp(am335x_sensor_dev->uart_dev ,"/dev/ttyS2") == 0)
											{
												printf("adjust no2 base value success\n");
												//close(fd);
										
											}
											else if(strcmp(am335x_sensor_dev->uart_dev ,"/dev/ttyS3") == 0)
											{
												printf("adjust co base value success\n");
												//close(fd);
								
											}
											else if(strcmp(am335x_sensor_dev->uart_dev ,"/dev/ttyS5") == 0)
											{
												printf("adjust o3 base value success\n");
												//close(fd);
									
											}
										}   
										else
									    {
									        if(strcmp(am335x_sensor_dev->uart_dev ,"/dev/ttyS1") == 0)
											{
												printf("adjust so2 base value failed\n");
												//close(fd);
											
											}
											else if(strcmp(am335x_sensor_dev->uart_dev ,"/dev/ttyS2") == 0)
											{
												printf("adjust no2 base value failed\n");
												//close(fd);
											
											}
											else if(strcmp(am335x_sensor_dev->uart_dev ,"/dev/ttyS3") == 0)
											{
												printf("adjust co base value failed\n");
												//close(fd);
												
											}
											else if(strcmp(am335x_sensor_dev->uart_dev ,"/dev/ttyS5") == 0)
											{
												printf("adjust o3 base value failed\n");
												//close(fd);
												
											}
										}
                                        memset(buffer, 0, sizeof(buffer));
                                }

                        }
#endif
                        //co , no2 , o3, so2, tvoc 均执行此命令
                        buffer[0]  = 0xFF;
                        buffer[1]  = 0xFF;
                        buffer[2]  = 0x01;
                        buffer[3]  = 0xFE;
                        buffer[4]  = 0x05;
                        buffer[5]  = 0x01;
                        buffer[6]  = 0x00;
                        buffer[7]  = 0x6C;
                        buffer[8]  = 0x07;
                        buffer[9]  = 0x74;
                        buffer[10] = 0xDD;

                        write(fd, buffer, 11);
                        memset(buffer, 0, sizeof(buffer));  
#if 0                   
                        if(index == AM335X_SENSOR_DATA_TVOC)
                        {
                                //矫正
                                buffer[0]  = 0xFF;
                                buffer[1]  = 0xFF;
                                buffer[2]  = 0x01;
                                buffer[3]  = 0x01;
                                buffer[4]  = 0x08;
                                buffer[5]  = 0x02;
                                buffer[6]  = 0x00;
                                buffer[7]  = 0x8F;
                                buffer[8]  = 0x03;
                                buffer[9]  = 0x01;
                                buffer[10] = 0x00;
                                buffer[11] = 0x50;
                                buffer[12] = 0xE5;
                                buffer[13] = 0xDD;
                                write(fd, buffer, 14);                        
                                memset(buffer, 0, sizeof(buffer));
                        }
#endif

#if 0
                        //FF FF 01 01 02 05 05 DD
                        buffer[0]  = 0xFF;
                        buffer[1]  = 0xFF;
                        buffer[2]  = 0x01;
                        buffer[3]  = 0x01;
                        buffer[4]  = 0x02;
                        buffer[5]  = 0x05;
                        buffer[6]  = 0x05;
                        buffer[7]  = 0xDD;

                        write(fd, buffer, 8);                        
                        memset(buffer, 0, sizeof(buffer));
#endif

                }
                else
                {
                }

                memset(buffer, 0, sizeof(buffer));

                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret > 0)
                {
                        gettimeofday(&tv, NULL);

                        len = read(fd, buffer, sizeof(buffer));
#if 0
                        if(arg->gtype == AM335X_SENSOR_DATA_SO2)
                        {
                                for(i = 0; i < 18; i++)
                                {
                                        printf("buffer[%d] = %x\n", i, buffer[i]);

                                }
                        }

                        printf("tvoc read len is %d\n", len);
#endif
                        if(len < am335x_sensor_dev->uart_length)
                        {
                                am335x_sensor_mem[index].count++;

                                am335x_printlog("%s: Data Length Error: %d\r\n", am335x_sensor_dev->gas_name, len);
                        }
                        else
                        {
                                if((buffer[0] != am335x_sensor_dev->uart_code[0]) \
                                                && (buffer[1] != am335x_sensor_dev->uart_code[1]))
                                {
                                        am335x_sensor_mem[index].count++;

                                        if(1 == log_prt)
                                                am335x_printlog("%s: Data Header Error: %02X %02X\r\n", am335x_sensor_dev->gas_name, buffer[0], buffer[1]);
                                }
                                else
                                {
                                        switch(arg->stype)
                                        {
                                                case AM335X_SENSOR_TYPE_WINSEN_ZE03:
                                                        am335x_sensor_mem[index].data[am335x_sensor_mem[index].index].value = buffer[2] * 256 + buffer[3];

                                                        am335x_sensor_mem[index].data[am335x_sensor_mem[index].index].time  = tv.tv_sec;

                                                        am335x_sensor_mem[index].index++;

                                                        if(am335x_sensor_mem[index].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[index].index = 0;
                                                        } 

                                                        am335x_sensor_mem[index].count = 0;

                                                        break;

                                                case AM335X_SENSOR_TYPE_WINSEN_ZE10:
                                                        am335x_sensor_mem[index].data[am335x_sensor_mem[index].index].value = buffer[4] * 256 + buffer[5];

                                                        am335x_sensor_mem[index].data[am335x_sensor_mem[index].index].time  = tv.tv_sec;

                                                        am335x_sensor_mem[index].index++;

                                                        if(am335x_sensor_mem[index].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[index].index = 0;
                                                        } 

                                                        am335x_sensor_mem[index].count = 0;

#ifdef AM335X_RAW_DATA_PRINT
                                                        am335x_printlog("%s: %02X %02X %02X %02X\r\n", 
                                                                        am335x_sensor_dev->gas_name, 
                                                                        buffer[2], buffer[3], buffer[4], buffer[5]);
#endif

#ifdef AM335X_RAW_DATA_OUTPUT
                                                        if(fptr != NULL)
                                                        {
                                                                fprintf(fptr, "%02X %02X %02X %02X | %s", 
                                                                                buffer[2], buffer[3], buffer[4], 
                                                                                buffer[5], ctime(&tv.tv_sec));

                                                                if((am335x_sensor_mem[index].index % 60) == 0)
                                                                {
                                                                        fflush(fptr);

                                                                        if(ftell(fptr) > AM335X_RAW_DATA_FILE_SIZE)
                                                                        {
                                                                                fseek(fptr, 0, SEEK_SET);
                                                                        }
                                                                }
                                                        }
#endif

                                                        break;

                                                case AM335X_SENSOR_TYPE_SINGOAN_SGA700:
                                                        if((buffer[14] != am335x_sensor_dev->gas_code) || (buffer[len - 1] != 0xDD))
                                                        {
                                                                am335x_sensor_mem[index].count++;
																if(am335x_sensor_dev->gas_code == 0x0A)
																{
																	printf("so2 sensor position error\n");
																}
																else if(am335x_sensor_dev->gas_code == 0x16)
																{
																	printf("no2 sensor position error\n");
																}
																else if(am335x_sensor_dev->gas_code == 0x02)
																{
																	printf("co sensor position error\n");

																}
																else if(am335x_sensor_dev->gas_code == 0x08)
																{
																	printf("o3 sensor position error\n");

																}
																else if(am335x_sensor_dev->gas_code == 0x27)
																{
																	printf("TVOC sensor position error\n");

																}

                                                                if(1 == log_prt)
                                                                        am335x_printlog("%s: Data Is Not My Type: %02X %02X\r\n", am335x_sensor_dev->gas_name, buffer[14], buffer[len - 1]);
                                                        }
                                                        else if(am335x_sensor_checksum8bit(&buffer[5], 11) != buffer[16])
                                                        {
                                                                am335x_sensor_mem[index].count++;

                                                                am335x_printlog("%s: Data CheckSum Error\r\n", am335x_sensor_dev->gas_name);
                                                        }
                                                        else
                                                        {
                                                                value = am335x_bcd2dec(&buffer[10], 3);
                                                                //printf("the TVOC is %d\n", value);

                                                                switch(buffer[13])
                                                                {
                                                                        case 0:
                                                                                value = value * 1000;

                                                                                break;
                                                                        case 1:
                                                                                value = value * 100;
                                                                                break;
                                                                        case 2:
                                                                                value = value * 10;
                                                                                break;
                                                                        case 3:
                                                                                value = value; //单位mg/m3
                                                                                //printf("the TVOC is %d\n", value);
                                                                                break;       
                                                                        default:
                                                                                break;
                                                                }

                                                                am335x_sensor_mem[index].data[am335x_sensor_mem[index].index].value = value;
                                                                printf("the %s is %f\n",am335x_sensor_dev->gas_name,am335x_sensor_mem[index].data[am335x_sensor_mem[index].index].value);

                                                                am335x_sensor_mem[index].data[am335x_sensor_mem[index].index].time  = tv.tv_sec;

                                                                am335x_sensor_mem[index].index++;

                                                                if(am335x_sensor_mem[index].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                                {
                                                                        am335x_sensor_mem[index].index = 0;
                                                                }

                                                                am335x_sensor_mem[index].count = 0;

#ifdef AM335X_RAW_DATA_PRINT
                                                                am335x_printlog("%s: %02X %02X %02X %02X %02X %02X %02X\r\n", 
                                                                                am335x_sensor_dev->gas_name, buffer[9], buffer[10], 
                                                                                buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
#endif

#ifdef AM335X_RAW_DATA_OUTPUT
                                                                if(fptr != NULL)
                                                                {
                                                                        fprintf(fptr, "%02X %02X %02X %02X %02X %02X %02X | %09d | %s", 
                                                                                        buffer[9],  buffer[10], buffer[11], buffer[12], buffer[13], 
                                                                                        buffer[14], buffer[15], value, ctime(&tv.tv_sec));

                                                                        if((am335x_sensor_mem[index].index % 60) == 0)
                                                                        {
                                                                                fflush(fptr);

                                                                                if(ftell(fptr) > AM335X_RAW_DATA_FILE_SIZE)
                                                                                {
                                                                                        fseek(fptr, 0, SEEK_SET);
                                                                                }
                                                                        }
                                                                }
#endif
                                                        }


                                                        break;

                                                default:
                                                        am335x_sensor_mem[index].count++;
                                        }

                                }
                        }
                }
                else
                {
                        am335x_sensor_mem[index].count++;

                        if(1 == log_prt)
                                am335x_printlog("%s: Data Read Timeout\r\n", am335x_sensor_dev->gas_name);
                }

                if(am335x_sensor_mem[index].count == 0)
                {
                        am335x_sensor_mem[index].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[index].flags = AM335X_SENSOR_RUN_ERR;

                        am335x_sensor_mem[index].count = AM335X_SENSOR_TRY_TIMES;
                }
#if 0
                printf("**********index = %d**********\n", index);
                ret_flag = am335x_sensor_flag_read(index);
                if(ret_flag == 0)
                {
                        printf("read_sensor_flag success\n");
                }else
                {
                        printf("read_sensor_flag failed\n");
                }
                printf("*****am335x_sensor_mem[index].flags***** = %d\n", am335x_sensor_mem[index].flags);
#endif

                sleep(1);
        }

        close(fd);

#ifdef AM335X_RAW_DATA_OUTPUT
        if(fptr != NULL)
        {
                fclose(fptr);
        }
#endif

        shmdt((char*)am335x_sensor_mem);
        shmdt((char*)am335x_so2_mem);
        am335x_closelog("am335x driver");
		g_pthread_exit = -8;
        pthread_exit(0);
}

/************************************************************
Name: am335x_pms_sensor_process
Func: pm sensor process
Para:
none
 *************************************************************/
void* am335x_pms_sensor_process(void *arg_t)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        struct am335x_sensor_mem_t *am335x_sensor_mem;
        unsigned char buffer[256];
        struct timeval tv;
        char *shmaddr;
        fd_set rfds;
        int  shmid;
        int  ret;
        int  len;
        int  fd;		
        struct sensor_para_t *arg = (struct sensor_para_t *)arg_t;

        am335x_openlog("am335x driver");

        if(arg->gtype != AM335X_SENSOR_PM)
        {
                am335x_printlog("PM: PM Type Error\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -11;
				pthread_exit(0);
        }
        prctl(PR_SET_NAME, "pm_pthread");

        switch(arg->stype)
        {
                case AM335X_SENSOR_TYPE_WINSEN_ZH03:
                        am335x_sensor_dev = &am335x_sensor_zh03_dev[arg->gtype];
                        break;

                case AM335X_SENSOR_TYPE_NOVA_SDS:
                        am335x_sensor_dev = &am335x_sensor_sds_dev[arg->gtype];
                        break;

                case AM335X_SENSOR_TYPE_AMPHENOL:
                        am335x_sensor_dev = &am335x_sensor_amp_dev[arg->gtype];
                        break;
                case AM335X_SENSOR_TYPE_PMS5003:
                        am335x_sensor_dev = &am335x_sensor_pms5003_dev[arg->gtype];
                        break;

                default:
                        am335x_printlog("PM: Product Type Error\r\n");
                        am335x_closelog("am335x driver");
                        printf("PM: Product Type Error\r\n");
						g_pthread_exit = -12;
						pthread_exit(0);
        }

        shmid = shmget(AM335X_SENSOR_DATA_ID, AM335X_SENSOR_DATA_MAX_NUMS * sizeof(struct am335x_sensor_mem_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("%s: Share Memory Get Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -13;
				pthread_exit(0);
        }

        am335x_sensor_mem = (struct am335x_sensor_mem_t*)shmat(shmid, 0, 0);

        if(am335x_sensor_mem == (struct am335x_sensor_mem_t*)(-1))
        {
                am335x_printlog("%s: Share Memory Link Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -14;
				pthread_exit(0);
        }

        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_PM010]),    0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_PM025]),    0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_PM100]),    0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100]), 0, sizeof(struct am335x_sensor_mem_t));

        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM]), 0, sizeof(struct am335x_sensor_mem_t));

        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].types    = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].types    = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].types    = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].types = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].types = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].types = arg->stype;

        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].types = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].types = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].types = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].types = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].types = arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].types = arg->stype;

        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].flags    = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].flags    = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].flags    = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].flags = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].flags = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].flags = AM335X_SENSOR_PRE_HEAT;

        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].flags = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].flags = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].flags = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].flags = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].flags = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].flags = AM335X_SENSOR_PRE_HEAT;

        fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

        if(fd < 0)
        {
                shmdt((char*)am335x_sensor_mem);

                am335x_printlog("%s: Uart Open Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -15;
				pthread_exit(0);
        }

        fcntl(fd, F_SETFL, 0);

        am335x_uart_set_opt(fd, am335x_sensor_dev->uart_speed, 8, 'N', 1,
                        am335x_sensor_dev->uart_length, am335x_sensor_dev->uart_timeout);

        while(1)
        {
                sleep(1);
                memset(buffer, 0, sizeof(buffer));

                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret > 0)
                {
                        gettimeofday(&tv, NULL);

                        len = read(fd, buffer, sizeof(buffer));

                        if(len < am335x_sensor_dev->uart_length)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].count++;

                                am335x_printlog("%s: Data Length Error: %d\r\n", am335x_sensor_dev->gas_name, len);
                        }
                        else
                        {
                                if((buffer[0] != am335x_sensor_dev->uart_code[0]) \
                                                && (buffer[1] != am335x_sensor_dev->uart_code[1]))
                                {
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].count++;

                                        if(1 == log_prt)
                                                am335x_printlog("%s: Data Header Error: %02X %02X\r\n", am335x_sensor_dev->gas_name, buffer[0], buffer[1]);
                                }
                                else
                                {
                                        switch(arg->stype)
                                        {
                                                case AM335X_SENSOR_TYPE_WINSEN_ZH03:
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index].value \
                                                                = buffer[10] * 256 + buffer[11];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].value \
                                                                = buffer[12] * 256 + buffer[13];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].value \
                                                                = buffer[14] * 256 + buffer[15];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index].value \
                                                                = buffer[4]  * 256 + buffer[5];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index].value \
                                                                = buffer[6]  * 256 + buffer[7];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index].value \
                                                                = buffer[8]  * 256 + buffer[9];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count = 0;

                                                        break;

                                                case AM335X_SENSOR_TYPE_NOVA_SDS:
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].value \
                                                                = (buffer[3] * 256 + buffer[2]) / 10;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].value \
                                                                = (buffer[5] * 256 + buffer[4]) / 10;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count = 0;

                                                        break;

                                                case AM335X_SENSOR_TYPE_AMPHENOL:
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index].value \
                                                                = ((buffer[11] << 24) + (buffer[10] << 16) + (buffer[9] << 8) + buffer[8]) / 100;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].value \
                                                                =  ((buffer[19] << 24) + (buffer[18] << 16) + (buffer[17] << 8) + buffer[16]) / 100;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].value \
                                                                =  ((buffer[35] << 24) + (buffer[34] << 16) + (buffer[33] << 8) + buffer[32]) / 100;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count = 0; 

                                                        break;

                                                case AM335X_SENSOR_TYPE_PMS5003:

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index].value \
                                                                = (buffer[4] << 8) | (buffer[5]);

                                                        //printf("the PM1.0 (cf = 1) is %d ug/m3\n",am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index].value);

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index].value \
                                                                =  (buffer[6] << 8) | buffer[7];

                                                        //printf("the PM2.5 (cf = 1) is %d ug/m3\n",am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index].value \
                                                                =  (buffer[8] << 8) | (buffer[9]);

                                                        //printf("the PM10 (cf = 1) is %d ug/m3\n",am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count = 0;


                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index].value \
                                                                = (buffer[10] << 8) | buffer[11];

                                                        //printf("the PM1.0 is %d ug/m3\n",am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].value \
                                                                =  (buffer[12] << 8) | buffer[13];

                                                        //printf("the PM2.5 is %d ug/m3\n",am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].value \
                                                                =  (buffer[14] << 8) | (buffer[15]);

                                                        //printf("the PM10 is %d ug/m3\n",am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].index].value \
                                                                =  (buffer[16] << 8) | (buffer[17]);

                                                        //printf("the 0.3um PM num is %d\n",am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].index].value \
                                                                =  (buffer[18] << 8) | (buffer[19]);

                                                        //printf("the 0.5um PM num is %d\n",am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].index].value \
                                                                =  (buffer[20] << 8) | (buffer[21]);

                                                        //printf("the 1.0um PM num is %d\n",am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].index].value \
                                                                =  (buffer[22] << 8) | (buffer[23]);

                                                        //printf("the 2.5um PM num is %d\n",am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].index].value \
                                                                =  (buffer[24] << 8) | (buffer[25]);

                                                        //printf("the 5.0um PM num is %d\n",am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].index].value \
                                                                =  (buffer[26] << 8) | (buffer[27]);

                                                        //printf("the 10um PM num is %d\n",am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].index].value);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].data[am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].count = 0;
#if 0
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].count++; 
#endif


                                                        break;

                                                default:
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count++; 
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].count++;
                                        }
                                }
                        }
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].count++;

                        if(1 == log_prt)
                                am335x_printlog("%s: Data Read Timeout\r\n", am335x_sensor_dev->gas_name);
                }

                if((am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count == 0))
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_03_NUM].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].count == 0) 
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_05_NUM].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_10_NUM].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_25_NUM].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_50_NUM].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].flags = AM335X_SENSOR_RUN_OK;
                }
                else
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].flags = AM335X_SENSOR_RUN_ERR;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_100_NUM].count = AM335X_SENSOR_TRY_TIMES;
                }
        }

        close(fd);       

        shmdt((char*)am335x_sensor_mem);

        am335x_closelog("am335x driver");
		g_pthread_exit = -16;
        pthread_exit(0); 
}

char read_atoms_sensor_adr(int fd)
{
        int i = 0;
        char buffer[256];
        buffer[0] = 0x82;
        buffer[1] = 0xFF;
        buffer[2] = 0x02;
        buffer[3] = 0x05;
        buffer[4] = 0x00;
        buffer[5] = buffer[1]^buffer[2]^buffer[3]^buffer[4];

        if (write(fd, buffer, 6) < 0)
        {
                printf("write atmos semsor addr failed\n");
        }

        memset(buffer, 0x99, 256);
        sleep(1);
        if (read(fd, buffer, 6) < 0)
        {
                printf("read atmos sensor addr failed\n");
        }

        if(buffer[6] == (buffer[1]^buffer[2]^buffer[3]^buffer[4]^buffer[5]))
        {
				/*初始化时将buffer初始化为0x99*/
				if(0 == buffer[4])
                {
                        printf("read addr exec success!!!\n");
                        return buffer[1];
                }else
                {
                        printf("read addr exec failed!!!\n");
                }
        }else
        {
                return -1;
        }
}

/*
./am335x_test -test set so2 40
*/
int am335x_set_base_value_test(char *dev, int value)
{
    int fd,i,len;
	char buffer[256];
	char buffer2[256];
	char *uar_dev =dev;
	fd = open(uar_dev, O_RDWR | O_NOCTTY | O_NDELAY);

    if(fd < 0)
    {
            am335x_printlog(" Uart Open Failed\r\n");
            am335x_closelog("am335x driver");
    }
	else
	{
		printf("open %s success\n", uar_dev);
	}

	fcntl(fd, F_SETFL, 0);

    am335x_uart_set_opt(fd, 115200, 8, 'N', 1, 18, 5);


    buffer[0]  = 0xFF;
    buffer[1]  = 0xFF;
    buffer[2]  = 0x01;
    buffer[3]  = 0xFE;
    buffer[4]  = 0x08;
    buffer[5]  = 0x02;
    buffer[6]  = 0x00;
    buffer[7]  = 0x8f;
    buffer[8]  = 0x03;
    buffer[9]  = 0x01;
    buffer[10] = 0x00;
	switch(value)
	{
			case 0:
					buffer[11] = 0x00;
					buffer[12] = 0x95;
					break;
	
			case 2:
					buffer[11] = 0x02;
					buffer[12] = 0x97;
					break;
	
			case 10:
					buffer[11] = 0x10;
					buffer[12] = 0xA5;
					break;
	
			case 20:
					buffer[11] = 0x20;
					buffer[12] = 0xB5;
					break;
	
			case 30:
					buffer[11] = 0x30;
					buffer[12] = 0xC5;
					break;
	
			case 40:
					buffer[11] = 0x40;
					buffer[12] = 0xD5;
					break;
	
			case 50:
					buffer[11] = 0x50;
					buffer[12] = 0xE5;
					break;
	
			case 60:
					buffer[11] = 0x60;
					buffer[12] = 0xF5;
					break;
	
			case 70:
					buffer[11] = 0x70;
					buffer[12] = 0x05;
					break;
	
			case 80:
					buffer[11] = 0x70;
					buffer[12] = 0x15;
					break;
	
			case 90:
					buffer[11] = 0x90;
					buffer[12] = 0x25;
					break;
	
			case 99:
					buffer[11] = 0x99;
					buffer[12] = 0x2E;
					break;
	
			default:
					printf("value is not permit, please input 0,2,10,20,30,40,50,60,70,80,90,99\n");
	
					return -1; /*数据输入出错*/
	}
	
    buffer[13] = 0xdd;
    write(fd, buffer, 14);
    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer2, sizeof(buffer2));
    for(i = 0; i < len; i++)
    {
            printf("RECV adjust base value CMD: buffer[%d] = %x\n",i,buffer2[i]);
    }
    memset(buffer, 0, sizeof(buffer));

    buffer[0]  = 0xFF;
    buffer[1]  = 0xFF;
    buffer[2]  = 0x01;
    buffer[3]  = 0xFE;
    buffer[4]  = 0x02;
    buffer[5]  = 0x05;
    buffer[6]  = 0x05;
    buffer[7]  = 0xdd;

    write(fd, buffer, 8);
    memset(buffer, 0, sizeof(buffer));

    len = read(fd, buffer, sizeof(buffer));
    for(i = 0; i < len; i++)
    {
            printf("RECV clear value to zero CMD: buffer[%d] = %x\n",i,buffer[i]);
    }

    if(buffer[5] == 0xA5)
    {
		if(strcmp(uar_dev ,"/dev/ttyS1") == 0)
		{
			printf("adjust so2 base value success\n");
			close(fd);
			uar_dev = NULL;
		}
		else if(strcmp(uar_dev ,"/dev/ttyS2") == 0)
		{
			printf("adjust no2 base value success\n");
			close(fd);
			uar_dev = NULL;
		}
		else if(strcmp(uar_dev ,"/dev/ttyS3") == 0)
		{
			printf("adjust co base value success\n");
			close(fd);
			uar_dev = NULL;
		}
		else if(strcmp(uar_dev ,"/dev/ttyS5") == 0)
		{
			printf("adjust o3 base value success\n");
			close(fd);
			uar_dev = NULL;
		}
			return 0;
    }
    else
    {
        if(strcmp(uar_dev ,"/dev/ttyS1") == 0)
		{
			printf("adjust so2 base value failed\n");
			close(fd);
			uar_dev = NULL;
		}
		else if(strcmp(uar_dev ,"/dev/ttyS2") == 0)
		{
			printf("adjust no2 base value failed\n");
			close(fd);
			uar_dev = NULL;
		}
		else if(strcmp(uar_dev ,"/dev/ttyS3") == 0)
		{
			printf("adjust co base value failed\n");
			close(fd);
			uar_dev = NULL;
		}
		else if(strcmp(uar_dev ,"/dev/ttyS5") == 0)
		{
			printf("adjust o3 base value failed\n");
			close(fd);
			uar_dev = NULL;
		}

			return -1;

    }
}

int am335x_read_sga700_data_test(char *dev)
{
    int fd,i,len, ret;
	char buffer[256];
	char *uar_dev =dev;
	fd = open(uar_dev, O_RDWR | O_NOCTTY | O_NDELAY);

    if(fd < 0)
    {
            am335x_printlog(" Uart Open Failed\r\n");
            am335x_closelog("am335x driver");
    } 

	fcntl(fd, F_SETFL, 0);

    am335x_uart_set_opt(fd, 115200, 8, 'N', 1, 18, 5);

    while(1)
    {
		buffer[0]  = 0xFF;
		buffer[1]  = 0xFF;
		buffer[2]  = 0x01;
		buffer[3]  = 0xFE;
		buffer[4]  = 0x05;
		buffer[5]  = 0x01;
		buffer[6]  = 0x00;
		buffer[7]  = 0x6C;
		buffer[8]  = 0x07;
		buffer[9]  = 0x74;
		buffer[10] = 0xDD;

		write(fd, buffer, 11);
		memset(buffer, 0, sizeof(buffer)); 
		ret = read(fd, buffer, sizeof(buffer));
    }

	return 0;
}

void* am335x_atoms_sensor_process(void *arg_t)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        struct am335x_sensor_mem_t *am335x_sensor_mem;
        unsigned char buffer[256];
        float float_buffer[256];
        float frt_buffer[256];
        float f_value;
        double d_value;
        struct timeval tv;
        fd_set rfds;
        int shmid;
        int ret;
        int len;
        int fd;
        int i,j = 0, j_count = 0;
        char atmos_addr;
        char *frt_str = "0R0";
        char value[256];
        int arg[3];	
        struct sensor_para_t *sensor_arg = arg_t;

        am335x_openlog("am335x_driver");

        if(sensor_arg->gtype != AM335X_SENSOR_ATOMS)
        {
                am335x_printlog("ATMOS: ATMOS Type Error\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -21;
				pthread_exit(0);
        }

        prctl(PR_SET_NAME, "atmos_pthread");
        switch(sensor_arg->stype)
        {
                case AM335X_SENSOR_TYPE_WXA:
                        am335x_sensor_dev = &am335x_sensor_atoms_dev[sensor_arg->gtype];
                        break;
                case AM335X_SENSOR_TYPE_FRT:
                        am335x_sensor_dev = &am335x_sensor_atomsFrt_dev[sensor_arg->gtype];	
                        break;
                default:
                        am335x_printlog("ATMOS: Product Type Error\r\n");
                        am335x_closelog("am335x driver");
                        printf("ATMOS: Product Type Error\r\n");
						g_pthread_exit = -22;
						pthread_exit(0);
        }

        shmid = shmget(AM335X_SENSOR_DATA_ID, AM335X_SENSOR_DATA_MAX_NUMS * sizeof(struct am335x_sensor_mem_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("%s: Share Memory Get Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -23;
				pthread_exit(0);
        }

        am335x_sensor_mem = (struct am335x_sensor_mem_t*)shmat(shmid, 0, 0);

        if(am335x_sensor_mem == (struct am335x_sensor_mem_t*)(-1))
        {
                am335x_printlog("%s: Share Memory Link Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -24;
				pthread_exit(0);
        }

        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE]),    0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY]),    0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE]),    0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION]), 0, sizeof(struct am335x_sensor_mem_t));
        memset((char*)(&am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION]), 0, sizeof(struct am335x_sensor_mem_t));


        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].types    = sensor_arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].types    = sensor_arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].types    = sensor_arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].types = sensor_arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].types = sensor_arg->stype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].types = sensor_arg->stype;

        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].flags    = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].flags       = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].flags   = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].flags     = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].flags = AM335X_SENSOR_PRE_HEAT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].flags = AM335X_SENSOR_PRE_HEAT;

        if(sensor_arg->stype == AM335X_SENSOR_TYPE_FRT)
        {
                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].types    = sensor_arg->stype;
                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].types = sensor_arg->stype;
                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].types = sensor_arg->stype;
                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].types = sensor_arg->stype;

                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].flags    = AM335X_SENSOR_PRE_HEAT;
                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].flags       = AM335X_SENSOR_PRE_HEAT;
                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].flags   = AM335X_SENSOR_PRE_HEAT;
                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].flags     = AM335X_SENSOR_PRE_HEAT;
        }

        fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

        if(fd < 0)
        {
                shmdt((char*)am335x_sensor_mem);
                am335x_printlog("%s: Uart Open Failed\r\n", am335x_sensor_dev->gas_name);
                am335x_closelog("am335x driver");
				g_pthread_exit = -25;
				pthread_exit(0);
        }else
        {
        }

        fcntl(fd, F_SETFL, 0);

        am335x_uart_set_opt(fd, am335x_sensor_dev->uart_speed, 8, 'N', 1,
                        am335x_sensor_dev->uart_length, am335x_sensor_dev->uart_timeout);

        if(sensor_arg->stype == AM335X_SENSOR_TYPE_WXA)
        {
                atmos_addr = read_atoms_sensor_adr(fd);
                if(-1 == atmos_addr)
                {
                        am335x_printlog("ATMOS: read atmosphere error\r\n");	
                }
        }
        while(1)
        {
                if(AM335X_SENSOR_TYPE_WXA == sensor_arg->stype)
                {
                        sleep(1);

                        buffer[0] = 0x82;
                        buffer[1] = atmos_addr;
                        buffer[2] = 0x02;
                        buffer[3] = 0x01;
                        buffer[4] = 0x00;
                        buffer[5] = buffer[1]^buffer[2]^buffer[3]^buffer[4];
#if 0
                        for(i = 0; i < 6; i++)
                        {
                                printf("buffer[%d] = %x\n",i,buffer[i]);
                        }
#endif
                        if (write(fd, buffer, 6) < 0)
                        {
                                printf("write atoms fd failed\n");
                        }

                }else if(AM335X_SENSOR_TYPE_FRT == sensor_arg->stype)
                {
                        if(strcmp(am335x_sensor_dev->uart_dev, "/dev/ttyXRUSB2") == 0)
                        {        
                                arg[0] = 2; arg[1] = 0x0C; arg[2] = 0x0B;

                                ioctl(fd, XR_USB_SERIAL_SET_REG, (unsigned long)arg);

                                arg[0] = 2; arg[1] = 0x1A; arg[2] = 0x0B;

                                ioctl(fd, XR_USB_SERIAL_SET_REG, (unsigned long)arg);
                        }
                        //sleep(1);
#if 1						
                        if (write(fd, "0XU,M=A,I=1\r\n", strlen("0XU,M=A,I=1\r\n")) < 0)
                        {
                                printf("write 0XU,M=A,I=1 fd failed\n");
                        }else
                        {
                        }
#endif						
#if 0
                        if (read(fd, buffer, 1) < 0)
                        {
                                printf("@@@@@@read  fd failed\n");
                                return -1;
                        }else
                        {
                                printf("@@@@@@buffer = %s\n", buffer);
                                printf("@@@@@@read buffer success\n");
                        }


                        while((len = read(fd, buffer, len))>0)
                        {
                                break;
                        }
                        printf("write %d Bytes\r\n", len);
#endif
                }

                memset(buffer, 0, sizeof(buffer));
                memset(frt_buffer, 0, sizeof(frt_buffer));
                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret > 0)
                {
                        gettimeofday(&tv, NULL);
                        while(1)
                        {
                                if(sensor_arg->stype == AM335X_SENSOR_TYPE_WXA)
                                {
                                        len = read(fd, &buffer[j_count], 1);
                                        if(len < 0)
                                        {
                                                printf("read atmosphere data error\n");
                                        }

                                        if(buffer[j_count] == 0x86)
                                        {
                                                break;
                                        }
                                        j_count++;
                                }
                                else if(sensor_arg->stype == AM335X_SENSOR_TYPE_FRT)
                                {
                                        len = read(fd, buffer, sizeof(buffer));
                                        if (strstr(buffer, frt_str) != NULL)
                                        {
                                                break;
                                        }

                                        if(len < 0)
                                        {
                                                printf("read atmosphere data error\n");
                                        }
                                }
                                memset(buffer, 0, sizeof(buffer));							
                        }

                        if(sensor_arg->stype == AM335X_SENSOR_TYPE_WXA)
                        {
                                j_count++;
                                len = read(fd, &buffer[j_count], sizeof(buffer) - j_count);
                                if(len < 0)
                                {
                                        printf("read ZM_WXA atmos failed\n");
                                }
#if 0	
                                for(i = 0; i < (len + 1); i++)
                                {
                                        printf("buffer[%d] is %x\n", i, buffer[i]);
                                }
                                printf("**************************\n");
#endif
                                j_count--;
                                memcpy(&float_buffer[0], &buffer[j_count + 6] , 4);
                                memcpy(&float_buffer[1], &buffer[j_count + 10], 4);
                                memcpy(&float_buffer[2], &buffer[j_count + 14], 4);
                                memcpy(&float_buffer[3], &buffer[j_count + 18], 4);
                                memcpy(&float_buffer[4], &buffer[j_count + 22], 4);
                                memcpy(&float_buffer[5], &buffer[j_count + 26], 4);
#if 0
                                for(i = 0; i < 6; i++)
                                {
                                        printf("float_buffer[%d] is %f\n", i, buffer[i]);
                                }
                                printf("**************************\n");
#endif
                                //memcpy(float_buffer, buffer, 32);
                                //len += read(fd, &buffer[6], 1);                                                 //\B4?\A6?\B8\C3\D3\D0У\D1\E9
                        }
                        else if(sensor_arg->stype == AM335X_SENSOR_TYPE_FRT)
                        {
                                find_frt_value("Dn=", "D,", buffer, 1, value);

                                //printf("strlen(value)= %d\n", strlen(value));
                                frt_buffer[AM335X_FRT_MIN_WIND_DIRECTION] = get_frt_value(value);
                                //printf("frt_buffer[0] = %f\n",frt_buffer[0]);

                                find_frt_value("Dm=", "D,", buffer, 2, value);
                                frt_buffer[AM335X_FRT_MAX_WIND_DIRECTION] = get_frt_value(value);
                                //printf("frt_buffer[1] = %f\n",frt_buffer[1]);

                                find_frt_value("Dx=", "D,", buffer, 3, value);
                                frt_buffer[AM335X_FRT_AVE_WIND_DIRECTION] = get_frt_value(value);
                                //printf("frt_buffer[2] = %f\n",frt_buffer[2]);

                                find_frt_value("Sn=", "M,", buffer, 1, value);
                                frt_buffer[AM335X_FRT_MIN_WIND_SPEED] = get_frt_value(value);
                                //printf("frt_buffer[3] = %f\n",frt_buffer[3]);

                                find_frt_value("Sm=", "M,", buffer, 2, value);
                                frt_buffer[AM335X_FRT_MAX_WIND_SPEED] = get_frt_value(value);
                                //printf("frt_buffer[4] = %f\n",frt_buffer[4]);

                                find_frt_value("Sx=", "M,", buffer, 3, value);
                                frt_buffer[AM335X_FRT_AVE_WIND_SPEED] = get_frt_value(value);
                                //printf("frt_buffer[5] = %f\n",frt_buffer[5]);

                                find_frt_value("Ta=", "C,", buffer, 1, value);
                                frt_buffer[AM335X_FRT_TEMPERATURE] = get_frt_value(value);
                                //printf("frt_buffer[6] = %f\n",frt_buffer[6]);

                                find_frt_value("Ua=", "P,", buffer, 1, value);
                                frt_buffer[AM335X_FRT_HUMIDITURY] = get_frt_value(value);
                                //printf("frt_buffer[7] = %f\n",frt_buffer[7]);

                                find_frt_value("Pa=", "H,", buffer, 1, value);
                                frt_buffer[AM335X_FRT_PRESSURE] = get_frt_value(value);
                                //printf("frt_buffer[8] = %f\n",frt_buffer[8]);

                                find_frt_value("Rc=", "M,", buffer, 4, value);
                                frt_buffer[AM335X_FRT_RAINFALL] = get_frt_value(value);
                                //printf("frt_buffer[9] = %f\n",frt_buffer[9]);							

                        }

                        if(sensor_arg->stype == AM335X_SENSOR_TYPE_WXA)
                        {
                                if(len < am335x_sensor_dev->uart_length)
                                {
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count++;
                                        j_count = 0;
                                        memset(buffer, 0, sizeof(buffer));
                                        am335x_printlog("%s: Data Length Error: %d\r\n", am335x_sensor_dev->gas_name, len);
                                }
                                else
                                {

                                        if((buffer[j_count] != am335x_sensor_dev->uart_code[0]))
                                        {
                                                printf("********inside not equal to 0x86***********\n");
                                                am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count++;
                                                am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count++;
                                                am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count++;
                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count++;
                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count++;
                                                am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count++;
                                                j_count = 0;
                                                memset(buffer, 0, sizeof(buffer));
                                                if(1 == log_prt)
                                                        am335x_printlog("%s: Data Header Error: %02X\r\n", am335x_sensor_dev->gas_name, buffer[0]);
                                        }
                                        else
                                        {
                                                j_count = 0;
                                                memset(buffer, 0, sizeof(buffer));
                                                switch(sensor_arg->stype)
                                                {
                                                        case AM335X_SENSOR_TYPE_WXA:
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].data[am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index].value \
                                                                        = float_buffer[0];
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].data[am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index].time  \
                                                                        = tv.tv_sec;
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index++;

                                                                if(am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                                {
                                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index = 0;
                                                                }

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count = 0;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].data[am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index].value \
                                                                        = float_buffer[1];
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].data[am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index].time  \
                                                                        = tv.tv_sec;
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index++;

                                                                if(am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                                {
                                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index = 0;
                                                                }

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count = 0;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].data[am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index].value \
                                                                        = float_buffer[2];
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].data[am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index].time  \
                                                                        = tv.tv_sec;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index++;

                                                                if(am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                                {
                                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index = 0;
                                                                }

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count = 0;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].data[am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index].value \
                                                                        = float_buffer[3];
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].data[am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index].time  \
                                                                        = tv.tv_sec;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index++;

                                                                if(am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                                {
                                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index = 0;
                                                                }

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count = 0;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].data[am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index].value \
                                                                        = float_buffer[4];
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].data[am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index].time  \
                                                                        = tv.tv_sec;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index++;

                                                                if(am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                                {
                                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index = 0;
                                                                }

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count = 0;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].data[am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index].value \
                                                                        = float_buffer[5];
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].data[am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index].time  \
                                                                        = tv.tv_sec;

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index++;

                                                                if(am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                                {
                                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index = 0;
                                                                }

                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count = 0;
                                                                memset(buffer, 0, sizeof(buffer));
                                                                break;

                                                        default:
                                                                printf("**************inside default***************\n");
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count++;
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count++;
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count++;
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count++;
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count++;
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count++;
                                                                j_count = 0;
                                                                memset(buffer, 0, sizeof(buffer));
                                                }
                                        }
                                }
                        }
                        else if(sensor_arg->stype == AM335X_SENSOR_TYPE_FRT)
                        {
                                if(len < am335x_sensor_dev->uart_length)
                                {
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].count++;
                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].count++;

                                        memset(buffer, 0, sizeof(buffer));
                                        am335x_printlog("%s: Data Length Error: %d\r\n", am335x_sensor_dev->gas_name, len);
                                }
                                else
                                {
                                        memset(buffer, 0, sizeof(buffer));
                                        switch(sensor_arg->stype)
                                        {
                                                case AM335X_SENSOR_TYPE_FRT:
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].data[am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index].value \
                                                                =  frt_buffer[AM335X_FRT_TEMPERATURE];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].data[am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index].time  \
                                                                = tv.tv_sec;
                                                        printf("the temperature in FRT is %f\n", frt_buffer[AM335X_FRT_TEMPERATURE]);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].data[am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index].value \
                                                                = frt_buffer[AM335X_FRT_HUMIDITURY];
                                                        printf("the humidity in frt is %f\n", frt_buffer[AM335X_FRT_HUMIDITURY]);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].data[am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index].time  \
                                                                = tv.tv_sec;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].data[am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index].value \
                                                                = frt_buffer[AM335X_FRT_PRESSURE];
                                                        printf("the pressure in FRT is %f\n", frt_buffer[AM335X_FRT_PRESSURE]);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].data[am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].data[am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index].value \
                                                                = frt_buffer[AM335X_FRT_AVE_WIND_SPEED];
                                                        printf("the wind speed in frt is %f\n", frt_buffer[AM335X_FRT_AVE_WIND_SPEED]);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].data[am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].data[am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index].value \
                                                                = frt_buffer[AM335X_FRT_AVE_WIND_DIRECTION];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].data[am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count = 0;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].data[am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index].value \
                                                                = frt_buffer[AM335X_FRT_RAINFALL];
                                                        printf("the rain fall in frt is %f\n", frt_buffer[AM335X_FRT_RAINFALL]);
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].data[am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count = 0;


                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].data[am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].index].value \
                                                                = frt_buffer[AM335X_FRT_MIN_WIND_SPEED];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].data[am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].count = 0;


                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].data[am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].index].value \
                                                                = frt_buffer[AM335X_FRT_MAX_WIND_SPEED];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].data[am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].count = 0;


                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].data[am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].index].value \
                                                                = frt_buffer[AM335X_FRT_MIN_WIND_DIRECTION];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].data[am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].count = 0;


                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].data[am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].index].value \
                                                                = frt_buffer[AM335X_FRT_MAX_WIND_DIRECTION];
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].data[am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].index].time  \
                                                                = tv.tv_sec;

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].index++;

                                                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].index >= AM335X_SENSOR_DATA_MAX_SIZE)
                                                        {
                                                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].index = 0;
                                                        }

                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].count = 0;
                                                        memset(buffer, 0, sizeof(buffer));
                                                        break;

                                                default:
                                                        printf("**************inside default***************\n");
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].count++;
                                                        am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].count++;
                                                        memset(buffer, 0, sizeof(buffer));
                                        }
                                }

                        }

                }
                else
                {
                        printf("error **********************ret < 0\n");
                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count++;
                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count++;

                        if(sensor_arg->stype == AM335X_SENSOR_TYPE_FRT)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].count++;
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].count++;
                        }

                        if(1 == log_prt)
                                am335x_printlog("%s: Data Read Timeout\r\n", am335x_sensor_dev->gas_name);
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].flags = AM335X_SENSOR_RUN_OK;
                }
                else if(am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count > 5)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].flags = AM335X_SENSOR_RUN_ERR;

                        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].flags = AM335X_SENSOR_RUN_OK;
                }
                else if(am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count > 5)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].flags = AM335X_SENSOR_RUN_ERR;

                        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].flags = AM335X_SENSOR_RUN_OK;
                }
                else if(am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count > 5)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].flags = AM335X_SENSOR_RUN_ERR;

                        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].flags = AM335X_SENSOR_RUN_OK;
                }
                else if(am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count > 5)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].flags = AM335X_SENSOR_RUN_ERR;

                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].flags = AM335X_SENSOR_RUN_OK;
                }
                else if(am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count > 5)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].flags = AM335X_SENSOR_RUN_ERR;

                        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count == 0)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].flags = AM335X_SENSOR_RUN_OK;
                }
                else if(am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count > 5)
                {
                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].flags = AM335X_SENSOR_RUN_ERR;

                        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].count = AM335X_SENSOR_TRY_TIMES;
                }

                if(sensor_arg->stype == AM335X_SENSOR_TYPE_FRT)
                {
                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].count == 0)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].flags = AM335X_SENSOR_RUN_OK;
                        }
                        else if(am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].count > 5)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].flags = AM335X_SENSOR_RUN_ERR;

                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_SPEED].count = AM335X_SENSOR_TRY_TIMES;
                        }

                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].count == 0)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].flags = AM335X_SENSOR_RUN_OK;
                        }
                        else if(am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].count > 5)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].flags = AM335X_SENSOR_RUN_ERR;

                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_SPEED].count = AM335X_SENSOR_TRY_TIMES;
                        }

                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].count == 0)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].flags = AM335X_SENSOR_RUN_OK;
                        }
                        else if(am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].count > 5)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].flags = AM335X_SENSOR_RUN_ERR;

                                am335x_sensor_mem[AM335X_SENSOR_DATA_MIN_WIND_DIRECTION].count = AM335X_SENSOR_TRY_TIMES;
                        }

                        if(am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].count == 0)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].flags = AM335X_SENSOR_RUN_OK;
                        }
                        else if(am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].count > 5)
                        {
                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].flags = AM335X_SENSOR_RUN_ERR;

                                am335x_sensor_mem[AM335X_SENSOR_DATA_MAX_WIND_DIRECTION].count = AM335X_SENSOR_TRY_TIMES;
                        }	

                        i = am335x_sensor_flag_read(AM335X_SENSOR_DATA_TEMPERATURE);
                        printf("AM335X_SENSOR_DATA_TEMPERATURE FLAG = %d\n", i);
                }
        }

        close(fd);       

        shmdt((char*)am335x_sensor_mem);

        am335x_closelog("am335x driver");
		g_pthread_exit = -26;
        pthread_exit(0);

}
/************************************************************
Name: am335x_sensor_flag_read
Func: get sensor data status
Para:
int item - sensor data number
 *************************************************************/
int am335x_sensor_flag_read(int item)
{ 
        struct am335x_sensor_mem_t *am335x_sensor_mem;
        int flags;
        int shmid;

        shmid = shmget(AM335X_SENSOR_DATA_ID, AM335X_SENSOR_DATA_MAX_NUMS * sizeof(struct am335x_sensor_mem_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                return -1;
        }

        am335x_sensor_mem = (struct am335x_sensor_mem_t*)shmat(shmid, 0, 0);

        if(am335x_sensor_mem == (struct am335x_sensor_mem_t*)(-1))
        {
                return -2;
        }

        flags = am335x_sensor_mem[item].flags;
        shmdt((char*)am335x_sensor_mem);

        return flags;
}

int emmc_test()
{
        int fd, i, errno, errno1;
        char buffer[10];
        fd = open("/dev/mmcblk1p3", O_RDWR|O_SYNC);	
        if(fd < 0)
        {
                printf("emmc_test: open /dev/mmcblk1p3 failed\r\n");
                return -1;
        }
        memset(buffer, 0xa5, 10);
        errno = write(fd, buffer, 10);
        if(errno < 0)
        {
                printf("write emmc failed\n");
        }	
        memset(buffer, 0x00, 10);
        errno1 = read(fd, buffer, 10);
        if(errno1 < 0)
        {
                printf("read emmc failed\n");
        }
        for(i = 0; i < 10; i++)
        {
                printf("buffer[%d\n]= %d\n", i, buffer[i]);
        }
        return 0;
}

/************************************************************
Name: am335x_sensor_data_read
Func: read sensor data
Para:
int                         item  - sensor data number	数据类型
struct am335x_sensor_data_t data  - data to store
int                         len   - number of data to read	要读的数据个数 70
unsigned int                start - start time, unit is second
unsigned int                end   - end time, unit is second
end should be bigger than start
 *************************************************************/
int am335x_sensor_data_read(int item, struct am335x_sensor_data_t *data, int len, unsigned int start, unsigned int end)
{
        struct am335x_sensor_mem_t *am335x_sensor_mem;
        unsigned int index;
        unsigned int first;
        unsigned int last;
        unsigned int total;
        int shmid;

        shmid = shmget(AM335X_SENSOR_DATA_ID, AM335X_SENSOR_DATA_MAX_NUMS * sizeof(struct am335x_sensor_mem_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                return -1;
        }

        am335x_sensor_mem = (struct am335x_sensor_mem_t*)shmat(shmid, 0, 0);

        if(am335x_sensor_mem == (struct am335x_sensor_mem_t*)(-1))
        {
                return -2;
        }

        index = am335x_sensor_mem[item].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {


                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        first = index;

        while(am335x_sensor_mem[item].data[first].time > start)
        {
                if(first > 0)
                {
                        first--;
                }
                else
                {
                        first = AM335X_SENSOR_DATA_MAX_SIZE - 1;
                }
        }

        last = index;

        while(am335x_sensor_mem[item].data[last].time > end)
        {
                last--;

                if(last > 0)
                {
                        last--;
                }
                else
                {
                        last = AM335X_SENSOR_DATA_MAX_SIZE - 1;
                }
        }

        if(last > first)
        {
                total = last - first + 1;

                if(total > len)

                {
                        shmdt((char*)am335x_sensor_mem);
                        return -3;
                }

                memcpy((char*)&data[0],(char*)&am335x_sensor_mem[item].data[first], total * sizeof(struct am335x_sensor_data_t));
        }
        else if(last < first)
        {
                total = AM335X_SENSOR_DATA_MAX_SIZE + last - first + 1;

                if(total > len)
                {
                        shmdt((char*)am335x_sensor_mem);
                        return -4;
                }

                memcpy((char*)&data[0],(char*)&am335x_sensor_mem[item].data[first], (total - (last + 1)) * sizeof(struct am335x_sensor_data_t));

                memcpy((char*)&data[total - (last + 1)], (char*)&am335x_sensor_mem[item].data[0], (last + 1) * sizeof(struct am335x_sensor_data_t));
        }
        else
        {
                shmdt((char*)am335x_sensor_mem);

                return -5;        
        }

        shmdt((char*)am335x_sensor_mem);


        return total;
}

/************************************************************
Name: am335x_sensor_show
Func: show all sensor data,debug function
Para:
none
 *************************************************************/
int am335x_sensor_show()
{
        struct am335x_sensor_mem_t *am335x_sensor_mem;
        unsigned int index;
        int shmid;

        shmid = shmget(AM335X_SENSOR_DATA_ID, AM335X_SENSOR_DATA_MAX_NUMS * sizeof(struct am335x_sensor_mem_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                return -1;
        }

        am335x_sensor_mem = (struct am335x_sensor_mem_t*)shmat(shmid, 0, 0);

        if(am335x_sensor_mem == (struct am335x_sensor_mem_t*)(-1))
        {
                return -2;
        }

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_SO2].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("SO2       : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_SO2].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_SO2].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_SO2].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_SO2].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_NO2].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("NO2       : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_NO2].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_NO2].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_NO2].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_NO2].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_CO].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("CO        : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CO].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CO].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CO].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CO].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_O3].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("O3        : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_O3].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_O3].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_O3].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_O3].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("PM1.0     : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("PM2.5     : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("PM10      : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("PM1.0(CF) : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("PM2.5(CF) : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].types);

        index = am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].index;

        if(index > 0)
        {
                index -= 1;
        }
        else
        {
                index = AM335X_SENSOR_DATA_MAX_SIZE - 1;
        }

        printf("PM10 (CF) : index = %06d, value = %05d, time = 0x%08x, flags = 0x%08x, type = %d\r\n", index,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].data[index].value,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].data[index].time,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].flags,
                        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].types);

        printf("\r\n\r\n");

        shmdt((char*)am335x_sensor_mem);

        return 0;
}

/************************************************************
Name: at_cmd_send
Func: AT CMD send
Para:
int   ttyACM - 4g module ACM serial
char *cmd    - cmd data
 *************************************************************/
int at_cmd_send(int ttyACM, char *cmd)
{
        am335x_printlog("AT Write: %s\r\n",cmd);

        if(write(ttyACM, cmd, strlen(cmd)) < 0)
        {
                return -1;
        }

        return 0;
}

/************************************************************
Name: at_cmd_read
Func: AT CMD response read
Para:
int   ttyACM  - 4g module ACM serial
char *result  - cmd response buffer
int   length  - length of result buffer
int   timeout - timeout to return
 *************************************************************/
int at_cmd_read(int ttyACM, char *result, int length, int timeout)
{
        struct timeval tv;
        fd_set rfds;
        int ret;

        FD_ZERO(&rfds);
        FD_SET(ttyACM, &rfds);

        tv.tv_sec  = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        ret = select(ttyACM+1, &rfds, NULL, NULL, &tv);

        if(ret > 0)
        {
                if(read(ttyACM, result, length) < 0)
                {
                        return -1;
                }
                else
                {
                        am335x_printlog("AT Read : %s\r\n", result);

                        return 0;
                }
        }
        else
        {
                return -2;
        }
}

/************************************************************
Name: at_cmd_parse_result
Func: find sub string in AT CMD response
Para:
char *result - cmd response buffer
char *substr - find substr in result string
 *************************************************************/
int at_cmd_parse_result(char *result, char *substr)
{
        char *rs;
        rs = strstr(result, substr);
        if(rs == NULL)
        {
                return -1;
        }
        return 0;
}

/************************************************************
Name: at_cmd_parse_value
Func: get value from AT CMD response
Para:
char *cmd    - which cmd response to parse
char *result - cmd response buffer
int   index  - which value
char *value  - value store buffer
 *************************************************************/
int at_cmd_parse_value(char *cmd, char *result, int index, char *value)
{
        int  len;
        char *st;
        char *ed;
        char *lr;
        int  i;

        if(cmd != NULL)
        {
                ed = strstr(result, cmd);
        }
        else
        {
                ed = result;
        }

        if(ed == NULL)
        {
                return -1;
        }

        st = strstr(ed, ":");

        if(st == NULL)
        {
                return -2;
        }

        st += 1;

        for(i = 0; i < index; i++)
        {
                ed = strstr(st, ",");

                if(ed == NULL)
                {
                        return -3;
                }

                st = ed + 1;
        }

        while((*st == '"') || (*st == ' '))
        {
                st += 1;
        }

        ed = strstr(st, ",");
        lr = strstr(st, "\r\n");

        if(ed == NULL)
        {
                ed = lr;
        }
        else
        {
                if((lr != NULL) && (ed > lr))
                {
                        ed = lr;
                }
        }

        if(ed == NULL)
        {
                return -4;
        }

        while((*(ed - 1) == '"') || (*(ed - 1) == ' '))
        {
                ed -= 1;
        }

        len = (unsigned int)ed - (unsigned int)st;

        if(len <= 0)
        {
                return -5;
        }
        else
        {
                memcpy(value, st, len);

                value[len] = '\0';
        }

        return 0;
}

/************************************************************
 Name: get_frt_value
 Func: 将富奥通气象传感器的数据做转换
 Para:
 char *value  - value store buffer

 ************************************************************/
float get_frt_value(char *value)
{

        int i, j = 0;
        float frt_buffer[1];
        float f_value = 0;
        for(i = 0; i < strlen(value); i++)
        {
                if(value[i] == '.')
                {
                        f_value = (value[i+1] - '0')*(0.1); 
                        i++;
                }
                else
                {
                        frt_buffer[j] = frt_buffer[j]*10 + value[i] - '0';										
                }
        }
        frt_buffer[j] += f_value; 
        return frt_buffer[j];

}
/************************************************************
Name: find_frt_value
Func: get value from FRT Sensor response
Para:
char *cmd      - which cmd response to parse
char *cmd2    - which cmd response to parse

char *result - cmd response buffer
int   index  - which value
char *value  - value store buffer
 *************************************************************/
int find_frt_value(char *cmd, char *cmd2, char *result, int index, char *value)
{
        int  len;
        char *st;
        char *ed;
        char *lr;
        int i;

        if((cmd != NULL) && (cmd2 != NULL))
        {
                ed = strstr(result, cmd);
                ed = ed + 3;
                //printf("ed=%s\n",ed);

                for(i = 0; i < index; i++)
                {
                        st = strstr(ed, cmd2);

                        if(st == NULL)
                        {
                                return -3;
                        }

                }
                //printf("st=%s\n",st);
        }
        else
        {
                ed = result;
                st = result;
        }

        if((ed == NULL) && (st == NULL))
        {
                return -1;
        }

        len = (unsigned int)st - (unsigned int)ed;
        //printf("len=%d\n",len);
        if(len <= 0)
        {
                return -5;
        }
        else
        {
                memcpy(value, ed, len);

                value[len] = '\0';
                //printf("value=%s\n",value);
        }

        return 0;
}


/************************************************************
Name: at_cmd_execute
Func: AT CMD send, AT CMD response read and check "OK"
Para:
int   ttyACM  - 4g module ACM serial
char *cmd     - which cmd response to parse
char *result  - cmd response buffer
int   length  - length of result buffer
int   timeout - timeout to return
 *************************************************************/
int at_cmd_execute(int ttyACM, char *cmd, char *result, int len, int timeout)
{
        int try_times;

        try_times = 3;

        while(try_times > 0)
        {
                memset(result, 0, len);

                if(at_cmd_send(ttyACM, cmd) < 0)
                {
                        try_times--;

                        sleep(1);

                        continue;
                }

                if(at_cmd_read(ttyACM, result, 256, timeout) < 0)
                {
                        try_times--;

                        sleep(1);

                        continue;
                }

                if(at_cmd_parse_result(result, "OK") != 0)
                {
                        try_times--;

                        sleep(1);

                        continue;
                }
                else
                {
                        break;
                }
        }

        if(try_times <= 0)
        {
                return -1;
        }

        return 0;
}

/************************************************************
Name: am335x_4g_module_power
Func: turn on/off 4g module power
Para:
int on: 1- power on, 0- popwer off
 *************************************************************/
int am335x_4g_module_power(int on)
{
#if(AM335X_BOARD_VERSION != 0x10)
        char *gpio_regs;
        char *ctrl_regs;
        int   mem_fd;
        int   bit;

        mem_fd = open("/dev/mem", O_RDWR|O_SYNC);

        if(mem_fd < 0)
        {
                am335x_printlog("am335x_4g_module_power: /dev/mem open failed\r\n");

                return -1;
        }

        ctrl_regs = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x44e00000);

        if(ctrl_regs == (char*)MAP_FAILED)
        {
                am335x_printlog("am335x_4g_module_power: mmap ctrl regs failed\r\n");

                close(mem_fd);

                return -2;
        }

        bit = *(volatile unsigned int*)(ctrl_regs + 0x0ac);
        if((bit & 0x00000003) != 0x00000002)
        {
                *(volatile unsigned int*)(ctrl_regs + 0x0ac) |= 0x00000002;
        }

        munmap(ctrl_regs, 0x1000);

        gpio_regs = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x4804c000);

        if(gpio_regs == (char*)MAP_FAILED)
        {
                am335x_printlog("am335x_4g_module_power: mmap gpio1 regs failed\r\n");

                close(mem_fd);

                return -3;
        }

        bit = *(volatile unsigned int*)(gpio_regs + 0x130);
        if((bit & 0x00000001) != 0)
        {
                *(volatile unsigned int*)(gpio_regs + 0x130) = 0x00000002;
        }

        *(volatile unsigned int*)(gpio_regs + 0x134) &= (~(1 << (AM335X_GPIO_4GM_PWR % 32)));//输出模式
        *(volatile unsigned int*)(gpio_regs + 0x134) &= (~(1 << (AM335X_GPIO_PCI_RST % 32)));

        if(on != 0)
        {
                *(volatile unsigned int*)(gpio_regs + 0x190) = (1 << (AM335X_GPIO_PCI_RST % 32));
                usleep(100000);
                *(volatile unsigned int*)(gpio_regs + 0x190) = (1 << (AM335X_GPIO_4GM_PWR % 32));
                usleep(100000);
                *(volatile unsigned int*)(gpio_regs + 0x194) = (1 << (AM335X_GPIO_PCI_RST % 32));
        }
        else
        {
                *(volatile unsigned int*)(gpio_regs + 0x190) = (1 << (AM335X_GPIO_PCI_RST % 32));
                usleep(100000);
                *(volatile unsigned int*)(gpio_regs + 0x194) = (1 << (AM335X_GPIO_4GM_PWR % 32));
        }

        munmap(gpio_regs, 0x1000);

        close(mem_fd);
#endif

        return 0;
}

/************************************************************
Name: am335x_4g_module_acm_open
Func: open 4g module at cmd channel
Para:
none
 *************************************************************/
int am335x_4g_module_acm_open()
{
        struct dirent *ent;
        DIR* usbPath;
        char ttyName[256];
        char buffer[256];
        int  ttyACM, ttyTEST;
        char *value;

        if((ttyTEST = open("/dev/ttyUSB2", O_RDWR | O_NOCTTY | O_NDELAY)) >= 0)
        {
                am335x_printlog("open /dev/ttyUSB2 success\r\n");
        }
        else if((ttyTEST = open("/dev/ttyUSB3", O_RDWR | O_NOCTTY | O_NDELAY)) >= 0)
        {
                am335x_printlog("open /dev/ttyUSB3 success\r\n");
        }
        else if((ttyTEST = open("/dev/ttyACM2", O_RDWR | O_NOCTTY | O_NDELAY)) >= 0)
        {
                am335x_printlog("open /dev/ttyACM2 success\r\n");		
        }
        else if((ttyTEST = open("/dev/ttyUSB4", O_RDWR | O_NOCTTY | O_NDELAY)) >= 0)
        {
                am335x_printlog("open /dev/ttyUSB4 success\r\n");		
        }
        else if((ttyTEST = open("/dev/ttyUSB5", O_RDWR | O_NOCTTY | O_NDELAY)) >= 0)
        {
                am335x_printlog("open /dev/ttyUSB5 success\r\n");		
        }
        else
        {
                am335x_printlog("open test failed\r\n");
        }

        am335x_uart_set_opt(ttyTEST, 115200, 8, 'N', 1, 255, 15);
        if(at_cmd_execute(ttyTEST, "AT+CGMM\r\n", buffer, 256, 5000) < 0)
        {
                am335x_printlog("AT CMD: AT+CGMM execute failure\r\n");
        }
        else
        {
                am335x_printlog("AAAAAAAAAAAAA buffer = %s\n",buffer);
                if(at_cmd_parse_result(buffer,"U9300C") < 0)
                {
                        am335x_printlog("4G module is not 9300\r\n");
                }else
                {
                        am335x_printlog("4G module is 9300\r\n");
                        g_4g_type = AM335X_4G_MODULE_LONGSUNG_U9300;
                }
        }

        if(at_cmd_execute(ttyTEST, "AT+CGMM?\r\n", buffer, 256, 5000) < 0)
        {
                am335x_printlog("AT CMD: AT+CGMM? execute failure\r\n");
        }
        else
        {
                if(at_cmd_parse_result(buffer,"+CGMM: \"L810 LTE Module\",\"L810\"") < 0)
                {
                        am335x_printlog("4G module is not L810\r\n");
                }else
                {
                        am335x_printlog("4G module is L810\r\n");
                        g_4g_type = AM335X_4G_MODULE_FIBOCOM_L810;
                }
        }

        close(ttyTEST);
#if 1
        switch(g_4g_type)
        {
                case AM335X_4G_MODULE_FIBOCOM_L710:
                        usbPath = opendir(AM335X_4G_MODULE_FIBOCOM_L710_ACM_SYS);
                        break;

                case AM335X_4G_MODULE_FIBOCOM_L810:
                        usbPath = opendir(AM335X_4G_MODULE_FIBOCOM_L810_ACM_SYS);
                        break;

                case AM335X_4G_MODULE_HUAWEI_ME909S:
                        printf("inside am335x_4g_module_acm_open(int type) case AM335X_4G_MODULE_HUAWEI_ME909S\n");
                        usbPath = opendir(AM335X_4G_MODULE_HUAWEI_ME909S_ACM_SYS);
                        break;

                case AM335X_4G_MODULE_LONGSUNG_U9300:
                        usbPath = opendir(AM335X_4G_MODULE_LONGSUNG_U9300_ACM_SYS);
                        break;

                default:
                        return -1;
        }
#endif

        if(usbPath == NULL)
        {
                am335x_printlog("4G module open acm sys dir failed\r\n");

                return -2;
        }

        while((ent = readdir(usbPath)) != NULL)
        {
                am335x_printlog("the file name is %s\r\n", ent->d_name);
                if((strstr(ent->d_name, "ttyUSB") != NULL) || (strstr(ent->d_name, "ttyACM") != NULL))
                {
                        memset(ttyName, 0, sizeof(ttyName));

                        sprintf(ttyName, "/dev/%s", ent->d_name);

                        closedir(usbPath);

                        am335x_printlog("4G module AT channel is %s\r\n", ttyName);

                        ttyACM = open(ttyName, O_RDWR | O_NOCTTY | O_NDELAY);

                        return ttyACM;
                }
        }

        printf("ent is NULL Why ???\n");
        closedir(usbPath);

        return -3;
}

/************************************************************
Name: am335x_4g_module_net_name
Func: open 4g module net name
Para:
none
 *************************************************************/
int am335x_4g_module_net_name(int type, char *netname)
{
        struct dirent *ent;
        DIR* usbPath;
        int  flag;

        switch(type)
        {
                case AM335X_4G_MODULE_FIBOCOM_L710:
                        usbPath = opendir(AM335X_4G_MODULE_FIBOCOM_L710_NET_SYS);
                        break;

                case AM335X_4G_MODULE_FIBOCOM_L810:
                        usbPath = opendir(AM335X_4G_MODULE_FIBOCOM_L810_NET_SYS);
                        break;

                case AM335X_4G_MODULE_HUAWEI_ME909S:
                        usbPath = opendir(AM335X_4G_MODULE_HUAWEI_ME909S_NET_SYS);
                        break;

                case AM335X_4G_MODULE_LONGSUNG_U9300:
                        usbPath = opendir(AM335X_4G_MODULE_LONGSUNG_U9300_NET_SYS);
                        break;

                default:
                        return -1;
        }

        if(usbPath == NULL)
        {
                am335x_printlog("4G module open net sys dir failed\r\n");

                return -2;
        }

        while((ent = readdir(usbPath)) != NULL)
        {
                if((strstr(ent->d_name, "eth") != NULL) || (strstr(ent->d_name, "wwan") != NULL) || (strstr(ent->d_name, "usb") != NULL))
                {
                        strcpy(netname, ent->d_name);

                        closedir(usbPath);

                        am335x_printlog("4G module net name is %s\r\n", netname);

                        return 0;
                }
        }

        closedir(usbPath);

        return -3;
}

/************************************************************
Name: am335x_4g_module_check
Func: 4g module hw check
Para:
int   ttyACM - 4g module ACM serial
int   flag   - 0: check serial and sim card;
1: check serial, sim card and signal
 *************************************************************/
int am335x_4g_module_check(int type, int ttyACM, int flag)
{
        char buffer[256];
        char model[32];
        char name[32];

        if(at_cmd_execute(ttyACM, "AT\r\n", buffer, 256, 10000) < 0)
        {
                am335x_printlog("AT CMD: AT execute failure\r\n");

                return -1;
        }

        sleep(1);

        if(at_cmd_execute(ttyACM, "ATE0\r\n", buffer, 256, 5000) < 0)
        {
                am335x_printlog("AT CMD: ATE execute failure\r\n");

                return -2;
        }

        sleep(1);


        if((type == AM335X_4G_MODULE_LONGSUNG_U9300) || (type == AM335X_4G_MODULE_HUAWEI_ME909S))
        {
                if(at_cmd_execute(ttyACM, "AT+CGMM\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT+CGMM? execute failure\r\n");

                        return -3;
                }
        }else
        {
                if(at_cmd_execute(ttyACM, "AT+CGMM?\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT+CGMM? execute failure\r\n");

                        return -3;
                }
        }

        sleep(1);

        if(AM335X_4G_MODULE_FIBOCOM_L810 == type)
        {
                if(at_cmd_execute(ttyACM, "AT+XACT=2\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT+XACT=2 execute failure\r\n");

                        return -5;
                }

                sleep(10);
        }else if(AM335X_4G_MODULE_FIBOCOM_L710 == type)
        {
                if(at_cmd_execute(ttyACM, "AT+GTRAT=0\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT+GTRAT=0 execute failure\r\n");

                        return -6;
                }

        }else if(AM335X_4G_MODULE_HUAWEI_ME909S == type)   
        {

                if(at_cmd_execute(ttyACM, "AT^SYSCFGEX=\"03\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT^SYSCFGEX=\"03\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,, execute failure\r\n");

                        return -7;
                }
        }else //U9300C
        {

#if 0
                   if(at_cmd_execute(ttyACM, "AT+MODODR=2\r\n", buffer, 256, 5000) < 0)
                   {
                   am335x_printlog("AT CMD: AT+MODODR=2 execute failure\r\n");

                   return -8;
                   }
#endif
        }

        if(at_cmd_execute(ttyACM, "AT+CPIN?\r\n", buffer, 256, 5000) < 0)
        {
                am335x_printlog("AT CMD: AT+CPIN? CMD execute failure\r\n");

                return -8;
        }

        if(check_flag != 0)
        {
                if(at_cmd_parse_result(buffer, "+CPIN: READY") != 0)
                {
                        am335x_printlog("4G module need PIN Code or has no SIM Card\r\n");

                        return -9;
                }
        }
        sleep(1);

        if(flag)
        {
                if(at_cmd_execute(ttyACM, "AT+CSQ\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT+CSQ CMD execute failure\r\n");

                        return -8;
                }

                if(at_cmd_parse_result(buffer, "+CSQ: 99") == 0)
                {
                        am335x_printlog("4G module has no single\r\n");

                        return -9;
                }

                sleep(1);
        }

        return 0;
}

/************************************************************
Name: am335x_4g_module_net_connect
Func: 4g module network connect
Para:
int   ttyACM - 4g module ACM serial
 *************************************************************/
int am335x_4g_module_net_connect(int type, int ttyACM)
{
        char buffer[256];
        int  net_operator;

        if(at_cmd_execute(ttyACM, "AT+COPS?\r\n", buffer, 256, 5000) < 0)
        {
                am335x_printlog("AT CMD: AT+COPS? execute failure\r\n");

                return -1;
        }

        if((at_cmd_parse_result(buffer, "CHINA MOBILE") == 0) || (at_cmd_parse_result(buffer, "CHN MOBILE") == 0))
        {
                net_operator = AM335X_4G_MODULE_CMCC;

                am335x_printlog("4G module has resgisted on CHINA MOBILE\r\n");
        }
        else if((at_cmd_parse_result(buffer, "CHINA-UNICOM") == 0) || (at_cmd_parse_result(buffer, "CHN-UNICOM") == 0))
        {
                net_operator = AM335X_4G_MODULE_CUCC;

                am335x_printlog("4G module has resgisted on CHINA UNICOM\r\n");
        }
        else
        {
                net_operator = AM335X_4G_MODULE_NONE;

                am335x_printlog("4G module has not resgisted on any network\r\n");

                return -2;
        }

        sleep(1);

        switch(type)
        {

                case AM335X_4G_MODULE_FIBOCOM_L710:     
                        if(at_cmd_execute(ttyACM, "AT+GTRAT=6,3,2\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+GTRAT=6,3,2 execute failure\r\n");

                                return -3;
                        }  

                        break;

                case AM335X_4G_MODULE_FIBOCOM_L810:
                        if(net_operator == AM335X_4G_MODULE_CMCC)
                        {
                                if(at_cmd_execute(ttyACM, "AT+XACT=6,2,1\r\n", buffer, 256, 5000) < 0)
                                {
                                        am335x_printlog("AT CMD: AT+XACT=6,2,1 execute failure\r\n");

                                        return -3;
                                }
                        }
                        else
                        {
                                if(at_cmd_execute(ttyACM, "AT+XACT=3,1,0\r\n", buffer, 256, 5000) < 0)
                                {
                                        am335x_printlog("AT CMD: AT+XACT=3,1,0 execute failure\r\n");

                                        return -4;
                                }
                        }

                        break;

                case AM335X_4G_MODULE_HUAWEI_ME909S:                   
                        if(at_cmd_execute(ttyACM, "AT^SYSCFGEX=\"03\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT^SYSCFGEX=\"030201\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,, execute failure\r\n");

                                return -5;
                        }

                        break; 

                case AM335X_4G_MODULE_LONGSUNG_U9300:									
                        if(at_cmd_execute(ttyACM, "AT+MODODR=2\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+MODODR=2 execute failure\r\n");

                                return -5;
                        }

                        break; 

                default:
                        return -3;
        }

        sleep(10);

        if(at_cmd_execute(ttyACM, "AT+COPS?\r\n", buffer, 256, 5000) < 0)
        {
                am335x_printlog("AT CMD: AT+COPS? execute failure\r\n");

                return -4;
        }

        sleep(1);

        if(at_cmd_execute(ttyACM, "AT+CGREG=2\r\n", buffer, 256, 5000) < 0)
        {
                am335x_printlog("AT CMD: AT+CGREG=2 execute failure\r\n");

                return -5;
        }

        sleep(1);

        switch(type)
        {
                case AM335X_4G_MODULE_FIBOCOM_L710:
                        if(at_cmd_execute(ttyACM, "AT+GTRNDIS=0,1\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+GTRNDIS=0,1 CMD execute failure\r\n");
                        }

                        if(at_cmd_execute(ttyACM, "AT+CGACT=0,1\r\n", buffer, 256, 40000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+CGACT=0,1 execute failure\r\n");
                        }

                        break;

                case AM335X_4G_MODULE_FIBOCOM_L810:
                        if(at_cmd_execute(ttyACM, "AT+CGACT=0,1\r\n", buffer, 256, 40000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+CGACT=0,1 execute failure\r\n");

                                return -6;
                        }

                        break;

                case AM335X_4G_MODULE_HUAWEI_ME909S:
                        if(at_cmd_execute(ttyACM, "AT+CGACT=0,1\r\n", buffer, 256, 40000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+CGACT=0,1 execute failure\r\n");

                                return -6;
                        }

                        break;

                case AM335X_4G_MODULE_LONGSUNG_U9300:
                        if(at_cmd_execute(ttyACM, "AT+CGACT=1,1\r\n", buffer, 256, 40000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+CGACT=1,1 execute failure\r\n");

                                return -6;
                        }

                        break;		

                default:
                        return -6;
        }

        sleep(1);

        if(net_operator == AM335X_4G_MODULE_CMCC)
        {
                switch(type)
                {
                        case AM335X_4G_MODULE_HUAWEI_ME909S:
                                if(at_cmd_execute(ttyACM, "AT^NDISDUP=1,1,\"cmnet\"\r\n", buffer, 256, 5000) < 0)
                                {
                                        am335x_printlog("AT CMD: AT^NDISDUP=1,1,cmnet CMD execute failure\r\n");

                                        return -9;            
                                }
                                break;

                        default:
                                if(at_cmd_execute(ttyACM, "AT+CGDCONT=1,\"ip\",\"cmnet\"\r\n", buffer, 256, 5000) < 0)
                                {
                                        am335x_printlog("AT CMD: AT+CGDCONT=1, ip, cmnet CMD execute failure\r\n");

                                        return -7;            
                                }
                }
        }else if(net_operator == AM335X_4G_MODULE_CUCC)
        {
                switch(type)
                {
                        case AM335X_4G_MODULE_HUAWEI_ME909S:
                                if(at_cmd_execute(ttyACM, "AT^NDISDUP=1,1,\"3gnet\"\r\n", buffer, 256, 5000) < 0)
                                {
                                        am335x_printlog("AT CMD: AT^NDISDUP=1,1,3gnet CMD execute failure\r\n");

                                        return -9;            
                                }
                                break;

                        default:
                                if(at_cmd_execute(ttyACM, "AT+CGDCONT=1,\"ip\",\"3gnet\"\r\n", buffer, 256, 5000) < 0)
                                {
                                        am335x_printlog("AT CMD: AT+CGDCONT=1, ip, 3gnet CMD execute failure\r\n");

                                        return -8;
                                }
                }
        }
#if 0
        if((net_operator == AM335X_4G_MODULE_CMCC) && (type != AM335X_4G_MODULE_HUAWEI_ME909S))
        {
                if(at_cmd_execute(ttyACM, "AT+CGDCONT=1,\"ip\",\"cmnet\"\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT+CGDCONT=1, ip, cmnet CMD execute failure\r\n");

                        return -7;            
                }
        }
        else if((net_operator == AM335X_4G_MODULE_CUCC) && (type != AM335X_4G_MODULE_HUAWEI_ME909S))
        {
                if(at_cmd_execute(ttyACM, "AT+CGDCONT=1,\"ip\",\"3gnet\"\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT+CGDCONT=1, ip, 3gnet CMD execute failure\r\n");

                        return -8;
                }
        }
        else if((net_operator == AM335X_4G_MODULE_CMCC) && (type == AM335X_4G_MODULE_HUAWEI_ME909S))
        {
                if(at_cmd_execute(ttyACM, "AT^NDISDUP=1,1,\"cmnet\"\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT^NDISDUP=1,1,cmnet CMD execute failure\r\n");

                        return -9;            
                }
        }
        else 
        {
                if(at_cmd_execute(ttyACM, "AT^NDISDUP=1,1,\"3gnet\"\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT^NDISDUP=1,1,3gnet CMD execute failure\r\n");

                        return -9;            
                }
        }
#endif        

        sleep(1);
#if 0
        if(at_cmd_execute(ttyACM, "AT+CGACT=1,1\r\n", buffer, 256, 150000) < 0)
        {
        am335x_printlog("AT CMD: AT+CGACT=1,1 CMD execute failure\r\n");
  
        return -9;
        }
#endif
        sleep(1);

        if(AM335X_4G_MODULE_FIBOCOM_L810 == type)
        {
                if(at_cmd_execute(ttyACM, "AT+XDNS=1,1\r\n", buffer, 256, 5000) < 0)
                {
                        am335x_printlog("AT CMD: AT+XDNS=1,1 CMD execute failure\r\n");

                        return -10;
                }

                sleep(1);
        }

        return 0;
}

/************************************************************
Name: am335x_4g_module_net_enable
Func: 4g module enable data state
Para:
int   ttyACM - 4g module ACM serial
 *************************************************************/
int am335x_4g_module_net_enable(int type, int ttyACM)
{
        char buffer[256];
        sleep(1);
        switch(type)
        {
                case AM335X_4G_MODULE_FIBOCOM_L710:
                        if(at_cmd_execute(ttyACM, "AT+GTRNDIS=1,1\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+GTRNDIS=1,1 CMD execute failure\r\n");

                                return -1;
                        }

                        break;

                case AM335X_4G_MODULE_FIBOCOM_L810:
                        if(at_cmd_execute(ttyACM, "AT+XDATACHANNEL=1,1,\"/USBCDC/2\",\"/USBHS/NCM/0\",0\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+XDATACHANNEL CMD execute failure\r\n");

                                return -1;
                        }

                        sleep(1);

                        if(at_cmd_execute(ttyACM, "AT+CGDATA=\"M-RAW_IP\",1\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+CGDATA CMD execute failure\r\n");

                                return -2;
                        }
                        break;

                case AM335X_4G_MODULE_HUAWEI_ME909S:

                        break;

                case AM335X_4G_MODULE_LONGSUNG_U9300:
		               /*if(at_cmd_execute(ttyACM, "AT+MODODR=2\r\n", buffer, 256, 5000) < 0)
		                          {
		                          am335x_printlog("AT CMD: AT+MODODR=2 CMD execute failure\r\n");

		                          return -1;
		                          }*/

                        if(at_cmd_execute(ttyACM, "AT+DATASTATUSEN=1\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+DATASTATUSEN=1 CMD execute failure\r\n");
                        }

                        //检查之前有没有拨号存在，若有则删除
                        if(at_cmd_execute(ttyACM, "AT$QCRMCALL=0,1\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT$QCRMCALL=0,1 CMD execute failure\r\n");
                        }

                        if(at_cmd_execute(ttyACM, "AT$QCRMCALL=1,1,1,2,1\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT$QCRMCALL=1,1,1,2,1 CMD execute failure\r\n");

                                return -2;
                        }

                        break;
                default:
                        return -1;
        }

        sleep(1);

        return 0;
}

/************************************************************
Name: am335x_4g_module_net_config
Func: 4g module config net interface
Para:
int   type   - 4g module type
int   ttyACM - 4g module ACM serial
char *newip  - 4g module ip address, output
 *************************************************************/
int am335x_4g_module_net_config(int type, int ttyACM, char *newip)
{
        char netname[256];
        char buffer[256];
        char gateway[16];
        char pdns[16];
        char sdns[16];
        char ip[16];
        int  dns;
        int  i;

        memset(ip,   0, sizeof(ip));
        memset(pdns, 0, sizeof(pdns));
        memset(sdns, 0, sizeof(sdns));

        switch(type)
        {
                case AM335X_4G_MODULE_FIBOCOM_L710:
                        if(am335x_4g_module_net_name(type, netname) < 0)
                        {
                                am335x_printlog("4G module get net name failure\r\n");

                                return -1;
                        }

                        if(at_cmd_execute(ttyACM, "AT+GTRNDIS?\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+GTRNDIS? execute failure\r\n");

                                return -2;
                        }

                        if(at_cmd_parse_value("+GTRNDIS: 1,1", buffer, 2, ip) < 0)
                        {
                                am335x_printlog("4G module has not got an ip address\r\n");

                                return -3;
                        }
						
                        if(at_cmd_parse_value("+GTRNDIS: 1,1", buffer, 3, pdns) < 0)
                        {
                                am335x_printlog("4G module has not got First DNS ip address\r\n");

                                return -4;
                        }

                        if(at_cmd_parse_value("+GTRNDIS: 1,1", buffer, 4, sdns) < 0)
                        {
                                am335x_printlog("4G module has not got Second DNS ip address\r\n");

                                return -5;
                        }

                        break;

                case AM335X_4G_MODULE_FIBOCOM_L810:
                        if(am335x_4g_module_net_name(type, netname) < 0)
                        {
                                am335x_printlog("4G module get net name failure\r\n");

                                return -1;
                        }

                        if(at_cmd_execute(ttyACM, "AT+CGDCONT?\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+CGDCONT? execute failure\r\n");

                                return -2;
                        }

                        if(at_cmd_parse_value("+CGDCONT: 1,", buffer, 3, ip) < 0)
                        {
                                am335x_printlog("4G module has not got an ip address\r\n");

                                return -3;
                        }
                        sleep(1);

                        if(at_cmd_execute(ttyACM, "AT+XDNS?\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+XDNS? execute failure\r\n");

                                return -4;
                        }

                        if(at_cmd_parse_value("+XDNS: 1,", buffer, 1, pdns) < 0)
                        {
                                am335x_printlog("4G module has not got First DNS ip address\r\n");

                                return -5;
                        }

                        if(at_cmd_parse_value("+XDNS: 1,", buffer, 2, sdns) < 0)
                        {
                                am335x_printlog("4G module has not got Second DNS ip address\r\n");

                                return -6;
                        }

                        break;

                case AM335X_4G_MODULE_HUAWEI_ME909S:

                        break;

                case AM335X_4G_MODULE_LONGSUNG_U9300:

                        system("killall udhcpc");
                        if(am335x_4g_module_net_name(type, netname) < 0)
                        {
                                am335x_printlog("4G module get net name failure\r\n");

                                return -1;
                        }

                        system("ifconfig usb0 up");
                        system("udhcpc -i usb0");

                        sleep(5);
                        if(at_cmd_execute(ttyACM, "AT+CGPADDR\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+CGPADDR? execute failure\r\n");

                                return -4;
                        }

                        if(at_cmd_parse_value("+CGPADDR: 1,", buffer, 1, ip) < 0)
                        {
                                am335x_printlog("4G module has not got an ip address\r\n");

                                return -5;
                        }
                        else
                        {
                                strcpy(newip , ip);
                        }

                        break;

                default:
                        return -1;
        }

        if((type != AM335X_4G_MODULE_HUAWEI_ME909S) && (type != AM335X_4G_MODULE_LONGSUNG_U9300))
        {
                am335x_printlog("4G module has got IP: %s DNS: %s, %s \r\n", ip, pdns, sdns);

                strcpy(newip,   ip);

                strcpy(gateway, ip);

                i = strlen(gateway) - 1;

                while((i >= 0) && (gateway[i] != '.'))
                {
                        gateway[i] = '\0';

                        i--;
                }

                gateway[i + 1] = '1';

                dns = open("/etc/resolv.conf", O_CREAT | O_RDWR | O_TRUNC);

                if(dns < 0)
                {
                        am335x_printlog("4G module /etc/resolv.conf open failure\r\n");

                        return -8;
                }

                sprintf(buffer, "nameserver %s\r\n\0", pdns);

                if(write(dns, buffer, strlen(buffer)) < 0)
                {
                        am335x_printlog("4G module write First DNS failed\r\n");
                }

                sprintf(buffer, "nameserver %s\r\n\0", sdns);

                if(write(dns, buffer, strlen(buffer)) < 0)
                {
                        am335x_printlog("4G module write Second DNS failed\r\n");
                }

                close(dns);

                sprintf(buffer, "ifconfig %s %s netmask 255.255.255.255 up -arp", netname, ip);

                am335x_printlog("%s\r\n", buffer);

                system(buffer);

                system("route del default");

                sprintf(buffer, "route add -host %s dev %s", gateway, netname);

                am335x_printlog("%s\r\n", buffer);

                system(buffer);

                sprintf(buffer, "route add default gw %s dev %s", gateway, netname);

                am335x_printlog("%s\r\n", buffer);

                system(buffer);
        }else
        {
                //system("ifconfig usb0 up");
                //system("udhcpc -i usb0");
        }
        return 0;
}

/************************************************************
Name: am335x_4g_module_net_check
Func: 4g module network check
Para:
int   ttyACM - 4g module ACM serial
char *ip     - current ip address, input
 *************************************************************/
int am335x_4g_module_net_check(int type, int ttyACM, char *ip)
{
        char buffer[256];
        char result[16];

        if(type == AM335X_4G_MODULE_LONGSUNG_U9300)
        {
                if(at_cmd_execute(ttyACM, "AT+CGPADDR\r\n", buffer, 256, 5000) < 0)
                {
                        strcpy(ip, "0.0.0.0");

                        am335x_printlog("AT CMD: AT+CGPADDR execute failure\r\n");

                        return AM335X_4G_MODULE_RET_CMD_ERROR;
                }

                memset(result, 0, sizeof(result));

                if(at_cmd_parse_value("+CGPADDR: 1,", buffer, 1, result) < 0)
                {
                        strcpy(ip, "0.0.0.0");

                        return AM335X_4G_MODULE_RET_CON_ERROR;
                }
        }
        else
        {
                if(at_cmd_execute(ttyACM, "AT+CGREG?\r\n", buffer, 256, 5000) < 0)
                {
                        strcpy(ip, "0.0.0.0");

                        am335x_printlog("AT CMD: AT+CGREG? execute failure\r\n");

                        return AM335X_4G_MODULE_RET_CMD_ERROR;
                }

                memset(result, 0, sizeof(result));

                if(at_cmd_parse_value("+CGREG:", buffer, 1, result) < 0)
                {
                        strcpy(ip, "0.0.0.0");

                        return AM335X_4G_MODULE_RET_REG_ERROR;
                }

                if(strcmp(result, "0") == 0)
                {
                        strcpy(ip, "0.0.0.0");

                        return AM335X_4G_MODULE_RET_REG_ERROR;
                }

                if(at_cmd_execute(ttyACM, "AT+CGDCONT?\r\n", buffer, 256, 5000) < 0)
                {
                        strcpy(ip, "0.0.0.0");

                        am335x_printlog("AT CMD: AT+CGDCONT? execute failure\r\n");

                        return AM335X_4G_MODULE_RET_CMD_ERROR;
                }

                memset(result, 0, sizeof(result));

                if(at_cmd_parse_value("+CGDCONT: 1,", buffer, 3, result) < 0)
                {
                        strcpy(ip, "0.0.0.0");

                        return AM335X_4G_MODULE_RET_CON_ERROR;
                }
        }

        if(strcmp(result, ip) != 0)
        {
                strcpy(ip, result);

                printf("inside am335x_4g_module_net_check AM335X_4G_MODULE_RET_IP_CHANGE\n");
                return AM335X_4G_MODULE_RET_IP_CHANGE;
        }

        return AM335X_4G_MODULE_RET_OK;
}

/************************************************************
Name: am335x_4g_module_status_read
Func: read 4g module status
Para:
char *ip - store ip address
 *************************************************************/
int am335x_4g_module_status_read(char *ip)
{
        struct am335x_4gnet_info_t *am335x_4gnet_info;
        int status;
        int shmid;

        shmid = shmget(AM335X_4G_NET_INFO_ID, sizeof(struct am335x_4gnet_info_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                return -1;
        }

        am335x_4gnet_info = (struct am335x_4gnet_info_t*)shmat(shmid, 0, 0);

        if(am335x_4gnet_info == (struct am335x_4gnet_info_t*)(-1))
        {
                return -2;
        }

        status = am335x_4gnet_info->status;

        if(ip != NULL)
        {
                strcpy(ip, (const char*)am335x_4gnet_info->ip);
        }

        shmdt((char*)am335x_4gnet_info);

        return status;
}

/************************************************************
Name: am335x_4g_module_status_show
Func: show 4g module status
Para:
none
 *************************************************************/
int am335x_4g_module_status_show()
{
        struct am335x_4gnet_info_t *am335x_4gnet_info;
        char ip[16];
        int status;
        int shmid;

        shmid = shmget(AM335X_4G_NET_INFO_ID, sizeof(struct am335x_4gnet_info_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                return -1;
        }

        am335x_4gnet_info = (struct am335x_4gnet_info_t*)shmat(shmid, 0, 0);

        if(am335x_4gnet_info == (struct am335x_4gnet_info_t*)(-1))
        {
                return -2;
        }

        printf("4G module status: %d, counter: %d, ip: %s\r\n\r\n", am335x_4gnet_info->status, am335x_4gnet_info->counter, am335x_4gnet_info->ip);

        shmdt((char*)am335x_4gnet_info);

        return 0;
}

/************************************************************
Name: am335x_4g_module_process
Func: 4g module process
Para:
none
 *************************************************************/
int am335x_4g_module_process()  
{
        struct am335x_4gnet_info_t *am335x_4gnet_info;
        int  ttyACM;
        int  status;
        int  result;
        int  count;
        int  shmid;
        char ip[16];
        char buffer[256];
        char *str = "+CSQ: ";
        char *sig_ptr = NULL;
		static int reboot = 0;

        am335x_openlog("am335x driver");

        shmid = shmget(AM335X_4G_NET_INFO_ID, sizeof(struct am335x_4gnet_info_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("4G module share memory create failed\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -33;
				pthread_exit(0);
        }

        am335x_4gnet_info = (struct am335x_4gnet_info_t*)shmat(shmid, 0, 0);

        if(am335x_4gnet_info == (struct am335x_4gnet_info_t*)(-1))
        {
                am335x_printlog("4G module share memory connect failed\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -34;
				pthread_exit(0);
        }

        am335x_4gnet_info->status  = status = AM335X_4G_MODULE_INIT;
        am335x_4gnet_info->counter = 0;
        am335x_4gnet_info->g_signal = 0;

        am335x_4gnet_info->ip[0] = ip[0] = '0';
        am335x_4gnet_info->ip[1] = ip[1] = '.';
        am335x_4gnet_info->ip[2] = ip[2] = '0';
        am335x_4gnet_info->ip[3] = ip[3] = '.';
        am335x_4gnet_info->ip[4] = ip[4] = '0';
        am335x_4gnet_info->ip[5] = ip[5] = '.';
        am335x_4gnet_info->ip[6] = ip[6] = '0';
        am335x_4gnet_info->ip[7] = ip[7] =  0;

#if(AM335X_BOARD_VERSION != 0x10)
        am335x_printlog("4G Module power on\r\n");
        if(am335x_4g_module_power(1) < 0 )
        {
                am335x_printlog("4G Module power on failed\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -35;
                pthread_exit(0);
        }

        sleep(10);
#endif

        count = 0;

        while(1)
        {
#if 1

                if((count > AM335X_4G_MODULE_TRY_COUNT) || (status < AM335X_4G_MODULE_INIT) || (status > AM335X_4G_MODULE_NET_CHECK))
                {
                        if(ttyACM >= 0)
                        {
                                close(ttyACM);
                        }

                        status = AM335X_4G_MODULE_INIT;

                        am335x_printlog("4G Module POWER OFF!!!\r\n");
                        sleep(30);
#if(AM335X_BOARD_VERSION != 0x10)
                        if(am335x_4g_module_power(0) < 0)
                        {
                                am335x_printlog("4G Module power off failed\r\n");
                        }
#endif

                        sleep(30);

                        am335x_printlog("4G Module POWER ON!!!\r\n");
                        sleep(10);
#if(AM335X_BOARD_VERSION != 0x10)
                        if(am335x_4g_module_power(1) < 0)
                        {
                                am335x_printlog("4G Module power on failed\r\n");
                        }
#endif
                        count = 0;
						reboot++;
						if(3 == reboot)
						{
							reboot = 0;
							am335x_printlog("run at command more than three times,reboot\r\n");
							system("reboot");
						}

                        sleep(10);
                }
#endif

                if(status == AM335X_4G_MODULE_INIT)
                {
                        if(ttyACM >= 0)
                        {
                                close(ttyACM);
                        }

                        ttyACM = am335x_4g_module_acm_open();

                        if(ttyACM < 0)
                        {
                                am335x_printlog("4G module AT channel open failed\r\n");

                                status = AM335X_4G_MODULE_INIT;
                        }
                        else
                        {
                                fcntl(ttyACM, F_SETFL, 0);

                                am335x_uart_set_opt(ttyACM, 115200, 8, 'N', 1, 255, 15);

                                status = AM335X_4G_MODULE_CHECK;
                        }
                }

                if(status == AM335X_4G_MODULE_CHECK)
                {
                        if(am335x_4g_module_check(g_4g_type, ttyACM, 1) < 0)
                        {
                                am335x_printlog("4G module check failure\r\n");

                                status = AM335X_4G_MODULE_INIT;
                        }
                        else
                        {
                                status = AM335X_4G_MODULE_NET_CONNECT;
                        }

                        if(at_cmd_execute(ttyACM, "AT+CSQ\r\n", buffer, 256, 5000) < 0)
                        {
                                am335x_printlog("AT CMD: AT+CSQ CMD execute failure\r\n");
                        }
#if 1    //信号强度过低告警判断
                        sig_ptr = strstr(buffer, str);

                        if(sig_ptr != NULL) 
                        {
                                if(*(sig_ptr+6) > '3')
                                {
                                        //warning
                                        am335x_4gnet_info->g_signal = -1;
                                }else if(*(sig_ptr+6) == '3')
                                {
                                        if((*(sig_ptr+7) == '0') || (*(sig_ptr+7) == '1'))
                                        {
                                                //normal
                                                am335x_4gnet_info->g_signal = 0;
                                        }else
                                        {
                                                //warning
                                                am335x_4gnet_info->g_signal = -1;
                                        }
                                }else
                                {
                                        if((*(sig_ptr+7) >= '0') && (*(sig_ptr+7) <= '9'))
                                        {
                                                //normal
                                                am335x_4gnet_info->g_signal = 0;
                                        }else
                                        {
                                                //warning
                                                am335x_4gnet_info->g_signal = -1;
                                        }
                                }
                        }else
                        {
                                printf("sig_ptr is NULL at 4009 line\n");
                        }
#endif

                        sleep(1);
                }
#if 0
                sig_buffer = (char *)malloc(sizeof(int));
                memcpy(sig_buffer,sig_ptr+6,2);
                sig_value = atoi(sig_buffer);
                printf("the signal value is %d\n",sig_value);
                if(sig_value > 10)
                {
                        printf("*********************************");
                        printf("the signal is weak\n");
                        printf("*********************************");
                }
#endif
                if(status == AM335X_4G_MODULE_NET_CONNECT)
                {
                        if(am335x_4g_module_net_connect(g_4g_type, ttyACM) < 0)
                        {
                                am335x_printlog("4G module net connect failure\r\n");

                                status = AM335X_4G_MODULE_CHECK;
                        }
                        else
                        {
                                status = AM335X_4G_MODULE_NET_ENABLE;
                                am335x_printlog("4G module net connect success\r\n");
                        }
                }

                if(status == AM335X_4G_MODULE_NET_ENABLE)
                {
                        if(am335x_4g_module_net_enable(g_4g_type, ttyACM) < 0)
                        {
                                am335x_printlog("4G module net enable failure\r\n");

                                status = AM335X_4G_MODULE_NET_CONNECT;
                        }
                        else
                        {
                                status = AM335X_4G_MODULE_NET_CONFIG;
                        }
                }

                if(status == AM335X_4G_MODULE_NET_CONFIG)
                {
                        if(am335x_4g_module_net_config(g_4g_type, ttyACM, ip) < 0)
                        {
                                am335x_printlog("4G module net config failure\r\n");

                                status = AM335X_4G_MODULE_NET_CONNECT;
                        }
                        else
                        {
                                status = AM335X_4G_MODULE_NET_CHECK;

                        }
                }

                if(g_4g_type != AM335X_4G_MODULE_HUAWEI_ME909S)
                {
                        if((status == AM335X_4G_MODULE_NET_CHECK))
                        {
                                count = 0;

                                result = am335x_4g_module_net_check(g_4g_type, ttyACM, ip);

                                if(result == AM335X_4G_MODULE_RET_CMD_ERROR)
                                {
                                        status = AM335X_4G_MODULE_INIT;
                                }
                                else if(result == AM335X_4G_MODULE_RET_REG_ERROR)
                                {
                                        status = AM335X_4G_MODULE_CHECK;
                                }           
                                else if(result == AM335X_4G_MODULE_RET_CON_ERROR)
                                {
                                        status = AM335X_4G_MODULE_NET_CONNECT;
                                }
                                else if(result == AM335X_4G_MODULE_RET_IP_CHANGE)
                                {
                                        printf("else if(result == AM335X_4G_MODULE_RET_IP_CHANGE)\n");
                                        status = AM335X_4G_MODULE_NET_ENABLE;    
                                }
                                else
                                {
                                        status = AM335X_4G_MODULE_NET_CHECK;
                                }
                        }
                        else
                        {
                                count++;
                        }
                }

                strcpy((char*)am335x_4gnet_info->ip, ip);

                am335x_4gnet_info->status = status;

                if(am335x_4gnet_info->status == 5)
                {
                        net_status = 5;
                }
                am335x_4gnet_info->counter++;

                am335x_printlog("4G module status: %d, ip: %s\r\n", status, ip);

                sleep(10);
        }

        shmdt((char*)am335x_4gnet_info);

        am335x_closelog("am335x driver");
	    g_pthread_exit = -36;
		pthread_exit(0);
        return 0;  
} 

/************************************************************
Name: am335x_4gnet_init
Func: am335x 4g network init
Para:
none
 *************************************************************/
void* am335x_4gnet_init(void*arg)
{
        struct am335x_4gnet_info_t *am335x_4gnet_info;
        pid_t pid;
        int shmid;
        int i;

        am335x_openlog("am335x driver");

        shmid = shmget(AM335X_4G_NET_INFO_ID, sizeof(struct am335x_4gnet_info_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("4G Net share memory create failed\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -31;
                pthread_exit(0);
        }

        prctl(PR_SET_NAME, "net_pthread");
        am335x_4gnet_info = (struct am335x_4gnet_info_t*)shmat(shmid, 0, 0);

        if(am335x_4gnet_info == (struct am335x_4gnet_info_t*)(-1))
        {
                am335x_printlog("4G Net share memory connect failed\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -32;
                pthread_exit(0);
        }

        am335x_closelog("am335x driver");

        am335x_4gnet_info->status  = AM335X_4G_MODULE_INIT;
        am335x_4gnet_info->counter = 0;

        am335x_4gnet_info->ip[0] = '0';
        am335x_4gnet_info->ip[1] = '.';
        am335x_4gnet_info->ip[2] = '0';
        am335x_4gnet_info->ip[3] = '.';
        am335x_4gnet_info->ip[4] = '0';
        am335x_4gnet_info->ip[5] = '.';
        am335x_4gnet_info->ip[6] = '0';
        am335x_4gnet_info->ip[7] =  0;

        shmdt((char*)am335x_4gnet_info);
        am335x_4g_module_process();
		g_pthread_exit = -31;
        pthread_exit(0);
}

int am335x_4gnet_init_test()
{
        struct am335x_4gnet_info_t *am335x_4gnet_info;
        pid_t pid;
        int shmid;
        int i;

        am335x_openlog("am335x driver");

        shmid = shmget(AM335X_4G_NET_INFO_ID, sizeof(struct am335x_4gnet_info_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("4G Net share memory create failed\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -31;
                pthread_exit(0);
        }

        prctl(PR_SET_NAME, "net_pthread");
        am335x_4gnet_info = (struct am335x_4gnet_info_t*)shmat(shmid, 0, 0);

        if(am335x_4gnet_info == (struct am335x_4gnet_info_t*)(-1))
        {
                am335x_printlog("4G Net share memory connect failed\r\n");
                am335x_closelog("am335x driver");
				g_pthread_exit = -32;
                pthread_exit(0);
        }

        am335x_closelog("am335x driver");

        am335x_4gnet_info->status  = AM335X_4G_MODULE_INIT;
        am335x_4gnet_info->counter = 0;

        am335x_4gnet_info->ip[0] = '0';
        am335x_4gnet_info->ip[1] = '.';
        am335x_4gnet_info->ip[2] = '0';
        am335x_4gnet_info->ip[3] = '.';
        am335x_4gnet_info->ip[4] = '0';
        am335x_4gnet_info->ip[5] = '.';
        am335x_4gnet_info->ip[6] = '0';
        am335x_4gnet_info->ip[7] =  0;

        shmdt((char*)am335x_4gnet_info);

        am335x_4g_module_process();	

		g_pthread_exit = -31;
		return 0;
}

/************************************************************
Name: am335x_gpio_init
Func: gpio init
Para:
none
 *************************************************************/
int am335x_gpio_init()
{
        char *gpio_regs[3];
        char *ctrl_regs;
        int mem_fd;
        int gpio;
        int bit;
        int i;

        am335x_openlog("am335x driver");

        mem_fd = open("/dev/mem", O_RDWR|O_SYNC);

        if(mem_fd < 0)
        {
                am335x_printlog("am335x_gpio_init: /dev/mem open failed\r\n");
                am335x_closelog("am335x driver");

                return -1;
        }

        ctrl_regs = (char*)mmap(0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x44e00000);

        if(ctrl_regs == (char*)MAP_FAILED)
        {
                close(mem_fd);

                am335x_printlog("am335x_gpio_init: mmap ctrl regs failed\r\n");
                am335x_closelog("am335x driver");

                return -2;

        }

        bit = *(volatile unsigned int*)(ctrl_regs + 0x408);
        if((bit & 0x00000003) != 0x00000002)
        {
                *(volatile unsigned int*)(ctrl_regs + 0x408) |= 0x00000002;
        }

        bit = *(volatile unsigned int*)(ctrl_regs + 0x0ac);
        if((bit & 0x00000003) != 0x00000002)
        {
                *(volatile unsigned int*)(ctrl_regs + 0x0ac) |= 0x00000002;
        }

        bit = *(volatile unsigned int*)(ctrl_regs + 0x0b0);
        if((bit & 0x00000003) != 0x00000002)
        {
                *(volatile unsigned int*)(ctrl_regs + 0x0b0) |= 0x00000002;
        }

        munmap(ctrl_regs, 0x20000);

        gpio_regs[0] = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x44e07000);

        if(gpio_regs[0] == (char*)MAP_FAILED)
        {
                close(mem_fd);

                am335x_printlog("am335x_gpio_init: mmap gpio0 regs failed\r\n");
                am335x_closelog("am335x driver");

                return -3;
        }

        gpio_regs[1] = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x4804c000);

        if(gpio_regs[1] == (char*)MAP_FAILED)
        {
                munmap(gpio_regs[0], 0x1000);

                close(mem_fd);

                am335x_printlog("am335x_gpio_init: mmap gpio1 regs failed\r\n");
                am335x_closelog("am335x driver");

                return -4;
        }

        gpio_regs[2] = (char*)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x481ac000);

        if(gpio_regs[2] == (char*)MAP_FAILED)
        {
                munmap(gpio_regs[0], 0x1000);

                munmap(gpio_regs[1], 0x1000);

                close(mem_fd);

                am335x_printlog("am335x_gpio_init: mmap gpio2 regs failed\r\n");
                am335x_closelog("am335x driver");

                return -5;
        }

        bit = *(volatile unsigned int*)(gpio_regs[0] + 0x130);
        if((bit & 0x00000001) != 0)
        {
                *(volatile unsigned int*)(gpio_regs[0] + 0x130) = 0x00000002;
        }

        bit = *(volatile unsigned int*)(gpio_regs[1] + 0x130);
        if((bit & 0x00000001) != 0)
        {
                *(volatile unsigned int*)(gpio_regs[1] + 0x130) = 0x00000002;
        }

        bit = *(volatile unsigned int*)(gpio_regs[2] + 0x130);
        if((bit & 0x00000001) != 0)
        {
                *(volatile unsigned int*)(gpio_regs[2] + 0x130) = 0x00000002;
        }

        for(i = 0; i < AM335X_GPIO_MAX_PIN; i++)
        {
                gpio = gpio_pin[i] / 32;
                bit  = gpio_pin[i] % 32;

                if(gpio_val[i] != 0)
                {
                        *(volatile unsigned int*)(gpio_regs[gpio] + 0x194) = (1 << bit);
                }
                else
                {
                        *(volatile unsigned int*)(gpio_regs[gpio] + 0x190) = (1 << bit);
                }

                if(gpio_dir[i] != 0)
                {
                        *(volatile unsigned int*)(gpio_regs[gpio] + 0x134) |= (1 << bit);
                }
                else
                {
                        *(volatile unsigned int*)(gpio_regs[gpio] + 0x134) &= (~(1 << bit));
                }
        }

        munmap(gpio_regs[0], 0x1000);
        munmap(gpio_regs[1], 0x1000);
        munmap(gpio_regs[2], 0x1000);

        close(mem_fd);

        am335x_closelog("am335x driver");

        return 0;
}

/********************************************************
Name: am335x_open_dog   
Func: am335x open dog
Para: none

Added by WangZedong 2016.9.20
 ********************************************************/
int am335x_open_dog(int fd)
{
        fd=open("/dev/watchdog1",O_WRONLY);

        if (fd < 0) 
        {
                printf("watchdog1 open failed\r\n");
                return -1;
        }else
        {
                return fd;
        }
}

/**********************************************************
Name: am335x_feed_dog
Func: am335x feed dog
Para: watch_dog_fd; sleep_time --- is second time

Added by WangZedong 2016.9.20
 **********************************************************/
int am335x_feed_dog(int watch_dog_fd, int sleep_time)
{
		sleep(sleep_time);
        if (write(watch_dog_fd, "\0", 1) < 0)
        {
                printf("watchdog1 write failed\n");
                return -1;
        }
        return 0;
}

/***********************************************************
Name: am335x_close_dog
Func: am335x close dog
Para: watch_dog_fd

Added by WangZedong 2016.9.20
 ************************************************************/
void am335x_close_dog(int watch_dog_fd)
{
        close(watch_dog_fd);
}

/************************************************************
  ALARM Process
 *************************************************************/
void *am335x_alarm_process(void *alarm_t)
{

        struct am335x_alarm_info_t *am335x_alarm_info;
        pid_t pid;
        int shmid;
        char *temp_dev = NULL;
        int id;
	  char *data_buf;
        int i;
        int value = 0;
        //char *test = NULL;
        int key_state;
        int fd;
        int ret;
        int code;
        struct input_event buf;
        int repeat_param[2];
		struct alarm_para_t *alarm = (struct alarm_para_t *)alarm_t;
		
		id = alarm->alarm_id;
		prctl(PR_SET_NAME, "alarm_thread");
        am335x_openlog("am335x driver");

        shmid = shmget(alarm->mem_id, sizeof(struct am335x_alarm_info_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("%d memory create failed\r\n",id);
                am335x_closelog("am335x driver");
				printf("%d memory create failed\r\n",id);
				g_pthread_exit = -41;
				pthread_exit(0);
                //return -1;
        }

        am335x_alarm_info = (struct am335x_alarm_info_t *)shmat(shmid, 0, 0);

        if(am335x_alarm_info == (struct am335x_alarm_info_t*)(-1))
        {
                am335x_printlog("%d memory connect failed\r\n",id);
                am335x_closelog("am335x driver");
				printf("%d memory connect failed\r\n",id);
				g_pthread_exit = -42;
				pthread_exit(0);

                //return -2;
        }

        am335x_closelog("am335x driver");       
        am335x_alarm_info->power_lost_code  =  0;
        am335x_alarm_info->arrester_code    =  0;
        am335x_alarm_info->door_systerm_code = 0;
        am335x_alarm_info->board_code       =  0;
        am335x_alarm_info->humidity_code    =  0;
        am335x_alarm_info->temperature_code =  0;
        am335x_alarm_info->cell_code        =  0;
        am335x_alarm_info->link_code        =  0;
        am335x_alarm_info->power_usb_code   =  0;
        am335x_alarm_info->power_5v_code    =  0;
        am335x_alarm_info->signal_code      =  0;
		//am335x_alarm_info->fan1_code        =  0;
		//am335x_alarm_info->fan2_code        =  0;

        if((alarm->alarm_id) != ALARM_FAN_ID)
        {
        	//printf("alarm->alarm_id = %d\n",alarm->alarm_id);
        	temp_dev = (char *)alarm->dev;
        	fd = open(temp_dev,O_RDONLY | O_NONBLOCK);
        
	        if(fd < 0)
	        {
					g_pthread_exit = -43;
	                am335x_printlog("Open failed\n");
					pthread_exit(0);
	        }else
	        {
	                am335x_printlog("Open success\n");
	        }
        }
        while(1)
        {
                sleep(1);

                if((id == 0x0001) || (id == 0x0002) || (id == 0x0003) || (id == 0x0009) || (id == 0x000a))
                {
                        repeat_param[0] = 500;
                        repeat_param[1] = 66;
                        ret = ioctl(fd,EVIOCSREP,(int*)repeat_param);
                        if(ret != 0)
                        {
                                am335x_printlog("set repeat_param failed.\n");
                        }else
                        {
                                am335x_printlog("set repeat_param success.\n");
                        }
                        ret = read(fd, &buf, sizeof(struct input_event));
                        if(ret <= 0)
                        {
                                am335x_printlog("read failed.\n");
                        }

                        code = buf.code;
                        am335x_printlog("code = %d\n",code);
                        key_state = buf.value;
                        am335x_printlog("key_state = %d\n",key_state);

                        switch(code)
                        {
                                case KEY_U:
                                        code = 22;
                                        if(key_state == 1)
                                        {
                                                am335x_alarm_info->power_usb_code  =  1;
                                        }else{
                                                am335x_alarm_info->power_usb_code  =  0;
                                        }
                                        break;
                                case KEY_V:
                                        code = 47;
                                        if(key_state == 1)
                                        {
                                                am335x_alarm_info->power_5v_code  =  1;
                                        }else{
                                                am335x_alarm_info->power_5v_code  =  0;
                                        }
                                        break;
                                case KEY_S:
                                        code = 31;
                                        if(key_state == 0)
                                        {
                                                am335x_alarm_info->arrester_code  =  1;
                                        }else{
                                                am335x_alarm_info->arrester_code  =  0;
                                        }
                                        break;
                                case KEY_D:
                                        code = 32;
                                        if(key_state == 0)
                                        {
                                                am335x_alarm_info->door_systerm_code  =  1;
                                        }else{
                                                am335x_alarm_info->door_systerm_code  =  0;
                                        }
                                        break;
                                case KEY_B:
                                        code = 48;
                                        if(key_state == 0)
                                        {
                                                am335x_alarm_info->power_lost_code  =  1;
                                        }else{
                                                am335x_alarm_info->power_lost_code  =  0;
                                        }
                                        break;
                                default:
                                        code = 0;
                                        break;
                        }

                        if(code != 0)
                        {
                                am335x_printlog("Key_%d state= %d.\n",code,key_state);
                        }

                        lseek(fd,0,SEEK_SET);
                }else if((id == 0x0005) || (id == 0x0006) || (id == 0x0007))
                {
                        data_buf = (char *)malloc(8);
                        ret = read(fd, data_buf,8);
                        value = atoi(data_buf);
                        if(ret <= 0)
                        {
                                printf("read failed.\n");
                        }

			     free(data_buf);
                        data_buf = NULL;
                        if(id == ALARM_HUMIDITY_ID)
                        {

				      am335x_printlog("humidity = %d\n", (value)/100);
                                if(((value)/100) > HUMIDITY_THRESHOLD_VALUE) 
                                {
                                        am335x_alarm_info->humidity_code = HUMIDITY_WARNING;
                                }else{
                                        am335x_alarm_info->humidity_code = 0;
                                }

                        }else if(id == ALARM_TEMP_ID)
                        {
                                if(((value)/1000) > TEMP_THRESHOLD_VALUE)
                                {
                                        am335x_alarm_info->temperature_code = TEMP_WARNING;
                                }else{
                                        am335x_alarm_info->temperature_code = 0;
                                }
                        }else if(id == ALARM_CELL_ID)          
                        {
                                if(((value/2048)*(1.8)*11) > VOLTAGE_VALUE)
                                {
                                        am335x_alarm_info->cell_code = CELL_WARNING;
                                }else{
                                        am335x_alarm_info->cell_code = 0;
                                }

                        }
						else
                        {
                        }
                        lseek(fd,0,SEEK_SET);
                }else
                {}
        }
        close(fd);
		g_pthread_exit = -43;
		pthread_exit(0);
}

/************************************************************
ALARM:  PowerLost-Fault
Arrester-Warning
DoorSysterm-Warning

 ************************************************************/
int am335x_check_alarm_status(int share_mem_id,int alarm_id, const char *dev)
{
        struct am335x_alarm_info_t *am335x_alarm_info;
        pid_t pid;
        int shmid;
        const char *temp_dev = dev;
        int id = alarm_id;
        int mem_id = share_mem_id;
        int i;

        am335x_openlog("am335x driver");

        shmid = shmget(share_mem_id, sizeof(struct am335x_alarm_info_t), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("%d memory create failed\r\n",id);
                am335x_closelog("am335x driver");

                return -1;
        }

        am335x_alarm_info = (struct am335x_alarm_info_t*)shmat(shmid, 0, 0);

        if(am335x_alarm_info == (struct am335x_alarm_info_t*)(-1))
        {
                am335x_printlog("%d memory connect failed\r\n",id);
                am335x_closelog("am335x driver");

                return -2;
        }

        am335x_closelog("am335x driver");
        am335x_alarm_info->power_lost_code  =  0;
        am335x_alarm_info->arrester_code    =  0;
        am335x_alarm_info->door_systerm_code = 0;
        am335x_alarm_info->board_code       =  0;
        am335x_alarm_info->humidity_code    =  0;
        am335x_alarm_info->temperature_code =  0;
        am335x_alarm_info->cell_code        =  0;
        am335x_alarm_info->link_code        =  0;
        am335x_alarm_info->power_usb_code   =  0;
        am335x_alarm_info->power_5v_code    =  0;
        am335x_alarm_info->signal_code      =  0;

        shmdt((char*)am335x_alarm_info);

        //am335x_alarm_process(mem_id,id, temp_dev);
        return 0;
}

int am335x_ddr_test()
{
        char *buffer;
        int   i;

        buffer = malloc(0x2000000);

        memset(buffer, 0x55, 0x2000000);

        for(i = 0; i < 0x2000000; i++)
        {
                if(buffer[i] != 0x55)
                {
                        free(buffer);
                        buffer = NULL;
                        return -1;
                }
        }

        memset(buffer, 0xAA, 0x2000000);

        for(i = 0; i < 0x2000000; i++)
        {
                if(buffer[i] != 0xAA)
                {
                        free(buffer);
                        buffer = NULL;
                        return -2;
                }
        }

        free(buffer);
        buffer = NULL;
        return 0;
}

int am335x_led_test()
{
        int i;

        for(i = 0; i < 5; i++)
        {
                am335x_gpio_write(AM335X_GPIO_LED1, 0);
                am335x_gpio_write(AM335X_GPIO_LED2, 0);

                usleep(50000);

                am335x_gpio_write(AM335X_GPIO_LED1, 1);
                am335x_gpio_write(AM335X_GPIO_LED2, 1);

                usleep(50000);
        }

        am335x_gpio_write(AM335X_GPIO_LED1, 0);
        am335x_gpio_write(AM335X_GPIO_LED2, 0);

        return 0;
}

int am335x_lcd_test()
{
        int i;

        if(am335x_lcd_clear() != 0)
        {
                return -1;
        }

        for(i = 0; i < AM335X_LCD_HEIGHT / AM335X_ASCII_HEIGHT; i++)
        {
                am335x_lcd_drawASC(0, i, 'L');
                am335x_lcd_drawASC(1, i, 'C');
                am335x_lcd_drawASC(2, i, 'D');
                am335x_lcd_drawASC(3, i, ' ');
                am335x_lcd_drawASC(4, i, 'T');
                am335x_lcd_drawASC(5, i, 'E');
                am335x_lcd_drawASC(6, i, 'S');
                am335x_lcd_drawASC(7, i, 'T');
        }

        if(am335x_lcd_display() != 0)
        {
                return -2;
        }

        return 0;
}

int am335x_eeprom_test()
{
        char buffer[256];
        int  i;

        memset(buffer, 0x55, 256);

        am335x_eeprom_write(3, 0, buffer, 256);

        memset(buffer, 0x00, 256);

        am335x_eeprom_read(3, 0, buffer, 256);

        for(i = 0; i < 256; i++)
        {
                if(buffer[i] != 0x55)
                {
                        printf("EEPROM Test: i = %d, buffer[i] = %x\n", i, buffer[i]);

                        return -1; 
                }
        }

        memset(buffer, 0xff, 256);

        am335x_eeprom_write(3, 0, buffer, 256);

        return 0;
}

int am335x_uart_test(char* uart)
{
        unsigned char buffer[128];
        struct timeval tv;
        fd_set rfds;
        int fd;
        int i;

        fd = open(uart, O_RDWR | O_NOCTTY | O_NDELAY);

        if(fd < 0)
        {
                return -1;
        }

        fcntl(fd, F_SETFL, 0);

        am335x_uart_set_opt(fd, 9600, 8, 'N', 1, 255, 10);

        memset(buffer, 0x55, sizeof(buffer));

        if(write(fd, buffer, sizeof(buffer)) < 0)
        {
                close(fd);

                return -2;
        }

        memset(buffer, 0x00, sizeof(buffer));

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        tv.tv_sec  = 2;
        tv.tv_usec = 0;

        if(select(fd+1, &rfds, NULL, NULL, &tv) <= 0)
        {
                close(fd);

                return -3;
        }

        if(read(fd, buffer, sizeof(buffer)) < 0)
        {
                close(fd);

                return -4;
        }

        for(i = 0; i < sizeof(buffer); i++)
        {
                if(buffer[i] != 0x55)
                {
                        printf("UART Test: i = %d, buffer[i] = %x\n", i, buffer[i]);

                        close(fd);

                        return -5;
                }
        }

        close(fd);

        return 0;
}

int am335x_extern_uart_test(char* uart)
{
        unsigned char buffer[128];
        struct timeval tv;
        fd_set rfds;
        int arg[3];
        int fd;
        int i;

        fd = open(uart, O_RDWR | O_NOCTTY | O_NDELAY);

        if(fd < 0)
        {
                return -1;
        }

        fcntl(fd, F_SETFL, 0);

        am335x_uart_set_opt(fd, 9600, 8, 'N', 1, 255, 10);

        if(strcmp(uart, "/dev/ttyXRUSB2") == 0)
        {        
                arg[0] = 2; arg[1] = 0x0C; arg[2] = 0x0B;

                ioctl(fd, XR_USB_SERIAL_SET_REG, (unsigned long)arg);

                arg[0] = 2; arg[1] = 0x1A; arg[2] = 0x0B;

                ioctl(fd, XR_USB_SERIAL_SET_REG, (unsigned long)arg);
        }

        memset(buffer, 0x55, sizeof(buffer));

        if(write(fd, buffer, sizeof(buffer)) < 0)
        {
                close(fd);

                return -2;
        }
#if 1
        memset(buffer, 0x00, sizeof(buffer));

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        tv.tv_sec  = 2;
        tv.tv_usec = 0;

        if(select(fd+1, &rfds, NULL, NULL, &tv) <= 0)
        {
                close(fd);

                return -3;
        }

        if(read(fd, buffer, sizeof(buffer)) < 0)
        {
                close(fd);

                return -4;
        }

        for(i = 0; i < sizeof(buffer); i++)
        {
                if(buffer[i] != 0x55)
                {
                        printf("EXTERN UART Test: i = %d, buffer[i] = %x\n", i, buffer[i]);

                        close(fd);

                        return -5;
                }
        }
#endif
        close(fd);

        return 0;
}

int am335x_extern_uart_process(char* uart)
{
        unsigned char buffer[256];
        int arg[3];
        int len;
        int fd;

        fd = open(uart, O_RDWR | O_NOCTTY | O_NDELAY);

        if(fd < 0)
        {
                return -1;
        }

        fcntl(fd, F_SETFL, 0);

        am335x_uart_set_opt(fd, 19200, 8, 'N', 1, 255, 10);

        while(1)
        {
                if(strcmp(uart, "/dev/ttyXRUSB2") == 0)
                {
                        arg[0] = 2; arg[1] = 0x0C; arg[2] = 0x0B;

                        ioctl(fd, XR_USB_SERIAL_SET_REG, (unsigned long)arg);

                        arg[0] = 2; arg[1] = 0x1A; arg[2] = 0x0B;

                        ioctl(fd, XR_USB_SERIAL_SET_REG, (unsigned long)arg);
                }

                memset(buffer, 0x00, sizeof(buffer));

                len = read(fd, buffer, sizeof(buffer));

                if(len <= 0)
                {
                        continue;
                }

                printf("%s read %d Bytes\r\n", uart, len);
                printf("buffer = %s\n",buffer);         
        }

        close(fd);

        return 0;
}

int am335x_gas_sensor_test(int type, int sensor)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        unsigned char buffer[32];
        struct timeval tv;
        fd_set rfds;
        int trycnt;
        int ret;
        int fd, i;

        if(AM335X_SENSOR_TYPE_WINSEN_ZE03 == type)
        {
                am335x_sensor_dev = &am335x_sensor_ze03_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_WINSEN_ZE10 == type)
        {
                am335x_sensor_dev = &am335x_sensor_ze10_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
        {
                am335x_sensor_dev = &am335x_sensor_sga700_dev[sensor];
        }
        else
        {
                return -1;
        }

        trycnt = 0;

        while(trycnt < AM335X_TEST_TRY_COUNT)
        {
                fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

                if(fd < 0)
                {
                        continue;
                }

                fcntl(fd, F_SETFL, 0);

                am335x_uart_set_opt(fd, 9600, 8, 'N', 1, 
                                am335x_sensor_dev->uart_length, 
                                am335x_sensor_dev->uart_timeout);

                if(AM335X_SENSOR_TYPE_WINSEN_ZE03 == type)
                {
                        buffer[0] = 0xFF;
                        buffer[1] = 0x01;
                        buffer[2] = 0x78;
                        buffer[3] = 0x03;
                        buffer[4] = 0x00;
                        buffer[5] = 0x00;
                        buffer[6] = 0x00;
                        buffer[7] = 0x00;
                        buffer[8] = 0x84;

                        if(write(fd, buffer, 9) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }
                else if(AM335X_SENSOR_TYPE_WINSEN_ZE10 == type)
                {
                        buffer[0] = 0xFF;
                        buffer[1] = 0x01;
                        buffer[2] = 0x78;
                        buffer[3] = 0x40;
                        buffer[4] = 0x00;
                        buffer[5] = 0x00;
                        buffer[6] = 0x00;
                        buffer[7] = 0x00;
                        buffer[8] = 0x47;

                        if(write(fd, buffer, 9) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }
                else if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
                {
                        buffer[0]  = 0xFF;
                        buffer[1]  = 0xFF;
                        buffer[2]  = 0x01;
                        buffer[3]  = 0xFE;//因为生产测试SO2 NO2 CO传感器不过，从0x01改为0xfe
                        buffer[4]  = 0x05;
                        buffer[5]  = 0x01;
                        buffer[6]  = 0x00;
                        buffer[7]  = 0x6C;
                        buffer[8]  = 0x07;
                        buffer[9]  = 0x74;
                        buffer[10] = 0xDD;

                        if(write(fd, buffer, 11) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }

                memset(buffer, 0, sizeof(buffer));

                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret <= 0)
                {
                        trycnt++;

                        close(fd);

                        continue;
                }

                if(read(fd, buffer, sizeof(buffer)) < 0)
                {
                        trycnt++;

                        close(fd);

                        continue;
                }

                close(fd);

                break;
        }
#if 0
        for(i = 0; i < 32; i++)
        {
                printf("gas name:%s buffer[%d] = %x\n",am335x_sensor_dev->gas_name,i,buffer[i]);
        }
#endif		
        if(trycnt >= AM335X_TEST_TRY_COUNT)
        {
                return -2;
        }

        if((buffer[0] != am335x_sensor_dev->uart_code[0]) \
                        && (buffer[1] != am335x_sensor_dev->uart_code[1]))
        {
                return -3;
        }

        if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
        {

			if(sensor == AM335X_SENSOR_SO2)
			{
				if(buffer[14] != 0x0A)
				{
					printf("so2 sensor position error\n");
					return -4;
				}
			}

			if(sensor == AM335X_SENSOR_NO2)
			{
				if(buffer[14] != 0x16)
				{
					printf("no2 sensor position error\n");
					return -4;
				}
			}

			if(sensor == AM335X_SENSOR_CO)
			{
				if(buffer[14] != 0x02)
				{
					printf("co sensor position error\n");
					return -4;

				}
			}

			if(sensor == AM335X_SENSOR_O3)
			{
				if(buffer[14] != 0x08)
				{
					printf("o3 sensor position error\n");
					return -4;

				}
			}

			if(sensor == AM335X_SENSOR_TVOC)
			{
				if(buffer[14] != 0x27)
				{
					printf("TVOC sensor position error\n");
					return -4;

				}
			}
			
            if(am335x_sensor_checksum8bit(&buffer[5], 11) != buffer[16])
            {
                    return -4;
            }
        }

        return 0;
}

int am335x_gas_sensor_read_id(int type, int sensor, char *sId)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        unsigned char buffer[32];
        struct timeval tv;
        fd_set rfds;
        int trycnt;
        int ret;
        int fd, i;

        if(AM335X_SENSOR_TYPE_WINSEN_ZE03 == type)
        {
                am335x_sensor_dev = &am335x_sensor_ze03_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_WINSEN_ZE10 == type)
        {
                am335x_sensor_dev = &am335x_sensor_ze10_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
        {
                am335x_sensor_dev = &am335x_sensor_sga700_dev[sensor];
        }
        else
        {
                return -1;
        }

        trycnt = 0;

        while(trycnt < AM335X_TEST_TRY_COUNT)
        {
                fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

                if(fd < 0)
                {
                        continue;
                }

                fcntl(fd, F_SETFL, 0);

                am335x_uart_set_opt(fd, 9600, 8, 'N', 1, 
                                am335x_sensor_dev->uart_length, 
                                am335x_sensor_dev->uart_timeout);

                if(AM335X_SENSOR_TYPE_WINSEN_ZE03 == type)
                {
                        buffer[0] = 0xFF;
                        buffer[1] = 0x01;
                        buffer[2] = 0x78;
                        buffer[3] = 0x03;
                        buffer[4] = 0x00;
                        buffer[5] = 0x00;
                        buffer[6] = 0x00;
                        buffer[7] = 0x00;
                        buffer[8] = 0x84;

                        if(write(fd, buffer, 9) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }
                else if(AM335X_SENSOR_TYPE_WINSEN_ZE10 == type)
                {
                        buffer[0] = 0xFF;
                        buffer[1] = 0x01;
                        buffer[2] = 0x78;
                        buffer[3] = 0x40;
                        buffer[4] = 0x00;
                        buffer[5] = 0x00;
                        buffer[6] = 0x00;
                        buffer[7] = 0x00;
                        buffer[8] = 0x47;

                        if(write(fd, buffer, 9) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }
                else if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
                {
                        buffer[0]  = 0xFF;
                        buffer[1]  = 0xFF;
                        buffer[2]  = 0x01;
                        buffer[3]  = 0x01;//闁搞儳濮崇拹鐔兼偨閻斿爼鐛撴繛鏉戭儓閻︾柇O2 NO2 CO濞磋偐濮甸崝鍛村闯閵娿倗鐟濋弶鈺佹祫缁辨繃绂?x01闁衡偓闁稖绀?xfe
                        buffer[4]  = 0x05;
                        buffer[5]  = 0x01;
                        buffer[6]  = 0x00;
                        buffer[7]  = 0x9b;
                        buffer[8]  = 0x03;
                        buffer[9]  = 0x9f;
                        buffer[10] = 0xDD;

                        if(write(fd, buffer, 11) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }

                memset(buffer, 0, sizeof(buffer));

                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret <= 0)
                {
                        trycnt++;

                        close(fd);

                        continue;
                }

                if(read(fd, buffer, sizeof(buffer)) < 0)
                {
                        trycnt++;

                        close(fd);

                        continue;
                }

                close(fd);

                break;
        }
#if 0
        for(i = 0; i < 32; i++)
        {
                printf("gas name:%s buffer[%d] = %x\n",am335x_sensor_dev->gas_name,i,buffer[i]);
        }
#endif		
        if(trycnt >= AM335X_TEST_TRY_COUNT)
        {
                return -2;
        }

        if((buffer[0] != am335x_sensor_dev->uart_code[0]) \
                        && (buffer[1] != am335x_sensor_dev->uart_code[1]))
        {
                return -3;
        }

        if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
        {
                for(i = 0; i < 14; i++)
                {
                        printf("buffer[%d] = %x\n", i, buffer[i]);
                }

                memcpy(sId, &buffer[6], 6);
                for(i = 0; i < 6; i++)
                {
                        printf("buffer[%d] = %x\n", i, buffer[i]);
                }
        }
        return 0;
}
int am335x_gas_sensor_test1(int type, int sensor)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        unsigned char buffer[32];
        struct timeval tv;
        fd_set rfds;
        int trycnt;
        int ret;
        int fd, i;

        if(AM335X_SENSOR_TYPE_WINSEN_ZE03 == type)
        {
                am335x_sensor_dev = &am335x_sensor_ze03_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_WINSEN_ZE10 == type)
        {
                am335x_sensor_dev = &am335x_sensor_ze10_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
        {
                am335x_sensor_dev = &am335x_sensor_sga700_dev[sensor];
        }
        else
        {
                return -1;
        }

        trycnt = 0;

        while(trycnt < AM335X_TEST_TRY_COUNT)
        {
                fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

                if(fd < 0)
                {
                        continue;
                }

                fcntl(fd, F_SETFL, 0);

                am335x_uart_set_opt(fd, 9600, 8, 'N', 1, 
                                am335x_sensor_dev->uart_length, 
                                am335x_sensor_dev->uart_timeout);
#if 0
                if(AM335X_SENSOR_TYPE_WINSEN_ZE03 == type)
                {
                        buffer[0] = 0xFF;
                        buffer[1] = 0x01;
                        buffer[2] = 0x78;
                        buffer[3] = 0x03;
                        buffer[4] = 0x00;
                        buffer[5] = 0x00;
                        buffer[6] = 0x00;
                        buffer[7] = 0x00;
                        buffer[8] = 0x84;

                        if(write(fd, buffer, 9) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }
                else if(AM335X_SENSOR_TYPE_WINSEN_ZE10 == type)
                {
                        buffer[0] = 0xFF;
                        buffer[1] = 0x01;
                        buffer[2] = 0x78;
                        buffer[3] = 0x40;
                        buffer[4] = 0x00;
                        buffer[5] = 0x00;
                        buffer[6] = 0x00;
                        buffer[7] = 0x00;
                        buffer[8] = 0x47;

                        if(write(fd, buffer, 9) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }
                else if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
                {
                        buffer[0]  = 0xFF;
                        buffer[1]  = 0xFF;
                        buffer[2]  = 0x01;
                        buffer[3]  = 0xFE;//因为生产测试SO2 NO2 CO传感器不过，从0x01改为0xfe
                        buffer[4]  = 0x05;
                        buffer[5]  = 0x01;
                        buffer[6]  = 0x00;
                        buffer[7]  = 0x6C;
                        buffer[8]  = 0x07;
                        buffer[9]  = 0x74;
                        buffer[10] = 0xDD;

                        if(write(fd, buffer, 11) < 0)
                        {
                                trycnt++;

                                close(fd);

                                continue;
                        }
                }
#endif
                memset(buffer, 0xb5, sizeof(buffer));

                if(write(fd, buffer, sizeof(buffer)) < 0)
                {
                        trycnt++;

                        close(fd);

                        continue;
                }

                memset(buffer, 0x00, sizeof(buffer));
                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret <= 0)
                {
                        trycnt++;

                        close(fd);

                        continue;
                }

                if(read(fd, buffer, sizeof(buffer)) < 0)
                {
                        trycnt++;

                        close(fd);

                        continue;
                }

                close(fd);

                break;
        }
#if 0
        for(i = 0; i < 32; i++)
        {
                printf("gas name:%s buffer[%d] = %x\n",am335x_sensor_dev->gas_name,i,buffer[i]);
        }
#endif		
        if(trycnt >= AM335X_TEST_TRY_COUNT)
        {
                return -2;
        }

        for(i = 0; i < sizeof(buffer); i++)
        {
                if(buffer[i] != 0xb5)
                {
                        return -1;
                }
        }
#if 0       
        if((buffer[0] != am335x_sensor_dev->uart_code[0]) \
                        && (buffer[1] != am335x_sensor_dev->uart_code[1]))
        {
                return -3;
        }

        if(AM335X_SENSOR_TYPE_SINGOAN_SGA700 == type)
        {
                if(am335x_sensor_checksum8bit(&buffer[5], 11) != buffer[16])
                {
                        return -4;
                }
        }
#endif
        return 0;
}

int am335x_atmos_sensor_test(int type, int sensor)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        unsigned char buffer[32];
        struct timeval tv;
        fd_set rfds;
        int trycnt;
        int ret;
        int fd,i;
        char atmos_addr;

        if(AM335X_SENSOR_TYPE_WXA == type)
        {
                am335x_sensor_dev = &am335x_sensor_atoms_dev[sensor];
        }
        else
        {
                return -1;
        }

        trycnt = 0;

        while(trycnt < AM335X_TEST_TRY_COUNT)
        {
                fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

                if(fd < 0)
                {
                        printf("error fd < 0\n");
                        continue;
                }

                fcntl(fd, F_SETFL, 0);

                am335x_uart_set_opt(fd, 9600, 8, 'N', 1, 
                                am335x_sensor_dev->uart_length, 
                                am335x_sensor_dev->uart_timeout);

                atmos_addr = read_atoms_sensor_adr(fd);
                if(AM335X_SENSOR_TYPE_WXA == type)
                {
                        sleep(1);

                        buffer[0] = 0x82;
                        buffer[1] = atmos_addr;
                        buffer[2] = 0x02;
                        buffer[3] = 0x01;
                        buffer[4] = 0x00;
                        buffer[5] = buffer[1]^buffer[2]^buffer[3]^buffer[4];
#if 0
                        for(i = 0; i < 6; i++)
                        {
                                printf("buffer[%d] = %x\n",i,buffer[i]);
                        }
#endif
                        if (write(fd, buffer, 6) < 0)
                        {
                                printf("write atoms fd failed\n");
                                trycnt++;
                                close(fd);
                                continue;
                        }
                }

                memset(buffer, 0, sizeof(buffer));				

                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret <= 0)
                {
                        trycnt++;

                        close(fd);
                        continue;
                }

                if(read(fd, buffer, 31) < 0)
                {
                        trycnt++;

                        close(fd);
                        continue;
                }

                for(i = 0; i < 31; i++)
                {
                        if(buffer[i] == am335x_sensor_dev->uart_code[0])
                        {
                                break;
                        }
                }
                if(i == 31)
                {
                        trycnt++;
                        i = 0;
                        close(fd);
                        continue;
                }

                close(fd);

                break;
        }

        if(trycnt >= AM335X_TEST_TRY_COUNT)
        {
                return -2;
        }
        return 0;
}

int am335x_pms_sensor_test(int type, int sensor)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        unsigned char buffer[32];
        struct timeval tv;
        fd_set rfds;
        int trycnt;
        int ret;
        int fd;

        if(AM335X_SENSOR_TYPE_WINSEN_ZH03 == type)
        {
                am335x_sensor_dev = &am335x_sensor_zh03_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_NOVA_SDS == type)
        {
                am335x_sensor_dev = &am335x_sensor_sds_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_PMS5003 == type)
        {
                am335x_sensor_dev = &am335x_sensor_pms5003_dev[sensor];
        }
        else
        {
                return -1;
        }

        trycnt = 0;

        while(trycnt < AM335X_TEST_TRY_COUNT)
        {
                fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

                if(fd < 0)
                {
                        printf("error fd < 0\n");
                        continue;
                }

                fcntl(fd, F_SETFL, 0);

                am335x_uart_set_opt(fd, 9600, 8, 'N', 1, 
                                am335x_sensor_dev->uart_length, 
                                am335x_sensor_dev->uart_timeout);

                memset(buffer, 0, sizeof(buffer));
                buffer[0] = am335x_sensor_dev->uart_code[0];
                buffer[1] = am335x_sensor_dev->uart_code[1];
                if(write(fd, buffer, sizeof(buffer)) < 0)
                {
                        trycnt++;
                        close(fd);
                        continue;
                }


                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret <= 0)
                {
                        trycnt++;

                        close(fd);
                        continue;
                }

                memset(buffer, 0, sizeof(buffer));
                if(read(fd, buffer, sizeof(buffer)) < 0)
                {
                        trycnt++;

                        close(fd);
                        continue;
                }

                if((buffer[0] != am335x_sensor_dev->uart_code[0]) \
                                && (buffer[1] != am335x_sensor_dev->uart_code[1]))
                {
                        trycnt++;

                        close(fd);
                        continue;
                }

                close(fd);

                break;
        }

        if(trycnt >= AM335X_TEST_TRY_COUNT)
        {
                return -2;
        }
#if 0
        if(AM335X_SENSOR_TYPE_NOVA_SDS == type)
        {
                if(am335x_sensor_checksum8bit(&buffer[2], 6) != buffer[8])
                {
                        return -3;
                }
        }
#endif
        return 0;
}

int am335x_pms_sensor_test2(int type, int sensor)
{
        struct am335x_sensor_dev_t *am335x_sensor_dev;
        unsigned char buffer[32];
        struct timeval tv;
        fd_set rfds;
        int trycnt;
        int ret;
        int fd,i;

        if(AM335X_SENSOR_TYPE_WINSEN_ZH03 == type)
        {
                am335x_sensor_dev = &am335x_sensor_zh03_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_NOVA_SDS == type)
        {
                am335x_sensor_dev = &am335x_sensor_sds_dev[sensor];
        }
        else if(AM335X_SENSOR_TYPE_PMS5003 == type)
        {
                am335x_sensor_dev = &am335x_sensor_pms5003_dev[sensor];
        }
        else
        {
                return -1;
        }

        trycnt = 0;

        while(trycnt < AM335X_TEST_TRY_COUNT)
        {
                fd = open(am335x_sensor_dev->uart_dev, O_RDWR | O_NOCTTY | O_NDELAY);

                if(fd < 0)
                {
                        continue;
                }

                fcntl(fd, F_SETFL, 0);

                am335x_uart_set_opt(fd, 9600, 8, 'N', 1, 
                                am335x_sensor_dev->uart_length, 
                                am335x_sensor_dev->uart_timeout);

                memset(buffer, 0xa5, sizeof(buffer));
                if(write(fd, buffer, sizeof(buffer)) < 0)
                {
                        trycnt++;

                        close(fd);

                        continue;
                }

                memset(buffer, 0x00, sizeof(buffer));
                FD_ZERO(&rfds);
                FD_SET(fd, &rfds);

                tv.tv_sec  = 10;
                tv.tv_usec = 0;

                ret = select(fd+1, &rfds, NULL, NULL, &tv);

                if(ret <= 0)
                {
                        trycnt++;

                        close(fd);
                        continue;
                }

                if(read(fd, buffer, sizeof(buffer)) < 0)
                {
                        trycnt++;

                        close(fd);
                        continue;
                }
#if 0
                if((buffer[0] != am335x_sensor_dev->uart_code[0]) \
                                && (buffer[1] != am335x_sensor_dev->uart_code[1]))
                {
                        trycnt++;

                        close(fd);

                        //printf("buffer[0] = %x ; buffer[1] = %x;\n", buffer[0], buffer[1]);
                        //printf("am335x_sensor_dev->uart_code[0] = %x; am335x_sensor_dev->uart_code[1] = %x\n",\
                        am335x_sensor_dev->uart_code[0],am335x_sensor_dev->uart_code[1]);
                        //printf("buffer[0] && buffer[1] error\n");
                        continue;
                }
#endif
                close(fd);

                break;
        }

        if(trycnt >= AM335X_TEST_TRY_COUNT)
        {
                return -2;
        }

        for(i = 0; i < sizeof(buffer); i++)
        {
                if(buffer[i] != 0xa5)
                {
                        return -1;
                }
        }
#if 0
        if(AM335X_SENSOR_TYPE_NOVA_SDS == type)
        {
                if(am335x_sensor_checksum8bit(&buffer[2], 6) != buffer[8])
                {
                        return -3;
                }
        }
#endif
        return 0;
}

int am335x_rtc_test()
{
        struct tm time;
        int ret = 0;
        /*time.tm_year = 2016;
          time.tm_mon  = 1;
          time.tm_mday = 1;
          time.tm_hour = 12;
          time.tm_min  = 0;
          time.tm_sec  = 0;

          ret = am335x_set_time(&time);*/
        return ret;
}

int am335x_emmc_test()
{
        struct stat statbuf;
        unsigned long length;
        char buffer[BLOCK_SIZE];
        int  fd;
        int  i,j,n;
        length = 128 * 1024 * 1024;


        fd = open("emmcfile", O_CREAT | O_RDWR | O_SYNC);

        if(fd < 0)
        {
                printf("emmcfile open failed\r\n");

                return -1;
        }

        for(i = 0; i < (length / BLOCK_SIZE); i++)
        {
                memset(buffer, 0xA5, BLOCK_SIZE);

                n = write(fd, buffer, BLOCK_SIZE);

                if(n < 0)
                {
                        printf("emmcfile write failed\r\n");

                        close(fd);

                        return -2;
                }
        }

        if((i == (length / BLOCK_SIZE)) && (length % BLOCK_SIZE) > 0)
        {
                memset(buffer, 0xA5, BLOCK_SIZE);

                n = write(fd, buffer, (length % BLOCK_SIZE));

                if(n < 0)
                {
                        printf("emmcfile write failed\r\n");

                        close(fd);

                        return -3;
                }
        }

        close(fd);

        if(stat("emmcfile", &statbuf) < 0)
        {
                length = 0;
        }
        else
        {
                length = statbuf.st_size;
        }

        if(length <= 0)
        {
                printf("emmcfile get length failed\r\n");

                return -4;
        }

        fd = open("emmcfile", O_RDWR|O_SYNC);

        if(fd < 0)
        {
                printf("emmcfile open failed\r\n");

                return -5;
        }

        i = 0;

        while(1)
        {
                memset(buffer, 0, BLOCK_SIZE);

                n = read(fd, buffer, BLOCK_SIZE);

                if(n < 0)
                {
                        printf("emmcfile read failed\r\n");

                        close(fd);

                        return -6;
                }
                else if(n == 0)
                {
                        close(fd);
                        return 0;
                }

                for(j = 0; j < n; j++)
                {
                        if(buffer[j] != 0xA5)
                        {
                                printf("emmcfile compare failed: %d,%d\r\n", i, j);

                                close(fd);

                                return -7;
                        }
                }

                i++;
        }

        close(fd);
}


int am335x_watchdog_test()
{
        int sleep_time = 59;//1 s
        int fd;
        int i = 0;

        dog_fd = am335x_open_dog(fd);
        while(1)
        {
                am335x_feed_dog(dog_fd,sleep_time);
                i++;
                if(i > 3)
                {
                        break;
                }
        }

        am335x_close_dog(dog_fd);

        return 0;
}

int am335x_4g_module_test(int type, int flag)
{
        char ip[16];
        int  ttyACM;

        ttyACM = am335x_4g_module_acm_open();

        if(ttyACM < 0)
        {
                return -2;
        }

        fcntl(ttyACM, F_SETFL, 0);

        am335x_uart_set_opt(ttyACM, 115200, 8, 'N', 1, 255, 15);

        if(flag)
        {
                if(am335x_4g_module_check(g_4g_type, ttyACM, 1) < 0)
                {
                        close(ttyACM);

                        return -3;
                }

                if(am335x_4g_module_net_connect(g_4g_type, ttyACM) < 0)
                {
                        close(ttyACM);

                        return -4;
                }

                if(am335x_4g_module_net_enable(g_4g_type, ttyACM) < 0)
                {
                        close(ttyACM);

                        return -5;
                }

                if(am335x_4g_module_net_config(g_4g_type, ttyACM, ip) < 0)
                {
                        close(ttyACM);

                        return -6;
                }
        }
        else
        {
                if(am335x_4g_module_check(g_4g_type, ttyACM, 0) < 0)
                {
                        close(ttyACM);

                        return -7;
                }
        }

        close(ttyACM);

        return 0;
}
/************************************************************
Name: am335x_drv_init
Func: am335x driver init
Para:  int *res       : return 
none

 *************************************************************/
int am335x_drv_init(int gas_ptype, int pms_ptype, int atom_ptype, int* rest)
{
        struct am335x_sensor_mem_t *am335x_sensor_mem;
        struct sensor_para_t sType[7];
		struct alarm_para_t  alarmType[4];
        unsigned int *regs;
        pthread_t gas_tid, pm_tid, atmos_tid, net_tid, 
			am2311_tid,alarm_tid1,alarm_tid2,
			alarm_tid3,alarm_tid4, so2_tid, 
			no2_tid, co_tid, o3_tid,tvoc_tid;
        int errno;
        void *res;
        //pid_t pid;
        int shmid,bvalue_shmid;
        int i;
        int value;
        int ret;
        STRUCT_WATCH_DOG *watch_dog_mem;
        int watchdog_semid,index = 0;		
        gettimeofday(&time0, NULL);
        am335x_openlog("am335x driver");
        sem_init(&sem,1,0);

        shmid = shmget(AM335X_SENSOR_DATA_ID, AM335X_SENSOR_DATA_MAX_NUMS * sizeof(struct am335x_sensor_mem_t), IPC_CREAT | 0666);
        if((shmid < 0))
        {
                am335x_printlog("INIT: Sensor Share Memory Create Failed\r\n");
                am335x_closelog("am335x driver");
                printf("error num is %d\n", errno);
                return -1;
        }

        am335x_sensor_mem = (struct am335x_sensor_mem_t*)shmat(shmid, 0, 0);

        if((am335x_sensor_mem == (struct am335x_sensor_mem_t*)(-1)))
        {
                am335x_printlog("INIT: Sensor Share Memory Connect Failed\r\n");
                am335x_closelog("am335x driver");

                return -2;
        }

        am335x_closelog("am335x driver");

        am335x_gpio_init();

#if 0
        am335x_lcd_clear();
        am335x_lcd_drawASC(0, 0, 'A');
        am335x_lcd_drawASC(1, 0, '3');
        am335x_lcd_drawASC(2, 0, '3');
        am335x_lcd_drawASC(3, 0, '5');
        am335x_lcd_drawASC(4, 0, 'C');
        am335x_lcd_drawASC(5, 0, 'P');
        am335x_lcd_drawASC(6, 0, 'I');
        am335x_lcd_drawASC(7, 0, 'T');

        am335x_lcd_drawASC(0, 1, 'I');
        am335x_lcd_drawASC(1, 1, 'N');
        am335x_lcd_drawASC(2, 1, 'I');
        am335x_lcd_drawASC(3, 1, 'T');
        am335x_lcd_drawASC(4, 1, ' ');
        am335x_lcd_drawASC(5, 1, 'O');
        am335x_lcd_drawASC(6, 1, 'K');
        am335x_lcd_drawASC(7, 1, '!');

        am335x_lcd_display();                                                                  
#endif
        memset((char*)am335x_sensor_mem, 0, AM335X_SENSOR_DATA_MAX_NUMS * sizeof(struct am335x_sensor_mem_t));

        am335x_sensor_mem[AM335X_SENSOR_DATA_SO2].types      = gas_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_NO2].types      = gas_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CO].types       = gas_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_O3].types       = gas_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_TVOC].types     = gas_ptype;

        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].types    = pms_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].types    = pms_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].types    = pms_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].types = pms_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].types = pms_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].types = pms_ptype;

        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].types            = atom_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].types          = atom_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].types  = atom_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].types           = atom_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].types              = atom_ptype;
        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].types        = atom_ptype;

        am335x_sensor_mem[AM335X_SENSOR_DATA_SO2].flags      = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_NO2].flags      = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CO].flags       = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_O3].flags       = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_TVOC].flags     = AM335X_SENSOR_NOT_INIT;

        am335x_sensor_mem[AM335X_SENSOR_DATA_PM010].flags    = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_PM025].flags    = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_PM100].flags    = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM010].flags = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM025].flags = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_CF_PM100].flags = AM335X_SENSOR_NOT_INIT;

        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_SPEED].flags           = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_AIR_PRESSURE].flags         = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_ACCUMULATED_RAINFALL].flags = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_TEMPERATURE].flags          = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_HUMIDITY].flags             = AM335X_SENSOR_NOT_INIT;
        am335x_sensor_mem[AM335X_SENSOR_DATA_WIND_DIRECTION].flags       = AM335X_SENSOR_NOT_INIT;	    

        shmdt((char*)am335x_sensor_mem);

        am335x_openlog("am335x driver");
        sType[S_SO2].gtype = AM335X_SENSOR_SO2;
        sType[S_SO2].stype = gas_ptype;
        errno = pthread_create(&so2_tid, NULL, am335x_gas_sensor_process, (void*)&sType[0]);
        if(errno != 0)
        {
                *rest = SO2_PTHREAD_ERROR;
                am335x_printlog("can not create so2 thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }


        sType[S_NO2].gtype = AM335X_SENSOR_NO2;
        sType[S_NO2].stype = gas_ptype;

        errno = pthread_create(&no2_tid, NULL, am335x_gas_sensor_process, (void*)&sType[1]);
        if(errno != 0)
        {        
                *rest = NO2_PTHREAD_ERROR;
                am335x_printlog("can not create no2 thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }
        sType[S_CO].gtype = AM335X_SENSOR_CO;
        sType[S_CO].stype = gas_ptype;

        errno = pthread_create(&co_tid, NULL, am335x_gas_sensor_process, (void*)&sType[2]);
        if(errno != 0)
        {        
                *rest = CO_PTHREAD_ERROR;
                am335x_printlog("can not create co thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }

        sType[S_O3].gtype = AM335X_SENSOR_O3;
        sType[S_O3].stype = gas_ptype;
        errno = pthread_create(&o3_tid, NULL, am335x_gas_sensor_process, (void*)&sType[3]);
        if(errno != 0)
        {        
                *rest = O3_PTHREAD_ERROR;
                am335x_printlog("can not create o3 thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }

        sType[S_TVOC].gtype = AM335X_SENSOR_TVOC;
        sType[S_TVOC].stype = gas_ptype;
        errno = pthread_create(&tvoc_tid, NULL, am335x_gas_sensor_process, (void*)&sType[4]);
        if(errno != 0)
        {       
                *rest = TVOC_PTHREAD_ERROR;
                am335x_printlog("can not create tvoc thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }

        sType[S_PM].gtype = AM335X_SENSOR_PM;
        sType[S_PM].stype = pms_ptype;
        errno = pthread_create(&pm_tid, NULL, am335x_pms_sensor_process, (void*)&sType[5]);
        if(errno != 0)
        {       
                *rest = PM_PTHREAD_ERROR;
                am335x_printlog("can not create pm thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }

        sType[S_ATMOS].gtype = AM335X_SENSOR_ATOMS;
        sType[S_ATMOS].stype = atom_ptype;
        errno = pthread_create(&atmos_tid, NULL, am335x_atoms_sensor_process, (void*)&sType[6]);
        if(errno != 0)
        {       		
                *rest = ATMOS_PTHREAD_ERROR;
                am335x_printlog("can not create atoms thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }

        errno = pthread_create(&net_tid, NULL, am335x_4gnet_init, NULL);
        if(errno != 0)
        {     		
                *rest = NET_4G_PTHREAD_ERROR;
                am335x_printlog("can not create 4g thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }
#if 1
		alarmType[0].mem_id = ALARM_MEM_ID;
		alarmType[0].alarm_id = ALARM_DOOR_SYSTEM_ID;
		alarmType[0].dev = AM335X_DOOR_SYSTEM_SYS;
		errno = pthread_create(&alarm_tid1, NULL, am335x_alarm_process, (void*)&alarmType[0]);
		if(errno != 0)
        {     		
                *rest = NET_4G_PTHREAD_ERROR;
                am335x_printlog("can not alarm1 thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }
#endif
#if 1
		alarmType[1].mem_id = ALARM_MEM_ID;
		alarmType[1].alarm_id = ALARM_HUMIDITY_ID;
		alarmType[1].dev = AM335X_HUMIDITY_SYS;
		errno = pthread_create(&alarm_tid2, NULL, am335x_alarm_process, (void*)&alarmType[1]);
		if(errno != 0)
        {     		
                *rest = NET_4G_PTHREAD_ERROR;
                am335x_printlog("can not create alarm2 thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }
#endif
#if 0
		alarmType[2].mem_id = ALARM_MEM_ID;
		alarmType[2].alarm_id = ALARM_FAN_ID;
		errno = pthread_create(&alarm_tid3, NULL, am335x_alarm_process, (void*)&alarmType[2]);
		if(errno != 0)
        {     		
                *rest = NET_4G_PTHREAD_ERROR;
                am335x_printlog("can not create alarm3 thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }


#endif
#if 1
		alarmType[2].mem_id = ALARM_MEM_ID;
		alarmType[2].alarm_id = ALARM_TEMP_ID;
		alarmType[2].dev = AM335X_TEMPERATURE_SYS;
		errno = pthread_create(&alarm_tid3, NULL, am335x_alarm_process, (void*)&alarmType[2]);
		if(errno != 0)
        {     		
                *rest = NET_4G_PTHREAD_ERROR;
                am335x_printlog("can not create alarm3 thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }
#endif

		alarmType[3].mem_id = ALARM_MEM_ID;
		alarmType[3].alarm_id = ALARM_CELL_ID;
		alarmType[3].dev = AM335X_VOLTAGE_SYS;
		errno = pthread_create(&alarm_tid4, NULL, am335x_alarm_process, (void*)&alarmType[3]);
		if(errno != 0)
        {     		
                *rest = NET_4G_PTHREAD_ERROR;
                am335x_printlog("can not create alarm4 thread %s\r\n", strerror(errno));
                am335x_closelog("am335x driver");
                exit(1);
        }


        //创建开门狗共享内存
        shmid = shmget(WATCH_DOG_ID, WATCH_DOG_ID_MAX_NUMS * sizeof(STRUCT_WATCH_DOG), IPC_CREAT | 0666);

        if(shmid < 0)
        {
                am335x_printlog("%d: Share Memory Get Failed\r\n", watch_dog_mem->taskID);

        }

        watch_dog_mem = (STRUCT_WATCH_DOG*)shmat(shmid, 0, 0);

        if(watch_dog_mem == (STRUCT_WATCH_DOG*)(-1))
        { 
                am335x_printlog("%d: Share Memory Link Failed\r\n", watch_dog_mem->taskID);
        }
#if 0
        	while(1)
            {	
         	   watch_dog_mem[index].LastCnt = watch_dog_mem[index].CurCnt;	
       		   watch_dog_mem[index].CurCnt++;
      		  sleep(5);
      		  }
#endif
#if 0
        watchdog_semid = get_sems(PROJ_WATCHDOG_ID);
        am335x_printlog("am335x_drv 000 watchdog_semid = %d",watchdog_semid);

        while(1)
        {
				am335x_printlog("g_pthread_exit = %d ",g_pthread_exit);
				if(g_pthread_exit == 0)
				{
					am335x_printlog("am335x_drv aaaa get_sems_val = %d",get_sems_val(watchdog_semid,index));
					mysem_P(watchdog_semid, index); // 对该信号量集中的1号信号	做P操作
					am335x_printlog("am335x_drv bbbb get_sems_val = %d",get_sems_val(watchdog_semid,index));
                }				
        }
#endif




        am335x_closelog("am335x driver");


        pthread_join(no2_tid,NULL);
        pthread_join(so2_tid,NULL);
        pthread_join(co_tid,NULL);
        pthread_join(o3_tid,NULL);
        pthread_join(tvoc_tid,NULL);
        pthread_join(pm_tid,NULL);
        pthread_join(atmos_tid,NULL);	
        pthread_join(net_tid,NULL);	
        pthread_join(alarm_tid1,NULL);
		pthread_join(alarm_tid2,NULL);
		pthread_join(alarm_tid3,NULL);
		pthread_join(alarm_tid4,NULL);

        //am335x_check_alarm_status(POWER_LOST_MEM_ID,ALARM_POWER_LOST_ID, AM335X_POWER_LOST_SYS);
        //am335x_check_alarm_status(HUMIDITY_MEM_ID,ALARM_HUMIDITY_ID, AM335X_HUMIDITY_SYS);
        //am335x_check_alarm_status(TEMP_MEM_ID,ALARM_TEMP_ID, AM335X_TEMPERATURE_SYS);
        //am335x_check_alarm_status(CELL_MEM_ID,ALARM_CELL_ID, AM335X_VOLTAGE_SYS);

        return 0;
}
