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
#include <time.h>
#include <sys/time.h>
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
#include "smartmotor_table.h"

//****************************************************************************
// DEFINES
#define MOTOR_INDEX_FIRST 0x77
#define INTERPOLATION_DATA_INDEX_OFFSET 6
#define INTERPOLATION_START_INDEX_OFFSET 12
#define TARGET_POSITION_INDEX_OFFSET 13
#define POSITION_FIFO_FILE "/tmp/alma_3d_spinitalia_pos_stream_pipe"
#define SYNC_DIVIDER_STATUS 15
#define SYNC_DIVIDER_TIMESTAMP 100
// bug quando do posizioni il motore si affloscia
// il simulatore è a specchio
// bug quando mando in rilasciato il programma di Enrico inonda di messaggi lo stream
// add comando azzeramento motore

//#define CBRN
//#define NO_LIMITS

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

/**
 * Tabella degli stati:
 *
 *               CT0
 * ACCESO ------> INIZIALIZZATO
 *
 *
 *
 *     EM2
 * ? -------> EMERGENZA
 *
 *
 *
 *                  CT2 P1
 * INIZIALIZZATO -----------> RICERCA_CENTRO
 *                  CT2 P3
 * INIZIALIZZATO -----------> RILASCIATO
 *                   CT6
 * INIZIALIZZATO -----------> SPENTO
 *
 *
 *
 *                  fine
 * RICERCA_CENTRO -----------> CENTRATO
 *
 *
 *
 *              CT4
 * CENTRATO --------------> SIMULAZIONE
 *            CT1 Mx Px VMx AMx
 * CENTRATO ---------------------> IN_POSIZIONE
 *             CT2 P3
 * CENTRATO -----------> RILASCIATO
 *              CT6
 * CENTRATO -----------> SPENTO
 *
 *
 *
 *                   CT5
 * SIMULAZIONE --------------> FERMO
 *                   fine
 * SIMULAZIONE --------------> FERMO
 *
 *                  fine
 * IN_POSIZIONE -------------> FERMO
 *
 *
 *            CT1 Mx Px VMx AMx
 * FERMO ---------------------> IN_POSIZIONE
 *         CT2 P2
 * FERMO -----------> CENTRAGGIO
 *          CT2 P3
 * FERMO -----------> RILASCIATO
 *           CT6
 * FERMO -----------> SPENTO
 *
 *
 *
 *               fine
 * CENTRAGGIO -----------> CENTRATO
 *
 *
 *             CT5 && centrato
 * RILASCIATO ------------------> FERMO
 *             CT0 && !centrato
 * RILASCIATO ------------------> INIZIALIZZATO
 *
 *
 *              CT0
 * EMERGENZA ----------> INIZIALIZZATO
 *
 *
 * Stati in cui i comandi sono validi:
 *
 *   CT0:    ACCESO, RILASCIATO, EMERGENZA
 *   CT2 P1: INIZIALIZZATO
 *   CT2 P2: FERMO
 *   CT4:    CENTRATO
 *   CT5:    SIMULAZIONE
 *   CT6:    INIZIALIZZATO, FERMO, CENTRATO
 *
 *   CT2 P3:    INIZIALIZZATO, CENTRATO, FERMO
 *   EM2:    X
 *
 */
#define ERRORE_ASINCRONO   0 /**< si è verificato un errore asincrono */
#define SPENTO             1 /**< tutti i nodi canopen spenti */
#define EMERGENZA          2 /**< il motore è bello stato EM2 */
#define ACCESO             3 /**< stato iniziale del motore */
#define INIZIALIZZATO      4 /**< motore inizializzato */
#define RICERCA_CENTRO     5 /**< motore in homing */
#define CENTRATO           6 /**< motore nell'origine */
#define SIMULAZIONE        8 /**< il motore sta eseguento una simulazione */
#define FERMO              9 /**< il motore è fermo */
#define CENTRAGGIO         10 /**< in movimento verso lo zero */
#define RILASCIATO         11 /**< il controllo dei motori è spento ed è presente il freno motore */
#define IN_POSIZIONE       13 /**< movimento in modalità posizione */
#define JOYSTICK_COLLEGATO 14 /**< movimento tramite streaming */
#define MOVIMENTO_LIBERO   15 /**< movimento tramite streaming */

#define cst_str2(c1, c2) ((unsigned int)0 | \
    (char)c2) << 8 | (char)c1

#define cst_str4(c1, c2, c3, c4) ((((unsigned int)0 | \
    (char)c4 << 8) | \
    (char)c3) << 8 | \
    (char)c2) << 8 | \
    (char)c1

#define INIT_ERR 2
#define QUIT 1

static int robot_state = ACCESO;
pthread_mutex_t robot_state_mux = PTHREAD_MUTEX_INITIALIZER
;
volatile int simulation_ready[TABLE_MAX_NUM];

void CheckReadRaw(CO_Data* d, UNS8 nodeid);
void CheckReadStringRaw(CO_Data* d, UNS8 nodeid);
void CheckWriteRaw(CO_Data* d, UNS8 nodeid);
int SmartWriteRaw(CO_Data* d, UNS8 nodeid);

void CheckWriteProgramDownload(CO_Data* d, UNS8 nodeid);
void CheckWriteProgramUpload(CO_Data* d, UNS8 nodeid);
void CheckReadProgramDownload(CO_Data* d, UNS8 nodeid);
void CheckReadProgramUpload(CO_Data* d, UNS8 nodeid);
UNS32 OnInterpUpdate(CO_Data* d, UNS8 nodeid);
void SmartStop(UNS8 nodeid, int from_callback);
void SimulationTableUpdate(CO_Data* d, UNS8 nodeid, UNS16 interpolation_status, int point_number,
    int from_callback);
void SimulationTableEnd(CO_Data* d, UNS8 nodeId, int machine_state,
UNS32 return_value);
int MotorTableIndexFromNodeId(UNS8 nodeId);
void SmartRelease(UNS8 nodeid, int from_callback, int brake);
void SimulationTableStart(CO_Data* d);
void SmartStopCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value);
void SmartPosition(UNS8 nodeid, long position, long velocity, long acceleration, int start,
    int from_callback);

//****************************************************************************
// GLOBALS

char BoardBusName[31];
char BoardBaudRate[5];
s_BOARD Board =
{
BoardBusName, BoardBaudRate
};
CO_Data* CANOpenShellOD_Data;
static timer_t timer;
static timer_t fake_update_timer;

pthread_t pipe_handler;
pthread_t pipe_write_handler;
pthread_mutex_t interpolator_mux[CANOPEN_NODE_NUMBER];
pthread_mutex_t position_mux = PTHREAD_MUTEX_INITIALIZER
;
pthread_cond_t position_ready = PTHREAD_COND_INITIALIZER
;
pthread_mutex_t exit_from_limit_mux = PTHREAD_MUTEX_INITIALIZER
;
pthread_mutex_t release_mux = PTHREAD_MUTEX_INITIALIZER
;
pthread_mutex_t motor_active_number_mutex = PTHREAD_MUTEX_INITIALIZER
;
char motor_position_write[CANOPEN_NODE_NUMBER]; /**< di chi ho già segnato la posizione */
//static struct timeval position_start_time;
struct timespec position_start_time;

volatile int interpolator_busy[CANOPEN_NODE_NUMBER];
UNS8 raw_response[33];
int raw_response_flag = -1;
UNS32 raw_response_size = 0;
int raw_report_flag = 0;
char raw_report[100];
char raw_cmd[100];

char program_file_path[100];

char LibraryPath[512];
e_nodeState node_state;

int machine_state = -1;
float angle_actual_rad[CANOPEN_NODE_NUMBER];
float angle_step_rad[CANOPEN_NODE_NUMBER];

struct table_data motor_table[TABLE_MAX_NUM + 1]; // numero di elementi pari al numero dei motori più l'elemento di broadcast

static int simulation_first_start[CANOPEN_NODE_NUMBER];

FILE *position_fp = NULL;

int fake_flag = 0;
int exit_from_limit_complete = 0;
int release_complete = 0;
int homing_executed = 0;

/**
 * @return: -1: errore, chunk_size: ok, <chunk_size: ultimi byte e fine del file
 */
int program_file_read(const char *file_path, char *program_chunk, int chunk_size)
{
  FILE *file = NULL;
  ssize_t read;
  static int cursor_position = 0;
  static int cursor_end_program = 0;
  static char end_program[] =
  {
  0xff, 0xff, 0x20
  };

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

  // se non ho letto 32 byte, significa che sono arrivato alla fine del file
  // e devo inserire la chiave di fine programmazione
  while((read < 32) && (cursor_end_program < 3))
  {
    program_chunk[read++] = end_program[cursor_end_program++];
  }

  // se sono riuscito a scrivere tutta la chiave, resetto le
  // variabili
  if(cursor_end_program == 3)
  {
    cursor_position = 0;
    cursor_end_program = 0;
  }

  fclose(file);

  return read;
}

void SmartClear(UNS8 nodeid)
{
  if(nodeid == 0)
  {
    int motor_index;
    for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    {
      pthread_mutex_lock(&interpolator_mux[motor_table[motor_index].nodeId]);
      interpolator_busy[motor_table[motor_index].nodeId] = 0;
      pthread_mutex_unlock(&interpolator_mux[motor_table[motor_index].nodeId]);

      simulation_first_start[motor_table[motor_index].nodeId] = 1;
      motor_started[motor_table[motor_index].nodeId] = 0;
    }
  }
  else
  {
    if(motor_active[nodeid])
    {
      pthread_mutex_lock(&interpolator_mux[nodeid]);
      interpolator_busy[nodeid] = 0;
      pthread_mutex_unlock(&interpolator_mux[nodeid]);

      simulation_first_start[nodeid] = 1;
      motor_started[nodeid] = 0;
    }
  }
}

void SmartBusVoltageCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  int stop_in_progress = 0;
  int motor_index;

  SmartClear(nodeId);

  for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    stop_in_progress |= motor_started[motor_table[motor_index].nodeId];

  if(stop_in_progress == 0)
  {
    pthread_mutex_lock(&robot_state_mux);
    robot_state = EMERGENZA;
    pthread_mutex_unlock(&robot_state_mux);
  }
}

void SmartFaultCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  if(is_register)
  {
    if((return_value & 0b0000000000001000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Servo bus voltage fault\n", SmartMotorError, nodeId);
      }
#endif
      add_event(CERR_BusVoltageFault, nodeId, 0, NULL);
    }

    if((return_value & 0b0000000000010000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Peak over-current occurred\n", SmartMotorError, nodeId);
      }
#endif
      add_event(CERR_OverCurrentFault, nodeId, 0, NULL);
    }

    if((return_value & 0b0000000000100000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Excessive temperature\n", SmartMotorError, nodeId);
      }
#endif

      add_event(CERR_TemperatureFault, nodeId, 0, NULL);
    }

    if((return_value & 0b0000000001000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Excessive position error\n", SmartMotorError, nodeId);
      }
#endif

      add_event(CERR_PositionFault, nodeId, 0, NULL);
    }

    if((return_value & 0b0000000010000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Velocity limit\n", SmartMotorError, nodeId);
      }
#endif

      add_event(CERR_VelocityFault, nodeId, 0, NULL);
    }

    if((return_value & 0b0000001000000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: First derivative (DE/Dt) of position error over limit\n",
            SmartMotorError, nodeId);
      }
#endif

      add_event(CERR_DerivativeFault, nodeId, 0, NULL);
    }

    if((return_value & 0b0001000000000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Right(+) over travel limit\n", SmartMotorError, nodeId);
      }
#endif

      pthread_mutex_lock(&robot_state_mux);
      if(robot_state == RILASCIATO)
      {
        pthread_mutex_unlock(&robot_state_mux);

        add_event(CERR_RightLimitFault, nodeId, 1, NULL);
      }
      else
      {
        pthread_mutex_unlock(&robot_state_mux);

        add_event(CERR_RightLimitFault, nodeId, 0, NULL);
      }

    }
    else if((return_value & 0b0010000000000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Left(-) over travel limit\n", SmartMotorError, nodeId);
      }
#endif
      add_event(CERR_LeftLimitFault, nodeId, 0, NULL);
    }
  }
  else
  {
    SmartStopCallback(d, nodeId, machine_state, is_register, return_value);

    /*if((return_value & 0b0011001011111000) == 0)
     {
     int stop_in_progress = 0;
     int motor_index;

     SmartClear(nodeId);

     for(motor_index = 0; motor_index < motor_active_number; motor_index++)
     stop_in_progress |= motor_started[motor_table[motor_index].nodeId];

     pthread_mutex_lock(&robot_state_mux);
     if((robot_state == SIMULAZIONE) || (robot_state == MOVIMENTO_LIBERO))
     {
     int motor_table_index = MotorTableIndexFromNodeId(nodeId);
     struct table_data_read data_read;

     QueueLast(&motor_table[motor_table_index], &data_read);

     InterpolationTimePeriod[nodeId - MOTOR_INDEX_FIRST] = 0;
     InterpolationTimeValue[nodeId - MOTOR_INDEX_FIRST] = 0;
     InterpolationData[nodeId - MOTOR_INDEX_FIRST] = data_read.position;

     sendPDOevent(d);
     }

     pthread_mutex_unlock(&robot_state_mux);

     if(stop_in_progress == 0)
     {
     pthread_mutex_lock(&robot_state_mux);
     switch(robot_state)
     {
     case SIMULAZIONE:
     CERR("CT4", CERR_SimulationError);
     OK("CT5");

     robot_state = FERMO;
     break;

     case MOVIMENTO_LIBERO:
     CERR("CB7", CERR_SimulationError);
     OK("CT5");

     robot_state = FERMO;
     break;

     case CENTRAGGIO:
     robot_state = FERMO;
     break;

     case EMERGENZA:
     OK("CT0");
     break;
     }

     pthread_mutex_unlock(&robot_state_mux);
     }
     }*/
  }
}

UNS32 OnPositionUpdate(CO_Data* d, const indextable * indextable_curr,
UNS8 bSubindex)
{
  static int motor_basket_num = 0;

  UNS8 nodeid = NodeId;

  if(fake_flag == 0)
  {
    if(Position_Actual_Value != motor_position[nodeid])
      motor_position[nodeid] = Position_Actual_Value;
  }

  pthread_mutex_lock(&position_mux);

  if(motor_position_write[nodeid] == 0)
  {
    motor_position_write[nodeid] = 1;

    motor_basket_num++;
  }

  fflush(stdout);
  if(motor_basket_num == motor_active_number)
  {
    motor_basket_num = 0;
    memset(motor_position_write, 0, sizeof(motor_position_write));
    pthread_cond_signal(&position_ready);
  }

  pthread_mutex_unlock(&position_mux);

  return 0;
}

int MotorTableIndexFromNodeId(UNS8 nodeId)
{
  int i;

  for(i = 0; i < (TABLE_MAX_NUM + 1); i++)
  {
    if(motor_table[i].nodeId == nodeId)
    {
      return i;
    }
  }

  return -1;
}

void SmartPositionTargetCallback(CO_Data* d, UNS8 nodeid, int machine_state, int is_register,
UNS32 return_value)
{
  if(return_value)
    return;

  motor_started[nodeid] = 0;

  int motor_index;
  int stop_in_progress = 0;

  pthread_mutex_lock(&exit_from_limit_mux);

  for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    stop_in_progress |= motor_started[motor_table[motor_index].nodeId];

  if(stop_in_progress == 0)
  {
    pthread_mutex_unlock(&exit_from_limit_mux);

    pthread_mutex_lock(&robot_state_mux);
    if((robot_state == RICERCA_CENTRO) || (robot_state == CENTRAGGIO))
    {
      if(robot_state == RICERCA_CENTRO)
        homing_executed = 1;

      OK("CT2");
      fflush(stdout);

      robot_state = CENTRATO;
      pthread_mutex_unlock(&robot_state_mux);

    }
    else if((robot_state == ACCESO) || (robot_state == EMERGENZA) || (robot_state == RILASCIATO))
    {
      robot_state = INIZIALIZZATO;
      pthread_mutex_unlock(&robot_state_mux);
      homing_executed = 0;

      OK("CT0");
    }
    else if(robot_state == IN_POSIZIONE)
    {
      robot_state = FERMO;
      pthread_mutex_unlock(&robot_state_mux);

      OK("CT1");
      fflush(stdout);
    }
    else
      pthread_mutex_unlock(&robot_state_mux);
  }
  else
    pthread_mutex_unlock(&exit_from_limit_mux);
}

UNS32 OnStatusUpdate(CO_Data* d, const indextable * indextable_curr, UNS8 bSubindex)
{
  UNS8 nodeid = NodeId;

  /** Fault management **/

  if(fake_flag == 0)
  {
    motor_status[nodeid] = Statusword;
    motor_mode[nodeid] = Modes_of_operation_display;
  }

  OnInterpUpdate(d, nodeid);

  // Bus voltage fault
  if((motor_status[nodeid] & 0b0000000000010000) == 0)
  {
    if(robot_state != EMERGENZA)
    {
      add_event(CERR_BusVoltageFault, nodeid, 0, NULL);

      if(fake_flag == 0)
      {
        struct state_machine_struct *origin_machine[] =
        {
        &smart_stop_machine
        };

        _machine_exe(d, nodeid, &SmartBusVoltageCallback, origin_machine, 1, 1, 0);
      }
      else
      {
        motor_mode[nodeid] = 0x3;
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("SUCC[node %x]: smart motor stopped\n", nodeid);
        }
#endif

        SmartBusVoltageCallback(d, nodeid, 0, 0, 0);
      }
    }
  }

  // se sono in modalità posizione ed il motore è nello stato "operation enabled"
  if(motor_mode[nodeid] == 0x1)
  {
    pthread_mutex_lock(&robot_state_mux);

    if(((robot_state == RICERCA_CENTRO) || (robot_state == CENTRAGGIO) || (robot_state == ACCESO)
        || (robot_state == EMERGENZA) || (robot_state == RILASCIATO)
        || (robot_state == IN_POSIZIONE))
        && ((motor_status[nodeid] & 0b0000000001101111) == 0b0000000000100111))
    {

      pthread_mutex_unlock(&robot_state_mux);

      // se ha concluso la tragliettoria
      if((motor_status[nodeid] & 0b0001010000000000) == 0b0001010000000000)
      {
        if(fake_flag == 0)
        {
          struct state_machine_struct *machine = &smart_stop_machine;

          _machine_exe(CANOpenShellOD_Data, nodeid, &SmartPositionTargetCallback, &machine, 1, 1,
              0);
        }
        else
        {
          motor_mode[nodeid] = 0x3;
          smartmotor_path_reset(nodeid, &motor_status[nodeid]);
          //motor_status[nodeid] &= ~0b0001010000000000;

#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf("SUCC[node %x]: smart motor stopped\n", nodeid);
          }
#endif
          SmartPositionTargetCallback(CANOpenShellOD_Data, nodeid, 0, 0, 0);
        }
      }
    }
    else
      pthread_mutex_unlock(&robot_state_mux);
  }

// Se mi trovo in modalità homing
  if(motor_mode[nodeid] == 0x6)
  {
    // Problemi nel concludere l'homing
    if((motor_status[nodeid] & 0b0010000000000000) > 0)
    {
      CERR("CT2", CERR_MoveError);

      if(motor_started[nodeid] == 1)
      {
        if(fake_flag == 0)
        {
          struct state_machine_struct *stop_machine[] =
          {
          &smart_stop_machine
          };

          _machine_exe(d, nodeid, &SmartFaultCallback, stop_machine, 1, 1, 0);
        }
        else
        {
          motor_mode[nodeid] = 0x3;

#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf("SUCC[node %x]: smart motor stopped\n", nodeid);
          }
#endif
          SmartFaultCallback(d, nodeid, 0, 0, 0);
        }
      }
    }

    // Homing concluso
    else if((motor_status[nodeid] & 0b0001010000000000) == 0b0001010000000000)
    {
      if(motor_started[nodeid] == 1)
      {
        int motor_table_index = MotorTableIndexFromNodeId(nodeid);

        struct state_machine_struct *origin_machine[] =
        {
        &smart_limit_enable_machine, &smart_position_set_machine, &smart_position_start_machine
        };

        _machine_exe(CANOpenShellOD_Data, nodeid, NULL, origin_machine, 3, 1, 4, 10, 10,
            motor_table[motor_table_index].backward_velocity, 0);
      }
    }
  }

  // Stato di fault CiA 402
  if((motor_status[nodeid] & 0b0000000001001111) == 0b0000000000001000)
  {
    // Move error
    if((motor_status[nodeid] & 0b0010000000000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Move error\n", SmartMotorError, nodeid);
      }
#endif
      add_event(CERR_MoveError, nodeid, 0, NULL);
    }

    pthread_mutex_lock(&robot_state_mux);
    if((robot_state != ACCESO) && (robot_state != EMERGENZA) && (robot_state != RICERCA_CENTRO)
        && (robot_state != RILASCIATO))
    {
      pthread_mutex_unlock(&robot_state_mux);

      add_event(CERR_MotorFault, nodeid, 0, NULL);

      if(fake_flag == 0)
      {
        struct state_machine_struct *fault_machine[] =
        {
        &smart_statusword_machine, &smart_stop_machine
        };

        _machine_exe(d, 0, &SmartFaultCallback, fault_machine, 2, 1, 1, 0x1);
      }
      else
      {
        pthread_mutex_lock(&robot_state_mux);
        if(robot_state == SIMULAZIONE)
        {
          motor_interp_status[nodeid] = 0x122d;

          motor_position[nodeid] = InterpolationData[nodeid - MOTOR_INDEX_FIRST];
        }

        pthread_mutex_unlock(&robot_state_mux);

        motor_mode[nodeid] = 0x3;

#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("SUCC[node %x]: smart motor stopped\n", nodeid);
        }
#endif
        SmartFaultCallback(d, nodeid, 0, 0, 0);
      }
    }
    else if(robot_state == RILASCIATO)
    {
      pthread_mutex_unlock(&robot_state_mux);

      if(fake_flag == 0)
      {
        struct state_machine_struct *fault_machine[] =
        {
        &smart_statusword_machine, &smart_off_machine
        };

        _machine_exe(CANOpenShellOD_Data, nodeid, &SmartFaultCallback, fault_machine, 2, 1, 1, 0x1);
      }
      else
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("SUCC[node %x]: Status word read\n", nodeid);
          printf("SUCC[node %x]: smartmotor off\n", nodeid);
        }
#endif
      }
    }
    else
      pthread_mutex_unlock(&robot_state_mux);
  }

  // Quick stop
  if((motor_status[nodeid] & 0b0000000001101111) == 0b0000000000000111)
  {
#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
    {
      printf("ERR[%d on node %x]: Quick stop active\n", SmartMotorError, nodeid);
    }
#endif
    add_event(CERR_QuickStop, nodeid, 0, NULL);

    SmartStop(0, 1);
  }

  return 0;
}

UNS32 OnInterpUpdate(CO_Data* d, UNS8 nodeid)
{
  if(fake_flag == 0)
    motor_interp_status[nodeid] = Interpolation_Mode_Status;

  // confronto lo stato precedente dell'interpolazione con quello attuale
  // per capire se ci sono stati cambiamenti
  //if(motor_interp_status[nodeid] != 0)
  //{
  // Interpolation started
  if(motor_mode[nodeid] == 0x7)
  {
    if(((motor_interp_status[nodeid] & 0b1000000000000000) > 0) && (motor_started[nodeid] == 0))
    {
      motor_started[nodeid] = 1;
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("INF[%d on node %x]: Interpolation started (sin IP mode)\n", SmartMotorError,
            nodeid);
      }
#endif
    }
    // Interpolation finished
    else if(((motor_interp_status[nodeid] & 0b1000000100000000) == 0)
        && (motor_started[nodeid] == 2))
    {
      motor_started[nodeid] = 0;

#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("INF[%d on node %x]: Interpolation finished (sin IP mode)\n", SmartMotorError,
            nodeid);
      }
#endif

      SimulationTableEnd(d, nodeid, 0, 0);
      if(fake_flag)
      {
//motor_mode[nodeid] = 0x3;

#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("SUCC[node %x]: smart motor stopped\n", nodeid);
        }
#endif
      }

      return 0;
    }

    //}

    if((motor_interp_status[nodeid] & 0b0000000001000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Position error tolerance exceeded (IP mode)\n", SmartMotorError,
            nodeid);
      }
#endif
      add_event(CERR_InterpPositionError, nodeid, 0, NULL);
    }

    if((motor_interp_status[nodeid] & 0b0000010000000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Invalid time units (IP mode)\n", SmartMotorError, nodeid);
      }
#endif
      add_event(CERR_InterpInvalidTimeError, nodeid, 0, NULL);
    }

    if((motor_interp_status[nodeid] & 0b0000100000000000) > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Invalid position increment (IP mode)\n", SmartMotorError,
            nodeid);
      }
#endif
      add_event(CERR_InterpInvalidPositionError, nodeid, 0, NULL);
    }

    if((motor_interp_status[nodeid] & 0b0110000000000000) > 0)
    {
      if((motor_interp_status[nodeid] & 0b0100000000000000) > 0)
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("ERR[%d on node %x]: FIFO underflow (IP mode)\n", SmartMotorError, nodeid);
        }
#endif
        add_event(CERR_InterpFIFOUnderError, nodeid, 0, NULL);
      }
      else if((motor_interp_status[nodeid] & 0b0010000000000000) > 0)
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("ERR[%d on node %x]: FIFO overflow (IP mode)\n", SmartMotorError, nodeid);
        }
#endif
        add_event(CERR_InterpFIFOOverError, nodeid, 0, NULL);
      }

      /*int motor_table_index = MotorTableIndexFromNodeId(nodeid);
       pthread_mutex_lock(&motor_table[motor_table_index].table_mutex);
       printf("INF[%d]: count = %d, cursor = %ld, end = %d\n, read = %d, write = %d, interp = %x\n",
       nodeid, motor_table[motor_table_index].count,
       motor_table[motor_table_index].cursor_position, motor_table[motor_table_index].end_reached,
       motor_table[motor_table_index].read_pointer, motor_table[motor_table_index].write_pointer,
       motor_interp_status[nodeid]);
       pthread_mutex_unlock(&motor_table[motor_table_index].table_mutex);*/

      SmartStop(0, 1);
    }
    else
    {
      pthread_mutex_lock(&robot_state_mux);
      if(robot_state == SIMULAZIONE)
      {
        pthread_mutex_unlock(&robot_state_mux);

        SimulationTableStart(d);

// Ho bisogno di lasciarmi un posto per un'eventuale chiusura della tabella, dovuta,
// per esempio, dalla ricezione di un errore dai motori
        SimulationTableUpdate(d, nodeid, motor_interp_status[nodeid],
            (motor_interp_status[nodeid] & 0x3F) - 1, 1);
      }
      else
        pthread_mutex_unlock(&robot_state_mux);
    }
  }

  return 0;
}

void SimulationInitCallback(CO_Data* d, UNS8 nodeid, int machine_state, int is_register,
UNS32 return_value)
{
  if(is_register == 0)
  {
    if(return_value)
    {
      CERR("CT4", InternalError);
      return;
    }
  }
  else
  {
    printf("Statusword3: %ld\n", return_value);
  }
}

void SimulationTableEnd(CO_Data* d, UNS8 nodeId, int machine_state,
UNS32 return_value)
{
  int motor_index = 0;
  int stop_in_progress = 0;

  if(return_value)
  {
    CERR("CT4", CERR_InternalError);

    return;
  }

  SmartClear(nodeId);

  if(fake_flag)
    smartmotor_table_reset(nodeId, &motor_interp_status[nodeId]);

  for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    stop_in_progress |= motor_started[motor_table[motor_index].nodeId];

  if(stop_in_progress == 0)
  {
    if(fake_flag)
    {
      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      {
        NodeId = motor_table[motor_index].nodeId;

        motor_status[NodeId] = 0b0000010000110111;
        motor_interp_status[NodeId] = 0x122d;
        motor_mode[NodeId] = 0x1;
        //printf("[%d] motor_status %d on table end\n", NodeId, motor_status[NodeId]);

      }
    }

    pthread_mutex_lock(&robot_state_mux);
    if(robot_state != FERMO)
    {
      robot_state = FERMO;
      pthread_mutex_unlock(&robot_state_mux);

      OK("CT4");
      fflush(stdout);
    }
    else
      pthread_mutex_unlock(&robot_state_mux);
  }
}
void SimulationTableStart(CO_Data* d)
{
  int motor_index = 0;
  int ready = 0;

  for(motor_index = 0; motor_index < motor_active_number; motor_index++)
  {
    if(simulation_ready[motor_index])
      ready++;
  }

  if(ready == motor_active_number)
  {
    if(fake_flag == 0)
    {
      InterpolationStart = 0x1f;

//d->PDO_status[0].last_message.cob_id = 0;
      sendPDOevent(d);
    }
    else
    {
      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
        motor_interp_status[motor_table[motor_index].nodeId] |= 0b1000000000000000;
    }

    for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      simulation_ready[motor_index] = 0;
  }
}

void SimulationTableUpdate(CO_Data* d, UNS8 nodeid, UNS16 interpolation_status, int point_number,
    int from_callback)
{
  int motor_table_index = MotorTableIndexFromNodeId(nodeid);

  if(motor_table_index < 0)
  {
#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
    {
      printf("ERR[%d on node %x]: Impossibile trovare la tabella associata. \n", InternalError,
          nodeid);
    }
#endif

    CERR("CT4", CERR_InternalError);
    return;
  }

  /*if(motor_table[motor_table_index].is_pipe)
   {
   if((interpolation_status & 0x3F) < 20)
   return;

   point_number = 20;
   }*/

  if((motor_active[nodeid] == 0) || (point_number <= 0))
    return;

  pthread_mutex_lock(&interpolator_mux[nodeid]);

  if(interpolator_busy[nodeid] == 0)
    interpolator_busy[nodeid] = 1;
  else
  {
    printf("interpolator busy %d\n", nodeid);

    pthread_mutex_unlock(&interpolator_mux[nodeid]);

    return;
  }

  pthread_mutex_unlock(&interpolator_mux[nodeid]);

  int i;
  int point_to_send;
  int valid_point = 0;
  int get_result = 0;
  int send_pdo_result = 0;
  struct table_data_read data_read;

  pthread_mutex_lock(&motor_table[motor_table_index].table_mutex);

  if(point_number >= motor_table[motor_table_index].count)
    point_to_send = motor_table[motor_table_index].count;
  else
    point_to_send = point_number;

  pthread_mutex_unlock(&motor_table[motor_table_index].table_mutex);

  /*if(motor_table[motor_table_index].is_pipe == 1)
    QueueSeek(&motor_table[motor_table_index], point_to_send);*/

  // se i punti da scrivere sono 44, allora il contatore deve andare da
  // 0 a 43
  for(i = 0; i < point_to_send; i++)
  {
    // QueueGet non dovrebbe avere bisogno del mutex in quanto il refiller
    // cambia soltanto il puntatore write, mentre la prima utilizza il read
    get_result = QueueGet(&motor_table[motor_table_index], &data_read, i);

    if(get_result == -1)
    {
      continue;
    }
    else if(get_result == -2)
    {
      break;
    }

    valid_point++;

    InterpolationTimePeriod[nodeid - MOTOR_INDEX_FIRST] = -3;

    while(data_read.time_ms > 256)
    {
      data_read.time_ms = data_read.time_ms / 10;

      InterpolationTimePeriod[nodeid - MOTOR_INDEX_FIRST]++;
    }

    InterpolationTimeValue[nodeid - MOTOR_INDEX_FIRST] = data_read.time_ms;
    InterpolationData[nodeid - MOTOR_INDEX_FIRST] = data_read.position;

    if(fake_flag == 0)
    {
      d->PDO_status[nodeid - MOTOR_INDEX_FIRST + INTERPOLATION_DATA_INDEX_OFFSET].last_message.cob_id =
          0;

      send_pdo_result = sendPDOevent(d);

      if(send_pdo_result != 0)
        printf("Errore PDO!\n");
    }
    else
    {
//motor_position[nodeid] = InterpolationData[nodeid - MOTOR_INDEX_FIRST];
      smartmotor_table_write(nodeid, &motor_interp_status[nodeid],
          InterpolationData[nodeid - MOTOR_INDEX_FIRST],
          InterpolationTimeValue[nodeid - MOTOR_INDEX_FIRST],
          InterpolationTimePeriod[nodeid - MOTOR_INDEX_FIRST]);
    }
  }

  //printf("[%d] %d / %d / %d\n", nodeid, valid_point, point_to_send, point_number);

  // per uscire dalla modalità ip mode devo impostare l'unità temporale a zero
  // e scrivere l'ultimo valore nella tabella.
  // Devo utilizzare l'sdo perchè altrimenti il dato non partirebbe in quanto è uguale
  // a quello precendente
  // Se il motore sta elaborando la tabella e il riempitore di tabella ha finito i punti,
  // significa che posso bloccare il movimento
  if((motor_table[motor_table_index].count == 0) // tutti i punti letti sono stati scritti
  && (motor_table[motor_table_index].end_reached == 1)        // file concluso
      && (valid_point < (interpolation_status & 0x3F)) // punti nella tabella
      && (motor_started[nodeid] != 2) // non ho ancora finalizzato la tabella
      && ((interpolation_status & 0x8000) > 0))
  {
    // devo forzare l'aggiornamento dello stato dell'interpolatore. Il problema
    // sorge quando ci sono pochi punti e chiudo subito la tabella. In questo caso
    // all'aggiornamento dello stato tramite callback non passo per lo start.
    //printf("stopping simulation \n");
    motor_started[nodeid] = 2;
    QueueLast(&motor_table[motor_table_index], &data_read);

    InterpolationTimePeriod[nodeid - MOTOR_INDEX_FIRST] = 0;
    InterpolationTimeValue[nodeid - MOTOR_INDEX_FIRST] = 0;
    InterpolationData[nodeid - MOTOR_INDEX_FIRST] = data_read.position;

//TODO: valutare se è possibile usare il PDO aggiornando il flag di scrittura invece che un SDO
    if(fake_flag == 0)
    {
//d->PDO_status[0].last_message.cob_id = 0;
      sendPDOevent(d);

      struct state_machine_struct *interpolation_machine[] =
      {
      &stop_interpolation_machine
      };

      _machine_exe(CANOpenShellOD_Data, nodeid, NULL, interpolation_machine, 1, 1, 1,
          data_read.position);
    }
    else
    {
      smartmotor_table_write(nodeid, &motor_interp_status[nodeid],
          InterpolationData[nodeid - MOTOR_INDEX_FIRST],
          InterpolationTimeValue[nodeid - MOTOR_INDEX_FIRST],
          InterpolationTimePeriod[nodeid - MOTOR_INDEX_FIRST]);

      /*motor_interp_status[nodeid] = 0x122d;

       motor_position[nodeid] =
       InterpolationData[nodeid - MOTOR_INDEX_FIRST];*/
    }
  }

  // Qui uso il mutex in quanto potrebbe esserci un race condition
  // sul count
  pthread_mutex_lock(&motor_table[motor_table_index].table_mutex);
  QueueUpdate(&motor_table[motor_table_index], point_to_send);
  pthread_mutex_unlock(&motor_table[motor_table_index].table_mutex);

  if(valid_point != 0)
  {
    // se la simulazione non è stata avviata ancora
    if(((interpolation_status & 0b1000000000000000) == 0))
    {
      int motor_table_index = MotorTableIndexFromNodeId(nodeid);
      simulation_ready[motor_table_index] = 1;
    }
  }

  pthread_mutex_lock(&interpolator_mux[nodeid]);
  interpolator_busy[nodeid] = 0;
  pthread_mutex_unlock(&interpolator_mux[nodeid]);

}

void SmartVelocityCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  printf("INFO[%d on node %x]: VT = %ld\n", SmartMotorError, nodeId, (long) return_value);
}

void SmartAccelerationCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  printf("INFO[%d on node %x]: ADT = %ld\n", SmartMotorError, nodeId, (long) return_value);
}

void SmartFollowingErrorCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  printf("INFO[%d on node %x]: EL = %ld\n", SmartMotorError, nodeId, (long) return_value);
}

void SmartVelocityGet(UNS8 nodeid)
{
  struct state_machine_struct *velocity_machine[] =
  {
  &smart_velocity_pp_get_machine
  };

  _machine_exe(CANOpenShellOD_Data, nodeid, &SmartVelocityCallback, velocity_machine, 1, 0, 0);
}

void SmartAccelerationGet(UNS8 nodeid)
{
  struct state_machine_struct *acceleration_machine[] =
  {
  &smart_acceleration_pp_get_machine
  };

  _machine_exe(CANOpenShellOD_Data, nodeid, &SmartAccelerationCallback, acceleration_machine, 1, 0,
      0);
}

void SmartFollowingErrorGet(UNS8 nodeid)
{
  struct state_machine_struct *following_error_machine[] =
  {
  &smart_following_error_get_machine
  };

  _machine_exe(CANOpenShellOD_Data, nodeid, &SmartFollowingErrorCallback, following_error_machine,
      1, 0, 0);
}

void SmartVelocitySet(char *sdo)
{
  int ret;

  int nodeid;
  long VT = 0;

  ret = sscanf(sdo, "sVTS#%2x,%4lx", &nodeid, &VT);

  if(ret == 2)
  {
    struct state_machine_struct *velocity_machine[] =
    {
    &smart_velocity_pp_set_machine
    };

    _machine_exe(CANOpenShellOD_Data, nodeid, NULL, velocity_machine, 1, 0, 1, VT);
  }
}

void SmartCheckCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  if(return_value)
  {
    pthread_mutex_lock(&robot_state_mux);

    switch(robot_state)
    {
      case INIZIALIZZATO:
        CERR("CT2", CERR_InternalError);
        break;
    }

    pthread_mutex_unlock(&robot_state_mux);
  }
}

void SmartHome(char *sdo)
{
  int ret;

  int nodeid;
  long home_offset = 0;
  long forward_velocity = 0;
  long backward_velocity = 0;

  ret = sscanf(sdo, "shom#%2x,%lx,%lx,%lx", &nodeid, &home_offset, &forward_velocity,
      &backward_velocity);

  if(ret >= 2)
  {
    if(nodeid == 0)
    {
      int motor_index;
      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      {
        simulation_first_start[motor_table[motor_index].nodeId] = 1;
        motor_started[motor_table[motor_index].nodeId] = 1;
      }
    }
    else
    {
      if(motor_active[nodeid])
      {
        motor_started[nodeid] = 1;
        simulation_first_start[nodeid] = 1;
      }
    }

    pthread_mutex_lock(&exit_from_limit_mux);
    exit_from_limit_complete = 0;
    pthread_mutex_unlock(&exit_from_limit_mux);

    struct state_machine_struct *machine = &smart_homing_machine;

    _machine_exe(CANOpenShellOD_Data, nodeid, &SmartCheckCallback, &machine, 1, 0, 3,
        forward_velocity, backward_velocity, home_offset);
  }
  else
    printf("Wrong command  : %s\n", sdo);

}

void SmartIntTest2(UNS8 nodeid)
{
  struct state_machine_struct *machine = &smart_interpolation_test2_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 0);
}

void SmartFileComplete(UNS8 nodeid)
{
  float complete_percent = 0;

  if(nodeid == 0)
  {
    int i;
    for(i = 1; i < CANOPEN_NODE_NUMBER; i++)
    {
      if(motor_active[i] > 0)
      {
        complete_percent = FileCompleteGet(i,
        SMART_TABLE_SIZE - (motor_interp_status[i] & 0x3F));

        printf("INFO[%d on node %x]: Percentuale completamento: %.0f%%\n", InternalError, nodeid,
            complete_percent);
      }
    }
  }
  else
  {
    complete_percent = FileCompleteGet(nodeid,
    SMART_TABLE_SIZE - (motor_interp_status[nodeid] & 0x3F));

    printf("INFO[%d on node %x]: Percentuale completamento: %.0f%%\n", InternalError, nodeid,
        complete_percent);
  }
}

void SmartIntTest1(UNS8 nodeid)
{
  struct state_machine_struct *machine = &smart_interpolation_test1_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 0);
}

void FakePositionUpdate(sigval_t val)
{
  int motor_index;

  for(motor_index = 0; motor_index < motor_active_number; motor_index++)
  {
    NodeId = motor_table[motor_index].nodeId;

    OnPositionUpdate(CANOpenShellOD_Data, NULL, 0);
  }

  //timer_delete(fake_update_timer);
}

int SimulationStart(UNS8 nodeid)
{
  InterpolationStart = 0xF;

  // devo distinguere il caso di broadcast dagli altri per i parametri strettamente
  // legati al motore
  if(nodeid != 0)
  {
    int motor_table_index = MotorTableIndexFromNodeId(nodeid);
    simulation_ready[motor_table_index] = 0;

    if(robot_state != MOVIMENTO_LIBERO)
    {
      if(motor_table_index >= 0)
      {
        motor_table[motor_table_index].is_pipe = 0;

        QueueInit(motor_table[motor_table_index].nodeId, &motor_table[motor_table_index]);

        if(QueueFill(&motor_table[motor_table_index]) < 0)
        {
          CERR("CT4", CERR_FileError);
          return -1;
        }
      }
    }

    if(simulation_first_start[nodeid])
    {
      simulation_first_start[nodeid] = 0;

      struct state_machine_struct *machine = &init_interpolation_machine;
      _machine_exe(CANOpenShellOD_Data, nodeid, &SimulationInitCallback, &machine, 1, 0, 1,
          motor_position[nodeid]);
    }
    else
    {
      struct state_machine_struct *machine = &resume_interpolation_machine;
      _machine_exe(CANOpenShellOD_Data, nodeid, &SimulationInitCallback, &machine, 1, 0, 1,
          motor_position[nodeid]);
    }
  }
  else
  {
    struct state_machine_struct *init_interpolation = &init_interpolation_machine;
    struct state_machine_struct *resume_interpolation = &resume_interpolation_machine;

    int motor_index;
    for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    {
      simulation_ready[motor_index] = 0;

      if(robot_state != MOVIMENTO_LIBERO)
      {
        motor_table[motor_index].is_pipe = 0;

        QueueInit(motor_table[motor_index].nodeId, &motor_table[motor_index]);

        if(QueueFill(&motor_table[motor_index]) < 0)
        {
          CERR("CT4", CERR_FileError);
          return -1;
        }
      }

      if(simulation_first_start[motor_table[motor_index].nodeId])
      {
        simulation_first_start[motor_table[motor_index].nodeId] = 0;

        if(fake_flag == 0)
          _machine_exe(CANOpenShellOD_Data, motor_table[motor_index].nodeId,
              &SimulationInitCallback, &init_interpolation, 1, 0, 1,
              motor_position[motor_table[motor_index].nodeId]);
        else
        {
          SimulationInitCallback(CANOpenShellOD_Data, motor_table[motor_index].nodeId, 0, 0, 0);

          NodeId = motor_table[motor_index].nodeId;
          motor_status[NodeId] = 0b0000000000110111;
          motor_interp_status[NodeId] = 0x122d;
          motor_mode[NodeId] = 0x7;
        }
      }
      else
      {
        if(fake_flag == 0)
          _machine_exe(CANOpenShellOD_Data, motor_table[motor_index].nodeId,
              &SimulationInitCallback, &resume_interpolation, 1, 0, 1,
              motor_position[motor_table[motor_index].nodeId]);
        else
        {
          SimulationInitCallback(CANOpenShellOD_Data, motor_table[motor_index].nodeId, 0, 0, 0);

          NodeId = motor_table[motor_index].nodeId;
          motor_status[NodeId] = 0b0000000000110111;
          motor_interp_status[NodeId] = 0x122d;
          motor_mode[NodeId] = 0x7;
        }
      }
    }
  }

  return 0;
}

void SmartIntStart(UNS8 nodeid)
{
  struct state_machine_struct *machine = &start_interpolation_machine;
  _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 0);
}

void SmartPositionCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  /*struct state_machine_struct *machine[] =
   {
   &smart_position_start_machine
   };

   int motor_index;
   for(motor_index = 0; motor_index < motor_active_number; motor_index++)
   {
   if(motor_started[motor_table[motor_index].nodeId])
   _machine_exe(d, motor_table[motor_index].nodeId, NULL, machine, 1, 1, 0);
   }*/
  int send_pdo_result;

  InterpolationStart = 0x3f;

  d->PDO_status[INTERPOLATION_START_INDEX_OFFSET].last_message.cob_id = 0;

  send_pdo_result = sendPDOevent(d);

  if(send_pdo_result != 0)
    printf("Errore PDO!\n");
}

/**
 *
 */
void SmartPosition(UNS8 nodeid, long position, long velocity, long acceleration, int start,
    int from_callback)
{
  int motor_table_index;
  MachineCallback_t machine_callback = NULL;

  InterpolationStart = 0x2f;

  CANOpenShellOD_Data->PDO_status[INTERPOLATION_START_INDEX_OFFSET].last_message.cob_id = 0;

  int send_pdo_result;
  send_pdo_result = sendPDOevent(CANOpenShellOD_Data);

  if(send_pdo_result != 0)
    printf("Errore PDO!\n");

  struct state_machine_struct *machine[] =
  {
  &smart_position_set_machine, &smart_position_start_machine
  };

  if(start)
    machine_callback = &SmartPositionCallback;

  if(nodeid == 0)
  {
    int motor_index;
    for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    {
      simulation_first_start[motor_table[motor_index].nodeId] = 1;
//motor_started[motor_table[motor_index].nodeId] = 0;

      if(fake_flag)
      {
        smartmotor_path_reset(motor_table[motor_index].nodeId,
            &motor_status[motor_table[motor_index].nodeId]);

        smartmotor_path_generate(motor_table[motor_index].nodeId, 8000,
            motor_position[motor_table[motor_index].nodeId], position, velocity, acceleration);

        motor_started[motor_table[motor_index].nodeId] = 1;
        motor_mode[motor_table[motor_index].nodeId] = 0x1;
        //motor_position[motor_table[motor_index].nodeId] = position;
        if(start)
          motor_status[motor_table[motor_index].nodeId] |= 0b0001000000000000;

#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("SUCC[node %x]: set target point\n", motor_table[motor_index].nodeId);
          printf("SUCC[node %x]: smart motor go to target point. . .\n",
              motor_table[motor_index].nodeId);
        }
#endif
      }
      else
      {
        if(motor_started[motor_table[motor_index].nodeId] == 0)
        {
          motor_started[motor_table[motor_index].nodeId] = 1;
          QueueInit(motor_table[motor_index].nodeId, &motor_table[motor_index]);

          _machine_exe(CANOpenShellOD_Data, motor_table[motor_index].nodeId, NULL, machine, 2,
              from_callback, 4, acceleration, acceleration, velocity, position);
        }
        else
        {
          VelocityProfile[motor_table[motor_index].nodeId - MOTOR_INDEX_FIRST] = velocity;
          PositionTarget[motor_table[motor_index].nodeId - MOTOR_INDEX_FIRST] = position;

          CANOpenShellOD_Data->PDO_status[motor_table[motor_index].nodeId - MOTOR_INDEX_FIRST
              + TARGET_POSITION_INDEX_OFFSET].last_message.cob_id = 0;

          int send_pdo_result;
          send_pdo_result = sendPDOevent(CANOpenShellOD_Data);

          if(send_pdo_result != 0)
            printf("Errore PDO!\n");

          if(start)
            SmartPositionCallback(CANOpenShellOD_Data, motor_table[motor_index].nodeId, 0, 0, 0);
        }
      }
    }
  }
  else
  {
    if(motor_active[nodeid])
    {
      simulation_first_start[nodeid] = 1;
//motor_started[nodeid] = 1;
    }

    if(fake_flag == 0)
    {
      if(motor_started[nodeid] == 0)
      {
        motor_started[nodeid] = 1;

        motor_table_index = MotorTableIndexFromNodeId(nodeid);

        QueueInit(nodeid, &motor_table[motor_table_index]);

        _machine_exe(CANOpenShellOD_Data, nodeid, machine_callback, machine, 1, from_callback, 4,
            acceleration, acceleration, velocity, position);

      }
      else
      {
        //printf("[%d] Resume position %ld with velocity %ld\n", nodeid, position, velocity);
        VelocityProfile[nodeid - MOTOR_INDEX_FIRST] = velocity;
        PositionTarget[nodeid - MOTOR_INDEX_FIRST] = position;

        CANOpenShellOD_Data->PDO_status[nodeid - MOTOR_INDEX_FIRST + TARGET_POSITION_INDEX_OFFSET].last_message.cob_id =
            0;

        send_pdo_result = sendPDOevent(CANOpenShellOD_Data);

        if(send_pdo_result != 0)
          printf("Errore PDO!\n");

        if(start)
          SmartPositionCallback(CANOpenShellOD_Data, nodeid, 0, 0, 0);
      }

      if((from_callback == 0) && (start == 0))
        OK("CT1");
    }
    else
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("SUCC[node %x]: smart motor go to target point. . .\n", nodeid);
      }
#endif
      smartmotor_path_reset(nodeid, &motor_status[nodeid]);

      smartmotor_path_generate(nodeid, 8000, motor_position[nodeid], position, velocity,
          acceleration);

      motor_mode[nodeid] = 0x1;
      motor_started[nodeid] = 1;
//motor_position[nodeid] = position;

      if(start)
      {
        int motor_index;
        for(motor_index = 0; motor_index < motor_active_number; motor_index++)
        {
          motor_status[motor_table[motor_index].nodeId] |= 0b0001000000000000;
        }
      }
      else
        motor_status[nodeid] &= 0b1110111111111111;

      if((from_callback == 0) && (start == 0))
        OK("CT1");
    }
  }
}

void SmartEmergencyCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  int stop_in_progress = 0;
  int motor_index;

  if(return_value)
  {
    CERR("EM2", CERR_InternalError);
    return;
  }

  SmartClear(nodeId);

  for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    stop_in_progress |= motor_started[motor_table[motor_index].nodeId];

  pthread_mutex_lock(&robot_state_mux);
  if(robot_state == SIMULAZIONE)
  {
    pthread_mutex_unlock(&robot_state_mux);
    int motor_table_index = MotorTableIndexFromNodeId(nodeId);
    struct table_data_read data_read;

    QueueLast(&motor_table[motor_table_index], &data_read);

    InterpolationTimePeriod[nodeId - MOTOR_INDEX_FIRST] = 0;
    InterpolationTimeValue[nodeId - MOTOR_INDEX_FIRST] = 0;
    InterpolationData[nodeId - MOTOR_INDEX_FIRST] = data_read.position;

    //d->PDO_status[0].last_message.cob_id = 0;
    sendPDOevent(d);
  }
  else
    pthread_mutex_unlock(&robot_state_mux);

  if(stop_in_progress == 0)
  {
    // Voglio resettare tutti i processi in coda, compreso quello di broadcast, nel caso
    // fosse attivo
    for(motor_index = 0; motor_index < (motor_active_number + 1); motor_index++)
    {
      QueueInit(motor_table[motor_index].nodeId, &motor_table[motor_index]);

      simulation_first_start[motor_table[motor_index].nodeId] = 1;
      motor_started[motor_table[motor_index].nodeId] = 0;

      if(fake_flag)
      {
        NodeId = motor_table[motor_index].nodeId;
        motor_status[NodeId] = 0b0000000000110111;
        motor_interp_status[NodeId] = 0x122d;
        motor_mode[NodeId] = 0x1;
      }
    }

    if(robot_state == SIMULAZIONE)
      CERR("CT4", CERR_SimulationError);

    pthread_mutex_lock(&robot_state_mux);

    if(robot_state != EMERGENZA)
    {
      robot_state = EMERGENZA;
      OK("EM2");
    }

    pthread_mutex_unlock(&robot_state_mux);
  }
}

void SmartStopCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  int stop_in_progress = 0;
  int motor_index;

  if(return_value)
  {
    if(robot_state == SIMULAZIONE)
      CERR("CT5", CERR_InternalError);
    else if(robot_state == INIZIALIZZATO)
      CERR("CT2", CERR_InternalError);
    else if(robot_state == MOVIMENTO_LIBERO)
      CERR("CB5", CERR_SimulationError);
    else
      add_event(InternalError, nodeId, 0, NULL);

    return;
  }

  SmartClear(nodeId);

  for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    stop_in_progress |= motor_started[motor_table[motor_index].nodeId];

  pthread_mutex_lock(&robot_state_mux);
  if(robot_state == SIMULAZIONE)
  {
    pthread_mutex_unlock(&robot_state_mux);

    int motor_table_index = MotorTableIndexFromNodeId(nodeId);
    struct table_data_read data_read;

    //for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    //{
    QueueLast(&motor_table[motor_table_index], &data_read);

    InterpolationTimePeriod[nodeId - MOTOR_INDEX_FIRST] = 0;
    InterpolationTimeValue[nodeId - MOTOR_INDEX_FIRST] = 0;
    InterpolationData[nodeId - MOTOR_INDEX_FIRST] = data_read.position;
    //}

    //d->PDO_status[0].last_message.cob_id = 0;
    sendPDOevent(d);
  }
  else
    pthread_mutex_unlock(&robot_state_mux);

  // entro in questo ramo solo se tutti i motori si sono fermati. Questo vuol dire
  // che, nel caso dovessi agire su qualche impostazione globale, devo usare un for
  // per tutti i motori
  if(stop_in_progress == 0)
  {
    if(fake_flag)
    {
      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      {
        NodeId = motor_table[motor_index].nodeId;
        motor_status[NodeId] = 0b0000010000110111;
        motor_interp_status[NodeId] = 0x122d;
        motor_mode[NodeId] = 0x1;
        //printf("[%d] motor_status %d on smart stop\n", NodeId, motor_status[NodeId]);

      }
    }

    // Voglio resettare tutti i processi in coda, compreso quello di broadcast, nel caso
    // fosse attivo
    for(motor_index = 0; motor_index < (motor_active_number + 1); motor_index++)
      QueueInit(motor_table[motor_index].nodeId, &motor_table[motor_index]);

    pthread_mutex_lock(&robot_state_mux);
    if((robot_state == SIMULAZIONE) || (robot_state == RILASCIATO)
        || (robot_state == MOVIMENTO_LIBERO))
    {
      if(robot_state == SIMULAZIONE)
      {
        CERR("CT4", CERR_SimulationError);
        OK("CT5");
      }
      else if(robot_state == MOVIMENTO_LIBERO)
      {
        CERR("CB7", CERR_SimulationError);
        OK("CB5");
      }

      robot_state = FERMO;
    }

    /*else if(robot_state == INIZIALIZZATO)
     {
     CERR("CT2", CERR_ConfigError);
     }*/
    pthread_mutex_unlock(&robot_state_mux);
  }
}

void SmartStop(UNS8 nodeid, int from_callback)
{
  if(fake_flag == 0)
  {
    struct state_machine_struct *machine = &smart_stop_machine;
    _machine_exe(CANOpenShellOD_Data, nodeid, &SmartStopCallback, &machine, 1, from_callback, 0);
  }
  else
  {
    if(nodeid == 0)
    {
      int motor_index;
      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      {
        motor_mode[motor_table[motor_index].nodeId] = 0x3;
        smartmotor_table_reset(motor_table[motor_index].nodeId,
            &motor_interp_status[motor_table[motor_index].nodeId]);

#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("SUCC[node %x]: smart motor stopped\n", motor_table[motor_index].nodeId);
        }
#endif
        SmartStopCallback(CANOpenShellOD_Data, motor_table[motor_index].nodeId, 0, 0, 0);
      }
    }
    else
    {
      motor_mode[nodeid] = 0x3;
      smartmotor_table_reset(nodeid, &motor_interp_status[nodeid]);

#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("SUCC[node %x]: smart motor stopped\n", nodeid);
      }
#endif

      SmartStopCallback(CANOpenShellOD_Data, nodeid, 0, 0, 0);
    }
  }
}

void SmartReleaseBrakeCallback(CO_Data* d, UNS8 Node_ID, int machine_state, int is_register,
UNS32 return_value)
{
  if(return_value)
  {
    CERR("CT2", InternalError);

    return;
  }

  pthread_mutex_lock(&release_mux);
  release_complete++;

  if(release_complete == motor_active_number)
  {
    release_complete = 0;
    pthread_mutex_unlock(&release_mux);

    OK("CT2");
    pthread_mutex_lock(&robot_state_mux);
    robot_state = RILASCIATO;
    pthread_mutex_unlock(&robot_state_mux);
  }
  else
    pthread_mutex_unlock(&release_mux);
}

void SmartRelease(UNS8 nodeid, int from_callback, int brake)
{
  if(fake_flag == 0)
  {
    pthread_mutex_lock(&release_mux);
    release_complete = 0;
    pthread_mutex_unlock(&release_mux);

    if(brake == 0)
    {
      struct state_machine_struct *machine[] =
      {
      &smart_limit_disable_machine, &torque_machine
      };

      _machine_exe(CANOpenShellOD_Data, nodeid, &SmartReleaseBrakeCallback, machine, 2,
          from_callback, 1, 0);
    }
    else
    {
      struct state_machine_struct *machine[] =
      {
      &smart_limit_enable_machine, &smart_off_machine
      };

      _machine_exe(CANOpenShellOD_Data, nodeid, &SmartReleaseBrakeCallback, machine, 2,
          from_callback, 1, 0);
    }
  }
  else
  {
    if(nodeid == 0)
    {
      pthread_mutex_lock(&release_mux);
      release_complete = 0;
      pthread_mutex_unlock(&release_mux);

      int motor_index;
      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      {
        motor_mode[motor_table[motor_index].nodeId] = 0x4;

        SmartReleaseBrakeCallback(CANOpenShellOD_Data, motor_table[motor_index].nodeId, 0, 0, 0);
      }
    }
    else
    {
      motor_mode[nodeid] = 0x4;

      SmartReleaseBrakeCallback(CANOpenShellOD_Data, nodeid, 0, 0, 0);
    }
  }

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

int SmartProgramUpload(CO_Data* d, UNS8 nodeid)
{
  char command[32];

  static char program_read[256];
  static int green_light = 0;
  static int byte_write = 0;

  switch(machine_state)
  {
    case 0:
      program_read[0] = '\0';
//printf("Sending UPLOAD command. . .\n");
      sprintf(command, "UPLOAD");
      writeNetworkDictCallBack(d, nodeid, 0x2500, 0x01, strlen(command),
      visible_string, command, CheckWriteProgramUpload, 0);
      break;

    case 1:
      progress_read:
// Check that the command in progress bit (bit 0) is 1 and
// response reade (bit 2) is 1
      if(raw_response_flag != -1)
      {
        raw_response_flag = -1;

        if((raw_response[0] & 0x3) == 3)
        {
          green_light = 1;
          machine_state++;
          //printf("Motor ready!\n");
          //printf("Start reading. . .\n");

          goto block_read;
        }
        else if((green_light == 1) && ((raw_response[0] & 0x3) == 0))
        {
          green_light = 0;
          raw_response_flag = -1;
          machine_state = -1;
          byte_write = 0;
          printf("%s\n", program_read);
        }
        else if(green_light == 1)
        {
          machine_state++;
          goto block_read;
        }
      }
      else
      {
        raw_response_flag = -1;

        //printf("Checking for green light. . .\n");
        readNetworkDictCallback(d, nodeid, 0x2500, 0x03, 0, CheckReadProgramUpload, 0);
      }
      break;

    case 2:
      block_read:
// Check that the command in progress bit (bit 0) is 1 and
// response reade (bit 2) is 1

      if(raw_response_flag != -1)
      {
        raw_response_flag = -1;

        int i = 0;
        //printf("Block read\n");
        for(i = 0; i < raw_response_size; i++)
        {
          if(raw_response[i] == 0)
            break;

          program_read[byte_write] = (char) raw_response[i];
          byte_write++;
        }

        machine_state--;
        goto progress_read;
      }

      raw_response_flag = -1;

//printf("Reading block. . .\n");
      readNetworkDictCallback(d, nodeid, 0x2500, 0x02, visible_string, CheckReadProgramUpload, 0);

      break;

    default:
      raw_response_flag = -1;
      machine_state = -1;
      break;
  }

  return 0;
}

int SmartProgramDownload(CO_Data* d, UNS8 nodeid)
{
//pthread_mutex_lock(&machine_mux[nodeid]);
  char command[32];
  int count = 0;
  int bytes_read;

  switch(machine_state)
  {
    case 0:
      printf("Sending LOAD command. . .\n");
      sprintf(command, "LOAD");
      writeNetworkDictCallBack(d, nodeid, 0x2500, 0x01, strlen(command),
      visible_string, command, CheckWriteProgramDownload, 0);

      machine_state++;
      break;

    case 1:
// Check that the command in progress bit (bit 0) is 0
      if((raw_response_flag != -1) && ((raw_response[0] & 0x1) == 0))
      {
        raw_response_flag = -1;
        machine_state++;
        printf("Motor ready!\n");
        printf("Start programming. . .\n");

        goto block_write;
      }
      else
      {
        printf("Checking for green light. . .\n");

        raw_response_flag = -1;

        readNetworkDictCallback(d, nodeid, 0x2500, 0x03, 0, CheckReadProgramDownload, 0);
      }
      break;

    case 2:
      block_write:
// Write program 32 bytes of data
      bytes_read = program_file_read(program_file_path, command, 32);

      if(bytes_read == -1)
      {
        printf("Errore [%s]. Errore inaspettato nella lettura del file.\n", strerror(errno));
        machine_state = -1;
      }
      else if(bytes_read == 32)
      {
        for(count = 0; count < bytes_read; count++)
          printf("%c", command[count]);

        writeNetworkDictCallBack(d, nodeid, 0x2500, 0x01, sizeof(command),
        visible_string, command, CheckWriteProgramDownload, 0);

        machine_state--;
      }
      else
      {
        printf("End of writing. . .\n");

        for(count = 0; count < bytes_read; count++)
          printf("%c", command[count]);

        writeNetworkDictCallBack(d, nodeid, 0x2500, 0x01, sizeof(command),
        visible_string, command, CheckWriteProgramDownload, 0);

        machine_state++;
      }
      break;

    case 3:
      machine_state = -1;
      printf("Motor programmed\n");
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
      if(raw_response_flag != -1)
      {
        if((raw_response[0] & 0x1) == 0)
        {
          raw_response_flag = -1;
          machine_state++;
          printf("Motor ready!\n");
        }
        else
        {
          printf("Errore: altro comando in esecuzione\n");
          raw_response_flag = -1;
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
      if((raw_response_flag != -1) && (raw_response[0] & 0x2) == 2)
      {
        printf("Command received\n");
        raw_response_flag = -1;
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
        if(raw_response_flag == -1)
        {
          printf("Read response\n");
          readNetworkDictCallback(d, nodeid, 0x2500, 0x02, visible_string, CheckReadStringRaw, 0);
          break;
        }
        else
        {
          printf("\nResult : %s\n", raw_report);
          raw_response_flag = -1;
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

  ret = sscanf(sdo, "tpd1#%2x,%2x,%3x,%8x,%2x,%4x", &nodeid, &pdo_number, &cob_id, &mapping_object1,
      &transmission_type, &transmission_time);

  struct state_machine_struct *machine = &map1_pdo_machine;

  if((ret >= 3) && (ret <= 6))
  {
    _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 12, 0x1800 | pdo_number,
        0xC0000000 | cob_id, 0x1A00 | pdo_number, 0x1A00 | pdo_number, mapping_object1,
        0x1A00 | pdo_number, 0x1800 | pdo_number, 0x40000000 | cob_id, 0x1800 | pdo_number,
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

  ret = sscanf(sdo, "tpd2#%2x,%2x,%3x,%8x,%8x,%2x,%4x", &nodeid, &pdo_number, &cob_id,
      &mapping_object1, &mapping_object2, &transmission_type, &transmission_time);

  struct state_machine_struct *machine = &map2_pdo_machine;

  if((ret >= 3) && (ret <= 7))
  {
    _machine_exe(CANOpenShellOD_Data, nodeid, NULL, &machine, 1, 0, 14, 0x1800 | pdo_number,
        0xC0000000 | cob_id, 0x1A00 | pdo_number, 0x1A00 | pdo_number, mapping_object1,
        0x1A00 | pdo_number, 0x1A00 | pdo_number, mapping_object2, 0x1A00 | pdo_number,
        0x1800 | pdo_number, 0x40000000 | cob_id, 0x1800 | pdo_number, transmission_type,
        0x1800 | pdo_number, transmission_time);
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
  motor_active_number = 0;

  if(fake_flag == 0)
    masterSendNMTstateChange(CANOpenShellOD_Data, nodeid, NMT_Reset_Node);
}

/* Reset all nodes on the network and print message when boot-up*/
void DiscoverNodes()
{
#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Wait for Slave nodes bootup...\n\n");
  }
#endif
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

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size, &abortCode) != SDO_FINISHED)
    printf("Master : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n", nodeid,
        abortCode);
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
      readNetworkDictCallback(CANOpenShellOD_Data, nodeid, 0x1000, 0x00, 0, CheckReadInfoSDO, 0);
      break;

    case 2: /* Get Vendor ID */
      readNetworkDictCallback(CANOpenShellOD_Data, nodeid, 0x1018, 0x01, 0, CheckReadInfoSDO, 0);
      break;

    case 3: /* Get Product Code */
      readNetworkDictCallback(CANOpenShellOD_Data, nodeid, 0x1018, 0x02, 0, CheckReadInfoSDO, 0);
      break;

    case 4: /* Get Revision Number */
      readNetworkDictCallback(CANOpenShellOD_Data, nodeid, 0x1018, 0x03, 0, CheckReadInfoSDO, 0);
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

    readNetworkDictCallback(CANOpenShellOD_Data, (UNS8) nodeid, (UNS16) index, (UNS8) subindex,
        (UNS8) datatype, CheckReadSDO, 0);
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

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size, &abortCode) != SDO_FINISHED)
    printf("\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n", nodeid,
        abortCode);
  else
    printf("\nResult : %x\n", data);

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(CANOpenShellOD_Data, nodeid, SDO_CLIENT);
}

/* Callback function that check the write SDO demand */
void CheckWriteSDO(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;

  if(getWriteResultNetworkDict(CANOpenShellOD_Data, nodeid, &abortCode) != SDO_FINISHED)
  {
#ifdef CANOPENSHELL_VERBOSE
    printf("\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n", nodeid,
        abortCode);
#endif
    add_event(InternalError, nodeid, 0, NULL);
  }
  else
    OK("PR5");

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
  long data;

  ret = sscanf(sdo, "wsdo#%2x,%4x,%2x,%2x,%lx", &nodeid, &index, &subindex, &size, &data);
  if(ret == 5)
  {
#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
    {
      printf("##################################\n");
      printf("#### Write SDO                ####\n");
      printf("##################################\n");
      printf("NodeId   : %2.2x\n", nodeid);
      printf("Index    : %4.4x\n", index);
      printf("SubIndex : %2.2x\n", subindex);
      printf("Size     : %2.2x\n", size);
      printf("Data     : %lx\n", data);
    }
#endif

    if(fake_flag == 0)
      writeNetworkDictCallBack(CANOpenShellOD_Data, nodeid, index, subindex, size, 0, &data,
          CheckWriteSDO, 0);
    else
      OK("PR5");
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

void UploadFromMotor(UNS8 nodeid)
{
  if(machine_state != -1)
  {
    printf("Error: complex command in progress.\n");
    return;
  }

  if(nodeid == 0)
  {
    printf("Ancora non è possibile programmare più dispositivi insieme\n");
    return;
  }

  printf("##################################\n");
  printf("### Upload firmware from motor ###\n");
  printf("##################################\n");

  machine_state = 0;
  SmartProgramUpload(CANOpenShellOD_Data, nodeid);
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

  ret = sscanf(sdo, "prog#%2x,%s\n", &nodeid, program_file_path);

  if(ret == 2)
  {
    printf("##################################\n");
    printf("### Download firmware to motor ###\n");
    printf("##################################\n");
    printf("NodeId   : %2.2x\n", nodeid);
    printf("File  : %s\n", program_file_path);

    machine_state = 0;
    SmartProgramDownload(CANOpenShellOD_Data, nodeid);
  }
  else
    printf("Wrong command  : %s\n", sdo);
}

void ExitFromLimitCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  if((is_register == 0) && (return_value == 1))
  {
    CERR("CT0", CERR_InternalError);
    return;
  }

  if(fake_flag == 0)
  {
    struct state_machine_struct *position_machine[] =
    {
    &smart_position_set_machine, &smart_position_start_machine
    };

    if((return_value & 0b0100000000000000) > 0) // right(+) limit
    {
      _machine_exe(d, nodeId, NULL, position_machine, 2, 1, 4, 10, 10, 100000, -16000);

      return;
    }
    else if((return_value & 0b1000000000000000) > 0)  // left(-) limit
    {
      _machine_exe(d, nodeId, NULL, position_machine, 2, 1, 4, 10, 10, 100000, 16000);
      return;
    }
  }

  pthread_mutex_lock(&exit_from_limit_mux);
  exit_from_limit_complete++;

  if(exit_from_limit_complete == motor_active_number)
  {
    exit_from_limit_complete = 0;
    pthread_mutex_unlock(&exit_from_limit_mux);

    pthread_mutex_lock(&robot_state_mux);
    robot_state = INIZIALIZZATO;
    pthread_mutex_unlock(&robot_state_mux);

    homing_executed = 0;

    OK("CT0");
    fflush(stdout);
  }
  else
    pthread_mutex_unlock(&exit_from_limit_mux);
}

void ConfigureSlaveNodeCallback(CO_Data* d, UNS8 nodeId, int machine_state, int is_register,
UNS32 return_value)
{
  if(return_value)
    CERR("CT0", CERR_InternalError);
  else
    printf("@M A%d\n", nodeId);
}

void ConfigureSlaveNode(CO_Data* d, UNS8 nodeid)
{
  SmartClear(nodeid);
  _machine_reset(d, nodeid);

  if(motor_active[nodeid] == 0)
  {
    pthread_mutex_lock(&motor_active_number_mutex);

    motor_active_number++;

    // al primo motore che si dichiara, registro i callback per la lettura
    // della statusword e del registro IP
    if(motor_active_number == 1)
    {
      pthread_mutex_unlock(&motor_active_number_mutex);

      if(fake_flag)
      {
        ConfigureSlaveNodeCallback(d, nodeid, 0, 0, 0);

        struct sigevent sigev;

        // Creo il timer e lo avvio
        memset(&sigev, 0, sizeof(struct sigevent));
        sigev.sigev_value.sival_int = 0;
        sigev.sigev_notify = SIGEV_THREAD;
        sigev.sigev_notify_attributes = NULL;
        sigev.sigev_notify_function = FakePositionUpdate;

        if(timer_create(CLOCK_MONOTONIC, &sigev, &fake_update_timer))
        {
#ifdef CANOPENSHELL_VERBOSE
          perror("timer_create()");
#endif
          CERR("CT0", CERR_InternalError);
        }

        long tv_nsec = 10000000;
        time_t tv_sec = 0;
        struct itimerspec timerValues;
        timerValues.it_value.tv_sec = tv_sec;
        timerValues.it_value.tv_nsec = tv_nsec;
        timerValues.it_interval.tv_sec = tv_sec;
        timerValues.it_interval.tv_nsec = tv_nsec;

        timer_settime(fake_update_timer, 0, &timerValues, NULL);

        motor_mode[nodeid] = 0x3;
        motor_status[nodeid] = 0x1637;
        motor_interp_status[nodeid] = 0X102d;
        goto fake_jump;
      }

// CONFIGURE HEARTBEAT TO 100 ms
// MAP TPDO 1 (COB-ID 180) to transmit "node id" (8-bit), "status word" (16-bit), "interpolation mode status" (16-bit), "modes of operation" (8-bit)
// MAP TPDO 2 (COB-ID 280) to transmit "node id" (8-bit), "position actual value" (32-bit)
// MAP TPDO 3 (COB-ID 380) to receive high resolution timestamp

// MAP RPDO 1 (COB-ID 200 + nodeid) to receive "Interpolation Time Index" (8-bit)" (0x60c2 sub2) "Interpolation Time Units" (8-bit)" (0x60c2 sub1)
// MAP RPDO 2 (COB-ID 300 + nodeid) to receive "Profile Velocity" (32-bit) (0x6081 sub0), "Target Position" (32-bit) (0x607a sub0)
// MAP RPDO 3 (COB-ID 400 + nodeid) to receive "Interpolation Data" (32-bit)" (0x06c1 sub1)
// MAP RPDO 4 (COB-ID 400) to receive "Control Word (16-bit)" (0x6040 sub0)

      struct state_machine_struct *configure_pdo_machine[] =
      {
          &heart_start_machine,
          &map4_pdo_machine,
          &map2_pdo_machine,
          &map1_pdo_machine,
          &map2_pdo_machine,
          &map2_pdo_machine,
          &map1_pdo_machine,
          &map1_pdo_machine,
          &smart_start_machine
      };

      _machine_exe(d, nodeid, &ConfigureSlaveNodeCallback, configure_pdo_machine, 9, 1, 97,

      100,

      0x1800, 0xC0000180, 0x1A00, 0x1A00, 0x20000008, 0x1A00, 0x60410010, 0x1A00, 0x24000010,
          0x1A00, 0x60610008, 0x1A00, 0x1800, 0x40000180, 0x1800, SYNC_DIVIDER_STATUS, 0x1800, 0, /*19*/

          0x1801, 0xC0000280, 0x1A01, 0x1A01, 0x20000008, 0x1A01, 0x60630020, 0x1A01, 0x1801,
          0x40000280, 0x1801, 0xfe, 0x1801, 10, /*33*/

          0x1802, 0xC0000380, 0x1A02, 0x1A02, 0x10130020, 0x1A02, 0x1802, 0x40000380, 0x1802,
          SYNC_DIVIDER_TIMESTAMP, 0x1802, 0, /*45*/

          0x1400, 0xC0000200 + nodeid, 0x1600, 0x1600, 0x60c20208, 0x1600, 0x60c20108, 0x1600,
          0x1400, 0x40000200 + nodeid, 0x1400, 0xFE, 0x1400, 0,/*59*/

          0x1401, 0xC0000300 + nodeid, 0x1601, 0x1601, 0x60810020, 0x1601, 0x607a0020, 0x1601,
          0x1401, 0x40000300 + nodeid, 0x1401, 0xfe, 0x1401, 0, /*73*/

          0x1402, 0xC0000400 + nodeid, 0x1602, 0x1602, 0x60c10120, 0x1602, 0x1402,
          0x40000400 + nodeid, 0x1402, 0xFE, 0x1402, 0, /*85*/

          0x1403, 0xC0000400, 0x1603, 0x1603, 0x60400010, 0x1603, 0x1403, 0x40000400, 0x1403, 0xFE,
          0x1403, 0 /*97*/
          );

      canopen_abort_code = RegisterSetODentryCallBack(d, 0x6061, 0, &OnStatusUpdate);

      if(canopen_abort_code)
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf(
              "Error[%d on node %x]: Impossibile registrare il callback per l'oggetto 0x2400 (Canopen abort code %x)\n",
              CANOpenError, nodeid, canopen_abort_code);
        }
#endif
        CERR("CT0", CERR_InternalError);
      }

      canopen_abort_code = RegisterSetODentryCallBack(d, 0x6063, 0, &OnPositionUpdate);

      if(canopen_abort_code)
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf(
              "Error[%d on node %x]: Impossibile registrare il callback per l'oggetto 0x2400 (Canopen abort code %x)\n",
              CANOpenError, nodeid, canopen_abort_code);
        }
#endif
        CERR("CT0", CERR_InternalError);
      }

      fflush(stdout);
    }
    else
    {
      pthread_mutex_unlock(&motor_active_number_mutex);

      if(fake_flag)
      {
        ConfigureSlaveNodeCallback(d, nodeid, 0, 0, 0);

        motor_mode[nodeid] = 0x3;
        motor_status[NodeId] = 0x1637;
        motor_interp_status[NodeId] = 0X102d;
        goto fake_jump;
      }

// CONFIGURE HEARTBEAT TO 100 ms
// MAP TPDO 1 (COB-ID 180) to transmit "node id" (8-bit), "status word" (16-bit), "interpolation mode status" (16-bit)
// MAP TPDO 2 (COB-ID 280) to transmit "node id" (8-bit), "position actual value" (32-bit)

// MAP RPDO 1 (COB-ID 200 + nodeid) to receive "Interpolation Time Index" (8-bit)" (0x60c2 sub2) "Interpolation Time Units" (8-bit)" (0x60c2 sub1)
// MAP RPDO 2 (COB-ID 300 + nodeid) to receive "Profile Velocity" (32-bit) (0x6081 sub0), "Target Position" (32-bit) (0x607a sub0)
// MAP RPDO 3 (COB-ID 400 + nodeid) to receive "Interpolation Data" (32-bit)" (0x60c1 sub1)
// MAP RPD0 4 (COB-ID 400) to receive "Control Word (16-bit)" (0x6040 sub0)
// MAP RPDO 5 (COB-ID 380) to receive high resolution timestamp

      struct state_machine_struct *configure_slave_machine[] =
      {
          &heart_start_machine,
          &map4_pdo_machine,
          &map2_pdo_machine,
          &map2_pdo_machine,
          &map2_pdo_machine,
          &map1_pdo_machine,
          &map1_pdo_machine,
          &map1_pdo_machine,
          &smart_start_machine
      };

      _machine_exe(d, nodeid, &ConfigureSlaveNodeCallback, configure_slave_machine, 9, 1, 97, 100,

      0x1800, 0xC0000180, 0x1A00, 0x1A00, 0x20000008, 0x1A00, 0x60410010, 0x1A00, 0x24000010,
          0x1A00, 0x60610008, 0x1A00, 0x1800, 0x40000180, 0x1800, 15, 0x1800, 0, /*19*/

          0x1801, 0xC0000280, 0x1A01, 0x1A01, 0x20000008, 0x1A01, 0x60630020, 0x1A01, 0x1801,
          0x40000280, 0x1801, 0xFE, 0x1801, 10, /*33*/

          0x1400, 0xC0000200 + nodeid, 0x1600, 0x1600, 0x60c20208, 0x1600, 0x60c20108, 0x1600,
          0x1400, 0x40000200 + nodeid, 0x1400, 0xFE, 0x1400, 0, /*47*/

          0x1401, 0xC0000300 + nodeid, 0x1601, 0x1601, 0x60810020, 0x1601, 0x607a0020, 0x1601,
          0x1401, 0x40000300 + nodeid, 0x1401, 0xfe, 0x1401, 0, /*61*/

          0x1402, 0xC0000400 + nodeid, 0x1602, 0x1602, 0x60c10120, 0x1602, 0x1402,
          0x40000400 + nodeid, 0x1402, 0xFE, 0x1402, 0, /*73*/

          0x1403, 0xC0000400, 0x1603, 0x1603, 0x60400010, 0x1603, 0x1403, 0x40000400, 0x1403, 0xFE,
          0x1403, 0, /*85*/

          0x1404, 0xC0000380, 0x1604, 0x1604, 0x10130020, 0x1604, 0x1404, 0x40000380, 0x1404, 0xFE,
          0x1404, 0 /*97*/

          );
    }

    fake_jump: motor_active[nodeid] = 1;
  }

  pthread_mutex_init(&interpolator_mux[nodeid], NULL);

// Controllo se una tabella sia stata già assegnata, altrimenti ne
// trovo una libera
  int motor_table_index = MotorTableIndexFromNodeId(nodeid);

  if(motor_table_index == -1) // tabella non assegnata
  {
    int i;
    for(i = 0; i < TABLE_MAX_NUM; i++)
    {
      if(motor_table[i].nodeId == 0)
      {
        pthread_mutex_init(&motor_table[i].table_mutex, NULL);
        QueueInit(nodeid, &motor_table[i]);
        break;
      }
    }
  }
  else
  {
    QueueInit(nodeid, &motor_table[motor_table_index]);
  }
}

void CANOpenShellOD_post_Emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg,
    const UNS8 errSpec[5])
{
  printf("Emergency message: %d", errCode);
}

void CANOpenShellOD_post_SlaveBootup(CO_Data* d, UNS8 nodeid)
{
#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Slave %x boot up\n", nodeid);
  }
#endif

  ConfigureSlaveNode(d, nodeid);
}

void CANOpenShellOD_heartbeatError(CO_Data* d, UNS8 nodeid)
{
  printf("Error[heartbeat]: node %d fail!\n", nodeid);
}

/***************************  CALLBACK FUNCTIONS  *****************************************/

/* Callback function that check the read SDO demand */
void CheckReadProgramUpload(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;
  UNS8 data[33];
  UNS32 size = 33;

//pthread_mutex_lock(&machine_mux[nodeid]);

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size, &abortCode) != SDO_FINISHED)
  {
    printf("\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n", nodeid,
        abortCode);
    fflush(stdout);
    raw_response_flag = -1;
    machine_state = -1;
  }
  else
  {
    memcpy(raw_response, data, size);
    raw_response_size = size;
    raw_response_flag = 1;
  }

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(d, nodeid, SDO_CLIENT);

//pthread_mutex_unlock(&machine_mux[nodeid]);

  SmartProgramUpload(d, nodeid);
}

void CheckReadProgramDownload(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;
  UNS32 data[33];
  UNS32 size = 64;

//pthread_mutex_lock(&machine_mux[nodeid]);

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size, &abortCode) != SDO_FINISHED)
  {
    printf("\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n", nodeid,
        abortCode);
    fflush(stdout);
    raw_response_flag = -1;
    machine_state = -1;
  }
  else
  {
    memcpy(raw_response, data, size);
    raw_response_size = size;
    raw_response_flag = 1;
  }

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(d, nodeid, SDO_CLIENT);

//pthread_mutex_unlock(&machine_mux[nodeid]);

  SmartProgramDownload(d, nodeid);
}

/* Callback function that check the read SDO demand */
void CheckReadRaw(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;
  UNS32 data[33];
  UNS32 size = 64;

//pthread_mutex_lock(&machine_mux[nodeid]);

  if(getReadResultNetworkDict(CANOpenShellOD_Data, nodeid, &data, &size, &abortCode) != SDO_FINISHED)
  {
    printf("\nResult : Failed in getting information for slave %2.2x, AbortCode :%4.4x \n", nodeid,
        abortCode);
    fflush(stdout);
    raw_response_flag = -1;
    machine_state = -1;
  }
  else
  {
    memcpy(raw_response, data, size);
    raw_response_size = size;
    raw_response_flag = 1;
  }

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
    printf("\nResult : Failed in getting information for slave %2.2x, AbortCode :%x \n", nodeid,
        abortCode);
    fflush(stdout);
    raw_response_flag = -1;
    machine_state = -1;
  }
  else
  {
    raw_response[0] = 1;
    raw_response_size = 1;
    raw_response_flag = 1;
  }

  /* Finalize last SDO transfer with this node */
  closeSDOtransfer(d, nodeid, SDO_CLIENT);

//pthread_mutex_unlock(&machine_mux[nodeid]);

  SmartWriteRaw(d, nodeid);
}

void CheckWriteProgramUpload(CO_Data* d, UNS8 nodeid)
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
  SmartProgramUpload(d, nodeid);
}

void CheckWriteProgramDownload(CO_Data* d, UNS8 nodeid)
{
  UNS32 abortCode;

  if(getWriteResultNetworkDict(d, nodeid, &abortCode) != SDO_FINISHED)
  {
    printf("Error: %d", abortCode);
    fflush(stdout);
    machine_state = -1;
  }

  closeSDOtransfer(d, nodeid, SDO_CLIENT);

//pthread_mutex_unlock(&machine_mux[nodeid]);
  SmartProgramDownload(d, nodeid);
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
#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Node_initialisation\n");
    fflush(stdout);
  }
#endif

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
#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Node_preOperational\n");
    fflush(stdout);
  }
#endif
}

void CANOpenShellOD_operational(CO_Data* d)
{
#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Node_operational\n");
    fflush(stdout);
  }
#endif
}

void CANOpenShellOD_stopped(CO_Data* d)
{
#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Node_stopped\n");
    fflush(stdout);
  }
#endif
}

void CANOpenShellOD_post_sync(CO_Data* d)
{
  //printf("Master_post_sync\n");

  if(fake_flag)
  {
    static int timeout_count = 0;
    int motor_index;

    timeout_count++;

    if(timeout_count == (SYNC_DIVIDER_STATUS - 1))
    {
      //printf("Master_post_sync status\n");
      // MAP TPDO 1 (COB-ID 180) to transmit "node id" (8-bit), "status word" (16-bit), "interpolation mode status" (16-bit), "modes of operation" (8-bit)

      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      {
        simulation_first_start[motor_table[motor_index].nodeId] = 1;

        NodeId = motor_table[motor_index].nodeId;
        Statusword = motor_status[NodeId];
        Interpolation_Mode_Status = motor_interp_status[NodeId]; //9201
        Modes_of_operation_display = motor_mode[NodeId];

        OnStatusUpdate(CANOpenShellOD_Data, NULL, 0);
      }

      timeout_count = 0;
    }
  }
  else
  {
    pthread_mutex_lock(&robot_state_mux);
    if(robot_state == MOVIMENTO_LIBERO)
    {
      pthread_mutex_unlock(&robot_state_mux);

      int motor_index;

      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      {
        /*printf("SmartPosition [%d] pos [%ld] read [%d] write [%d]\n",
         motor_table[motor_index].nodeId, motor_table[motor_index].position[0],
         motor_table[motor_index].read_pointer,
         motor_table[motor_index].write_pointer);*/

        if(motor_table[motor_index].read_pointer)
        {
          SmartPosition(motor_table[motor_index].nodeId, motor_table[motor_index].position[0],
              motor_table[motor_index].forward_velocity, 100,
              motor_table[motor_index].write_pointer, 1);

          if(motor_table[motor_index].write_pointer)
            motor_table[motor_index].write_pointer = 0;
        }
      }
    }
    else
      pthread_mutex_unlock(&robot_state_mux);
  }
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

  if(fake_flag == 0)
  {
    /* Load can library */
    LoadCanDriver(LibraryPath);
  }

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

  if(fake_flag == 0)
  {
    if(!canOpen(&Board, CANOpenShellOD_Data))
      return INIT_ERR;
  }
  /* Defining the node Id */
  setNodeId(CANOpenShellOD_Data, NodeID);
  /* Start Timer thread */
  StartTimerLoop(&Init);
  return 0;
}

void help_menu(void)
{
  printf("   MANDATORY COMMAND:\n");
  printf("     load#CanLibraryPath,channel,baudrate,nodeid\n");
  printf("       ex: load#libcanfestival_can_socket.so,0,1M,8\n");
  printf("\n");
  printf("   OPTIONAL COMMAND:\n");
  printf("     fake : run with fake motor\n");
  printf("     verb : activate debug messages\n");
  printf("       ex: load#libcanfestival_can_socket.so,0,1M,8\n");
  printf("   NETWORK: (if nodeid=0x00 : broadcast)\n");
  printf("     srst#nodeid : Reset a node\n");
  printf("     spre#nodeid : Enter in pre-operational mode\n");
  printf("     soff#nodeid : Stop node\n");
  printf("\n");
  printf("   SDO: (size in bytes)\n");
  printf("     info#nodeid\n");
  printf("     rsdo#nodeid,index,subindex : read sdo\n");
  printf("        ex : rsdo#42,1018,01\n");
  printf("     wsdo#nodeid,index,subindex,size,data : write sdo\n");
  printf("        ex : wsdo#42,6200,01,01,FF\n");
  printf("\n");
  printf("   SMART MOTOR:\n");
  printf("     CT0 M<num_mot> : Discover nodes and check if there's num_mot motors\n");
  printf(
      "     CT2 P1 : start homing procedure for all motors. Parameters has been read by motor's file \n");
  printf(
      "     CB2 P1 : start homing procedure for all motors. Parameters has been read by motor's file \n");
  printf("     CT2 P2 : all motors returns to the center point \n");
  printf("     CT2 P3 : release motors with brake \n");
  printf("     CT4 : starts simulation \n");
  printf("     CB4 : starts simulation \n");
  printf("     CT5 : stops simulation (valid only in simulation state) \n");
  printf("     CT6 : quit application \n");
  printf("     CB7 : move links by streaming \n");

  printf("     EM2 : stop all motors \n");
  printf("     PR1: ottiene lo stato\n");
  printf("     PR5 M<mot_num> O<index_hex> S<subindex> T<bit_num><tipo> <valore> :\n");
  printf("        imposta il valore dell'oggetto canopen indicato \n");
  printf("        ex : PR5 M119 O6065 S0 T32U 100\n");

  printf(
      "     shom#nodeid,offset,vel_forw,vel_back : start homing for nodeid with forward velocity vel_forw, backward velocity vel_back and distance from limit equal to offset\n");
  printf("        ex : shom#77,7d0,2710,2710\n");
  printf("     simu#nodeid : start simulation reading data from tables/<nodeid>.mot file\n");
  printf("     prog#nodeid,file_name : download file_name firmware to motor nodeid\n");
  printf("     uplo#nodeid : read firmware from motor\n");

  printf("     ssta#nodeid : Reset error and make motor operative\n");
  printf("     ssto#nodeid : Stop a node and motor\n");
  printf("     sraw#nodeid,report_flag,command : raw cmd to motor\n");
  printf("        ex : sraw#3f,1,RMODE\n");
  printf("     tpdo#nodeid,pdo_num - 1,cob_id,map1,map2,entry_num,type,time : map tpdo\n");
  printf("        ex : tpdo#3f,1,182,60410010,606c0020,2,FE,64\n");
  printf("     shrt#nodeid,cycle : set and start heartbeat time cycle [ms]\n");
  printf("     szer#nodeid : set motor at origin\n");
  printf("     int1#nodeid : interpolation test 1\n");
  printf("     int2#nodeid : interpolation test 2\n");
  printf("     sin1#nodeid,angle step,amplitude,point number : generate sin wave\n");
  printf("     ints#nodeid : start interpolated movement \n");
  printf("     svel#nodeid : read the target velocity\n");
  printf("     sVTS#nodeid,VT : set target velocity to VT\n");
  printf("     sprg#nodeid : get actual simulation progress in %%\n");
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

void DiscoverTimeout(sigval_t val)
{
  pthread_mutex_lock(&motor_active_number_mutex);
  if(motor_active_number != val.sival_int)
  {
    pthread_mutex_unlock(&motor_active_number_mutex);

    CERR("CT0", CERR_ConfigError);
  }
  else
  {
    pthread_mutex_unlock(&motor_active_number_mutex);

    pthread_mutex_lock(&exit_from_limit_mux);
    exit_from_limit_complete = 0;
    pthread_mutex_unlock(&exit_from_limit_mux);

#ifdef NO_LIMITS
    struct state_machine_struct *exit_from_limit_machine[] =
    {
      &smart_statusword_machine
    };
#else
    struct state_machine_struct *exit_from_limit_machine[] =
    {
    &smart_limit_enable_machine, &smart_statusword_machine
    };
#endif

    int motor_index;
    for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    {
// se il motore è alimentato
      if((motor_status[motor_table[motor_index].nodeId] & 0b0000000000010000) > 0)
      {
        if(fake_flag == 0)
        {
#ifdef NO_LIMITS
          _machine_exe(CANOpenShellOD_Data, motor_table[motor_index].nodeId, &ExitFromLimitCallback,
              exit_from_limit_machine, 1, 0, 1, 0x1);
#else
          _machine_exe(CANOpenShellOD_Data, motor_table[motor_index].nodeId, &ExitFromLimitCallback,
              exit_from_limit_machine, 2, 0, 1, 0x1);
#endif
        }
        else
        {
#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf("SUCC[node %x]: smartmotor enable limits\n", motor_table[motor_index].nodeId);
            printf("SUCC[node %x]: Status word read\n", motor_table[motor_index].nodeId);
          }
#endif
          ExitFromLimitCallback(CANOpenShellOD_Data, motor_table[motor_index].nodeId, 0, 0, 0);
        }
      }
      else
      {
        printf("ERR[node %x]: smartmotor off\n", motor_table[motor_index].nodeId);
      }

    }
  }

  timer_delete(timer);
}

void ProcessEmergencyTripode(char* command)
{
  switch(command[2] - '0')
  {
    case 2:
      if((robot_state == EMERGENZA) || (robot_state == ACCESO))
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      if(fake_flag == 0)
      {
        struct state_machine_struct *machine[] =
        {
        &smart_limit_disable_machine, &smart_stop_machine
        };

        _machine_exe(CANOpenShellOD_Data, 0, &SmartEmergencyCallback, machine, 2, 0, 0);
      }
      else
      {
        int motor_index;
        for(motor_index = 0; motor_index < motor_active_number; motor_index++)
        {
          smartmotor_table_reset(motor_table[motor_index].nodeId,
              &motor_interp_status[motor_table[motor_index].nodeId]);

#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf("SUCC[node %x]: smart motor stopped\n", motor_table[motor_index].nodeId);
          }
#endif
          SmartEmergencyCallback(CANOpenShellOD_Data, motor_table[motor_index].nodeId, 0, 0, 0);
        }
      }
      break;

    default:
      CERR(command, CERR_NotFound);
      break;
  }

  return;

  permission_denied:
  CERR(command, CERR_PermissionDenied);
  return;
}

int ProcessCommandTripode(char* command)
{
  struct sigevent sigev;
  int parse_num = 0;
  int parse_int = 0;
  char parse_str[256];

  long position;
  long velocity;
  long acceleration;
  char start;

  switch(command[2] - '0')
  {
    case 0: // Reset di tutti i motori e loro dichiarazione

      parse_num = sscanf(command, "CT0 M%d", &parse_int);

      if(parse_num == 1)
      {
        pthread_mutex_lock(&robot_state_mux);
        if((robot_state != ACCESO) && (robot_state != EMERGENZA)
            && ((robot_state != RILASCIATO)
                || ((robot_state == RILASCIATO) && (homing_executed == 1))))
        {
          pthread_mutex_unlock(&robot_state_mux);
          goto permission_denied;
        }

        pthread_mutex_unlock(&robot_state_mux);

        DiscoverNodes();

        if(fake_flag)
        {
          ConfigureSlaveNode(CANOpenShellOD_Data, 119);
          ConfigureSlaveNode(CANOpenShellOD_Data, 120);
          ConfigureSlaveNode(CANOpenShellOD_Data, 121);
          ConfigureSlaveNode(CANOpenShellOD_Data, 122);
#ifdef CBRN
          ConfigureSlaveNode(CANOpenShellOD_Data, 123);
#endif

          for(parse_num = 0; parse_num < motor_active_number; parse_num++)
          {
            motor_mode[motor_table[parse_num].nodeId] = 0x3;
            motor_status[motor_table[parse_num].nodeId] = 0x1637;
            motor_interp_status[motor_table[parse_num].nodeId] = 0X102d;
          }

          CANOpenShellOD_Data->post_sync = CANOpenShellOD_post_sync;
        }

        // Creo il timer e lo avvio
        memset(&sigev, 0, sizeof(struct sigevent));
        sigev.sigev_value.sival_int = parse_int;
        sigev.sigev_notify = SIGEV_THREAD;
        sigev.sigev_notify_attributes = NULL;
        sigev.sigev_notify_function = DiscoverTimeout;

        if(timer_create(CLOCK_REALTIME, &sigev, &timer))
        {
          perror("timer_create()");
          break;
        }

        long tv_nsec = 0;
        long tv_sec = 3;
        struct itimerspec timerValues;
        timerValues.it_value.tv_sec = tv_sec;
        timerValues.it_value.tv_nsec = tv_nsec;
        timerValues.it_interval.tv_sec = 0;
        timerValues.it_interval.tv_nsec = 0;

        timer_settime(timer, 0, &timerValues, NULL);
      }
      else
        goto fail;
      break;

    case 1:
// alle condizioni ho aggiunto IN_POSIZIONE per evitare che
// ci si trovi bloccati quando si passa il comando senza lo start.
      pthread_mutex_lock(&robot_state_mux);
      if((robot_state != CENTRATO) && (robot_state != FERMO) && (robot_state != IN_POSIZIONE))
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }
      pthread_mutex_unlock(&robot_state_mux);

      parse_num = sscanf(command, "CT1 M%d P%ld VM%ld AM%ld %c", &parse_int, &position, &velocity,
          &acceleration, &start);

      if(parse_num == 4)
        SmartPosition(parse_int, position, velocity, acceleration, 0, 0);
      else if(parse_num == 5)
        SmartPosition(parse_int, position, velocity, acceleration, 1, 0);
      else
        goto fail;

      pthread_mutex_lock(&robot_state_mux);
      robot_state = IN_POSIZIONE;
      pthread_mutex_unlock(&robot_state_mux);

      break;

    case 2:
      switch(cst_str2(command[4], command[5]))
      {
        case cst_str2('P', '1'):
          // leggo i parametri dal file e li passo alla
          // funzione di homing

          pthread_mutex_lock(&robot_state_mux);
          if(robot_state != INIZIALIZZATO)
          {
            pthread_mutex_unlock(&robot_state_mux);
            goto permission_denied;
          }

          pthread_mutex_unlock(&robot_state_mux);

          int file_status = 0;
          // inizializzo la coda e controllo che il file motore esista
          for(parse_num = 0; parse_num < motor_active_number; parse_num++)
          {
            QueueInit(motor_table[parse_num].nodeId, &motor_table[parse_num]);

            //file_status = QueueOpenFile(&motor_table[parse_num]);

            if(file_status < 0)
            {
              CERR("CT2", CERR_FileError);
              return 0;
            }

            parse_int = QueuePut(&motor_table[parse_num], 1);

            if(parse_int != 1)
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf("%d\n", parse_int);
                printf("ERR[%d on node %x]: Errore nel file.\n", InternalError,
                    motor_table[parse_num].nodeId);
              }
#endif

              CERR("CT2", CERR_ConfigError);
              fflush(stdout);
              return 0;
            }
          }

          // verifico che le informazioni nel file siano secondo il protocollo
          for(parse_num = 0; parse_num < motor_active_number; parse_num++)
          {
            // per leggere i parametri di homing non ho bisogno di utilizzare la funzione
            // QueueGet perché non vado ad incidere sul buffer circolare
            if(motor_table[parse_num].type != 'H')
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf("ERR[%d on node %x]: Errore nella riga di homing.\n", InternalError,
                    motor_table[parse_num].nodeId);
              }
#endif

              CERR("CT2", CERR_ConfigError);
              return 0;
            }
          }

          pthread_mutex_lock(&robot_state_mux);
          robot_state = RICERCA_CENTRO;
          pthread_mutex_unlock(&robot_state_mux);

          for(parse_num = 0; parse_num < motor_active_number; parse_num++)
          {
            sprintf(parse_str, "shom#%x,%lx,%lx,%lx", motor_table[parse_num].nodeId,
                motor_table[parse_num].offset, motor_table[parse_num].forward_velocity,
                motor_table[parse_num].backward_velocity);

            if(fake_flag == 0)
              SmartHome(parse_str);
            else
            {
              motor_started[motor_table[parse_num].nodeId] = 1;
              simulation_first_start[motor_table[parse_num].nodeId] = 1;
            }
          }

          if(fake_flag)
          {
            pthread_mutex_lock(&exit_from_limit_mux);
            exit_from_limit_complete = 0;
            pthread_mutex_unlock(&exit_from_limit_mux);

            for(parse_num = 0; parse_num < motor_active_number; parse_num++)
            {
              motor_position[motor_table[parse_num].nodeId] = 0;
              SmartPositionTargetCallback(CANOpenShellOD_Data, motor_table[parse_num].nodeId, 0, 0,
                  0);
            }
          }
          break;

        case cst_str2('P', '2'):
          // leggo i parametri dal file e li passo alla
          // funzione di homing

          pthread_mutex_lock(&robot_state_mux);
          if(robot_state != FERMO)
          {
            pthread_mutex_unlock(&robot_state_mux);
            goto permission_denied;
          }

          robot_state = CENTRAGGIO;
          pthread_mutex_unlock(&robot_state_mux);
          SmartPosition(0, 0, 300000, 10, 1, 0);

          break;

        case cst_str2('P', '3'):
          pthread_mutex_lock(&robot_state_mux);
          if((robot_state != INIZIALIZZATO) && (robot_state != CENTRATO) && (robot_state != FERMO))
          {
            pthread_mutex_unlock(&robot_state_mux);
            goto permission_denied;
          }

          pthread_mutex_unlock(&robot_state_mux);

#ifdef CBRN
          SmartRelease(0, 0, 1);
#else
          SmartRelease(0, 0, 1);
#endif
          break;

        default:
          goto fail;
          break;
      }
      break;

    case 4:
      pthread_mutex_lock(&robot_state_mux);
      if(robot_state != CENTRATO)
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      pthread_mutex_unlock(&robot_state_mux);

      if(SimulationStart(0) == 0)
      {
        pthread_mutex_lock(&robot_state_mux);
        robot_state = SIMULAZIONE;
        pthread_mutex_unlock(&robot_state_mux);
      }
      break;

    case 5:
      pthread_mutex_lock(&robot_state_mux);
      if((robot_state != SIMULAZIONE)
          && ((robot_state != RILASCIATO) || ((robot_state == RILASCIATO) && (homing_executed == 0))))
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      pthread_mutex_unlock(&robot_state_mux);

      SmartStop(0, 0);
      break;

    case 6:
      pthread_mutex_lock(&robot_state_mux);
      if((robot_state == EMERGENZA) && (robot_state == RICERCA_CENTRO)
          && (robot_state == SIMULAZIONE) && (robot_state == CENTRAGGIO))
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      pthread_mutex_unlock(&robot_state_mux);
      return QUIT;
      break;

    default:
      command[3] = '\0';
      CERR(command, CERR_NotFound);
      break;
  }

  return 0;

  fail:
  command[3] = '\0';
  CERR(command, CERR_ParamError);
  return 0;

  permission_denied:
  command[3] = '\0';
  CERR(command, CERR_PermissionDenied);
  return 0;
}

int ProcessCommandBraccio(char* command)
{
  int parse_num = 0;
  int parse_int = 0;
  char parse_str[256];

  switch(command[2] - '0')
  {
    case 2:
      switch(cst_str2(command[4], command[5]))
      {
        case cst_str2('P', '1'):
          // leggo i parametri dal file e li passo alla
          // funzione di homing

          pthread_mutex_lock(&robot_state_mux);
          if(robot_state != FERMO)
          {
            pthread_mutex_unlock(&robot_state_mux);
            goto permission_denied;
          }

          pthread_mutex_unlock(&robot_state_mux);

          int file_status = 0;
          // inizializzo la coda e controllo che il file motore esista
          for(parse_num = 0; parse_num < motor_active_number; parse_num++)
          {
            QueueInit(motor_table[parse_num].nodeId, &motor_table[parse_num]);
            file_status = QueueOpenFile(&motor_table[parse_num]);

            if(file_status < 0)
            {
              CERR("CT2", CERR_FileError);
              return 0;
            }

            parse_int = QueuePut(&motor_table[parse_num], 1);

            if(parse_int != 1)
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf("%d\n", parse_int);
                printf("ERR[%d on node %x]: Errore nel file.\n", InternalError,
                    motor_table[parse_num].nodeId);
              }
#endif

              CERR("CT2", CERR_ConfigError);
              return 0;
            }
          }

          // verifico che le informazioni nel file siano secondo il protocollo
          for(parse_num = 0; parse_num < motor_active_number; parse_num++)
          {
            // per leggere i parametri di homing non ho bisogno di utilizzare la funzione
            // QueueGet perché non vado ad incidere sul buffer circolare
            if(motor_table[parse_num].type != 'H')
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf("ERR[%d on node %x]: Errore nella riga di homing.\n", InternalError,
                    motor_table[parse_num].nodeId);
              }
#endif

              CERR("CT2", CERR_ConfigError);
              return 0;
            }
          }

          pthread_mutex_lock(&robot_state_mux);
          robot_state = RICERCA_CENTRO;
          pthread_mutex_unlock(&robot_state_mux);

          for(parse_num = 0; parse_num < motor_active_number; parse_num++)
          {
            sprintf(parse_str, "shom#%x,%lx,%lx,%lx", motor_table[parse_num].nodeId,
                motor_table[parse_num].offset, motor_table[parse_num].forward_velocity,
                motor_table[parse_num].backward_velocity);

            if(fake_flag == 0)
              SmartHome(parse_str);
            else
            {
              motor_started[motor_table[parse_num].nodeId] = 1;
              simulation_first_start[motor_table[parse_num].nodeId] = 1;
            }
          }

          if(fake_flag)
          {
            pthread_mutex_lock(&exit_from_limit_mux);
            exit_from_limit_complete = 0;
            pthread_mutex_unlock(&exit_from_limit_mux);

            for(parse_num = 0; parse_num < motor_active_number; parse_num++)
              SmartPositionTargetCallback(CANOpenShellOD_Data, motor_table[parse_num].nodeId, 0, 0,
                  0);
          }
          break;

        default:
          goto fail;
          break;
      }
      break;

    case 3:
      pthread_mutex_lock(&robot_state_mux);
      if((robot_state != FERMO) && (robot_state != INIZIALIZZATO) && (robot_state != CENTRATO))
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      pthread_mutex_unlock(&robot_state_mux);

      int motor_index;
      int queuefill_return = 0;
      SmartClear(0);

      motor_index = MotorTableIndexFromNodeId(0);
//for(motor_index = 0; motor_index < motor_active_number; motor_index++)
//{
      motor_table[motor_index].is_pipe = 1;

      if((queuefill_return = QueueFill(&motor_table[motor_index])) < 0)
      {
        CERR("CB3", CERR_FileError);
        break;
      }
//}

      struct state_machine_struct *set_position_machine[] =
      {
      &smart_set_mode_machine
      };

      _machine_exe(CANOpenShellOD_Data, 0, NULL, set_position_machine, 1, 0, 0);

      CANOpenShellOD_Data->post_sync = CANOpenShellOD_post_sync;

      pthread_mutex_lock(&robot_state_mux);
      if(queuefill_return == 0)
      {
        robot_state = JOYSTICK_COLLEGATO;
        OK("CB3");
      }
      pthread_mutex_unlock(&robot_state_mux);

      break;

    case 4:
      pthread_mutex_lock(&robot_state_mux);
      if(robot_state != JOYSTICK_COLLEGATO)
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      robot_state = MOVIMENTO_LIBERO;
      pthread_mutex_unlock(&robot_state_mux);

      break;

    case 5:
      pthread_mutex_lock(&robot_state_mux);
      if(robot_state != MOVIMENTO_LIBERO)
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      pthread_mutex_unlock(&robot_state_mux);

      SmartStop(0, 0);
      break;

    case 7:
      pthread_mutex_lock(&robot_state_mux);
      if(robot_state != JOYSTICK_COLLEGATO)
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      robot_state = MOVIMENTO_LIBERO;
      pthread_mutex_unlock(&robot_state_mux);

      break;

    default:
      CERR(command, CERR_NotFound);
      break;
  }

  return 0;

  fail:
  CERR(command, CERR_ParamError);
  return 0;

  permission_denied:
  CERR(command, CERR_PermissionDenied);
  return 0;
}

int ProcessParameterTripode(char* command)
{
  char parse_str[256];
  int parse_num = 0;
  int nodeid = 0;
  long index = 0;
  int subindex = 0;
  int size = 0;
  char type;
  long value = 0;

  switch(command[2] - '0')
  {
    case 1:
      pthread_mutex_lock(&robot_state_mux);
      switch(robot_state)
      {
        case SPENTO:
          sprintf(parse_str, "PR1: %d, Spento", robot_state);
          break;

        case EMERGENZA:
          sprintf(parse_str, "PR1: %d, Emergenza", robot_state);
          break;

        case ACCESO:
          sprintf(parse_str, "PR1: %d, Acceso", robot_state);
          break;

        case INIZIALIZZATO:
          sprintf(parse_str, "PR1: %d, Inizializzato", robot_state);
          break;

        case RICERCA_CENTRO:
          sprintf(parse_str, "PR1: %d, In ricerca del centro", robot_state);
          break;

        case CENTRATO:
          sprintf(parse_str, "PR1: %d, Centrato", robot_state);
          break;

        case SIMULAZIONE:
          sprintf(parse_str, "PR1: %d, Simulazione", robot_state);
          break;

        case FERMO:
          sprintf(parse_str, "PR1: %d, Fermo", robot_state);
          break;

        case CENTRAGGIO:
          sprintf(parse_str, "PR1: %d, In centraggio", robot_state);
          break;

        case RILASCIATO:
          sprintf(parse_str, "PR1: %d, Rilasciato", robot_state);
          break;

        case IN_POSIZIONE:
          sprintf(parse_str, "PR1: %d, In posizione", robot_state);
          break;

        case JOYSTICK_COLLEGATO:
          sprintf(parse_str, "PR1: %d, Joystick collegato", robot_state);
          break;

        case MOVIMENTO_LIBERO:
          sprintf(parse_str, "PR1: %d, Movimento libero", robot_state);
          break;

        default:
          sprintf(parse_str, "PR1: %d, Sconosciuto", robot_state);
          break;
      }
      pthread_mutex_unlock(&robot_state_mux);

      OK(parse_str);
      break;

    case 5:
      pthread_mutex_lock(&robot_state_mux);
      if((robot_state == RICERCA_CENTRO) || (robot_state == SIMULAZIONE)
          || (robot_state == CENTRAGGIO) || (robot_state == IN_POSIZIONE)
          || (robot_state == ACCESO))
      {
        pthread_mutex_unlock(&robot_state_mux);
        goto permission_denied;
      }

      pthread_mutex_unlock(&robot_state_mux);

//0x2101, 0x3, 2, 0, 0x2
      parse_num = sscanf(command, "PR5 M%d O%lx S%d T%d%c %lx", &nodeid, &index, &subindex, &size,
          &type, &value);

//printf("parse %d node %d index %ld subindex %d, size %d, type %c, value %ld\n", parse_num,
//    nodeid, index, subindex, size, type, value);
      if(parse_num == 6)
      {
        if(nodeid == 0)
          goto permission_denied;

        size = size / 8;
        sprintf(parse_str, "wsdo#%x,%lx,%x,%x,%lx", nodeid, index, subindex, size, value);
        WriteDeviceEntry(parse_str);
      }
      else
        goto fail;
      break;

    default:
      CERR(command, CERR_NotFound);
      break;
  }

  return 0;

  fail:
  CERR(command, CERR_ParamError);
  return 0;

  permission_denied:
  CERR(command, CERR_PermissionDenied);
  return 0;
}

int ProcessCommand(char* command)
{
  int ret = 0;
  int NodeID;

  return_event();

  switch(cst_str2(command[0], command[1]))
  {
    case cst_str2('E', 'M'):
      ProcessEmergencyTripode(command);
      break;

    case cst_str2('C', 'T'):
      return ProcessCommandTripode(command);
      break;

    case cst_str2('C', 'B'):
      return ProcessCommandBraccio(command);
      break;

    case cst_str2('P', 'R'):
      return ProcessParameterTripode(command);
      break;

    default:
      EnterMutex();

      switch(cst_str4(command[0], command[1], command[2], command[3]))
      {
        case cst_str4('f', 'a', 'k', 'e'):
          fake_flag = 1;
          break;

        case cst_str4('v', 'e', 'r', 'b'):
          verbose_flag = 1;
          break;

        case cst_str4('l', 'o', 'a', 'd'): // Library Interface
          ret = sscanf(command, "load#%100[^,],%30[^,],%4[^,],%d", LibraryPath, BoardBusName,
              BoardBaudRate, &NodeID);

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

        case cst_str4('s', 'h', 'o', 'm'):
          LeaveMutex();
          SmartHome(command);
          break;

        case cst_str4('s', 'i', 'm', 'u'):
          LeaveMutex();
          SimulationStart(ExtractNodeId(command + 5));
          break;

        case cst_str4('s', 's', 't', 'o'): // Smart Stop
          LeaveMutex();
          SmartStop(ExtractNodeId(command + 5), 0);
          break;

        case cst_str4('s', 'r', 'a', 'w'): // RAW command
          RawCmdMotor(command);
          break;

        case cst_str4('h', 'e', 'l', 'p'): // Display Help
          help_menu();
          break;

        case cst_str4('s', 'z', 'e', 'r'): // OK
          LeaveMutex();
          SmartPosition(ExtractNodeId(command + 5), 0, 300000, 10, 1, 0);
          break;

        case cst_str4('p', 'r', 'o', 'g'): // download program to motor
          DownloadToMotor(command);
          break;

        case cst_str4('u', 'p', 'l', 'o'): // download program from motor
          UploadFromMotor(ExtractNodeId(command + 5));
          break;

        case cst_str4('w', 's', 'd', 'o'): // Write device entry
          WriteDeviceEntry(command);
          break;

        case cst_str4('r', 's', 'd', 'o'): // Read device entry
          ReadDeviceEntry(command);
          break;

        case cst_str4('s', 'v', 'e', 'l'):
          LeaveMutex();
          SmartVelocityGet(ExtractNodeId(command + 5));
          break;

        case cst_str4('s', 'a', 'c', 'c'):
          LeaveMutex();
          SmartAccelerationGet(ExtractNodeId(command + 5));
          break;

        case cst_str4('s', 'f', 'o', 'l'):
          LeaveMutex();
          SmartFollowingErrorGet(ExtractNodeId(command + 5));
          break;

        case cst_str4('q', 'u', 'i', 't'): // Quit application
          LeaveMutex();
          return QUIT;

        default:
          CERR(command, CERR_NotFound);
          break;
      }

      LeaveMutex();
      break;
  }

  return 0;
}

void signal_handler(int signum)
{
  switch(signum)
  {
    case SIGINT:
// Garbage collection
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("Terminating program...\n");

        printf("Finishing.\n");
      }
#endif

      if(position_fp > 0)
        fclose(position_fp);

      if(fake_flag == 0)
        remove(POSITION_FIFO_FILE);
      else
        remove("/tmp/fake_alma_3d_spinitalia_pos_stream_pipe");

      _machine_destroy();

      if(fake_flag == 0)
      {
        stopSYNC(CANOpenShellOD_Data);
//heartbeatStop(CANOpenShellOD_Data);

// Stop timer thread
        StopTimerLoop(&Exit);

        /* Close CAN board */
        canClose(CANOpenShellOD_Data);
      }

      TimerCleanup();

      exit(signum);
      break;

    case SIGPIPE:
      printf("sigpipe\n");
      fflush(stdout);
      break;
  }
}

void *PipePositionOpenHandler(char *pipe_name)
{
#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Waiting for someone who wants to read positions. . .\n");
    fflush(stdout);
  }
#endif
  position_fp = fopen(pipe_name, "w");

  if(position_fp == NULL)
    perror("position pipe");

  fflush(stdout);
  signal(SIGPIPE, SIG_IGN);

  return NULL;
}

void *PipePositionWriteHandler()
{
  char position_message[256];
//struct timeval position_stop_time;
  struct timespec position_stop_time;
  static float file_complete[CANOPEN_NODE_NUMBER];
  static float file_complete_min = 0;

  pthread_mutex_lock(&position_mux);

  while(1)
  {
    pthread_cond_wait(&position_ready, &position_mux);

    // Inizializzo il timer di partenza, nel caso non lo fosse
    /*if((position_start_time.tv_sec == 0) && (position_start_time.tv_usec == 0))
     gettimeofday(&position_start_time, NULL);*/
    if((position_start_time.tv_sec == 0) && (position_start_time.tv_nsec == 0))
      clock_gettime(CLOCK_MONOTONIC, &position_start_time);

    int motor_index;

    if(fake_flag)
    {
      for(motor_index = 0; motor_index < motor_active_number; motor_index++)
      {
        if((motor_mode[motor_table[motor_index].nodeId] == 0x1)
            && ((motor_status[motor_table[motor_index].nodeId] & 0b0001000000000000)
                == 0b0001000000000000))
        {
          //if(motor_table[motor_index].nodeId == 119)
          //  printf("smartmotor_path_read: %x\n", motor_status[motor_table[motor_index].nodeId]);

          smartmotor_path_read(motor_table[motor_index].nodeId,
              &motor_status[motor_table[motor_index].nodeId],
              &motor_position[motor_table[motor_index].nodeId]);
        }
        else if((motor_mode[motor_table[motor_index].nodeId] == 0x7)
            && (motor_interp_status[motor_table[motor_index].nodeId] & 0b1000000000000000))
        {
          // Se sono in modalità simulazione ed è stato impartito il comando di avvio, allora posso avviare
          // il lettore di tabella

          //if((motor_interp_status[motor_table[motor_index].nodeId] & 0x3f) < 45)
          //{
          smartmotor_table_read(motor_table[motor_index].nodeId,
              &motor_interp_status[motor_table[motor_index].nodeId],
              &motor_position[motor_table[motor_index].nodeId]);
          //}
        }
      }
    }

    // Carico la stringa delle posizioni da inviare
    for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    {
      if(position_message[0] == '\0')
        sprintf(position_message, "@M%d S%li", motor_table[motor_index].nodeId,
            motor_position[motor_table[motor_index].nodeId]);
      else
        sprintf(position_message, "%s @M%d S%li", position_message, motor_table[motor_index].nodeId,
            motor_position[motor_table[motor_index].nodeId]);
    }

    for(motor_index = 0; motor_index < motor_active_number; motor_index++)
    {
      if(robot_state != MOVIMENTO_LIBERO)
      {
        if(fake_flag == 0)
          file_complete[motor_table[motor_index].nodeId] = FileCompleteGet(
              motor_table[motor_index].nodeId,
              SMART_TABLE_SIZE - (motor_interp_status[motor_table[motor_index].nodeId] & 0x3F));
        else
          file_complete[motor_table[motor_index].nodeId] = FileCompleteGet(
              motor_table[motor_index].nodeId, 1);

        // restituisco la percentuale di completamento più bassa
        if((file_complete_min == 0)
            || (file_complete[motor_table[motor_index].nodeId] < file_complete_min))
          file_complete_min = file_complete[motor_table[motor_index].nodeId];
      }
      else
      {
        // il campo della percentuale di completamento diventa il numero di posizioni attualmente
        // pronte per essere lette dal motore al prossimo ciclo di update.
        if((file_complete_min == 0)
            || (file_complete[motor_table[motor_index].nodeId] < file_complete_min))
          file_complete_min = motor_table[motor_index].count;
      }
    }

    if(position_fp != NULL)
    {
      pthread_mutex_lock(&(event_buffer.error_mux));
      if(event_buffer.count > 0)
      {
        pthread_mutex_unlock(&(event_buffer.error_mux));
        sprintf(position_message, "%s AS0", position_message);
        pthread_mutex_lock(&robot_state_mux);
      }
      else
      {
        pthread_mutex_unlock(&(event_buffer.error_mux));
        pthread_mutex_lock(&robot_state_mux);
        sprintf(position_message, "%s AS%d", position_message, robot_state);
      }

//gettimeofday(&position_stop_time, NULL);
      clock_gettime(CLOCK_MONOTONIC, &position_stop_time);

      /*long position_delay_us = ((position_stop_time.tv_sec - position_start_time.tv_sec) * 1000000
       + position_stop_time.tv_usec - position_start_time.tv_usec);*/
      long position_delay_us = ((position_stop_time.tv_sec - position_start_time.tv_sec)
          * 1000000000 + position_stop_time.tv_nsec - position_start_time.tv_nsec);

      sprintf(position_message, "%s T%.2f", position_message,
          ((float) position_delay_us) / 1000000);

      if(robot_state == SIMULAZIONE)
        sprintf(position_message, "%s C%.0f\n", position_message, file_complete_min);
      /*else if(robot_state == MOVIMENTO_LIBERO)
       sprintf(position_message, "%s C%f\n", position_message, file_complete_min);
       else
       sprintf(position_message, "%s C0\n", position_message);*/
      else
        sprintf(position_message, "%s C%.0f\n", position_message, file_complete_min);

      pthread_mutex_unlock(&robot_state_mux);

      fputs(position_message, position_fp);

      fflush(position_fp);

      file_complete_min = 0;

//gettimeofday(&position_start_time, NULL);
      clock_gettime(CLOCK_MONOTONIC, &position_start_time);

    }

    position_message[0] = '\0';
  }

  pthread_mutex_unlock(&position_mux);

  return NULL;
}

int open_pipe(char *pipe_name, pthread_t *open_proc_handler, void *open_proc_func,
    pthread_t *task_proc_handler, void *task_proc_func)
{
  int err;
  err = pthread_create(open_proc_handler, NULL, open_proc_func, (void *) pipe_name);

  if(err != 0)
  {
    printf("can't create thread:[%s]", strerror(err));
    return -1;
  }

  err = pthread_create(task_proc_handler, NULL, task_proc_func, NULL);

  if(err != 0)
  {
    printf("can't create thread:[%s]", strerror(err));
    return -1;
  }

  return 0;
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
//int sysret = 0;
  int i = 0;

  /* Print help and exit immediatly*/
  if(argc < 2)
  {
    help_menu();
    exit(1);
  }

  signal(SIGINT, signal_handler);
//signal(SIGTERM, signal_handler);

//load#libcanfestival_can_socket.so,0,1M,8
  /* Strip command-line*/

  event_buffer.count = 0;
  pthread_mutex_init(&(event_buffer.error_mux), NULL);

  /* Init stack timer */
  TimerInit();

  for(i = 1; i < argc; i++)
  {
    if(ProcessCommand(argv[i]) == INIT_ERR)
      goto init_fail;
  }

  if(fake_flag == 0)
  {
    // inizializzo la named pipe per i dati di posizione
    umask(0);
    mknod(POSITION_FIFO_FILE, S_IFIFO | 0666, 0);

    open_pipe(POSITION_FIFO_FILE, &pipe_handler, &PipePositionOpenHandler, &pipe_write_handler,
        &PipePositionWriteHandler);
  }
  else
  {
    // inizializzo la named pipe per i dati di posizione
    umask(0);
    mknod("/tmp/fake_alma_3d_spinitalia_pos_stream_pipe", S_IFIFO | 0666, 0);

    open_pipe("/tmp/fake_alma_3d_spinitalia_pos_stream_pipe", &pipe_handler,
        &PipePositionOpenHandler, &pipe_write_handler, &PipePositionWriteHandler);
  }

#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Starting. . .\n");
  }
#endif

  _machine_init();

  if(fake_flag == 0)
  {
//heartbeatInit(CANOpenShellOD_Data);
    startSYNC(CANOpenShellOD_Data);
    PDOInit(CANOpenShellOD_Data);
  }

  /* Enter in a loop to read stdin command until "quit" is called */
  while(ret != QUIT)
  {
    // wait on stdin for string command
    res = fgets(command, sizeof(command), stdin);

    if((res != NULL) && (strlen(command) > 1))
    {
//sysret = system(CLEARSCREEN);
      command[strlen(command) - 1] = '\0';
      if(command[strlen(command) - 1] == '\r')
        command[strlen(command) - 1] = '\0';

      ret = ProcessCommand(command);
      fflush(stdout);
    }
  }

#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    printf("Finishing.\n");
  }
#endif

  if(fake_flag == 0)
  {
    stopSYNC(CANOpenShellOD_Data);
    //heartbeatStop(CANOpenShellOD_Data);

    // Stop timer thread
    StopTimerLoop(&Exit);

    /* Close CAN board */
    canClose(CANOpenShellOD_Data);
  }
  else if(fake_update_timer)
    timer_delete(fake_update_timer);

  _machine_destroy();

  if(position_fp > 0)
    fclose(position_fp);

  if(fake_flag == 0)
    unlink(POSITION_FIFO_FILE);
  else
    unlink("/tmp/fake_alma_3d_spinitalia_pos_stream_pipe");

  init_fail: TimerCleanup();
  return 0;
}

