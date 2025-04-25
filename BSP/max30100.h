#ifndef MAX30100_H
#define MAX30100_H

#include "stm32f1xx_hal.h"

#define MAX30100_ADDRESS          0x57 << 1  // 7-bit address is 0x57, shifted for HAL

// Register addresses
#define MAX30100_REG_INTR_STATUS  0x00
#define MAX30100_REG_INTR_ENABLE  0x01
#define MAX30100_REG_FIFO_WR_PTR  0x02
#define MAX30100_REG_OVF_COUNTER  0x03
#define MAX30100_REG_FIFO_RD_PTR  0x04
#define MAX30100_REG_FIFO_DATA    0x05
#define MAX30100_REG_MODE_CONFIG  0x06
#define MAX30100_REG_SPO2_CONFIG  0x07
#define MAX30100_REG_LED_CONFIG  0x09
#define MAX30100_REG_TEMP_INTG   0x16
#define MAX30100_REG_TEMP_FRAC   0x17
#define MAX30100_REG_REV_ID      0xFE
#define MAX30100_REG_PART_ID     0xFF

// Mode configuration
#define MAX30100_MODE_HR_ONLY    0x02
#define MAX30100_MODE_SPO2_HR    0x03

// SpO2 configuration
#define MAX30100_SPO2_ADC_4096   0x03
#define MAX30100_SPO2_SR_100HZ   0x03

// LED configuration
#define MAX30100_LED_CURRENT_27MA 0x1F

// Interrupt flags
#define MAX30100_INTR_SPO2_RDY   0x10
#define MAX30100_INTR_HR_RDY     0x20
#define MAX30100_INTR_FIFO_FULL  0x80



#define BUFFER_SIZE 500
#define FILTER_WINDOW 5

typedef struct {
    I2C_HandleTypeDef *hi2c;
    GPIO_TypeDef *int_port;
    uint16_t int_pin;
    uint8_t buffer[4];  // FIFO data buffer (IR + RED)
} MAX30100_HandleTypeDef;

// Function prototypes
HAL_StatusTypeDef MAX30100_Init(MAX30100_HandleTypeDef *hmax, I2C_HandleTypeDef *hi2c, GPIO_TypeDef *int_port, uint16_t int_pin);
HAL_StatusTypeDef MAX30100_ReadFIFO(MAX30100_HandleTypeDef *hmax, uint16_t *ir_data, uint16_t *red_data);
HAL_StatusTypeDef MAX30100_ReadReg(MAX30100_HandleTypeDef *hmax, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef MAX30100_WriteReg(MAX30100_HandleTypeDef *hmax, uint8_t reg, uint8_t data);
uint8_t MAX30100_GetIntStatus(MAX30100_HandleTypeDef *hmax);
void MAX30100_Reset(MAX30100_HandleTypeDef *hmax);

uint16_t moving_average(uint16_t *buf, uint8_t size, uint8_t index);
//uint8_t calculate_heart_rate(uint16_t *ir_data, uint8_t size) ;
int CalculateHeartRate(uint16_t *ir_data, int data_count);

#endif // MAX30100_H