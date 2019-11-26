#include <stdlib.h>
#include <math.h>

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "app_main.h"
#include "ADC.h"

static const adc_unit_t unit = ADC_UNIT_1;
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static esp_adc_cal_characteristics_t *adc_chars;

static double voltage2distance(uint32_t voltage);
static double distance2tanklevel(uint32_t distance);

void adc_init()
{

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel,atten);

    //Characterize ADC at particular atten
    esp_adc_cal_characteristics_t *adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

    int val = adc1_get_raw(channel);
}

uint32_t read_adc()
{
    uint32_t adc_reading = 0, voltage;
    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        if (unit == ADC_UNIT_1) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        } else {
            int raw;
            adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
            adc_reading += raw;
        }
    }
    adc_reading /= NO_OF_SAMPLES;
    voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    return voltage; // mV
}

static double voltage2distance(uint32_t voltage)
{
    double distance = 785409 * pow(voltage, -1.174);
    return distance; // mm
}

static double distance2tanklevel(uint32_t distance)
{
    double height = 1000 - distance + TANK_LEVEL_OFFSET;
    return height / 10; // %
}

double read_tank_level()
{
    uint32_t voltage = read_adc();
    double distance = voltage2distance(voltage);
    double tanklevel = distance2tanklevel(distance);
    return tanklevel;
}