/*
 * smartmotor_table.c
 *
 *  Created on: 01/dic/2015
 *      Author: luca
 */

#include "smartmotor_table.h"
#include "CANOpenShellMasterError.h"
#include <math.h>

#define CANOPEN_NODE_NUMBER 128
#define SMARTMOTOR_TABLE_SIZE 45
#define PATH_MAX_SIZE 525000

unsigned char smartmotor_table_ptr_wr[CANOPEN_NODE_NUMBER];
unsigned char smartmotor_table_ptr_rd[CANOPEN_NODE_NUMBER];
unsigned char smartmotor_table_count[CANOPEN_NODE_NUMBER];

long table_point[CANOPEN_NODE_NUMBER][SMARTMOTOR_TABLE_SIZE];

unsigned int smartmotor_path_ptr_rd[CANOPEN_NODE_NUMBER];
unsigned int smartmotor_path_count[CANOPEN_NODE_NUMBER];

long path_point[CANOPEN_NODE_NUMBER][PATH_MAX_SIZE];
pthread_mutex_t buffer_mux[CANOPEN_NODE_NUMBER];

void smartmotor_get_free(int nodeid, UNS32 *interp_status)
{
  *interp_status = SMARTMOTOR_TABLE_SIZE - smartmotor_table_count[nodeid];
}

void smartmotor_table_reset(int nodeid, UNS32 *interp_status)
{
  pthread_mutex_lock(&buffer_mux[nodeid]);
  smartmotor_table_ptr_wr[nodeid] = 0;
  smartmotor_table_ptr_rd[nodeid] = 0;
  smartmotor_table_count[nodeid] = 0;
  *interp_status &= 0b0111111111000000;
  *interp_status += SMARTMOTOR_TABLE_SIZE;
  pthread_mutex_unlock(&buffer_mux[nodeid]);
}

void smartmotor_table_write(int nodeid, UNS32 *interp_status, long point,
    long time_value, long time_period)
{
  if((time_value == 0) && (time_period == 0))
  {
    // se il punto passato è uguale al precendente, allora posso chiudere
    // la tabella dell'interpolatore
    if(point == table_point[nodeid][smartmotor_table_ptr_wr[nodeid] - 1])
    {
      //printf("close table\n");
      pthread_mutex_lock(&buffer_mux[nodeid]);
      *interp_status &= 0b0111111111111111;
      pthread_mutex_unlock(&buffer_mux[nodeid]);
    }
    else
    {
      // altrimenti si tratta di un errore nella scala tempi
      //printf("[%d] errore nella scala dei tempi\n", nodeid);
      pthread_mutex_lock(&buffer_mux[nodeid]);
      *interp_status |= 0b0000010000000000;
      pthread_mutex_unlock(&buffer_mux[nodeid]);
    }
    return;
  }

  // in caso di overflow, aggiorno lo stato del registro
  if(smartmotor_table_count[nodeid] > SMARTMOTOR_TABLE_SIZE)
  {
    pthread_mutex_lock(&buffer_mux[nodeid]);
    *interp_status |= 0b0010000000000000;
    pthread_mutex_unlock(&buffer_mux[nodeid]);
    return;
  }

  table_point[nodeid][smartmotor_table_ptr_wr[nodeid]] = point;

  pthread_mutex_lock(&buffer_mux[nodeid]);
  smartmotor_table_ptr_wr[nodeid]++;

  if(smartmotor_table_ptr_wr[nodeid] == SMARTMOTOR_TABLE_SIZE)
    smartmotor_table_ptr_wr[nodeid] = 0;

  smartmotor_table_count[nodeid]++;
  *interp_status -= 1;

  pthread_mutex_unlock(&buffer_mux[nodeid]);
}

void smartmotor_table_read(int nodeid, UNS32 *interp_status, long *point)
{
  if(smartmotor_table_count[nodeid] == 0)
  {
    // controllo underflow
    pthread_mutex_lock(&buffer_mux[nodeid]);
    *interp_status |= 0b0100000000000000;
    pthread_mutex_unlock(&buffer_mux[nodeid]);
    return;
  }

  *point = table_point[nodeid][smartmotor_table_ptr_rd[nodeid]];

  pthread_mutex_lock(&buffer_mux[nodeid]);
  smartmotor_table_ptr_rd[nodeid]++;

  if(smartmotor_table_ptr_rd[nodeid] == SMARTMOTOR_TABLE_SIZE)
    smartmotor_table_ptr_rd[nodeid] = 0;

  smartmotor_table_count[nodeid]--;
  *interp_status += 1;

  // controllo overflow
  if((*interp_status & 0x3f) > SMARTMOTOR_TABLE_SIZE)
  {
    *interp_status |= 0b0010000000000000;
  }

  pthread_mutex_unlock(&buffer_mux[nodeid]);
}

void smartmotor_path_reset(int nodeid, UNS16 *motor_status)
{
  smartmotor_path_count[nodeid] = 0;
  smartmotor_path_ptr_rd[nodeid] = 0;

  *motor_status &= 0b1111101111111111;

  //printf("[%d]reset motor status %d\n", nodeid, *motor_status);
}

void smartmotor_path_read(int nodeid, UNS16 *motor_status, long *position)
{
  if(smartmotor_path_count[nodeid] == 0)
  {
    //printf("[%d] exit due trajectory finished\n", nodeid);
    *motor_status |= 0b0001010000000000;
    return;
  }

  *position = path_point[nodeid][smartmotor_path_ptr_rd[nodeid]];
  //printf("[%d] position: %ld\n", nodeid, *position);
  smartmotor_path_ptr_rd[nodeid]++;
  smartmotor_path_count[nodeid]--;

  if(smartmotor_path_count[nodeid] == 0)
  {
    //printf("[%d] Trajectory finish\n", nodeid);
    *motor_status |= 0b0001010000000000;
  }

  return;
}

int smartmotor_path_generate(int nodeid, int encoder_count, long start_step,
    long stop_step, long velocity, long acceleration)
{
  double vel_step_per_sec = velocity * 8000.0 / 65536;
  double acc_step_per_sec_sec = acceleration * 8000.0 / 8.192;

  //printf("[%d] vel %f acc %f\n", nodeid, vel_step_per_sec, acc_step_per_sec_sec);
  double acc_time;
  double vel_time;

  int dir;
  int index;

  const float time_step = 0.01;
  long last_position;
  long last_vel;

  double time_approx;
  double acc_final;

  if(acc_step_per_sec_sec <= 0)
    return -1;

  acc_time = sqrt(fabs(start_step - stop_step) / acc_step_per_sec_sec);

  if(acc_time <= 0)
  {
    //printf("exit for [%d] due acceleration time too small %f\n", nodeid, acc_time) ;

    return -1;
  }


  if(vel_step_per_sec < (acc_step_per_sec_sec * acc_time))
  {
    // tratto a velocità costante
    acc_time = vel_step_per_sec / acc_step_per_sec_sec;
    vel_time = (fabs(stop_step - start_step) - acc_step_per_sec_sec * pow(acc_time, 2)) / (acc_step_per_sec_sec * acc_time);
  }
  else // senza velocità costante
    vel_time = 0;

  if(stop_step > start_step)
    dir = 1;
  else
    dir = -1;

  time_approx = round((2 * acc_time + vel_time) / time_step + 0.5) * time_step;

  if((time_approx / time_step) > PATH_MAX_SIZE)
  {
    //printf("Too much point for [%d]. I can't handle %f\n", nodeid, (time_approx / time_step));
    return -1;
  }

  // rampa di velocità iniziale
  for(index = 0; index < (acc_time / time_step); index++)
  {
    path_point[nodeid][index] = start_step + dir * 0.5 * acc_step_per_sec_sec * pow(index * time_step, 2);
  }

  // tratto a velocità costante
  last_position = path_point[nodeid][index - 1];
  last_vel = dir * acc_step_per_sec_sec * (index - 1) * time_step;

  for(index = ((acc_time + time_step) / time_step); index < ((acc_time + vel_time) / time_step); index++)
  {
    path_point[nodeid][index] = last_position + last_vel * ((index * time_step) - acc_time);
  }

  // decellerazione costante
  last_position = path_point[nodeid][index - 1];

  // ricalcolo della decellerazione per arrivare esattamente al punto desiderato

  acc_final = fabs(2 * ((stop_step - last_position) - last_vel * (time_approx - (acc_time + vel_time)))) / pow((time_approx - (acc_time + vel_time)), 2);


  for(index = ((acc_time + vel_time + time_step) / time_step); index < (time_approx / time_step); index++)
  {
    path_point[nodeid][index] = last_position + last_vel * ((index * time_step) - acc_time - vel_time) - dir * 0.5 * acc_final * pow((index * time_step) - acc_time - vel_time, 2);
  }

  path_point[nodeid][index] = stop_step;

  smartmotor_path_count[nodeid] = index + 1;
  //printf("generated %d points [%d]\n", smartmotor_path_count[nodeid], nodeid);

  return 0;
}
