#pragma once
#include <Arduino.h>
#include "MS5611.h"

#define SDA_PIN GPIO_NUM_27
#define SCL_PIN GPIO_NUM_14

extern MS5611 altimetro;
extern float media;

void initMS5611();
void calibMS5611();
void leituraMS5611();
