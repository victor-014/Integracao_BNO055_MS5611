#include "altimetro.h"

MS5611 altimetro(0x77);

float media = 0;
void calibMS5611(){
    Serial.println("Iniciando Calibração do MS5611...");
    altimetro.setOversampling(OSR_ULTRA_LOW); //número de bits de precisão da leitura: ULTRA_LOW=8, ULTRA_HIGH=12
    int CAL_SAMPLES = 100;
    float Total = 0;
    float leitura_calibracao = 0;
    int result = 0;
    unsigned long inicio_calibracao = micros();
    for (int i=0; i<CAL_SAMPLES; i++){
        result = altimetro.read();
        Serial.printf("[%d] ", i);
        if (result == MS5611_READ_OK){
            leitura_calibracao = altimetro.getPressurePascal();
            Total += leitura_calibracao;
            Serial.println(leitura_calibracao);
        } else{
            Serial.print("Erro na leitura: "); Serial.println(result);
        }
    }
    unsigned long final_calibracao = micros();
    media = Total/CAL_SAMPLES;
    Serial.print("Média: "); Serial.println(media);
    Serial.print("Tempo gasto: "); Serial.println(final_calibracao - inicio_calibracao);
}

void initMS5611(void){
    if (altimetro.begin()){
        Serial.println("MS5611 inicializado!");
        Serial.print("Endereço"); Serial.println(altimetro.getAddress());
        delay(1000);
    } else{
        while(1){
            Serial.println("Erro na inicialização do MS5611");
            delay(1000);
        }
    }
}

void leituraMS5611(){
    int result = 0;
    float altitude = 0;
    float altitude_maxima = 0;
    float altitude_minima = 0;
    float altitude_total = 0;
    for (int i=0; i<10; i++){
        result = altimetro.read();
        Serial.printf("[%d]", i);
        if (result == MS5611_READ_OK){
            altitude = altimetro.getAltitude(media);
            if (altitude > altitude_maxima) altitude_maxima = altitude;
            if (altitude < altitude_minima) altitude_minima = altitude;
            altitude_total += altitude;
            Serial.println(altitude);
        } else{
            Serial.println("Erro na leitura");
        }
    }
    Serial.print("Altitude média: ");Serial.println(altitude_total/10);
    Serial.print("Altitude máxima: ");Serial.println(altitude_maxima);
    Serial.print("Altitude minima: ");Serial.println(altitude_minima);
}