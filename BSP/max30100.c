#include "max30100.h"
#include "stm32f1xx_hal.h"

HAL_StatusTypeDef MAX30100_Init(MAX30100_HandleTypeDef *hmax, I2C_HandleTypeDef *hi2c, GPIO_TypeDef *int_port, uint16_t int_pin) {
    hmax->hi2c = hi2c;
    hmax->int_port = int_port;
    hmax->int_pin = int_pin;
    
    // Reset the device
    MAX30100_Reset(hmax);
    HAL_Delay(10);
    
    // Check if device is present
    uint8_t part_id;
    if(MAX30100_ReadReg(hmax, MAX30100_REG_PART_ID, &part_id) != HAL_OK) {
        return HAL_ERROR;
    }
    
    if(part_id != 0x11) {  // MAX30100 part ID is 0x11
        return HAL_ERROR;
    }
    
    // Configure the sensor
    // Mode configuration: SpO2 + HR mode
    if(MAX30100_WriteReg(hmax, MAX30100_REG_MODE_CONFIG, MAX30100_MODE_SPO2_HR) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // SpO2 configuration: 100Hz sampling rate, 4096 ADC range
    if(MAX30100_WriteReg(hmax, MAX30100_REG_SPO2_CONFIG, (MAX30100_SPO2_ADC_4096 << 5) | (MAX30100_SPO2_SR_100HZ << 2)) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // LED configuration: 27mA for both LEDs
    if(MAX30100_WriteReg(hmax, MAX30100_REG_LED_CONFIG, (MAX30100_LED_CURRENT_27MA << 4) | MAX30100_LED_CURRENT_27MA) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Enable interrupts
    if(MAX30100_WriteReg(hmax, MAX30100_REG_INTR_ENABLE, MAX30100_INTR_SPO2_RDY | MAX30100_INTR_HR_RDY | MAX30100_INTR_FIFO_FULL) != HAL_OK) {
        return HAL_ERROR;
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef MAX30100_ReadFIFO(MAX30100_HandleTypeDef *hmax, uint16_t *ir_data, uint16_t *red_data) {
    // Read 4 bytes from FIFO (IR + RED)
    if(HAL_I2C_Mem_Read(hmax->hi2c, MAX30100_ADDRESS, MAX30100_REG_FIFO_DATA, 1, hmax->buffer, 4, 100) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Combine bytes to form 16-bit values (data is 18-bit but we'll use 16-bit)
    *ir_data = (hmax->buffer[0] << 8) | hmax->buffer[1];
    *red_data = (hmax->buffer[2] << 8) | hmax->buffer[3];
    
    return HAL_OK;
}

HAL_StatusTypeDef MAX30100_ReadReg(MAX30100_HandleTypeDef *hmax, uint8_t reg, uint8_t *data) {
    return HAL_I2C_Mem_Read(hmax->hi2c, MAX30100_ADDRESS, reg, 1, data, 1, 100);
}

HAL_StatusTypeDef MAX30100_WriteReg(MAX30100_HandleTypeDef *hmax, uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write(hmax->hi2c, MAX30100_ADDRESS, reg, 1, &data, 1, 100);
}

uint8_t MAX30100_GetIntStatus(MAX30100_HandleTypeDef *hmax) {
    uint8_t status;
    MAX30100_ReadReg(hmax, MAX30100_REG_INTR_STATUS, &status);
    return status;
}

void MAX30100_Reset(MAX30100_HandleTypeDef *hmax) {
    MAX30100_WriteReg(hmax, MAX30100_REG_MODE_CONFIG, 0x40);  // Reset bit
}




uint16_t moving_average(uint16_t *buf, uint8_t size, uint8_t index) {
    uint32_t sum = 0;
    uint8_t count = 0;
    
    for(int8_t i = -size/2; i <= size/2; i++) {
        int16_t pos = index + i;
        if(pos >= 0 && pos < BUFFER_SIZE) {
            sum += buf[pos];
            count++;
        }
    }
    
    return sum / count;
}


#if 0
// Simple heart rate calculation (for demonstration)
// In a real application, you would use proper signal processing
uint8_t CalculateHeartRate(uint16_t *ir_data, uint8_t size) {
    // This is a very simplified approach
    // Real implementation would use FFT or peak detection
    
    // Find min and max values
    uint16_t min = 0xFFFF;
    uint16_t max = 0;
    for(uint8_t i = 0; i < size; i++) {
        if(ir_data[i] < min) min = ir_data[i];
        if(ir_data[i] > max) max = ir_data[i];
    }
    
    // Count crossings of the middle value
    uint16_t threshold = (min + max) / 2;
    uint8_t crossings = 0;
    uint8_t last_state = (ir_data[0] > threshold);
    
    for(uint8_t i = 1; i < size; i++) {
        uint8_t current_state = (ir_data[i] > threshold);
        if(current_state != last_state) {
            crossings++;
            last_state = current_state;
        }
    }
    
    // Convert crossings to BPM (assuming 100Hz sampling rate)
    float bpm = (crossings / 2.0) * (60.0 / (size / 100.0));
    return (uint8_t)bpm;
}
#else
int CalculateHeartRate(uint16_t *ir_data, int data_count)
{
    // ???????,???????????????
    int sum = 0;
    for (int i = 0; i < data_count; i++)
    {
        sum += ir_data[i];
    }
    return (sum / data_count)/3;
}
#endif