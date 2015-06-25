/*
 This file is part of CanFestival, a library implementing CanOpen Stack.

 Copyright (C): Edouard TISSERANT and Francis DUPIN

 See COPYING file for copyrights details.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if defined(WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#define CLEARSCREEN "cls"
#define SLEEP(time) Sleep(time * 1000)
#else
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define CLEARSCREEN "clear"
#define SLEEP(time) sleep(time)
#endif

//****************************************************************************
// INCLUDES
#include <stdarg.h>
#include <signal.h>
#include <sys/stat.h>
#include <math.h>
#include "canfestival.h"
#include "CANOpenShell.h"
#include "CANOpenShellMasterOD.h"
#include "CANOpenShellStateMachine.h"
#include "CANOpenShellMasterError.h"
#include "file_parser.h"
#include "utils.h"

//****************************************************************************
// DEFINES
#define MOTOR_INDEX_FIRST 0x77
#define POSITION_FIFO_FILE "SWP.33.05.02.0.0/positions"

#define TABLE_MAX_NUM 6
#define cst_str4(c1, c2, c3, c4) ((((unsigned int)0 | \
    (char)c4 << 8) | \
    (char)c3) << 8 | \
    (char)c2) << 8 | \
    (char)c1

#define INIT_ERR 2
#define QUIT 1

void CheckReadRaw(CO_Data* d, UNS8 nodeid);
void CheckReadStringRaw(CO_Data* d, UNS8 nodeid);
void CheckWriteRaw(CO_Data* d, UNS8 nodeid);
int SmartWriteRaw(CO_Data* d, UNS8 nodeid);

void CheckWriteProgram(CO_Data* d, UNS8 nodeid);
void CheckReadProgram(CO_Data* d, UNS8 nodeid);
UNS32 OnInterpUpdate(CO_Data* d, UNS8 nodeid);
void SmartStop(UNS8 nodeid, int from_callback);
void SmartSin1Stop(UNS8 nodeid);
void SimulationStop(UNS8 nodeid);
void SmartSin1(CO_Data* d, UNS8 nodeid, int angle_step_grad, long amplitude,
    int point_number, int from_callback);
void SimulationTableUpdate(CO_Data* d, UNS8 nodeid, int point_number,
    int from_callback);

//****************************************************************************
// GLOBALS

char BoardBusName[31];
char BoardBaudRate[5];
s_BOARD Board =
    {BoardBusName, BoardBaudRate};
CO_Data* CANOpenShellOD_Data;

pthread_mutex_t sinwave_mux[CANOPEN_NODE_NUMBER];
volatile int sinwave_busy[CANOPEN_NODE_NUMBER];

int raw_response = -1;
int raw_report_flag = 0;
char raw_report[100];
char raw_cmd[100];

char program_file_path[100];

char LibraryPath[512];
e_nodeState node_state;

int machine_state = -1;
float angle_actual_rad[CANOPEN_NODE_NUMBER];
float angle_step_rad[CANOPEN_NODE_NUMBER];

struct table_data motor_table[TABLE_MAX_NUM];

static int simulation_first_start[CANOPEN_NODE_NUMBER];

FILE *position_fp = NULL;

/**
 * @return: -1: errore, chunk_size: ok, <chunk_size: ultimi byte e fine del file
 */
int program_file_read(const char *file_path, char *program_chunk,
    int chunk_size)
{
  FILE *file = NULL;
  ssize_t read;
  static int cursor_position = 0;

  memset(program_chunk, '\0', chunk_size);
  file = fopen(file_path, "r");

  if(file == NULL)
    return -1;

  if(fseek(file, cursor_position, SEEK_SET) == -1)
  {
    fclose(file);
    return -1;
  }

  // I caratteri che determinano la fine della stringa sono: " ", "\t", "\n", "'", "\r", "\a"
  read = fread(program_chunk, 1, chunk_size, file);

  cursor_position += read;

  fclose(file);

  return read;
}

void SmartFaultCallback(CO_Data* d, UNS8 nodeId, int machine_state,
    UNS32 return_value)
{
  UNS8 nodeid = NodeId;

  if((return_value & 0b0000000000001000) > 0)
    printf("ERR[%d on node %x]: Servo bus voltage fault\n", SmartMotorError,
        nodeid);

  if((return_value & 0b0000000000010000) > 0)
    printf("ERR[%d on node %x]: Peak over-current occurred\n", SmartMotorError,
        nodeid);

  if((return_value & 0b0000000000100000) > 0)
    printf("ERR[%d on node %x]: Excessive temperature\n", SmartMotorError,
        nodeid);

  if((return_value & 0b0000000001000000) > 0)
    printf("ERR[%d on node %x]: Excessive position error\n", SmartMotorError,
        nodeid);

  if((return_value & 0b0000000010000000) > 0)
    printf("ERR[%d on node %x]: Velocity limit\n", SmartMotorError, nodeid);

  if((return_value & 0b0000001000000000) > 0)
    printf(
        "ERR[%d on node %x]: First derivative (DE/Dt) of position error over limit\n",
        SmartMotorError, nodeid);

  if((return_value & 0b0001000000000000) > 0)
    printf("ERR[%d on node %x]: Right(+) over travel limit\n", SmartMotorError,
        nodeid);
  else if((return_value & 0b0010000000000000) > 0)
    printf("ERR[%d on node %x]: Left(-) over travel limit\n", SmartMotorError,
        nodeid);
}

UNS32 OnPositionUpdate(CO_Data* d, const indextable * indextable_curr,
    UNS8 bSubindex)
{
  char position_message[20];
  UNS8 nodeid = NodeId;

  if(Position_Actual_Value != motor_position[nodeid])
  {
    motor_position[nodeid] = Position_Actual_Value;

    if(position_fp != NULL)
    {
      sprintf(position_message, "%d %li\n", nodeid, motor_position[nodeid]);

      fputs(position_message, position_fp);
      fflush(position_fp);
    }
  }

  return 0;
}

UNS32 OnStatusUpdate(CO_Data* d, const indextable * indextable_curr,
    UNS8 bSubindex)
{
  UNS8 nodeid = NodeId;
  UNS16 statusword = Statusword;

  /** Fault management **/
  if((statusword & 0b0000000001001111) == 0b0000000000001000)
  {
    if(motor_homing[nodeid] == 1)
      motor_homing[nodeid] = 0;
    else
      motor_started[nodeid] = 0;

    struct state_machine_struct *fault_machine[] =
        {&smart_statusword_machine, &smart_reset_statusword_machine};

    _machine_exe(CANOpenShellOD_Data, nodeid, &SmartFaultCallback,
        fault_machine, 2, 1, 0);
  }

  if((statusword & 0b0001010000000000) == 0b0001010000000000)
  {
    if(motor_homing[nodeid] == 1)
    {
      motor_homing[nodeid] = 0;

      struct state_machine_struct *origin_machine[] =
          {&smart_origin_machine};

      _machine_exe(CANOpenShellOD_Data, nodeid, NULL, origin_machine, 1, 1, 0);
    }
  }

  if((statusword & 0b0010000000000000) > 0)
  {
    printf("ERR[%d on node %x]: Move error\n", SmartMotorError, nodeid);

    if(motor_homing[nodeid] == 1)
      motor_homing[nodeid] = 0;
  }

  if((statusword & 0b0000000001101111) == 0b0000000000000111)
  {
    //UNS32 data = 0X80;
    printf("ERR[%d on node %x]: Quick stop active\n", SmartMotorError, nodeid);

    _machine_reset(d, nodeid);

    //writeNetworkDictCallBack(d, NodeId, 0x6040, 0x0, 2, 0, &data, NULL, 1);
    SmartStop(nodeid, 1);
  }

  if((Interpolation_Mode_Status & 0b0000000001000000) > 0)
    printf("ERR[%d on node %x]: Position error tolerance exceeded (IP mode)\n",
        SmartMotorError, nodeid);

  if((Interpolation_Mode_Status & 0b0000010000000000) > 0)
    printf("ERR[%d on node %x]: Invalid time units (IP mode)\n",
        SmartMotorError, nodeid);

  if((Interpolation_Mode_Status & 0b0000100000000000) > 0)
    printf("ERR[%d on node %x]: Invalid position increment (IP mode)\n",
        SmartMotorError, nodeid);

  if((Interpolation_Mode_Status & 0b0000100000000000) > 0)
    printf("ERR[%d on node %x]: Invalid position increment error (IP mode)\n",
        SmartMotorError,
        nodeid);

  if((Interpolation_Mode_Status & 0b0100000000000000) > 0)
    printf("ERR[%d on node %x]: FIFO underflow (IP mode)\n", SmartMotorError,
        nodeid);

  if((Interpolation_Mode_Status & 0b0010000000000000) > 0)
    printf("ERR[%d on node %x]: FIFO overflow (IP mode)\n", SmartMotorError,
        nodeid);
  else
    OnInterpUpdate(d, nodeid);

  return 0;
}

UNS32 OnInterpUpdate(CO_Data* d, UNS8 nodeid)
{
  static UNS32 smart_motor_status[CANOPEN_NODE_NUMBER];

  if(smart_motor_status[nodeid] != 0)
  {
    if((smart_motor_status[nodeid] & 0b1000000000000000)
        < (Interpolation_Mode_Status & 0b1000000000000000))
      printf("INF[%d on node %x]: Interpolation started (sin IP mode)\n",
          SmartMotorError, nodeid);
    else if((smart_motor_status[nodeid] & 0b1000000000000000)
        > (Interpolation_Mode_Status & 0b1000000000000000))
    {
      printf("INF[%d on node %x]: Interpolation finished (sin IP mode)\n",
          SmartMotorError, nodeid);

      smart_motor_status[nodeid] = Interpolation_Mode_Status;

      //SmartSin1Stop(NodeId);
      SimulationStop(nodeid);

      return 0;
    }
  }

  smart_motor_status[nodeid] = Interpolation_Mode_Status;

  if((motor_started[nodeid] == 1)
  //&& ((Interpolation_Mode_Status & 0x3F) > 0x16)
  )
  {
    //SmartSin1(d, NodeId, 0, 0, Interpolation_Mode_Status & 0x3F, 1);
    SimulationTableUpdate(d, nodeid, Interpolation_Mode_Status & 0x3F, 1);
  }

  return 0;
}

void SmartUpdateSin1Callback(CO_Data* d, UNS8 nodeid, int machine_state,
    UNS32 return_value)
{
  int i;
  pthread_mutex_lock(&sinwave_mux[nodeid]);
  for(i = 0; i < machine_state; i++)
  {
    angle_actual_rad[nodeid] += angle_step_rad[nodeid];
    angle_actual_rad[nodeid] = fmod(angle_actual_rad[nodeid], 2 * M_PI);
  }

  sinwave_busy[nodeid] = 0;
  pthread_mutex_unlock(&sinwave_mux[nodeid]);
}

void SmartSin1Callback(CO_Data* d, UNS8 nodeid, int machine_state,
    UNS32 return_value)
{
  pthread_mutex_lock(&sinwave_mux[nodeid]);
  sinwave_busy[nodeid] = 0;
  pthread_mutex_unlock(&sinwave_mux[nodeid]);
}

int MotorTableIndexFromNodeId(UNS8 nodeId)
{
  int i;

  for(i = 0; i < TABLE_MAX_NUM; i++)
  {
    if(motor_table[i].nodeId == nodeId)
    {
      return i;
    }
  }

  return -1;
}

void SimulationInitCallback(CO_Data* d, UNS8 nodeid, int machine_state,
    UNS32 return_value)
{
  motor_started[nodeid] = 1;
}

/**
 * param:
 *   angle_step_grad
 *   point number
 */
void SimulationTableUpdate(CO_Data* d, UNS8 nodeid, int point_number,
    int from_callback)
{
  int motor_table_index = MotorTableIndexFromNodeId(nodeid);

  if(motor_table_index < 0)
  {
    printf("ERR[%d on node %x]: Impossibile trovare la tabella associata. \n",
        InternalError, nodeid);

    return;
  }

  if(motor_active[nodeid] == 0)
  {
    return;
  }

  pthread_mutex_lock(&motor_table[motor_table_index].table_mutex);
  pthread_mutex_lock(&sinwave_mux[nodeid]);
  if((sinwave_busy[nodeid] == 0) && (point_number > 0))
    sinwave_busy[nodeid] = 1;
  else
  {
    if(sinwave_busy[nodeid] == 1)
      printf("sinwave busy %d\n", nodeid);

    pthread_mutex_unlock(&sinwave_mux[nodeid]);
    pthread_mutex_unlock(&motor_table[motor_table_index].table_mutex);

    return;
  }

  pthread_mutex_unlock(&sinwave_mux[nodeid]);

  int i;
  int point_to_send;
  struct table_data_read data_read;

  if(point_number > motor_table[motor_table_index].count)
    point_to_send = motor_table[motor_table_index].count;
  else
    point_to_send = point_number - 1; // mi riservo uno spazio per l'eventuale uscata dall'ip mode

  for(i = 0; i < point_to_send; i++)
  {
    if(QueueGet(&motor_table[motor_table_index], &data_read, i) == -1)
      continue;

    InterpolationTimePeriod[nodeid - MOTOR_INDEX_FIRST] = -3;

    while(data_read.time_ms > 256)
    {
      data_read.time_ms = data_read.time_ms / 10;

      InterpolationTimePeriod[nodeid - MOTOR_INDEX_FIRST]++;
    }

    InterpolationTimeValue[nodeid - MOTOR_INDEX_FIRST] = data_read.time_ms;

    InterpolationData[nodeid - MOTOR_INDEX_FIRST] = data_read.position;

    //printf("[%d]: %li\n", nodeid, InterpolationData[nodeid - MOTOR_INDEX_FIRST]);

    sendPDOevent(d);
  }

  // per uscire dalla modalità ip mode devo impostare l'unità temporale a zero
  // e scrivere l'ultimo valore nella tabella.
  // Devo utilizzare l'sdo perchè altrimenti il dato non partirebbe in quanto è uguale
  // a quello precendente

  if((motor_started[nodeid] == 1)
      && ((Interpolation_Mode_Status & 0b1000000000000000) == 0))
  {
    struct state_machine_struct *interpolation_machine[] =
        {&start_interpolation_machine};

    _machine_exe(CANOpenShellOD_Data, nodeid, NULL,
        interpolation_machine, 1, 1, 0);
  }

  // Se il motore sta elaborando la tabella e il riempitore di tabella ha finito i punti, significa che posso bloccare il movimento
  if(((Interpolation_Mode_Status & 0b1000000000000000) > 0)
      //&& ((Interpolation_Mode_Status & 0x3F) == 45)
      && (motor_table[motor_table_index].count == 0)
      && (motor_table[motor_table_index].end_reached == 1))
  {
    QueueLast(&motor_table[motor_table_index], &data_read);

    struct state_machine_struct *interpolation_machine[] =
        {&stop_interpolation_machine};

    _machine_exe(CANOpenShellOD_Data, nodeid, NULL,
        interpolation_machine, 1, 1, 1, data_read.position);

    motor_started[nodeid] = 0;
  }

  pthread_mutex_unlock(&motor_table[motor_table_index].table_mutex);

  QueueUpdate(&motor_table[motor_table_index], point_to_send);

  pthread_mutex_lock(&sinwave_mux[nodeid]);
  sinwave_busy[nodeid] = 0;
  pthread_mutex_unlock(&sinwave_mux[nodeid]);

}

/**
 * param:
 *   angle_step_grad
 *   point number
 */
void SmartSin1(CO_Data* d, UNS8 nodeid, int angle_step_grad, long amplitude,
    int point_number, int from_callback)
{
  static struct timeval start_delay[CANOPEN_NODE_NUMBER],
      stop_delay[CANOPEN_NODE_NUMBER];

  static long int position_max = 0;
  float current_angle = 0;

  if(((motor_started[nodeid] == 0) && (angle_step_grad == 0))
      || (motor_active[nodeid] == 0))
  {
    if(motor_started[nodeid])
    {
      printf("motor stopped\n");
      motor_started[nodeid] = 0;
      angle_step_rad[nodeid] = 0;
    }

    return;
  }

  pthread_mutex_lock(&sinwave_mux[nodeid]);
  if((sinwave_busy[nodeid] == 0) && (point_number > 0))
    sinwave_busy[nodeid] = 1;
  else
  {
    pthread_mutex_unlock(&sinwave_mux[nodeid]);
    if(sinwave_busy[nodeid] == 1)
    {
      if(start_delay[nodeid].tv_usec == 0)
      {
        if(gettimeofday(&start_delay[nodeid], NULL))
        {
          perror("gettimeofday()");
        }
      }

      printf("sinwave busy %d\n", nodeid);
    }

    return;
  }

  if(start_delay[nodeid].tv_usec != 0)
  {
    if(gettimeofday(&stop_delay[nodeid], NULL))
    {
      perror("gettimeofday()");
    }

    printf("Time elapsed: %li us\n",
        (stop_delay[nodeid].tv_sec - start_delay[nodeid].tv_sec) * 1000000
            + stop_delay[nodeid].tv_usec
            - start_delay[nodeid].tv_usec);

    start_delay[nodeid].tv_usec = 0;
  }

  pthread_mutex_unlock(&sinwave_mux[nodeid]);

  if(angle_step_grad != 0)
  {
    angle_step_rad[nodeid] = angle_step_grad * M_PI / 180;
    angle_actual_rad[nodeid] = 0;
  }

  current_angle = angle_actual_rad[nodeid];

  if(amplitude != 0)
    position_max = amplitude;

  int i;

  if(sin_interpolation_function != NULL)
  {
    free(sin_interpolation_function);
    sin_interpolation_function = NULL;
  }

  if(sin_interpolation_param != NULL)
  {
    free(sin_interpolation_param);
    sin_interpolation_param = NULL;
  }

  if(point_number)
  {
    /*sin_interpolation_function = malloc(point_number * sizeof(void *));
     sin_interpolation_param = malloc(point_number * 5 * sizeof(UNS32));

     for(i = 0; i < point_number; i++)
     {
     sin_interpolation_function[i] = &writeNetworkDictCallBack; // Write data point
     sin_interpolation_param[i * 5] = 0x60C1;
     sin_interpolation_param[i * 5 + 1] = 0x1;
     sin_interpolation_param[i * 5 + 2] = 4;
     sin_interpolation_param[i * 5 + 3] = 0;
     sin_interpolation_param[i * 5 + 4] = (long) (position_max
     * sin(current_angle));

     current_angle += angle_step_rad[nodeid];
     current_angle = fmod(current_angle, 2 * M_PI);
     }

     sin_interpolation_machine.function = sin_interpolation_function;
     sin_interpolation_machine.function_size = point_number;
     sin_interpolation_machine.param = sin_interpolation_param;
     sin_interpolation_machine.param_size = point_number * 5;*/

    if(motor_started[nodeid] == 0)
    {
      struct state_machine_struct *interpolation_machine[] =
          {&init_interpolation_machine};

      if(_machine_exe(CANOpenShellOD_Data, nodeid, &SmartSin1Callback,
          interpolation_machine, 1, from_callback, 0) == 0)
        motor_started[nodeid] = 1;
    }
    else
    {
      /*struct state_machine_struct *interpolation_machine[] =
       {&sin_interpolation_machine};

       if(_machine_exe(CANOpenShellOD_Data, nodeid, &SmartUpdateSin1Callback,
       interpolation_machine, 1, from_callback, 0) == 1)
       SmartSin1Stop(nodeid);*/

      for(i = 0; i < point_number; i++)
      {
        InterpolationData[nodeid - MOTOR_INDEX_FIRST] = (long) (position_max
            * sin(angle_actual_rad[nodeid]));

        sendPDOevent(d);

        angle_actual_rad[nodeid] += angle_step_rad[nodeid];
        angle_actual_rad[nodeid] = fmod(angle_actual_rad[nodeid], 2 * M_PI);
      }

      sinwave_busy[nodeid] = 0;
    }
  }
}

void SimulationStop(UNS8 nodeid)
{
  int motor_table_index = MotorTableIndexFromNodeId(nodeid);

  QueueInit(nodeid, &motor_table[motor_table_index]);

  pthread_mutex_lock(&motor_table[motor_table_index].table_mutex);
  sinwave_busy[nodeid] = 0;
  pthread_mutex_unlock(&motor_table[motor_table_index].table_mutex);

  motor_started[nodeid] = 0;

  //SmartStop(nodeid, 1);
}

void SmartSin1Stop(UNS8 nodeid)
{
  pthread_mutex_lock(&sinwave_mux[nodeid]);
  sinwave_busy[nodeid] = 0;
  pthread_mutex_unlock(&sinwave_mux[nodeid]);

  motor_started[nodeid] = 0;
}

void SinFunction(char *sdo)
{
  int ret;

  int nodeid;
  int angle_step_grad = 0x00;
  int point_number = 0x00;
  long amplitude = 0;

  ret = sscanf(sdo, "sin1#%2x,%2x,%4lx,%2x", &nodeid, &angle_step_grad,
      &amplitude, &point_number);

  if(ret >= 4)
  {
    // Inizializzo i mutex
    int i;
    for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
    {
      pthread_mutex_init(&sinwave_mux[i], NULL);
    }

    SmartSin1(CANOpenShellOD_Data, nodeid, angle_step_grad, amplitude,
        point_number, 0);
  }
  else
    printf("Wrong command  : %s\n", sdo);

}

void SmartHome(char *sdo)
{
  int ret;

  int nodeid;
  long home_offset = 0;

  ret = sscanf(sdo, "shom#%2x,%4lx", &nodeid, &home_offset);

  if(ret >= 2)
  {
    if(nodeid == 0)
    {
      int i;
      for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
      {
        if(motor_active[nodeid])
          motor_homing[i] = 1;
      }
    }
    else
      motor_homing[nodeid] = 1;

    struct state_machine_struct *machine = &smart_homing_machine;
    _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 1,
        home_offset);
  }
  else
    printf("Wrong command  : %s\n", sdo);

}

/**
 * Scrive i punti di una sinusoide nel file del motore indicato.
 */
int GenerateSinData(int nodeid, long amplitude, long period, long tsamp,
    long num_of_period)
{
  float num_of_points = 0;
  float angle_step_rad = 0;
  float angle_rad = 0;

  long position = 0;
  long position_prev = 0;

  int i, j;

  if(tsamp <= 0)
  {
    printf(
        "ERR[%d on node %x]: Tempo campionamento non valido (GenerateSinData)\n",
        InternalError, nodeid);
    return -1;
  }

  num_of_points = period / tsamp;

  if(num_of_points <= 0)
  {
    printf(
        "ERR[%d on node %x]: Periodo dell'onda non valido (GenerateSinData)\n",
        InternalError, nodeid);

    return -1;
  }

  FILE *file = NULL;
  char file_temp[256];
  char file_destination[256];
  sprintf(file_temp, "%s%d.mot.temp", FILE_DIR, nodeid);

  file = fopen(file_temp, "w+");

  if(file == NULL)
  {
    perror("file");

    return -1;
  }

  angle_step_rad = 2 * M_PI / num_of_points;

  // genero il primo punto, assumendo che il motore si trova nell'origine
  fprintf(file, "%li 100\n", position);
  angle_rad += angle_step_rad;

  for(j = 0; j < num_of_period; j++)
  {
    for(i = 0; i < (num_of_points - 1); i++, angle_rad += angle_step_rad)
    {
      position = (long) (amplitude * sin(angle_rad));

      // devo assicurarmi di non richiedere una velocità al motore troppo alta
      // Se la velocità massima è di 30 giri/s, con un encoder da 4000 passi, equivarrà
      // a 120000 step/s, cioè 120 step/ms.
      // Quindi, la differenza tra il punto corrente e quello precedente divisa per il tempo
      // richiesto per percorrerla, deve essere minore di 120.

      if((labs(position - position_prev) / tsamp) >= 120)
      {
        printf(
            "ERR[%d on node %x]: Velocità richiesta troppo elevata (GenerateSinData)\n",
            InternalError, nodeid);
        fclose(file);
        return -1;
      }
      else
      {
        fprintf(file, "%li %li\n", position, tsamp);
        position_prev = position;
      }
    }
  }

  // l'ultimo punto sarà lo stesso iniziale
  //fprintf(file, "0 %li\n", tsamp);
  fclose(file);

  sprintf(file_destination, "%s%d.mot", FILE_DIR, nodeid);

  remove(file_destination);

  if(cp(file_destination, file_temp) == 0)
    remove(file_temp);
  else
    printf(
        "ERR[%d on node %x]: Impossibile copiare il file (GenerateSinData)\n",
        InternalError, nodeid);

  return 0;
}

void GenerateMotorData(char *sdo)
{
  int ret;

  int nodeid;
  char param[100];

  ret = sscanf(sdo, "sgen#%2x,%s", &nodeid, param);

  if(ret >= 2)
  {
    char *token = NULL;

    token = strtok(param, ",");

    if(token != NULL)
    {
      if(strcmp(token, "sin") == 0)
      {
        long param[4]; /* amplitude, period, tsamp, num of period */
        int param_count = 0;

        while(token != NULL)
        {
          token = strtok(NULL, ",\r\n\0");
          if(token != NULL)
          {
            sscanf(token, "%lx", &param[param_count]);
            param_count++;
          }
        }

        if(param_count < 4)
          goto fail;

        if(GenerateSinData(nodeid, param[0], param[1], param[2], param[3]) >= 0)
          printf("sin function generated\n");
        else
          goto fail;
      }
      else
        goto fail;
    }
  }
  else
    printf("Wrong command  : %s\n", sdo);

  return;

  fail:
  printf("Wrong command  : %s\n", sdo);

}

void SmartIntTest2(UNS8 nodeid)
{
  struct state_machine_struct *machine = &smart_interpolation_test2_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 0);
}

void SmartIntTest1(UNS8 nodeid)
{
  struct state_machine_struct *machine = &smart_interpolation_test1_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 0);
}

void SimulationStart(UNS8 nodeid)
{
  // devo distinguere il caso di broadcast dagli altri per i parametri strettamente
  // legati al motore
  if(nodeid != 0)
  {
    if(simulation_first_start[nodeid])
    {
      simulation_first_start[nodeid] = 0;
      struct state_machine_struct *machine = &init_interpolation_machine;
      _machine_exe(CANOpenShellOD_Data, nodeid, &SimulationInitCallback,
          &machine, 1, 0, 0);
    }
    else
    {
      struct state_machine_struct *machine = &resume_interpolation_machine;
      _machine_exe(CANOpenShellOD_Data, nodeid, &SimulationInitCallback,
          &machine, 1, 0, 1, motor_position[nodeid]);
    }
  }
  else
  {
    int i;
    struct state_machine_struct *init_interpolation =
        &init_interpolation_machine;
    struct state_machine_struct *resume_interpolation =
        &resume_interpolation_machine;

    for(i = 1; i < CANOPEN_NODE_NUMBER; i++)
    {
      if(motor_active[i] > 0)
      {
        if(simulation_first_start[i])
        {
          simulation_first_start[i] = 0;
          _machine_exe(CANOpenShellOD_Data, i, &SimulationInitCallback,
              &init_interpolation, 1, 0, 0);
        }
        else
        {
          _machine_exe(CANOpenShellOD_Data, i, &SimulationInitCallback,
              &resume_interpolation, 1, 0, 1, motor_position[i]);
        }
      }
    }
  }
}

void SmartIntStart(UNS8 nodeid)
{
  struct state_machine_struct *machine = &start_interpolation_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 0);
}

void SmartOrigin(UNS8 nodeid)
{
  struct state_machine_struct *machine = &smart_position_zero_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 0);
}

void SmartStop(UNS8 nodeid, int from_callback)
{
  struct state_machine_struct *machine = &smart_stop_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, from_callback,
      0);
}

/* Start heartbeat producer on smartmotor */
void StartHeart(char* sdo)
{
  int ret = 0;
  int nodeid;
  int time_ms;

  ret = sscanf(sdo, "shrt#%2x,%4x", &nodeid, &time_ms);
  if(ret == 2)
  {
    struct state_machine_struct *machine = &heart_start_machine;
    _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 1, time_ms);
  }
  else
    printf("Wrong command  : %s\n", sdo);
}

int SmartProgram(CO_Data* d, UNS8 nodeid)
{
//pthread_mutex_lock(&machine_mux[nodeid]);
  char command[32];
  int count = 0;
  int bytes_read;

  switch(machine_state)
  {
    case 0:
      printf("Sending LOAD command. . .\n");
      sprintf(command, "LOAD\r");
      writeNetworkDictCallBack(d, nodeid, 0x2500, 0x01, strlen(command),
          visible_string, command, CheckWriteProgram, 0);
      break;

    case 1:
      // Check that the command in progress bit (bit 0) is 0
      if((raw_response != -1) && ((raw_response & 0x1) == 0))
      {
        raw_response = -1;
        machine_state++;
        printf("Motor ready!\n");
        printf("Start programming. . .\n");
      }
      else
      {
        printf("Checking for green light. . .\n");
        readNetworkDictCallback(d, nodeid, 0x2500, 0x03, 0, CheckReadProgram,
            0);
        break;
      }

    case 2:
      // Write program 32 bytes of data
      bytes_read = program_file_read(program_file_path, command, 32);

      if(bytes_read == -1)
      {
        printf("Errore [%s]. Errore inaspettato nella lettura del file.\n",
            strerror(errno));
        machine_state = -1;
        break;
      }
      else if(bytes_read > 0)
      {
        machine_state -= 2;

        for(count = 0; count < bytes_read; count++)
          printf("%x", command[count]);

        writeNetworkDictCallBack(d, nodeid, 0x2500, 0x01, bytes_read,
            visible_string, command, CheckWriteProgram, 0);
        break;
      }
      else
        machine_state++;

    case 3:
      // End transmission
      printf("Ending transmission. . .\n");
      command[0] = 0xff;
      command[1] = 0xff;
      command[2] = 0x20;

      for(count = 0; count < 3; count++)
        printf("%x", command[count]);

      writeNetworkDictCallBack(d, nodeid, 0x2500, 0x01, 3, visible_string,
          command, CheckWriteProgram, 0);
      break;

    case 4:
      printf("Program complete\n");

      machine_state = -1;
      break;

    case -1:
      printf("Impossibile inviare il comando al nodo %d\n", nodeid);
      machine_state = -1;
      break;
  }

//pthread_mutex_unlock(&machine_mux[nodeid]);

  return 0;
}

int SmartWriteRaw(CO_Data* d, UNS8 nodeid)
{
  switch(machine_state)
  {
    case 0:
      // Check that the command in progress bit (bit 0) is 0
      if(raw_response != -1)
      {
        if((raw_response & 0x1) == 0)
        {
          raw_response = -1;
          machine_state++;
          printf("Motor ready!\n");
        }
        else
        {
          printf("Errore: altro comando in esecuzione\n");
          raw_response = -1;
          machine_state = 0;
          break;
        }
      }
      else
      {
        printf("Checking for green light. . .\n");
        readNetworkDictCallback(d, nodeid, 0x2500, 0x03, 0, CheckReadRaw, 0);
        break;
      }

    case 1:
      printf("Sending command. . .\n");
      writeNetworkDictCallBack(d, nodeid, 0x2500, 0x01, strlen(raw_cmd),
          visible_string, raw_cmd, CheckWriteRaw, 0);
      break;

    case 2:
      // Check that the command complete bit (bit 1) is 1
      if((raw_response != -1) && (raw_response & 0x2) == 2)
      {
        printf("Command received\n");
        raw_response = -1;
        machine_state++;
      }
      else
      {
        printf("Checking for rececption. . .\n");
        readNetworkDictCallback(d, nodeid, 0x2500, 0x03, 0, CheckReadRaw, 0);
        break;
      }

    case 3:
      if(raw_report_flag == 1)
      {
        if(raw_response == -1)
        {
          printf("Read response\n");
          readNetworkDictCallback(d, nodeid, 0x2500, 0x02, visible_string,
              CheckReadStringRaw, 0);
          break;
        }
        else
        {
          printf("\nResult : %s\n", raw_report);
          raw_response = -1;
          machine_state++;
        }
      }

    case 4:
      printf("Comando inviato correttamente\n");

      machine_state = -1;
      break;

    case -1:
      printf("Impossibile inviare il comando al nodo %d\n", nodeid);
      machine_state = -1;
      break;
  }

//pthread_mutex_unlock(&machine_mux[nodeid]);

  return 0;
}

void Map1TPDO(char *sdo)
{
  int ret;

  int nodeid;
  int pdo_number = 0x00;
  UNS32 cob_id = 0x182;
  UNS32 mapping_object1 = 0x60410010;
  UNS32 transmission_type = 0xFE;
  UNS32 transmission_time = 0x0064;

  ret = sscanf(sdo, "tpd1#%2x,%2x,%3x,%8x,%2x,%4x", &nodeid, &pdo_number,
      &cob_id, &mapping_object1, &transmission_type, &transmission_time);

  struct state_machine_struct *machine = &map1_pdo_machine;

  if((ret >= 3) && (ret <= 6))
  {
    _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 12,
        0x1800 | pdo_number, 0xC0000000 | cob_id, 0x1A00 | pdo_number,
        0x1A00 | pdo_number, mapping_object1, 0x1A00 | pdo_number,
        0x1800 | pdo_number, 0x40000000 | cob_id, 0x1800 | pdo_number,
        transmission_type, 0x1800 | pdo_number, transmission_time);
  }
  else
    printf("Wrong command  : %s\n", sdo);

}

void Map2TPDO(char *sdo)
{
  int ret;

  int nodeid;
  int pdo_number = 0x00;
  UNS32 cob_id = 0x182;
  UNS32 mapping_object1 = 0x60410010;
  UNS32 mapping_object2 = 0x606c0020;
  UNS32 transmission_type = 0xFE;
  UNS32 transmission_time = 0x0064;

  ret = sscanf(sdo, "tpd2#%2x,%2x,%3x,%8x,%8x,%2x,%4x", &nodeid, &pdo_number,
      &cob_id, &mapping_object1, &mapping_object2, &transmission_type,
      &transmission_time);

  struct state_machine_struct *machine = &map2_pdo_machine;

  if((ret >= 3) && (ret <= 7))
  {
    _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 14,
        0x1800 | pdo_number, 0xC0000000 | cob_id, 0x1A00 | pdo_number,
        0x1A00 | pdo_number, mapping_object1, 0x1A00 | pdo_number,
        0x1A00 | pdo_number, mapping_object2, 0x1A00 | pdo_number,
        0x1800 | pdo_number, 0x40000000 | cob_id, 0x1800 | pdo_number,
        transmission_type, 0x1800 | pdo_number, transmission_time);
  }
  else
    printf("Wrong command  : %s\n", sdo);

}

void StartNode(UNS8 nodeid)
{
  struct state_machine_struct *machine = &smart_start_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 0);
}

/* Ask a slave node to reset */
void ResetNode(UNS8 nodeid)
{
  masterSendNMTstateChange(CANOpenShellOD_Data, nodeid, NMT_Reset_Node);
}

/* Reset all nodes on the network and print message when boot-up*/
void DiscoverNodes()
{
  printf("Wait for Slave nodes bootup...\n\n");
  ResetNode(0x00);
}

void PreoperationalNode(UNS8 nodeid)
{
  masterSendNMTstateChange(CANOpenShellOD_Data, nodeid,
      NMT_Enter_PreOperational);
}

void StopNode(UNS8 nodeid)
{
  masterSendNMTstateChange(CANOpenShellOD_Data, nodeid, NMT_Stop_Node);
}

int get_info_step = 0;
/* Callback function that check the read SDO demand */
void CheckReadInfoSDO(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;
  UNS32 data = 0;
  UNS32 size = 64;

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size,
      &abortCode) != SDO_FINISHED)
    printf(
        "Master : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n",
        nodeid, abortCode);
  else
  {
    /* Display data received */
    switch(get_info_step)
    {
      case 1:
        printf("Device type     : %x\n", data);
        break;
      case 2:
        printf("Vendor ID       : %x\n", data);
        break;
      case 3:
        printf("Product Code    : %x\n", data);
        break;
      case 4:
        printf("Revision Number : %x\n", data);
        break;
    }
  }
  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(CANOpenShellOD_Data, nodeid, SDO_CLIENT);

  GetSlaveNodeInfo(nodeid);
}

/* Retrieve node informations located at index 0x1000 (Device Type) and 0x1018 (Identity) */
void GetSlaveNodeInfo(UNS8 nodeid)
{
  switch(++get_info_step)
  {
    case 1: /* Get device type */
      printf("##################################\n");
      printf("#### Informations for node %x ####\n", nodeid);
      printf("##################################\n");
      readNetworkDictCallback(CANOpenShellOD_Data, nodeid, 0x1000, 0x00, 0,
          CheckReadInfoSDO, 0);
      break;

    case 2: /* Get Vendor ID */
      readNetworkDictCallback(CANOpenShellOD_Data, nodeid, 0x1018, 0x01, 0,
          CheckReadInfoSDO, 0);
      break;

    case 3: /* Get Product Code */
      readNetworkDictCallback(CANOpenShellOD_Data, nodeid, 0x1018, 0x02, 0,
          CheckReadInfoSDO, 0);
      break;

    case 4: /* Get Revision Number */
      readNetworkDictCallback(CANOpenShellOD_Data, nodeid, 0x1018, 0x03, 0,
          CheckReadInfoSDO, 0);
      break;

    case 5: /* Print node info */
      get_info_step = 0;
      break;
  }
}

/* Read a slave node object dictionary entry */
void ReadDeviceEntry(char* sdo)
{
  int ret = 0;
  int nodeid;
  int index;
  int subindex;
  int datatype = 0;

  ret = sscanf(sdo, "rsdo#%2x,%4x,%2x", &nodeid, &index, &subindex);
  if(ret == 3)
  {
    printf("##################################\n");
    printf("#### Read SDO                 ####\n");
    printf("##################################\n");
    printf("NodeId   : %2.2x\n", nodeid);
    printf("Index    : %4.4x\n", index);
    printf("SubIndex : %2.2x\n", subindex);

    readNetworkDictCallback(CANOpenShellOD_Data, (UNS8) nodeid, (UNS16) index,
        (UNS8) subindex, (UNS8) datatype, CheckReadSDO, 0);
  }
  else
    printf("Wrong command  : %s\n", sdo);
}

/* Callback function that check the read SDO demand */
void CheckReadSDO(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;
  UNS32 data = 0;
  UNS32 size = 64;

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size,
      &abortCode) != SDO_FINISHED)
    printf(
        "\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n",
        nodeid, abortCode);
  else
    printf("\nResult : %x\n", data);

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(CANOpenShellOD_Data, nodeid, SDO_CLIENT);
}

/* Callback function that check the write SDO demand */
void CheckWriteSDO(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;

  if(getWriteResultNetworkDict(CANOpenShellOD_Data, nodeid,
      &abortCode) != SDO_FINISHED)
    printf(
        "\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n",
        nodeid, abortCode);
//else
//  printf("\nSend data OK\n");

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(CANOpenShellOD_Data, nodeid, SDO_CLIENT);
}

/* Write a slave node object dictionnary entry */
void WriteDeviceEntry(char* sdo)
{
  int ret = 0;
  int nodeid;
  int index;
  int subindex;
  int size;
  int data;

  ret = sscanf(sdo, "wsdo#%2x,%4x,%2x,%2x,%x", &nodeid, &index, &subindex,
      &size, &data);
  if(ret == 5)
  {
    printf("##################################\n");
    printf("#### Write SDO                ####\n");
    printf("##################################\n");
    printf("NodeId   : %2.2x\n", nodeid);
    printf("Index    : %4.4x\n", index);
    printf("SubIndex : %2.2x\n", subindex);
    printf("Size     : %2.2x\n", size);
    printf("Data     : %x\n", data);

    writeNetworkDictCallBack(CANOpenShellOD_Data, nodeid, index, subindex,
        size, 0, &data, CheckWriteSDO, 0);
  }
  else
    printf("Wrong command  : %s\n", sdo);
}

/* Write a raw command to motor */
void RawCmdMotor(char* sdo)
{
  int ret = 0;
  int nodeid;

  if(machine_state != -1)
  {
    printf("Error: complex command in progress.\n");
    return;
  }

  ret = sscanf(sdo, "sraw#%2x,%2d,%s", &nodeid, &raw_report_flag, raw_cmd);

  if(ret == 3)
  {
    printf("##################################\n");
    printf("#### Raw comamnd              ####\n");
    printf("##################################\n");
    printf("NodeId   : %2.2x\n", nodeid);
    printf("Command  : %s\n", raw_cmd);

    machine_state = 0;
    SmartWriteRaw(CANOpenShellOD_Data, nodeid);
  }
  else
    printf("Wrong command  : %s\n", sdo);
}

/* Write a raw command to motor */
void DownloadToMotor(char* sdo)
{
  int ret = 0;
  int nodeid;

  if(machine_state != -1)
  {
    printf("Error: complex command in progress.\n");
    return;
  }

  ret = sscanf(sdo, "prog#%2x,%s", &nodeid, program_file_path);

  if(ret == 2)
  {
    printf("##################################\n");
    printf("### Download firmware to motor ###\n");
    printf("##################################\n");
    printf("NodeId   : %2.2x\n", nodeid);
    printf("File  : %s\n", program_file_path);

    machine_state = 0;
    SmartProgram(CANOpenShellOD_Data, nodeid);
  }
  else
    printf("Wrong command  : %s\n", sdo);
}

/*
 * Studio banda necessaria:
 *
 *      oggetto       byte       temp       num. motori
 * -----------------------------------------------------
 *      TPDO2         3+5         10 ms     6
 *      SYNC          3           100 ms    1
 *      TPDO1         3+5         10  ms    6
 *      HEARTBEAT     3+1         100 ms    6
 *      TIMESTAMP     3+4         100 ms    1
 *
 * Per 6 motori si ha una richiesta di banda di:
 *
 * B = 9940 b/s = 79520 B/s
 *
 * In questo calcolo si dovrebbero aggiungere anche i comandi verso il motori, come
 * la loro configurazione ed i punti di aggiornamento della tabella. In particolare
 * questi ultimi dipendono da quanto sono distanziati i punti e del tempo utile per
 * raggiungerli. Punti distanti temporalmente richiederanno meno scambio dati.
 */
void ConfigureSlaveNode(CO_Data* d, UNS8 nodeid)
{
  SmartSin1Stop(nodeid);
  _machine_reset(d, nodeid);

  motor_active[nodeid] = 1;
  motor_started[nodeid] = 0;
  simulation_first_start[nodeid] = 1;

  if(nodeid == MOTOR_INDEX_FIRST)
  {
    // CONFIGURE HEARTBEAT TO 100 ms
    // MAP TPDO 1 (COB-ID 180) to transmit "node id" (8-bit), "status word" (16-bit), "interpolation mode status" (16-bit)
    // MAP TPDO 2 (COB-ID 280) to transmit "node id" (8-bit), "position actual value" (32-bit)
    // MAP TPDO 3 (COB-ID 380) to receive high resolution timestamp

    // MAP RPDO 1 (COB-ID 200 + nodeid) to receive "Interpolation Time Index" (8-bit)" (0x06c2 sub2)
    // MAP RPDO 2 (COB-ID 300 + nodeid) to receive "Interpolation Time Units" (8-bit)" (0x06c2 sub1)
    // MAP RPDO 3 (COB-ID 400 + nodeid) to receive "Interpolation Data" (32-bit)" (0x06c1 sub1)

    struct state_machine_struct *configure_pdo_machine[] =
        {
            &heart_start_machine, &map3_pdo_machine, &map2_pdo_machine,
            &map1_pdo_machine, &map1_pdo_machine, &map1_pdo_machine,
            &map1_pdo_machine, &smart_start_machine
        };

    _machine_exe(d, nodeid, NULL, configure_pdo_machine, 8, 1, 79,
        100,

        0x1800, 0xC0000180, 0x1A00, 0x1A00, 0x20000008, 0x1A00, 0x60410010,
        0x1A00, 0x24000010, 0x1A00, 0x1800, 0x40000180, 0x1800, 0xFE, 0x1800,
        0x32,

        0x1801, 0xC0000280, 0x1A01, 0x1A01, 0x20000008, 0x1A01, 0x60630020,
        0x1A01, 0x1801, 0x40000280, 0x1801, 0xFE, 0x1801, 0xa,

        0x1802, 0xC0000380, 0x1A02, 0x1A02, 0x10130020, 0x1A02, 0x1802,
        0x40000380, 0x1802, 10, 0x1802, 0,

        0x1400, 0xC0000200 + nodeid, 0x1600, 0x1600, 0x60c20208, 0x1600, 0x1400,
        0x40000200 + nodeid, 0x1400, 0xFE, 0x1400, 0,

        0x1401, 0xC0000300 + nodeid, 0x1601, 0x1601, 0x60c20108, 0x1601, 0x1401,
        0x40000300 + nodeid, 0x1401, 0xFE, 0x1401, 0,

        0x1402, 0xC0000400 + nodeid, 0x1602, 0x1602, 0x60c10120, 0x1602, 0x1402,
        0x40000400 + nodeid, 0x1402, 0xFE, 0x1402, 0

        );

    canopen_abort_code = RegisterSetODentryCallBack(d, 0x2400, 0,
        &OnStatusUpdate);

    if(canopen_abort_code)
      printf(
          "Error[%d on node %x]: Impossibile registrare il callback per l'oggetto 0x2400 (Canopen abort code %x)\n",
          CANOpenError, nodeid, canopen_abort_code);

    canopen_abort_code = RegisterSetODentryCallBack(d, 0x6063, 0,
        &OnPositionUpdate);

    if(canopen_abort_code)
      printf(
          "Error[%d on node %x]: Impossibile registrare il callback per l'oggetto 0x2400 (Canopen abort code %x)\n",
          CANOpenError, nodeid, canopen_abort_code);

    fflush(stdout);
  }
  else
  {
    // CONFIGURE HEARTBEAT TO 100 ms
    // MAP TPDO 1 (COB-ID 180) to transmit "node id" (8-bit), "status word" (16-bit), "interpolation mode status" (16-bit)
    // MAP TPDO 2 (COB-ID 280) to transmit "node id" (8-bit), "position actual value" (32-bit)

    // MAP RPDO 1 (COB-ID 200 + nodeid) to receive "Interpolation Time Index" (8-bit)" (0x06c2 sub2)
    // MAP RPDO 2 (COB-ID 300 + nodeid) to receive "Interpolation Time Units" (8-bit)" (0x06c2 sub1)
    // MAP RPDO 3 (COB-ID 400 + nodeid) to receive "Interpolation Data" (32-bit)" (0x06c1 sub1)
    // MAP RPDO 4 (COB-ID 380) to receive high resolution timestamp
    struct state_machine_struct *configure_slave_machine[] =
        {
            &heart_start_machine, &map3_pdo_machine, &map2_pdo_machine,
            &map1_pdo_machine, &map1_pdo_machine, &map1_pdo_machine,
            &map1_pdo_machine, &smart_start_machine
        };

    _machine_exe(d, nodeid, NULL, configure_slave_machine, 8, 1, 79,
        100,

        0x1800, 0xC0000180, 0x1A00, 0x1A00, 0x20000008, 0x1A00, 0x60410010,
        0x1A00, 0x24000010, 0x1A00, 0x1800, 0x40000180, 0x1800, 0xFE, 0x1800,
        0x32,

        0x1801, 0xC0000280, 0x1A01, 0x1A01, 0x20000008, 0x1A01, 0x60630020,
        0x1A01, 0x1801, 0x40000280, 0x1801, 0xFE, 0x1801, 0xa,

        0x1400, 0xC0000200 + nodeid, 0x1600, 0x1600, 0x60c20208, 0x1600, 0x1400,
        0x40000200 + nodeid, 0x1400, 0xFE, 0x1400, 0,

        0x1401, 0xC0000300 + nodeid, 0x1601, 0x1601, 0x60c20108, 0x1601, 0x1401,
        0x40000300 + nodeid, 0x1401, 0xFE, 0x1401, 0,

        0x1402, 0xC0000400 + nodeid, 0x1602, 0x1602, 0x60c10120, 0x1602, 0x1402,
        0x40000400 + nodeid, 0x1402, 0xFE, 0x1402, 0,

        0x1403, 0xC0000380, 0x1603, 0x1603, 0x10130020, 0x1603, 0x1403,
        0x40000380, 0x1403, 0xFE, 0x1403, 0);
  }

// Controllo se una tabella sia stata già assegnata, altrimenti ne
// trovo una libera
  int motor_table_index = MotorTableIndexFromNodeId(nodeid);

  if(motor_table_index == -1)
  {
    int i;
    for(i = 0; i < TABLE_MAX_NUM; i++)
    {
      if(motor_table[i].nodeId == 0)
      {
        QueueInit(nodeid, &motor_table[i]);
        break;
      }
    }
  }
  else
    QueueInit(nodeid, &motor_table[motor_table_index]);
}

void CANOpenShellOD_post_Emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode,
    UNS8 errReg)
{
  printf("Emergency message: %d", errCode);
}

void CANOpenShellOD_post_SlaveBootup(CO_Data* d, UNS8 nodeid)
{
  printf("Slave %x boot up\n", nodeid);

  ConfigureSlaveNode(d, nodeid);
}

void CANOpenShellOD_heartbeatError(CO_Data* d, UNS8 nodeid)
{
  printf("Error[heartbeat]: node %d fail!\n", nodeid);
}

/***************************  CALLBACK FUNCTIONS  *****************************************/

/* Callback function that check the read SDO demand */
void CheckReadProgram(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;
  UNS32 data = 0;
  UNS32 size = 64;

//pthread_mutex_lock(&machine_mux[nodeid]);

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size,
      &abortCode) != SDO_FINISHED)
  {
    printf(
        "\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n",
        nodeid, abortCode);
    fflush(stdout);
    raw_response = -1;
    machine_state = -1;
  }
  else
    raw_response = data;

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(d, nodeid, SDO_CLIENT);

//pthread_mutex_unlock(&machine_mux[nodeid]);

  SmartProgram(d, nodeid);
}

/* Callback function that check the read SDO demand */
void CheckReadRaw(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;
  UNS32 data = 0;
  UNS32 size = 64;

//pthread_mutex_lock(&machine_mux[nodeid]);

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size,
      &abortCode) != SDO_FINISHED)
  {
    printf(
        "\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n",
        nodeid, abortCode);
    fflush(stdout);
    raw_response = -1;
    machine_state = -1;
  }
  else
    raw_response = data;

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(d, nodeid, SDO_CLIENT);

//pthread_mutex_unlock(&machine_mux[nodeid]);

  SmartWriteRaw(d, nodeid);
}

/* Callback function that check the read SDO demand */
void CheckReadStringRaw(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;
  UNS32 size = sizeof(raw_report);

//pthread_mutex_lock(&machine_mux[nodeid]);

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, raw_report, &size,
      &abortCode) != SDO_FINISHED)
  {
    printf(
        "\nResult : Failed in getting information for slave %2.2x, AbortCode :%x \n",
        nodeid, abortCode);
    fflush(stdout);
    raw_response = -1;
    machine_state = -1;
  }
  else
    raw_response = 1;

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(d, nodeid, SDO_CLIENT);

//pthread_mutex_unlock(&machine_mux[nodeid]);

  SmartWriteRaw(d, nodeid);
}

void CheckWriteProgram(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;

  if(getWriteResultNetworkDict(d, nodeid, &abortCode) != SDO_FINISHED)
  {
    printf("Error: %d", abortCode);
    fflush(stdout);
    machine_state = -1;
  }

  closeSDOtransfer(d, nodeid, SDO_CLIENT);

  machine_state++;

//pthread_mutex_unlock(&machine_mux[nodeid]);
  SmartProgram(d, nodeid);
}

void CheckWriteRaw(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;

//pthread_mutex_lock(&machine_mux[nodeid]);
  if(getWriteResultNetworkDict(d, nodeid, &abortCode) != SDO_FINISHED)
  {
    printf("Error: %d", abortCode);
    fflush(stdout);
    machine_state = -1;
  }

  closeSDOtransfer(d, nodeid, SDO_CLIENT);

  machine_state++;

//pthread_mutex_unlock(&machine_mux[nodeid]);
  SmartWriteRaw(d, nodeid);
}

void CANOpenShellOD_initialisation(CO_Data* d)
{
  printf("Node_initialisation\n");
  fflush(stdout);

  /*UNS32 PDO1_COBID = 0x0182;
   UNS32 size = sizeof(UNS32);
   UNS8 transmission_type = 0xff;
   UNS32 size_type = sizeof(UNS8);*/

  /*****************************************
   * Define RPDOs to match slave ID=0 TPDOs*
   *****************************************/
//writeLocalDict( d, /*CO_Data* d*/
//0x1400, /*UNS16 index*/
//0x01, /*UNS8 subind*/
//&PDO1_COBID, /*void * pSourceData,*/
//&size, /* UNS8 * pExpectedSize*/
//RW);
  /* UNS8 checkAccess */

//writeLocalDict( d, /*CO_Data* d*/
//0x1400, /*UNS16 index*/
//0x02, /*UNS8 subind*/
//&transmission_type, /*void * pSourceData,*/
//&size_type, /* UNS8 * pExpectedSize*/
//RW);
  /* UNS8 checkAccess */
}

void CANOpenShellOD_preOperational(CO_Data* d)
{
  printf("Node_preOperational\n");
  fflush(stdout);
}

void CANOpenShellOD_operational(CO_Data* d)
{
  printf("Node_operational\n");
  fflush(stdout);
}

void CANOpenShellOD_stopped(CO_Data* d)
{
  printf("Node_stopped\n");
  fflush(stdout);
}

void CANOpenShellOD_post_sync(CO_Data* d)
{
  printf("Master_post_sync\n");
}

void CANOpenShellOD_post_TPDO(CO_Data* d)
{
  printf("Master_post_TPDO\n");
}

/***************************  INITIALISATION  **********************************/
void Init(CO_Data* d, UNS32 id)
{
  if(Board.baudrate)
  {
    /* Init node state*/
    setState(CANOpenShellOD_Data, Initialisation);
    setState(CANOpenShellOD_Data, Operational);
  }
}

/***************************  CLEANUP  *****************************************/
void Exit(CO_Data* d, UNS32 nodeid)
{
  if(strcmp(Board.baudrate, "none"))
  {
    /* Reset all nodes on the network */
    masterSendNMTstateChange(CANOpenShellOD_Data, 0, NMT_Reset_Node);

    /* Stop master */
    setState(CANOpenShellOD_Data, Stopped);
  }
}

int NodeInit(int NodeID)
{
  CANOpenShellOD_Data = &CANOpenShellMasterOD_Data;

  /* Load can library */
  LoadCanDriver(LibraryPath);

  /* Define callback functions */
  CANOpenShellOD_Data->initialisation = CANOpenShellOD_initialisation;
  CANOpenShellOD_Data->preOperational = CANOpenShellOD_preOperational;
  CANOpenShellOD_Data->operational = CANOpenShellOD_operational;
  CANOpenShellOD_Data->stopped = CANOpenShellOD_stopped;
//CANOpenShellOD_Data->post_sync = CANOpenShellOD_post_sync;
//CANOpenShellOD_Data->post_TPDO = CANOpenShellOD_post_TPDO;
  CANOpenShellOD_Data->post_SlaveBootup = CANOpenShellOD_post_SlaveBootup;
  CANOpenShellOD_Data->heartbeatError = CANOpenShellOD_heartbeatError;
  CANOpenShellOD_Data->post_emcy = CANOpenShellOD_post_Emcy;

  if(!canOpen(&Board, CANOpenShellOD_Data))
    return INIT_ERR;

  /* Defining the node Id */
  setNodeId(CANOpenShellOD_Data, NodeID);
  /* Start Timer thread */
  StartTimerLoop(&Init);
  return 0;
}

void
help_menu(void)
{
  printf("   MANDATORY COMMAND (must be the first command):\n");
  printf("     load#CanLibraryPath,channel,baudrate,nodeid\n");
  printf("\n");
  printf("   NETWORK: (if nodeid=0x00 : broadcast)\n");
  printf("     srst#nodeid : Reset a node\n");
  printf("     spre#nodeid : Enter in pre-operational mode\n");
  printf("     soff#nodeid : Stop node\n");
  printf("     scan : Reset all nodes and print message when bootup\n");
  printf("\n");
  printf("   SDO: (size in bytes)\n");
  printf("     info#nodeid\n");
  printf("     rsdo#nodeid,index,subindex : read sdo\n");
  printf("        ex : rsdo#42,1018,01\n");
  printf("     wsdo#nodeid,index,subindex,size,data : write sdo\n");
  printf("        ex : wsdo#42,6200,01,01,FF\n");
  printf("\n");
  printf("   SMART MOTOR:\n");
  printf("     ssta#nodeid : Reset error and make motor operative\n");
  printf("     ssto#nodeid : Stop a node and motor\n");
  printf("     sraw#nodeid,report_flag,command : raw cmd to motor\n");
  printf("        ex : sraw#3f,1,RMODE\n");
  printf(
      "     tpdo#nodeid,pdo_num - 1,cob_id,map1,map2,entry_num,type,time : map tpdo\n");
  printf("        ex : tpdo#3f,1,182,60410010,606c0020,2,FE,64\n");
  printf("     shrt#nodeid,cycle : set and start heartbeat time cycle [ms]\n");
  printf("     szer#nodeid : set motor at origin\n");
  printf("     int1#nodeid : interpolation test 1\n");
  printf("     int2#nodeid : interpolation test 2\n");
  printf(
      "     sin1#nodeid,angle step,amplitude,point number : generate sin wave\n");
  printf("     ints#nodeid : start interpolated movement \n");
  printf("     shom#nodeid : homing\n");
  printf(
      "     simu#nodeid : start simulation reading data from tables/<nodeid>.mot file\n");
  printf("     sgen#nodeid,type,<param> : generate simulation data\n");
  printf(
      "                 type: sin,amplitude,period[ms],tsamp[ms],num of period\n");
  printf("        ex : sgen#77,sin,3e0,3e8,a,1\n");
  printf("\n");
  printf("   Note: All numbers are hex\n");
  printf("\n");
  printf("     help : Display this menu\n");
  printf("     quit : Quit application\n");
  printf("\n");
  printf("\n");
}

int ExtractNodeId(char *command)
{
  int nodeid;
  sscanf(command, "%2x", &nodeid);
  return nodeid;
}

int ProcessCommand(char* command)
{
  int ret = 0;
  int NodeID;

  EnterMutex();
  switch(cst_str4(command[0], command[1], command[2], command[3]))
  {
    case cst_str4('h', 'e', 'l', 'p'): /* Display Help*/
      help_menu();
      break;

    case cst_str4('s', 'z', 'e', 'r'):
      LeaveMutex();
      SmartOrigin(ExtractNodeId(command + 5));
      break;

    case cst_str4('i', 'n', 't', 's'):
      LeaveMutex();
      SmartIntStart(ExtractNodeId(command + 5));
      break;

    case cst_str4('s', 'i', 'm', 'u'):
      LeaveMutex();
      SimulationStart(ExtractNodeId(command + 5));
      break;

    case cst_str4('s', 'g', 'e', 'n'):
      LeaveMutex();
      GenerateMotorData(command);
      break;

    case cst_str4('i', 'n', 't', '1'):
      LeaveMutex();
      SmartIntTest1(ExtractNodeId(command + 5));
      break;

    case cst_str4('i', 'n', 't', '2'):
      LeaveMutex();
      SmartIntTest2(ExtractNodeId(command + 5));
      break;

    case cst_str4('s', 'i', 'n', '1'):
      LeaveMutex();
      SinFunction(command);
      break;

    case cst_str4('s', 'h', 'o', 'm'):
      LeaveMutex();
      SmartHome(command);
      break;

    case cst_str4('t', 'p', 'd', '1'):
      LeaveMutex();
      Map1TPDO(command);
      break;

    case cst_str4('t', 'p', 'd', '2'):
      LeaveMutex();
      Map2TPDO(command);
      break;

    case cst_str4('s', 'h', 'r', 't'):
      LeaveMutex();
      StartHeart(command);
      break;

    case cst_str4('s', 's', 't', 'a'): /* Slave Start*/
      LeaveMutex();
      StartNode(ExtractNodeId(command + 5));
      break;

    case cst_str4('s', 's', 't', 'o'): /* Slave Stop */
      LeaveMutex();
      SmartStop(ExtractNodeId(command + 5), 0);
      break;

    case cst_str4('s', 'r', 's', 't'): /* Slave Reset */
      ResetNode(ExtractNodeId(command + 5));
      break;

    case cst_str4('s', 'p', 'r', 'e'): /* Slave Reset */
      PreoperationalNode(ExtractNodeId(command + 5));
      break;

    case cst_str4('s', 'o', 'f', 'f'): /* Slave Reset */
      StopNode(ExtractNodeId(command + 5));
      break;

    case cst_str4('i', 'n', 'f', 'o'): /* Retrieve node informations */
      GetSlaveNodeInfo(ExtractNodeId(command + 5));
      break;

    case cst_str4('r', 's', 'd', 'o'): /* Read device entry */
      ReadDeviceEntry(command);
      break;

    case cst_str4('w', 's', 'd', 'o'): /* Write device entry */
      WriteDeviceEntry(command);
      break;

    case cst_str4('s', 'c', 'a', 'n'): /* Discover nodes */
      DiscoverNodes();
      break;

    case cst_str4('q', 'u', 'i', 't'): /* Quit application */
      LeaveMutex();
      return QUIT;

    case cst_str4('l', 'o', 'a', 'd'): /* Library Interface*/
      ret = sscanf(command, "load#%100[^,],%30[^,],%4[^,],%d", LibraryPath,
          BoardBusName, BoardBaudRate, &NodeID);

      if(ret == 4)
      {
        LeaveMutex();
        ret = NodeInit(NodeID);
        return ret;
      }
      else
      {
        printf("Invalid load parameters\n");
      }
      break;

    case cst_str4('s', 'r', 'a', 'w'): /* RAW command */
      RawCmdMotor(command);
      break;

    case cst_str4('p', 'r', 'o', 'g'): /* download program to motor */
      DownloadToMotor(command);
      break;

    default:
      if(*command != '\n')
        help_menu();
      break;
  }

  LeaveMutex();
  return 0;
}

void signal_handler(int signum)
{
// Garbage collection
  printf("Terminating program...\n");

  printf("Finishing.\n");

  stopSYNC(CANOpenShellOD_Data);
  heartbeatStop(CANOpenShellOD_Data);

// Stop timer thread
  StopTimerLoop(&Exit);

  /* Close CAN board */
  canClose(CANOpenShellOD_Data);

  TimerCleanup();

  exit(signum);
}

/****************************************************************************/
/***************************  MAIN  *****************************************/
/****************************************************************************/

int main(int argc, char** argv)
{
  extern char *optarg;
  char command[200];
  char* res;
  int ret = 0;
  int sysret = 0;
  int i = 0;

  /* Print help and exit immediatly*/
  if(argc < 2)
  {
    help_menu();
    exit(1);
  }

//signal(SIGINT, signal_handler);
//signal(SIGTERM, signal_handler);

  // inizializzo la named pipe per i dati di posizione

  umask(0);
  mknod(POSITION_FIFO_FILE, S_IFIFO|0666, 0);

  printf("Waiting for someone who wants to read positions. . .\n");

  position_fp = fopen(POSITION_FIFO_FILE, "w");

  if(position_fp == NULL)
    perror("position pipe");

  printf("Starting. . .\n");

  /* Init stack timer */
  TimerInit();

  /* Strip command-line*/
  for(i = 1; i < argc; i++)
  {
    if(ProcessCommand(argv[i]) == INIT_ERR)
      goto init_fail;
  }

  _machine_init();

//heartbeatInit(CANOpenShellOD_Data);
  startSYNC(CANOpenShellOD_Data);
  PDOInit(CANOpenShellOD_Data);

  /* Enter in a loop to read stdin command until "quit" is called */
  while(ret != QUIT)
  {
    // wait on stdin for string command
    res = fgets(command, sizeof(command), stdin);

    if(res != NULL)
    {
      sysret = system(CLEARSCREEN);
      ret = ProcessCommand(command);
      fflush(stdout);
    }
  }

  printf("Finishing.\n");

  if(position_fp > 0)
    fclose(position_fp);

  _machine_destroy();

  stopSYNC(CANOpenShellOD_Data);
//heartbeatStop(CANOpenShellOD_Data);

// Stop timer thread
  StopTimerLoop(&Exit);

  /* Close CAN board */
  canClose(CANOpenShellOD_Data);

  init_fail: TimerCleanup();
  return 0;
}

