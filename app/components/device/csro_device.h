#ifndef CSRO_DEVICE_H_
#define CSRO_DEVICE_H_

#include "csro_common.h"

typedef struct
{
    uint8_t index;
    uint8_t pin_num;

    uint8_t state;

    uint8_t on_source;
    timer_t on_tim;
    
    uint8_t off_source;
    time_t  off_tim;
} csro_switch;

typedef struct
{
    uint8_t state;
    uint8_t holdtime;
} csro_button;



void csro_device_prepare_basic_message(void);
void csro_device_prepare_timer_message(void);
void csro_device_handle_self_message(MessageData* data);
void csro_device_handle_group_message(MessageData* data);
void csro_device_alarm_action(uint16_t action);
void csro_device_init(void);


void csro_nlight_prepare_basic_message(void);
void csro_nlight_prepare_timer_message(void);
void csro_nlight_handle_self_message(MessageData *data);
void csro_nlight_alarm_action(uint16_t action);
void csro_nlight_init(void);


void csro_dlight_prepare_basic_message(void);
void csro_dlight_prepare_timer_message(void);
void csro_dlight_handle_self_message(MessageData *data);
void csro_dlight_alarm_action(uint16_t action);
void csro_dlight_init(void);


void csro_motor_prepare_basic_message(void);
void csro_motor_prepare_timer_message(void);
void csro_motor_handle_self_message(MessageData *data);
void csro_motor_alarm_action(uint16_t action);
void csro_motor_init(void);


void csro_air_monitor_prepare_basic_message(void);
void csro_air_monitor_prepare_timer_message(void);
void csro_air_monitor_handle_self_message(MessageData* data);
void csro_air_monitor_alarm_action(uint16_t action);
void csro_air_monitor_init(void);


void csro_air_system_prepare_basic_message(void);
void csro_air_system_prepare_timer_message(void);
void csro_air_system_handle_self_message(MessageData* data);
void csro_air_system_alarm_action(uint16_t action);
void csro_air_system_init(void);


#endif