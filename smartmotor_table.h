/*
 * smartmotor_table.h
 *
 *  Created on: 01/dic/2015
 *      Author: luca
 */

#include "CANOpenShellStateMachine.h"

#ifndef SMARTMOTOR_TABLE_H_
#define SMARTMOTOR_TABLE_H_

void smartmotor_get_free(int nodeid, UNS32 *interp_status);
void smartmotor_table_reset(int nodeid, UNS32 *interp_status);
void smartmotor_table_write(int nodeid, UNS32 *interp_status, long point,
    long time_value, long time_period);
void smartmotor_table_read(int nodeid, UNS32 *interp_status, long *point);

void smartmotor_path_reset(int nodeid, UNS16 *motor_status);
int smartmotor_path_generate(int nodeid, int encoder_count, long start_step,
    long stop_step, long velocity, long acceleration);
void smartmotor_path_read(int nodeid, UNS16 *motor_status, long *position);
void smartmotor_path_lock();
void smartmotor_path_unlock();



#endif /* SMARTMOTOR_TABLE_H_ */
