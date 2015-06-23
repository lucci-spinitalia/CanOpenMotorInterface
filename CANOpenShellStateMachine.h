/*
 * state_machine.h
 *
 *  Created on: 12/mag/2015
 *      Author: luca
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include "canfestival.h"

#define CANOPEN_NODE_NUMBER 128 // 127 nodi più quello di broadcast
struct state_machine_struct
{
  void **function;
  int function_size;
  UNS32 *param;
  int param_size;
  char **error;
};

pthread_mutex_t machine_mux[CANOPEN_NODE_NUMBER];
extern int motor_active[CANOPEN_NODE_NUMBER];
extern int motor_homing[CANOPEN_NODE_NUMBER];
extern volatile int motor_started[CANOPEN_NODE_NUMBER];
extern long motor_position[CANOPEN_NODE_NUMBER];

extern void **sin_interpolation_function;
extern UNS32 *sin_interpolation_param;

extern struct state_machine_struct sin_interpolation_machine;

extern void **sin_final_function;
extern UNS32 *sin_final_param;

extern struct state_machine_struct sin_final_machine;

extern struct state_machine_struct smart_start_machine;
extern struct state_machine_struct map1_pdo_machine;
extern struct state_machine_struct map2_pdo_machine;
extern struct state_machine_struct map3_pdo_machine;
extern struct state_machine_struct heart_start_machine;
extern struct state_machine_struct smart_stop_machine;
extern struct state_machine_struct smart_position_zero_machine;
extern struct state_machine_struct smart_interpolation_test1_machine;
extern struct state_machine_struct smart_interpolation_test2_machine;
extern struct state_machine_struct stop_interpolation_machine;
extern struct state_machine_struct start_interpolation_machine;
extern struct state_machine_struct init_interpolation_machine;
extern struct state_machine_struct smart_homing_machine;
extern struct state_machine_struct smart_statusword_machine;
extern struct state_machine_struct smart_reset_statusword_machine;
extern struct state_machine_struct smart_origin_machine;
extern struct state_machine_struct resume_interpolation_machine;

typedef UNS8 (*writeNetworkDictCallBack_t)(CO_Data* d, UNS8 nodeId, UNS16 index,
    UNS8 subIndex, UNS32 count, UNS8 dataType, void *data,
    SDOCallback_t Callback, UNS8 useBlockMode);

typedef UNS8 (*readNetworkDictCallback_t)(CO_Data* d, UNS8 nodeId, UNS16 index,
    UNS8 subIndex, UNS8 dataType, SDOCallback_t Callback, UNS8 useBlockMode);

typedef void (*masterSendNMTstateChange_t)(CO_Data* d, UNS8 Node_ID, UNS8 cs);

typedef void (*CustomFunction_t)(CO_Data* d, UNS8 Node_ID, UNS32 *param_array,
    UNS32 param_array_size);

typedef void (*MachineCallback_t)(CO_Data* d, UNS8 Node_ID, int machine_state,
    UNS32 return_value);

void _machine_init();
void _machine_destroy();
void _machine_reset(CO_Data* d, UNS8 nodeId);

int _machine_exe(CO_Data *d, UNS8 nodeId, MachineCallback_t machine_callback,
    struct state_machine_struct *machine[], int callback_num, int from_callback,
    int var_count, ...);

#endif /* STATE_MACHINE_H_ */
