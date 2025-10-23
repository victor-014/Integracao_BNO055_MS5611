#pragma once

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#define BNO_ADDRESS 0x29
#define BNO055_SAMPLERATE_DELAY_MS 1000

extern Adafruit_BNO055 bno;

void initBNO055();
void displaySensorDetails();
void displaySensorStatus();
void configBNO055();
void calibBNO055();
void printEvent(sensors_event_t* event);
void leituraBNO055();