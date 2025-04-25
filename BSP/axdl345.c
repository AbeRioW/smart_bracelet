#include "axdl345.h"

void start_read_mode(void)
{
		//启动测量模式
    uint8_t data[] = {0x2D, 0x08}; // 寄存器值0x2D,值0x08表示启动测量
    HAL_I2C_Master_Transmit(&hi2c2, ADXL345_ADDR << 1, data, 2, HAL_MAX_DELAY);
}

void read_xyz(void)
{
	char show_xyz[10];
	uint8_t regAddr = 0x32; //加速度数据起始寄存器地址
	uint8_t dataRaw[6]; // 6个字节分别存储x,y,z轴的加速度数据

	HAL_I2C_Master_Transmit(&hi2c2, ADXL345_ADDR << 1, &regAddr, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c2, ADXL345_ADDR << 1, dataRaw, 6, HAL_MAX_DELAY);

	int16_t accelerometer_data[3]; // 存储解析后的加速度数据

	accelerometer_data[0] = (int16_t)((dataRaw[1] << 8) | dataRaw[0]); // X轴
	accelerometer_data[1] = (int16_t)((dataRaw[3] << 8) | dataRaw[2]); // Y轴
	accelerometer_data[2] = (int16_t)((dataRaw[5] << 8) | dataRaw[4]); // Z轴
	sprintf(show_xyz,"x:%d y:%d z:%d",accelerometer_data[0],accelerometer_data[1],accelerometer_data[2]);
	
						OLED_ShowString(0,20,(uint8_t*)show_xyz,8,1);
					OLED_Refresh();
}