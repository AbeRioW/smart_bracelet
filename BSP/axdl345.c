#include "axdl345.h"

void start_read_mode(void)
{
		//��������ģʽ
    uint8_t data[] = {0x2D, 0x08}; // �Ĵ���ֵ0x2D,ֵ0x08��ʾ��������
    HAL_I2C_Master_Transmit(&hi2c2, ADXL345_ADDR << 1, data, 2, HAL_MAX_DELAY);
}

void read_xyz(void)
{
	char show_xyz[10];
	uint8_t regAddr = 0x32; //���ٶ�������ʼ�Ĵ�����ַ
	uint8_t dataRaw[6]; // 6���ֽڷֱ�洢x,y,z��ļ��ٶ�����

	HAL_I2C_Master_Transmit(&hi2c2, ADXL345_ADDR << 1, &regAddr, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c2, ADXL345_ADDR << 1, dataRaw, 6, HAL_MAX_DELAY);

	int16_t accelerometer_data[3]; // �洢������ļ��ٶ�����

	accelerometer_data[0] = (int16_t)((dataRaw[1] << 8) | dataRaw[0]); // X��
	accelerometer_data[1] = (int16_t)((dataRaw[3] << 8) | dataRaw[2]); // Y��
	accelerometer_data[2] = (int16_t)((dataRaw[5] << 8) | dataRaw[4]); // Z��
	sprintf(show_xyz,"x:%d y:%d z:%d",accelerometer_data[0],accelerometer_data[1],accelerometer_data[2]);
	
						OLED_ShowString(0,20,(uint8_t*)show_xyz,8,1);
					OLED_Refresh();
}