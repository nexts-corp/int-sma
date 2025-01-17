#ifndef EVENT_H
#define EVENT_H

#include "main.h"

//Sensor Input
#define SENSOR_EVENT_def                    0x00

#define SENSOR1_ANY_EVENT_def               0x10
#define SENSOR1_LOWER_EVENT_def             0x11
#define SENSOR1_LOWER_EXTREME_EVENT_def    0x12
#define SENSOR1_UPPER_EVENT_def             0x13
#define SENSOR1_UPPERY_EXTREME_EVENT_def    0x14
#define SENSOR1_ERROR_EVENT_def             0x15

#define SENSOR2_ANY_EVENT_def               0x20
#define SENSOR2_LOWER_EVENT_def             0x21
#define SENSOR2_LOWER_EXTREME_EVENT_def    0x22
#define SENSOR2_UPPER_EVENT_def             0x23
#define SENSOR2_UPPERY_EXTREME_EVENT_def    0x24
#define SENSOR2_ERROR_EVENT_def             0x25

#define SENSOR3_ANY_EVENT_def               0x30
#define SENSOR3_LOWER_EVENT_def             0x31
#define SENSOR3_LOWER_EXTREME_EVENT_def    0x32
#define SENSOR3_UPPER_EVENT_def             0x33
#define SENSOR3_UPPERY_EXTREME_EVENT_def    0x34
#define SENSOR3_ERROR_EVENT_def             0x35

#define SENSOR4_ANY_EVENT_def               0x40
#define SENSOR4_LOWER_EVENT_def             0x41
#define SENSOR4_LOWER_EXTREME_EVENT_def    0x42
#define SENSOR4_UPPER_EVENT_def             0x43
#define SENSOR4_UPPERY_EXTREME_EVENT_def    0x44
#define SENSOR4_ERROR_EVENT_def             0x45

#define SENSOR5_ANY_EVENT_def               0x50
#define SENSOR5_LOWER_EVENT_def             0x51
#define SENSOR5_LOWER_EXTREME_EVENT_def    0x52
#define SENSOR5_UPPER_EVENT_def             0x53
#define SENSOR5_UPPERY_EXTREME_EVENT_def    0x54
#define SENSOR5_ERROR_EVENT_def             0x55

#define SENSOR6_ANY_EVENT_EVENT_def         0x60
#define SENSOR6_LOWER_EVENT_def             0x61
#define SENSOR6_LOWER_EXTREME_EVENT_def    0x62
#define SENSOR6_UPPER_EVENT_def             0x63
#define SENSOR6_UPPERY_EXTREME_EVENT_def    0x64
#define SENSOR6_ERROR_EVENT_def             0x65

#define SENSOR7_ANY_EVENT_def               0x70
#define SENSOR7_LOWER_EVENT_def             0x71
#define SENSOR7_LOWER_EXTREME_EVENT_def    0x72
#define SENSOR7_UPPER_EVENT_def             0x73
#define SENSOR7_UPPERY_EXTREME_EVENT_def    0x74
#define SENSOR7_ERROR_EVENT_def             0x75


//Digital Input
#define DI_ANY_EVENT_def                    0x01
#define DI_ANY_RISING_EDGE_EVENT_def        0x02
#define DI_ANY_FALLING_EDGE_EVENT_def       0x03

#define DI1_EVENT_def                       0x80
#define DI1_RISING_EDGE_EVENT_def           0x81
#define DI1_FALLING_EDGE_EVENT_def          0x82

#define DI2_EVENT_def                       0x83
#define DI2_RISING_EDGE_EVENT_def           0x84
#define DI2_FALLING_EDGE_EVENT_def          0x85

#define DI3_EVENT_def                       0x86
#define DI3_RISING_EDGE_EVENT_def           0x87
#define DI3_FALLING_EDGE_EVENT_def          0x88

#define DI4_EVENT_def                       0x89
#define DI4_RISING_EDGE_EVENT_def           0x8a
#define DI4_FALLING_EDGE_EVENT_def          0x8b

#define DI5_EVENT_def                       0x90
#define DI5_RISING_EDGE_EVENT_def           0x91
#define DI5_FALLING_EDGE_EVENT_def          0x92

#define DI6_EVENT_def                       0x93
#define DI6_RISING_EDGE_EVENT_def           0x94
#define DI6_FALLING_EDGE_EVENT_def          0x95

#define DI7_EVENT_def                       0x96
#define DI7_RISING_EDGE_EVENT_def           0x97
#define DI7_FALLING_EDGE_EVENT_def          0x98

#define DI8_EVENT_def                       0x99
#define DI8_RISING_EDGE_EVENT_def           0x9a
#define DI8_FALLING_EDGE_EVENT_def          0x9b


//Analog Input
#define AI_ANY_EVENT_def                    0x04
#define AI_ANY_HIGHER_EVENT_def             0x05
#define AI_ANY_LOWER_EDGE_EVENT_def         0x06

#define AI1_EVENT_def                       0xa0
#define AI1_HIGHER_EVENT_def                0xa1
#define AI1_LOWER_EVENT_def                 0xa2

#define AI2_EVENT_def                       0xa3
#define AI2_HIGHER_EVENT_def                0xa4
#define AI2_LOWER_EVENT_def                 0xa5

#define AI3_EVENT_def                       0xa6
#define AI3_HIGHER_EVENT_def                0xa7
#define AI3_LOWER_EVENT_def                 0xa8

#define AI4_EVENT_def                       0xa9
#define AI4_HIGHER_EVENT_def                0xaa
#define AI4_LOWER_EVENT_def                 0xab

#define AI5_EVENT_def                       0xb0
#define AI5_HIGHER_EVENT_def                0xb1
#define AI5_LOWER_EVENT_def                 0xb2

#define AI6_EVENT_def                       0xb3
#define AI6_HIGHER_EVENT_def                0xb4
#define AI6_LOWER_EVENT_def                 0xb5

#define AI7_EVENT_def                       0xb6
#define AI7_HIGHER_EVENT_def                0xb7
#define AI7_LOWER_EVENT_def                 0xb8

#define AI8_EVENT_def                       0xb9
#define AI8_HIGHER_EVENT_def                0xba
#define AI8_LOWER_EVENT_def                 0xbb


//Other
#define MAIN_POWER_DOWN_EVENT_def           0xe0
#define LOW_BATTERY_EVENT_def               0xe1
#define LOW_MEMORY_def                      0xe2
#define LAN_LOSS_COMM_def                   0xe3
#define LAN_UNKNOW_FAILURE_def              0xe4
#define WIFI_LOSS_COMM_def                  0xe5
#define WIFI_UNKNOW_FAILURE_def             0xe6
#define WIFI_LOW_SIGNAL_def                 0xe7
#define GSM_LOW_SIGNAL_def                  0xe8
#define GSM_DATE_EXPIRED_def                0xe9
#define GSM_NO_OPERATOR_def                 0xea
#define GSM_TOP_UP_def                      0xeb
#define GSM_LOW_CREDIT_BALANCE_def          0xec
#define GSM_UNKNOW_FAILURE_def              0xed

iChar_t iEventRead(iDataMini_t *pviOutData_arg);
void iPrintEventByCase(iChar_t viEventID_arg);
void iTestSetValueSensorConfig();

#endif 