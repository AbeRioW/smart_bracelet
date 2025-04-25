#ifndef AXDL345_H
#define AXDL345_H

#include "main.h"
#include "i2c.h"
#include "oled.h"
#include "stdio.h"

#define ADXL345_ADDR 0x53

void start_read_mode(void);
void read_xyz(void);

#endif