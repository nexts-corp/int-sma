#ifndef CONFIG_DEF_H
#define CONFIG_DEF_H

#include "main.h"

//Appendix C
#define CONF_PROPERTIES         0x10

#define CONF_OPERATEING         0x20


#define CONF_SEN_ANAL_CH0         0x30
#define CONF_SEN_ANAL_CH1         0x31
#define CONF_SEN_ANAL_CH2         0x32
#define CONF_SEN_ANAL_CH3         0x33
#define CONF_SEN_ANAL_CH4         0x34

#define CONF_DIGI_IN             0x40
#define CONF_DIGI_IN_CH0         0x40
#define CONF_DIGI_IN_CH1         0x41
#define CONF_DIGI_IN_CH2         0x42
#define CONF_DIGI_IN_CH3         0x43
#define CONF_DIGI_IN_CH4         0x44
#define CONF_DIGI_IN_CH5         0x45
#define CONF_DIGI_IN_CH6         0x46
#define CONF_DIGI_IN_CH7         0x47

#define CONF_DIGI_OUT             0x50
#define CONF_DIGI_OUT_CH0         0x50
#define CONF_DIGI_OUT_CH1         0x51
#define CONF_DIGI_OUT_CH2         0x52
#define CONF_DIGI_OUT_CH3         0x53
#define CONF_DIGI_OUT_CH4         0x54
#define CONF_DIGI_OUT_CH5         0x55
#define CONF_DIGI_OUT_CH6         0x56
#define CONF_DIGI_OUT_CH7         0x57

#define CONF_ANA_IN             0x60
#define CONF_ANA_IN_CH0         0x60
#define CONF_ANA_IN_CH1         0x61
#define CONF_ANA_IN_CH2         0x62
#define CONF_ANA_IN_CH3         0x63
#define CONF_ANA_IN_CH4         0x64
#define CONF_ANA_IN_CH5         0x65
#define CONF_ANA_IN_CH6         0x66
#define CONF_ANA_IN_CH7         0x67

#define CONF_ANA_OUT             0x70
#define CONF_ANA_OUT_CH0         0x70
#define CONF_ANA_OUT_CH1         0x71
#define CONF_ANA_OUT_CH2         0x72
#define CONF_ANA_OUT_CH3         0x73
#define CONF_ANA_OUT_CH4         0x74
#define CONF_ANA_OUT_CH5         0x75
#define CONF_ANA_OUT_CH6         0x76
#define CONF_ANA_OUT_CH7         0x77

#define CONF_BUZZER             0x80

#define CONF_ENVIRON_COND       0x90


//Appendix B
#define EVENT_MAINTENACE        0x10

#define EVENT_LEN               0x20

#define EVENT_WIFI              0x30

#define EVENT_POWER             0x40

#define EVENT_BATTERY           0x50

#define EVENT_MEMORY            0x60

#define EVENT_COMMUNICATION     0x70

#define EVENT_DIGI_IN_CH0       0x80
#define EVENT_DIGI_IN_CH1       0x81
#define EVENT_DIGI_IN_CH2       0x82
#define EVENT_DIGI_IN_CH3       0x83
#define EVENT_DIGI_IN_CH4       0x84
#define EVENT_DIGI_IN_CH5       0x85
#define EVENT_DIGI_IN_CH6       0x86
#define EVENT_DIGI_IN_CH7       0x87

#define EVENT_ANA_IN_CH0        0x90
#define EVENT_ANA_IN_CH1        0x91
#define EVENT_ANA_IN_CH2        0x92
#define EVENT_ANA_IN_CH3        0x93
#define EVENT_ANA_IN_CH4        0x94
#define EVENT_ANA_IN_CH5        0x95
#define EVENT_ANA_IN_CH6        0x96
#define EVENT_ANA_IN_CH7        0x97

#define EVENT_DATA_CH0          0xA0
#define EVENT_DATA_CH1          0xA1
#define EVENT_DATA_CH2          0xA2
#define EVENT_DATA_CH3          0xA3
#define EVENT_DATA_CH4          0xA4

#define EVENT_SENSOR_CH0        0xB0
#define EVENT_SENSOR_CH1        0xB1
#define EVENT_SENSOR_CH2        0xB2
#define EVENT_SENSOR_CH3        0xB3
#define EVENT_SENSOR_CH4        0xB4




#define EVENT_S_NORMAL          0x00
#define EVENT_S_MAINTENANCE     0x01
#define EVENT_S_UNKNOW          0xFF

#define EVENT_S_NO_CONNECT      0x01

#define EVENT_S_MAIN_POWER_DOWN 0x01

#define EVENT_S_LOW_BATTERY     0x01

#define EVENT_S_FALLING_EDGE    0x00
#define EVENT_S_RISING_EDGE     0x01

#define EVENT_S_LOWER_LIMIT_EXCEED          0x01
#define EVENT_S_LOWER_LIMIT_EXCEED_DELAY    0x02
#define EVENT_S_LOWER_EXTREME_LIMIT_EXCEED  0x03
#define EVENT_S_UPPER_LIMIT_EXCEED          0x04
#define EVENT_S_UPPER_LIMIT_EXCEED_DELAY    0x05
#define EVENT_S_UPPER_EXTREME_LIMIT_EXCEED  0x06

#define EVENT_S_SENSOR_ERROR                0x01
#define EVENT_S_SENSOR_ERROR_LOWER_SCALE    0x02
#define EVENT_S_SENSOR_ERROR_UPPER_SCALE    0x03
#define EVENT_S_SENSOR_ERROR_DISCONNECT     0x04
#define EVENT_S_SENSOR_ERROR_UNKNOW         0x05

#endif 