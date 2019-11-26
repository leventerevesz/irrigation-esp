#ifndef ADC_H
#define ADC_H

#define DEFAULT_VREF    1097        //
#define NO_OF_SAMPLES   64          // Multisampling

void adc_init();
uint32_t read_adc();
double read_tank_level();

#endif