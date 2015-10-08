#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "CANOpenShellStateMachine.h"
#include "CANOpenShellMasterError.h"

int motor_active[CANOPEN_NODE_NUMBER]; /**< indica se un motore si è dichiarato */
volatile int motor_started[CANOPEN_NODE_NUMBER];
int motor_active_number; /**< tiene conto di quanti motori sono presenti */
long motor_position[CANOPEN_NODE_NUMBER]; /**< ultima posizione del motore */
UNS32 motor_interp_status[CANOPEN_NODE_NUMBER]; /**< ultimo stato del registro di interpolazione del motore*/
UNS16 motor_status[CANOPEN_NODE_NUMBER]; /**< ultimo stato del motore */
UNS8 motor_mode[CANOPEN_NODE_NUMBER]; /**< ultimo stato del motore */

long next_machine_size[CANOPEN_NODE_NUMBER];
struct state_machine_struct **next_machine[CANOPEN_NODE_NUMBER];

int next_var_count[CANOPEN_NODE_NUMBER];
int var_count_total[CANOPEN_NODE_NUMBER];
UNS32 *args_ptr[CANOPEN_NODE_NUMBER];
UNS32 *next_args[CANOPEN_NODE_NUMBER];

int machine_state_index[CANOPEN_NODE_NUMBER];
int machine_state_param_index[CANOPEN_NODE_NUMBER];

MachineCallback_t callback_user[CANOPEN_NODE_NUMBER];

#ifndef SDO_SYNC
pthread_mutex_t machine_mux[CANOPEN_NODE_NUMBER];
volatile int machine_run[CANOPEN_NODE_NUMBER];
#else
pthread_mutex_t machine_mux;
volatile int machine_run = 0;
pthread_cond_t machine_run_cond = PTHREAD_COND_INITIALIZER;
#endif

void *smart_start[12] =
    {
        &masterSendNMTstateChange, // Start canopen node
        &writeNetworkDictCallBack, // Change state: switched off
        &writeNetworkDictCallBack, // Reset status word
        &writeNetworkDictCallBack, // Change state: switched off
        &writeNetworkDictCallBack, // Set mode velocity
        &writeNetworkDictCallBack, // Set velocity in PV mode
        &writeNetworkDictCallBack, // Set acceleration
        &writeNetworkDictCallBack, // Set deceleration
        &writeNetworkDictCallBack, // Set following error window
        &writeNetworkDictCallBack, // Change state: ready to switch on
        &writeNetworkDictCallBack, // Change state: switched on
        &writeNetworkDictCallBack, // Start command
    };
UNS32 smart_start_param[56] =
    {
        NMT_Start_Node,  // Start canopen node
        0x6040, 0x0, 2, 0, 0x0,  // Change state: switched off
        0x6040, 0x0, 2, 0, 0x80,  // Reset status word
        0x6040, 0x0, 2, 0, 0x0,  // Change state: switched off
        0x6060, 0x0, 1, 0, 0x3,  // Set mode velocity
        0x60FF, 0x0, 4, 0, 0x0,  // Set velocity in PV mode
        0x6083, 0x0, 4, 0, 0x1000,  // Set acceleration
        0x6084, 0x0, 4, 0, 0xa,  // Set deceleration
        0x6065, 0x0, 4, 0, 2000,  // Set following error window
        0x6040, 0x0, 2, 0, 0x6,  // Change state: ready to switch on
        0x6040, 0x0, 2, 0, 0x7,  // Change state: switched on
        0x6040, 0x0, 2, 0, 0xF,  // Start command
    };

char *smart_start_error[2] =
    {
        "smartmotor started",
        "Cannot start smartmotor"
    };

struct state_machine_struct smart_start_machine =
    {smart_start, 12, smart_start_param, 56, smart_start_error};

void *map1_pdo[7] =
    {
        &writeNetworkDictCallBack, // Set bit 31 of the COB-ID
        &writeNetworkDictCallBack, // Set the number of entry to 0
        &writeNetworkDictCallBack, // Set the mapping object
        &writeNetworkDictCallBack, // Set the number of entry
        &writeNetworkDictCallBack, // Clear bit 31 of the COB-ID
        &writeNetworkDictCallBack, // Set transmission type on event timer
        &writeNetworkDictCallBack  // Set transmission time
    };
/**
 * Optional parameter:
 *      0x1800 | pdo_number
 *      0xC0000000 | COB-ID
 *      0x1A00 | pdo_number
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 1
 *      0x1A00 | pdo_number
 *      0x1800 | pdo_number
 *      0x40000000 | COB-ID
 *      0x1800 | pdo_number
 *      transmission type
 *      0x1800 | pdo_number
 *      transmission time
 */UNS32 map1_pdo_param[35] =
    {
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Set bit 31 of the COB-ID
        0xFFFFFFFF, 0x0, 1, 0, 0x00, // Set the number of entry to 0
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Set the mapping object: status word
        0xFFFFFFFF, 0x0, 1, 0, 0x01, // Set the number of entry
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Clear bit 31 of the COB-ID
        0xFFFFFFFF, 0x2, 1, 0, 0xFFFFFFFF, // Set transmission type on event timer
        0xFFFFFFFF, 0x5, 2, 0, 0xFFFFFFFF // Set transmission time on event timer
    };

char *map1_pdo_error[2] =
    {
        "PDO mapped",
        "Cannot map PDO"
    };

struct state_machine_struct map1_pdo_machine =
    {map1_pdo, 7, map1_pdo_param, 35, map1_pdo_error};

void *map2_pdo[8] =
    {
        &writeNetworkDictCallBack, // Set bit 31 of the COB-ID
        &writeNetworkDictCallBack, // Set the number of entry to 0
        &writeNetworkDictCallBack, // Set the mapping object: status word
        &writeNetworkDictCallBack, // Set the mapping object: actual velocity
        &writeNetworkDictCallBack, // Set the number of entry
        &writeNetworkDictCallBack, // Clear bit 31 of the COB-ID
        &writeNetworkDictCallBack, // Set transmission type on event timer
        &writeNetworkDictCallBack  // Set transmission time
    };
/**
 * Optional parameter:
 *      0x1800 | pdo_number
 *      0xC0000000 | COB-ID
 *      0x1A00 | pdo_number
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 1
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 2
 *      0x1A00 | pdo_number
 *      0x1800 | pdo_number
 *      0x40000000 | COB-ID
 *      0x1800 | pdo_number
 *      transmission type
 *      0x1800 | pdo_number
 *      transmission time
 */UNS32 map2_pdo_param[40] =
    {
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Set bit 31 of the COB-ID
        0xFFFFFFFF, 0x0, 1, 0, 0x00, // Set the number of entry to 0
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Set the mapping object: status word
        0xFFFFFFFF, 0x2, 4, 0, 0xFFFFFFFF, // Set the mapping object: actual velocity
        0xFFFFFFFF, 0x0, 1, 0, 0x02, // Set the number of entry
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Clear bit 31 of the COB-ID
        0xFFFFFFFF, 0x2, 1, 0, 0xFFFFFFFF, // Set transmission type on event timer
        0xFFFFFFFF, 0x5, 2, 0, 0xFFFFFFFF // Set transmission time on event timer
    };

char *map2_pdo_error[2] =
    {
        "PDO mapped",
        "Cannot map PDO"
    };

struct state_machine_struct map2_pdo_machine =
    {map2_pdo, 8, map2_pdo_param, 40, map2_pdo_error};

void *map3_pdo[9] =
    {
        &writeNetworkDictCallBack, // Set bit 31 of the COB-ID
        &writeNetworkDictCallBack, // Set the number of entry to 0
        &writeNetworkDictCallBack, // Set the mapping object 1
        &writeNetworkDictCallBack, // Set the mapping object 2
        &writeNetworkDictCallBack, // Set the mapping object 3
        &writeNetworkDictCallBack, // Set the number of entry
        &writeNetworkDictCallBack, // Clear bit 31 of the COB-ID
        &writeNetworkDictCallBack, // Set transmission type on event timer
        &writeNetworkDictCallBack  // Set transmission time
    };
/**
 * Optional parameter:
 *      0x1800 | pdo_number
 *      0xC0000000 | COB-ID
 *      0x1A00 | pdo_number
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 1
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 2
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 3
 *      0x1A00 | pdo_number
 *      0x1800 | pdo_number
 *      0x40000000 | COB-ID
 *      0x1800 | pdo_number
 *      transmission type
 *      0x1800 | pdo_number
 *      transmission time
 */UNS32 map3_pdo_param[45] =
    {
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Set bit 31 of the COB-ID
        0xFFFFFFFF, 0x0, 1, 0, 0x00, // Set the number of entry to 0
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Set the mapping object 1
        0xFFFFFFFF, 0x2, 4, 0, 0xFFFFFFFF, // Set the mapping object 2
        0xFFFFFFFF, 0x3, 4, 0, 0xFFFFFFFF, // Set the mapping object 3
        0xFFFFFFFF, 0x0, 1, 0, 0x03, // Set the number of entry
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Clear bit 31 of the COB-ID
        0xFFFFFFFF, 0x2, 1, 0, 0xFFFFFFFF, // Set transmission type on event timer
        0xFFFFFFFF, 0x5, 2, 0, 0xFFFFFFFF // Set transmission time on event timer
    };

char *map3_pdo_error[2] =
    {
        "PDO mapped",
        "Cannot map PDO"
    };

struct state_machine_struct map3_pdo_machine =
    {map3_pdo, 9, map3_pdo_param, 45, map3_pdo_error};

void *map4_pdo[10] =
    {
        &writeNetworkDictCallBack, // Set bit 31 of the COB-ID
        &writeNetworkDictCallBack, // Set the number of entry to 0
        &writeNetworkDictCallBack, // Set the mapping object 1
        &writeNetworkDictCallBack, // Set the mapping object 2
        &writeNetworkDictCallBack, // Set the mapping object 3
        &writeNetworkDictCallBack, // Set the mapping object 4
        &writeNetworkDictCallBack, // Set the number of entry
        &writeNetworkDictCallBack, // Clear bit 31 of the COB-ID
        &writeNetworkDictCallBack, // Set transmission type on event timer
        &writeNetworkDictCallBack  // Set transmission time
    };
/**
 * Optional parameter:
 *      0x1800 | pdo_number
 *      0xC0000000 | COB-ID
 *      0x1A00 | pdo_number
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 1
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 2
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 3
 *      0x1A00 | pdo_number
 *      MAPPING OBJECT 4
 *      0x1A00 | pdo_number
 *      0x1800 | pdo_number
 *      0x40000000 | COB-ID
 *      0x1800 | pdo_number
 *      transmission type
 *      0x1800 | pdo_number
 *      transmission time
 */UNS32 map4_pdo_param[50] =
    {
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Set bit 31 of the COB-ID
        0xFFFFFFFF, 0x0, 1, 0, 0x00, // Set the number of entry to 0
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Set the mapping object 1
        0xFFFFFFFF, 0x2, 4, 0, 0xFFFFFFFF, // Set the mapping object 2
        0xFFFFFFFF, 0x3, 4, 0, 0xFFFFFFFF, // Set the mapping object 3
        0xFFFFFFFF, 0x4, 4, 0, 0xFFFFFFFF, // Set the mapping object 4
        0xFFFFFFFF, 0x0, 1, 0, 0x04, // Set the number of entry
        0xFFFFFFFF, 0x1, 4, 0, 0xFFFFFFFF, // Clear bit 31 of the COB-ID
        0xFFFFFFFF, 0x2, 1, 0, 0xFFFFFFFF, // Set transmission type on event timer
        0xFFFFFFFF, 0x5, 2, 0, 0xFFFFFFFF // Set transmission time on event timer
    };

char *map4_pdo_error[2] =
    {
        "PDO mapped",
        "Cannot map PDO"
    };

struct state_machine_struct map4_pdo_machine =
    {map4_pdo, 10, map4_pdo_param, 50, map4_pdo_error};

void *heart_start[1] =
    {
        &writeNetworkDictCallBack
    };
UNS32 heart_start_param[5] =
    {
        0x1017, 0x0, 2, 0, 0xFFFFFFFF
    };

char *heart_start_error[2] =
    {
        "Heartbeat configured",
        "Cannot configure heartbeat"
    };

struct state_machine_struct heart_start_machine =
    {heart_start, 1, heart_start_param, 5, heart_start_error};

void *smart_stop[10] =
    {
        &writeNetworkDictCallBack,  // Change state: switched off
        &writeNetworkDictCallBack,  // Change state: switched off
        &writeNetworkDictCallBack,  // Change state: switched off
        &writeNetworkDictCallBack,  // Disable positive limit switch input
        &writeNetworkDictCallBack,  // Disable negative limit switch input
        &writeNetworkDictCallBack,  // Set mode velocity
        &writeNetworkDictCallBack,  // Set velocity in PV mode
        &writeNetworkDictCallBack,  // Change state: ready to switch on
        &writeNetworkDictCallBack,  // Change state: switched on
        &writeNetworkDictCallBack,  // Start command
    };
UNS32 smart_stop_param[50] =
    {
        0x6040, 0x0, 2, 0, 0x0,  // Change state: switched off
        0x6040, 0x0, 2, 0, 0x80,  // Change state: switched off
        0x6040, 0x0, 2, 0, 0x0,  // Change state: switched off
        0x2101, 0x3, 2, 0, 0x2,  // Disable positive limit switch input
        0x2101, 0x3, 2, 0, 0x3,  // Disable negative limit switch input
        0x6060, 0x0, 1, 0, 0x3,  // Set mode velocity
        0x60FF, 0x0, 4, 0, 0x0,  // Set velocity in PV mode
        0x6040, 0x0, 2, 0, 0x6,  // Change state: ready to switch on
        0x6040, 0x0, 2, 0, 0x7,  // Change state: switched on
        0x6040, 0x0, 2, 0, 0xF,  // Start command
    };

char *smart_stop_error[2] =
    {
        "smart motor stopped",
        "cannost stop smartmotor"
    };

struct state_machine_struct smart_stop_machine =
    {smart_stop, 10, smart_stop_param, 50, smart_stop_error};

void *smart_position_set_function[10] =
    {
        &writeNetworkDictCallBack, // Reset the status word
        &writeNetworkDictCallBack, // Set mode position
        &writeNetworkDictCallBack, // Set profile speed
        &writeNetworkDictCallBack, // Set target position to zero
        &writeNetworkDictCallBack, // Set acceleration
        &writeNetworkDictCallBack, // Set deceleration
        &writeNetworkDictCallBack, // Change state: ready to switch on
        &writeNetworkDictCallBack, // Change state: switched on
        &writeNetworkDictCallBack, // Enable command, single setpoint (motion not actually started yet)
        &writeNetworkDictCallBack, // Begin motion to target position
    };
/*
 * param
 * profile speed
 * target point
 */UNS32 smart_position_set_param[50] =
    {
        0x6040, 0x0, 2, 0, 0x80, // Reset status word
        0x6060, 0x0, 1, 0, 0x1, // Set mode position
        0x6081, 0x0, 4, 0, 0xFFFFFFFF, // Set profile speed
        0x607A, 0x0, 4, 0, 0xFFFFFFFF, // Set target position to destination
        0x6083, 0x0, 4, 0, 0xa, // Set acceleration
        0x6084, 0x0, 4, 0, 0xa, // Set deceleration
        0x6040, 0x0, 2, 0, 0x6, // Change state: ready to switch on
        0x6040, 0x0, 2, 0, 0x7, // Change state: switched on
        0x6040, 0x0, 2, 0, 0x2F, // Enable command, single setpoint (motion not actually started yet)
        0x6040, 0x0, 2, 0, 0x3F, // Begin motion to target position
    };

char *smart_position_set_error[2] =
    {
        "smart motor go to target point. . .",
        "cannot reach target point"
    };

struct state_machine_struct smart_position_set_machine =
    {smart_position_set_function, 10, smart_position_set_param, 50,
        smart_position_set_error};

void *smart_interpolation_test1[19] =
    {
        &writeNetworkDictCallBack, // Reset the status word
        &writeNetworkDictCallBack, // Set interpolation mode
        &writeNetworkDictCallBack, // Change state: ready to switch on
        &writeNetworkDictCallBack, // Change state: switched on
        &writeNetworkDictCallBack, // Clear buffer
        &writeNetworkDictCallBack, // Enable buffer
        &writeNetworkDictCallBack, // Set time period to 1 (second)
        &writeNetworkDictCallBack, // Set time period to seconds
        &writeNetworkDictCallBack, // Write data point 1
        &writeNetworkDictCallBack, // Write data point 2
        &writeNetworkDictCallBack, // Write data point 3
        &writeNetworkDictCallBack, // Write data point 4
        &writeNetworkDictCallBack, // Write data point 5
        &writeNetworkDictCallBack, // Write data point 6
        &writeNetworkDictCallBack, // Write zero-length segment
        &writeNetworkDictCallBack, // Write data point
        &writeNetworkDictCallBack, // This is required to satisfy CiA 402 drive state machine
        &writeNetworkDictCallBack, // Enable command (motion not actually started yet
        &writeNetworkDictCallBack // Begin motion
    };
UNS32 smart_interpolation_test1_param[95] =
    {
        0x6040, 0x0, 2, 0, 0x80, // Reset the status word
        0x6060, 0x0, 1, 0, 0x7, // Set interpolation mode
        0x6040, 0x0, 2, 0, 0x6, // Change state: ready to switch on
        0x6040, 0x0, 2, 0, 0x7, // Change state: switched on
        0x60C4, 0x6, 1, 0, 0x0, // Clear buffer
        0x60C4, 0x6, 1, 0, 0x1, // Enable buffer
        0x60C2, 0x1, 1, 0, 0x1, // Set time period to 1 (second)
        0x60C2, 0x2, 1, 0, 0x0, // Set time period to seconds
        0x60C1, 0x1, 4, 0, 0x0, // Write data point 1
        0x60C1, 0x1, 4, 0, 0x3E8, // Write data point 2
        0x60C1, 0x1, 4, 0, 0xBB8, // Write data point 3
        0x60C1, 0x1, 4, 0, 0x7D0, // Write data point 4
        0x60C1, 0x1, 4, 0, 0x3E8, // Write data point 5
        0x60C1, 0x1, 4, 0, 0x0, // Write data point 6
        0x60C2, 0x1, 1, 0, 0x0, // Write zero-length segment
        0x60C1, 0x1, 4, 0, 0x0, // Write data point
        0x6040, 0x0, 2, 0, 0x6, // This is required to satisfy CiA 402 drive state machine
        0x6040, 0x0, 2, 0, 0xF, // Enable command (motion not actually started yet
        0x6040, 0x0, 2, 0, 0x1F // Begin motion
    };

char *smart_interpolation_test1_error[2] =
    {
        "smartmotor interpolation mode started",
        "Cannot start smartmotor in interpolation mode"
    };

struct state_machine_struct smart_interpolation_test1_machine =
    {smart_interpolation_test1, 19, smart_interpolation_test1_param, 95,
        smart_interpolation_test1_error};

void *smart_interpolation_test2[19] =
    {
        &writeNetworkDictCallBack, // Reset the status word
        &writeNetworkDictCallBack, // Change state: ready to switch on
        &writeNetworkDictCallBack, // Change state: switched on
        &writeNetworkDictCallBack, // Clear buffer
        &writeNetworkDictCallBack, // Enable buffer
        &writeNetworkDictCallBack, // Set time period to seconds
        &writeNetworkDictCallBack, // Set time period to 1 (second)
        &writeNetworkDictCallBack, // Set interpolation mode
        &writeNetworkDictCallBack, // Write data point 1
        &writeNetworkDictCallBack, // Write data point 2
        &writeNetworkDictCallBack, // Write data point 3
        &writeNetworkDictCallBack, // Write data point 4
        &writeNetworkDictCallBack, // Write data point 5
        &writeNetworkDictCallBack, // Write data point 6
        &writeNetworkDictCallBack, // Write zero-length segment
        &writeNetworkDictCallBack, // Write data point
        &writeNetworkDictCallBack, // This is required to satisfy CiA 402 drive state machine
        &writeNetworkDictCallBack, // Enable command (motion not actually started yet
        &writeNetworkDictCallBack // Begin motion
    };
UNS32 smart_interpolation_test2_param[95] =
    {
        0x6040, 0x0, 2, 0, 0x80, // Reset the status word
        0x6040, 0x0, 2, 0, 0x6, // Change state: ready to switch on
        0x6040, 0x0, 2, 0, 0x7, // Change state: switched on
        0x60C4, 0x6, 1, 0, 0x0, // Clear buffer
        0x60C4, 0x6, 1, 0, 0x1, // Enable buffer
        0x60C2, 0x2, 1, 0, 0xFD, // Set time period to seconds
        0x60C2, 0x1, 1, 0, 0x14, // Set time period to 1 (second)
        0x6060, 0x0, 1, 0, 0x7, // Set interpolation mode
        0x60C1, 0x1, 4, 0, 0x0, // Write data point 1
        0x60C1, 0x1, 4, 0, 0x3E8, // Write data point 2
        0x60C1, 0x1, 4, 0, 0xBB8, // Write data point 3
        0x60C1, 0x1, 4, 0, 0x7D0, // Write data point 4
        0x60C1, 0x1, 4, 0, 0x3E8, // Write data point 5
        0x60C1, 0x1, 4, 0, 0x0, // Write data point 6
        0x60C2, 0x1, 1, 0, 0x0, // Write zero-length segment
        0x60C1, 0x1, 4, 0, 0x0, // Write data point
        0x6040, 0x0, 2, 0, 0x6, // This is required to satisfy CiA 402 drive state machine
        0x6040, 0x0, 2, 0, 0xF, // Enable command (motion not actually started yet
        0x6040, 0x0, 2, 0, 0x1F // Begin motion
    };

char *smart_interpolation_test2_error[2] =
    {
        "smartmotor interpolation mode started",
        "Cannot start smartmotor in interpolation mode"
    };

struct state_machine_struct smart_interpolation_test2_machine =
    {smart_interpolation_test2, 19, smart_interpolation_test2_param, 95,
        smart_interpolation_test2_error};

void *init_interpolation_function[10] =
    {
        &writeNetworkDictCallBack, // Change state: ready to switch on
        &writeNetworkDictCallBack, // Change state: switched on
        //&writeNetworkDictCallBack, // Enable positive limit switch
        //&writeNetworkDictCallBack, // Enable negative limit switch
        &writeNetworkDictCallBack, // Enable command (motion not actually started yet)
        &writeNetworkDictCallBack, // Clear buffer
        &writeNetworkDictCallBack, // Enable buffer
        &writeNetworkDictCallBack, // Set time period to seconds
        &writeNetworkDictCallBack, // Set time period to 1 (second)
        &writeNetworkDictCallBack, // Set interpolation mode
        &writeNetworkDictCallBack, // Set interpolation sub-mode (spline)
        &writeNetworkDictCallBack // Set first point to zero
    };
/*
 * Param
 *   current_position
 */UNS32 init_interpolation_param[50] =
    {
        0x6040, 0x0, 2, 0, 0x6, // Change state: ready to switch on
        0x6040, 0x0, 2, 0, 0x7, // Change state: switched on
        //0x2309, 0x0, 2, 0, -4, // Enable positive limit switch
        //0x2309, 0x0, 2, 0, -5, // Enable negative limit switch
        0x6040, 0x0, 2, 0, 0xF, // Enable command (motion not actually started yet)
        0x60C4, 0x6, 1, 0, 0x0, // Clear buffer
        0x60C4, 0x6, 1, 0, 0x1, // Enable buffer
        0x60C2, 0x1, 1, 0, 0x1, // Set time period to 1 (second)
        0x60C2, 0x2, 1, 0, 0x00, // Set time period to seconds
        0x6060, 0x0, 1, 0, 0x7, // Set interpolation mode
        0X60C0, 0X0, 2, 0, 0, // Set interpolation sub-mode
        0x60C1, 0x1, 4, 0, 0xFFFFFFFF // Set first point to current position
    };

char *init_interpolation_error[2] =
    {
        "smartmotor interpolation mode init",
        "Cannot init smartmotor in interpolation mode"
    };

struct state_machine_struct init_interpolation_machine =
    {
        init_interpolation_function, 10, init_interpolation_param, 50,
        init_interpolation_error
    };

void *start_interpolation_function[1] =
    {
        &writeNetworkDictCallBack // Begin motion
    };
UNS32 start_interpolation_param[5] =
    {
        0x6040, 0x0, 2, 0, 0x1F // Begin motion
    };

char *start_interpolation_error[2] =
    {
        "smartmotor interpolation mode start",
        "Cannot start smartmotor in interpolation mode"
    };

struct state_machine_struct start_interpolation_machine =
    {start_interpolation_function, 1, start_interpolation_param, 5,
        start_interpolation_error};

void *stop_interpolation_function[2] =
    {
        &writeNetworkDictCallBack, // Write zero-length segment
        &writeNetworkDictCallBack, // Repeat final data
    //&writeNetworkDictCallBack // leave drive on but holding at the ending position
    };
UNS32 stop_interpolation_param[10] =
    {
        0x60C2, 0x1, 1, 0, 0x0, // Write zero-length segment
        0x60C1, 0x1, 4, 0, 0xFFFFFFFF, // Repeat final data
    //0x6040, 0x0, 2, 0, 0x0F, // leave drive on but holding at the ending position
    };

char *stop_interpolation_error[2] =
    {
        "smartmotor interpolation mode closing. . .",
        "Cannot stop simulation"
    };

struct state_machine_struct stop_interpolation_machine =
    {stop_interpolation_function, 2, stop_interpolation_param, 10,
        stop_interpolation_error};

void *resume_interpolation_function[3] =
    {
        &writeNetworkDictCallBack, // Enable command (motion not actually started yet)
        &writeNetworkDictCallBack, // back to the desire value
        &writeNetworkDictCallBack  // Repeat final data
    };
/*
 * param:
 *   - current position
 */UNS32 resume_interpolation_param[15] =
    {
        0x6040, 0x0, 2, 0, 0xF, // Enable command (motion not actually started yet)
        0x60C2, 0x1, 1, 0, 0x64, // back to the desire value
        0x60C1, 0x1, 4, 0, 0xFFFFFFFF, // Repeat final data
    };

char *resume_interpolation_error[2] =
    {
        "Interpolation resumed",
        "Cannot resume interpolation"
    };

struct state_machine_struct resume_interpolation_machine =
    {
        resume_interpolation_function, 3, resume_interpolation_param, 15,
        resume_interpolation_error
    };

void *smart_off_function[3] =
    {
        &writeNetworkDictCallBack, // Change state: switched off
        &writeNetworkDictCallBack, // Change state: switched off
        &writeNetworkDictCallBack, // Change state: switched off
    };
UNS32 smart_off_param[15] =
    {
        0x6040, 0x0, 2, 0, 0x0,  // Change state: switched off
        0x6040, 0x0, 2, 0, 0x80,  // Change state: switched off
        0x6040, 0x0, 2, 0, 0x0,  // Change state: switched off
    };

char *smart_off_error[2] =
    {
        "smartmotor off",
        "Cannot switch off smartmotor"
    };

struct state_machine_struct smart_off_machine =
    {
        smart_off_function, 3, smart_off_param, 15, smart_off_error
    };

void *torque_function[9] =
    {
        &writeNetworkDictCallBack, // Set mode torque
        &writeNetworkDictCallBack, // Set torque slope to 200
        &writeNetworkDictCallBack, // Set torque to zero
        &writeNetworkDictCallBack, // Change state ready to switch on
        &writeNetworkDictCallBack // Start command
    };
/*
 * Param
 *   torque
 */UNS32 torque_param[25] =
    {
        0x6060, 0x0, 1, 0, 0x4, // Set mode torque
        0x6087, 0x0, 4, 0, 0xC8, // Set torque slope to 200
        0x6071, 0x0, 2, 0, 0xFFFFFFFF, // Set torque
        0x6040, 0x0, 2, 0, 0x6, // Change state ready to switch on
        0x6040, 0x0, 2, 0, 0xf // Start command
    };

char *torque_error[2] =
    {
        "smartmotor torque mode",
        "Cannot pass smartmotor in torque mode"
    };

struct state_machine_struct torque_machine =
    {
        torque_function, 5, torque_param, 25, torque_error
    };

void *smart_limit_disable_function[2] =
    {
        &writeNetworkDictCallBack, // Enable left limit switch
        &writeNetworkDictCallBack // Enable right limit switch
    };
UNS32 smart_limit_disable_param[10] =
    {
        0x2101, 0x3, 2, 0, 0x2,  // Disable positive limit switch input
        0x2101, 0x3, 2, 0, 0x3,  // Disable negative limit switch input

    };

char *smart_limit_disable_error[2] =
    {
        "smartmotor disable limits",
        "Cannot disable limits"
    };

struct state_machine_struct smart_limit_disable_machine =
    {smart_limit_disable_function, 2, smart_limit_disable_param, 10,
        smart_limit_disable_error};

void *smart_limit_enable_function[2] =
    {
        &writeNetworkDictCallBack, // Enable left limit switch
        &writeNetworkDictCallBack // Enable right limit switch
    };
/*
 * param:
 *   forward velocity
 *   backward velocity
 *   homing offset
 */UNS32 smart_limit_enable_param[10] =
    {
        0x2309, 0x0, 2, 0, -4, // Enable positive limit switch
        0x2309, 0x0, 2, 0, -5 // Enable negative limit switch

    };

char *smart_limit_enable_error[2] =
    {
        "smartmotor enable limits",
        "Cannot enable limits"
    };

struct state_machine_struct smart_limit_enable_machine =
    {smart_limit_enable_function, 2, smart_limit_enable_param, 10,
        smart_limit_enable_error};

void *smart_homing_function[15] =
    {
        &writeNetworkDictCallBack, // Reset status word
        &writeNetworkDictCallBack, // Reset status word
        &writeNetworkDictCallBack, // Reset status word
        &writeNetworkDictCallBack, // Enable left limit switch
        &writeNetworkDictCallBack, // Enable right limit switch
        &writeNetworkDictCallBack, // Set mode of operation to homing
        &writeNetworkDictCallBack, // Set homing method
        &writeNetworkDictCallBack, // Set homing speed during search for switch
        &writeNetworkDictCallBack, // Set homing speed during search for zero
        &writeNetworkDictCallBack, // Set homing acceleration
        &writeNetworkDictCallBack, // Set homing offset
        &writeNetworkDictCallBack, // This is required to satisfy cia 402 drive state machine
        &writeNetworkDictCallBack, // This is required to satisfy cia 402 drive state machine
        &writeNetworkDictCallBack, // Enable command (motion not actually started yet)
        &writeNetworkDictCallBack, // Begin motion
    };
/*
 * param:
 *   forward velocity
 *   backward velocity
 *   homing offset
 */UNS32 smart_homing_param[75] =
    {
        0x6040, 0x0, 2, 0, 0x0, // Reset status word
        0x6040, 0x0, 2, 0, 0x80, // Reset status word
        0x6040, 0x0, 2, 0, 0x0, // Reset status word
        0x6060, 0x0, 1, 0, 0x6, // Set mode of operation to homing
        0x6098, 0x0, 1, 0, 18, // Set homing method
        0x2309, 0x0, 2, 0, -4, // Enable positive limit switch
        0x2309, 0x0, 2, 0, -5, // Enable negative limit switch
        0x6099, 0x1, 4, 0, 0xFFFFFFFF, // Set homing speed during search for switch
        0x6099, 0x2, 4, 0, 0xFFFFFFFF, // Set homing speed during search for zero
        0x609a, 0x0, 4, 0, 10, // Set homing acceleration
        0x607c, 0x0, 4, 0, 0xFFFFFFFF, // Set homing offset
        0x6040, 0x0, 2, 0, 0x6, // This is required to satisfy cia 402 drive state machine
        0x6040, 0x0, 2, 0, 0x7, // This is required to satisfy cia 402 drive state machine
        0x6040, 0x0, 2, 0, 0xF, // Enable command (motion not actually started yet)
        0x6040, 0x0, 2, 0, 0x1F, // Begin motion

    };

char *smart_homing_error[2] =
    {
        "smartmotor homing. . .",
        "Cannot run smart motor in homing mode"
    };

struct state_machine_struct smart_homing_machine =
    {smart_homing_function, 15, smart_homing_param, 75,
        smart_homing_error};

void *smart_acceleration_pp_get_function[1] =
    {
        &readNetworkDictCallback // Read smartmotor ADT
    };
UNS32 smart_acceleration_pp_get_param[3] =
    {
        0x6083, 0x0, 0 // Read smartmotor ADT
    };

char *smart_acceleration_pp_get_error[2] =
    {
        NULL,
        "Cannot read ADT"
    };

struct state_machine_struct smart_acceleration_pp_get_machine =
    {smart_acceleration_pp_get_function, 1, smart_acceleration_pp_get_param, 3,
        smart_acceleration_pp_get_error};

void *smart_following_error_get_function[1] =
    {
        &readNetworkDictCallback // Read smartmotor EL
    };
UNS32 smart_following_error_get_param[3] =
    {
        0x6065, 0x0, 0 // Read smartmotor EL
    };

char *smart_following_error_get_error[2] =
    {
        NULL,
        "Cannot read EL"
    };

struct state_machine_struct smart_following_error_get_machine =
    {smart_following_error_get_function, 1, smart_following_error_get_param, 3,
        smart_following_error_get_error};

void *smart_velocity_pp_get_function[1] =
    {
        &readNetworkDictCallback // Read smartmotor VT
    };
UNS32 smart_velocity_pp_get_param[3] =
    {
        0x6081, 0x0, 0 // Read smartmotor VT
    };

char *smart_velocity_pp_get_error[2] =
    {
        NULL,
        "Cannot read VT"
    };

struct state_machine_struct smart_velocity_pp_get_machine =
    {smart_velocity_pp_get_function, 1, smart_velocity_pp_get_param, 3,
        smart_velocity_pp_get_error};

void *smart_velocity_pp_set_function[1] =
    {
        &writeNetworkDictCallBack, // Reset status word
    };
/*
 * PARAM
 * VT
 */UNS32 smart_velocity_pp_set_param[5] =
    {
        0x6081, 0x0, 4, 0, 0xFFFFFFFF // Reset status word
    };

char *smart_velocity_pp_set_error[2] =
    {
        "Velocity target set",
        "Cannot set the velocity target"
    };

struct state_machine_struct smart_velocity_pp_set_machine =
    {smart_velocity_pp_set_function, 1, smart_velocity_pp_set_param, 5,
        smart_velocity_pp_set_error};

void *smart_statusword_function[1] =
    {
        &readNetworkDictCallback // Read smartmotor status word 0
    };
UNS32 smart_statusword_param[3] =
    {
        0x2304, 0x1, 0 // Read smartmotor status word 0
    };

char *smart_statusword_error[2] =
    {
        "Status word read",
        "Cannot read status word0 register"
    };

struct state_machine_struct smart_statusword_machine =
    {smart_statusword_function, 1, smart_statusword_param, 3,
        smart_statusword_error};

void *smart_reset_statusword_function[1] =
    {
        &writeNetworkDictCallBack, // Reset status word
    };
UNS32 smart_reset_statusword_param[5] =
    {
        0x6040, 0x0, 2, 0, 0x80 // Reset status word
    };

char *smart_reset_statusword_error[2] =
    {
        "Status word cleared",
        "Cannot reset status word"
    };

struct state_machine_struct smart_reset_statusword_machine =
    {smart_reset_statusword_function, 1, smart_reset_statusword_param, 5,
        smart_reset_statusword_error};

void *smart_origin_function[1] =
    {
        &writeNetworkDictCallBack, // Set origin
    };
UNS32 smart_origin_param[5] =
    {
        0x2202, 0x0, 4, 0, 0x00  // Set origin
    };

char *smart_origin_error[2] =
    {
        "Origin set",
        "Cannot set origin"
    };

struct state_machine_struct smart_origin_machine =
    {smart_origin_function, 1, smart_origin_param, 5, smart_origin_error};

void _machine_init()
{
  int i = 0;

  pthread_mutexattr_t Attr;

  pthread_mutexattr_init(&Attr);
  //pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_ERRORCHECK);

  for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
  {
    motor_active[i] = 0;

    motor_active[i] = 0;
    motor_started[i] = 0;

    next_machine_size[i] = 0;
    next_var_count[i] = 0;
    var_count_total[i] = 0;
    machine_state_index[i] = 0;
    machine_state_param_index[i] = 0;

#ifndef SDO_SYNC
    pthread_mutex_init(&machine_mux[i], &Attr);
#endif
  }

#ifdef SDO_SYNC
  pthread_mutex_init(&machine_mux, &Attr);
  pthread_cond_init(&machine_run_cond, NULL);
#endif

  motor_active[0] = 1;
}

void _machine_destroy()
{
#ifndef SDO_SYNC
  int i = 0;

  for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
  {
    pthread_mutex_destroy(&machine_mux[i]);
  }
#else
  pthread_mutex_destroy(&machine_mux);
#endif
}

void _machine_reset(CO_Data* d, UNS8 nodeId)
{
  int sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

#ifdef CANOPENSHELL_VERBOSE
  if(verbose_flag)
  {
    if((sdo_result != 0) && (sdo_result != 0xFF))
    {
      printf("ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
          InternalError, nodeId, machine_state_index[nodeId],
          next_machine[nodeId][0]->error[1], sdo_result);
    }
  }
#endif

  motor_active[nodeId] = 0;
  motor_started[nodeId] = 0;

  next_machine_size[nodeId] = 0;
  next_var_count[nodeId] = 0;
  var_count_total[nodeId] = 0;
  machine_state_index[nodeId] = 0;
  machine_state_param_index[nodeId] = 0;

  if(args_ptr[nodeId] != NULL)
  {
    free(args_ptr[nodeId]);
    args_ptr[nodeId] = NULL;
    next_args[nodeId] = NULL;
  }

  if(next_machine[nodeId] != NULL)
  {
    free(next_machine[nodeId]);
    next_machine[nodeId] = NULL;
  }

#ifndef SDO_SYNC
  machine_run[nodeId] = 0;
#endif
}

/**
 * @TODO: adeguare i commenti
 */
/**
 * Callback per le richieste di tipo SDO alla funzione _vsmart_start().
 *
 * @param d: puntatore al nodo canopen
 * @param nodeID - ID del nodo da cui proviene la risposta
 *
 * @remark: In caso di errore, la macchina viene fermata e viene caricato il codice il relativo codice
 * nella variabile globale canopen_abort_code.
 */

void _machine_callback(CO_Data* d, UNS8 nodeId)
{
  _machine_exe(d, nodeId, NULL, NULL, 0, 1, 0);
}

/**
 * Inizializza i parametri del motore SmartMotor e lo abilita.
 *
 * @param d: puntatore al nodo CanOpen.
 * @param nodeId: ID del nodo con cui comunicare
 * @param callback: array di callback da cui viene presa la prossima funzione da eseguire alla fine della macchina a stati
 * @param callback_num: dimensione della matrice di callback
 * @param var_count: numero di variabili contenute nella lista args
 * @param args: lista dei parametri da passare alla funzione successiva
 *
 */

int _machine_exe(CO_Data *d, UNS8 nodeId, MachineCallback_t machine_callback,
    struct state_machine_struct *machine[], int callback_num, int from_callback,
    int var_count,
    ...)
{
  void *last_function = NULL;
  UNS32 readNetworkDictCallback_result = 0;
  int result_value = 0;

  int lock_value;
  int i, j;

  static int function_call_number[CANOPEN_NODE_NUMBER];

  if(callback_num != 0) // se la funzione non è stata richiamata dal callback
  {
#ifndef SDO_SYNC
    // Controllo se _machine_exe sia impegnata con lo stesso motore in un'altra
    // operazione
    if(nodeId != 0)
    {
      lock_value = pthread_mutex_lock(&machine_mux[nodeId]);

      if(machine_run[nodeId] == 1)
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf(
              "ERR[%d on node %x state %d]: %s (Operazione in corso)\n",
              InternalError, nodeId, machine_state_index[nodeId],
              machine[0]->error[1]);
        }
#endif

        lock_value = pthread_mutex_unlock(&machine_mux[nodeId]);

        fflush(stdout);
        return 1;
      }
    }
    else
    {
      for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
      {
        if(motor_active[i] > 0)
        {
          lock_value = pthread_mutex_lock(&machine_mux[i]);

          if(machine_run[i] == 1)
          {
#ifdef CANOPENSHELL_VERBOSE
            if(verbose_flag)
            {
              printf(
                  "ERR[%d on node %x state %d]: %s (Operazione in corso node 0)\n",
                  InternalError, i, machine_state_index[i],
                  machine[0]->error[1]);

              printf("callback num: %d\n", callback_num);
            }
#endif

            lock_value = pthread_mutex_unlock(&machine_mux[i]);

            fflush(stdout);
            return 1;
          }

          lock_value = pthread_mutex_unlock(&machine_mux[i]);
        }
      }
    }

#else
    lock_value = pthread_mutex_lock(&machine_mux);

    if(machine_run == 1)
    {
      printf("machine wait %d\n", nodeId);
      pthread_cond_wait(&machine_run_cond, &machine_mux);
    }

    printf("machine %d\n", nodeId);
#endif

    if(machine_state_index[nodeId] > 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf(
            "WARN[%d on node %x state %d]: Qualcuno è entrato senza bussare...\n",
            InternalError, nodeId, machine_state_index[nodeId]);
      }
#endif
      lock_value = pthread_mutex_unlock(&machine_mux[nodeId]);
      return -1;
    }

    // se non è una richiesta broadcast
    if(nodeId != 0)
    {
#ifndef SDO_SYNC
      machine_run[nodeId] = 1;
#else
      machine_run = 1;
#endif
      callback_user[nodeId] = machine_callback;
    }
    else
    {
      for(i = 1; i < CANOPEN_NODE_NUMBER; i++)
      {
        if(motor_active[i] > 0)
          callback_user[i] = machine_callback;
      }

#ifdef SDO_SYNC
      machine_run = 1;
#endif
    }

#ifndef SDO_SYNC
    lock_value = pthread_mutex_unlock(&machine_mux[nodeId]);
#else
    lock_value = pthread_mutex_unlock(&machine_mux);
#endif

    // Memorizzo il vettore con le prossime funzioni da eseguire
    if(callback_num == 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x state %d]: Nessuna funzione\n", InternalError,
            nodeId, machine_state_index[nodeId]);
      }
#endif

      result_value = 1;
      goto finalize;
    }

    // memorizzo il numero di macchine presenti nell'array dei callback
    if(nodeId != 0)
      next_machine_size[nodeId] = callback_num;
    else  // se è un messaggio broadcast
    {
      for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
      {
        if(motor_active[i] > 0)
          next_machine_size[i] = callback_num;
      }
    }

    // salvo l'array dei callback
    if(nodeId != 0)
    {
      if(next_machine[nodeId] == NULL)
      {
        next_machine[nodeId] = malloc(
            next_machine_size[nodeId]
                * sizeof(struct state_machine_struct *));

        if(next_machine[nodeId] == NULL)
        {
#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf(
                "ERR[%d on node %x state %d]: %s (memoria finita per allocare next_machine[%d])\n",
                InternalError, nodeId, machine_state_index[nodeId],
                next_machine[nodeId][0]->error[1], nodeId);
          }
#endif

          result_value = 1;
          goto finalize;
        }
      }
      else
      {
        free(next_machine[nodeId]);
        next_machine[nodeId] = NULL;
        next_machine[nodeId] = malloc(
            next_machine_size[nodeId]
                * sizeof(struct state_machine_struct *));

        if(next_machine[nodeId] == NULL)
        {
#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf(
                "ERR[%d on node %x state %d]: %s (memoria finita per allocare next_machine[%d])\n",
                InternalError, nodeId, machine_state_index[nodeId],
                next_machine[nodeId][0]->error[1], nodeId);
          }
#endif

          result_value = 1;
          goto finalize;
        }
      }

      for(i = 0; i < next_machine_size[nodeId]; i++)
        next_machine[nodeId][i] = machine[i];
    }
    else
    {
      for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
      {
        if(motor_active[i] > 0)
        {
          if(next_machine[i] == NULL)
          {
            next_machine[i] = malloc(
                next_machine_size[i]
                    * sizeof(struct state_machine_struct *));

            if(next_machine[i] == NULL)
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf(
                    "ERR[%d on node %x state %d]: %s (memoria finita per allocare next_machine[%d])\n",
                    InternalError, i, machine_state_index[i],
                    next_machine[i][0]->error[1], i);
              }
#endif

              result_value = 1;
              goto finalize;
            }
          }
          else
          {
            free(next_machine[i]);
            next_machine[i] = NULL;
            next_machine[i] = malloc(
                next_machine_size[i]
                    * sizeof(struct state_machine_struct *));

            if(next_machine[i] == NULL)
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf(
                    "ERR[%d on node %x state %d]: %s (memoria finita per allocare next_machine[%d])\n",
                    InternalError, i, machine_state_index[i],
                    next_machine[i][0]->error[1], i);
              }
#endif

              result_value = 1;
              goto finalize;
            }
          }
        }

        for(j = 0; j < next_machine_size[i]; j++)
          next_machine[i][j] = machine[j];
      }
    }

    // se è la prima funzione e se ci entro per la prima volta, mi salvo le variabili
    if(var_count != 0)
    {
      if(nodeId != 0)
      {
        next_var_count[nodeId] = var_count;
        var_count_total[nodeId] = var_count;

        if(args_ptr[nodeId] == NULL)
        {
          args_ptr[nodeId] = malloc(next_var_count[nodeId] * sizeof(UNS32));
          if(args_ptr[nodeId] == NULL)
          {
#ifdef CANOPENSHELL_VERBOSE
            if(verbose_flag)
            {
              printf(
                  "ERR[%d on node %x state %d]: %s (memoria finita per allocare args_ptr[%d])\n",
                  InternalError, nodeId, machine_state_index[nodeId],
                  next_machine[nodeId][0]->error[1], nodeId);
            }
#endif

            result_value = 1;
            goto finalize;
          }
        }
      }
      else
      {
        for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
        {
          if(motor_active[i] > 0)
          {
            next_var_count[i] = var_count;
            var_count_total[i] = var_count;

            if(args_ptr[i] == NULL)
            {
              args_ptr[i] = malloc(next_var_count[i] * sizeof(UNS32));
              if(args_ptr[i] == NULL)
              {
#ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (memoria finita per allocare args_ptr[%d])\n",
                      InternalError, i, machine_state_index[i],
                      next_machine[i][0]->error[1], i);
                }
#endif

                result_value = 1;
                goto finalize;
              }
            }
          }
        }
      }

      va_list args;
      va_start(args, var_count);

      if(nodeId != 0)
      {
        for(i = 0; i < var_count; i++)
          args_ptr[nodeId][i] = va_arg(args, UNS32);
      }
      else
      {
        for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
        {
          if(motor_active[i] > 0)
          {
            for(j = 0; j < var_count; j++)
              args_ptr[i][j] = va_arg(args, UNS32);
          }
        }
      }

      va_end(args);

      if(nodeId != 0)
        next_args[nodeId] = args_ptr[nodeId];
      else
      {
        for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
        {
          if(motor_active[i] > 0)
          {
            next_args[i] = args_ptr[nodeId];
          }
        }
      }
    }
  }
  else // if(callback_num != 0) // se la funzione non è stata richiamata dal callback
  {
    // Se al ritorno della callback l'indice è zero, vuol dire che è
    // stato resettato tutto
    if(machine_state_index[nodeId] <= 0)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x state %d]: Errore al ritorno dal callback\n",
            InternalError,
            nodeId, machine_state_index[nodeId]);
      }
#endif

      fflush(stdout);
      return 1;
    }
    else
    {
      if(machine_state_index[nodeId]
          > next_machine[nodeId][0]->function_size)
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf(
              "WARN[%d on node %x state %d]: Qualcuno si è intrufolato dalla porta posteriore...\n",
              InternalError, nodeId, machine_state_index[nodeId]);

          printf("Il numero di chiamate a questa funzione è: %d\n",
              function_call_number[nodeId]);
        }
#endif
      }

#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        if(function_call_number[nodeId] < 1)
          printf(
              "WARN[%d on node %x state %d]: Sei andato un po' troppo veloce...\n",
              InternalError, nodeId, machine_state_index[nodeId]);
      }
#endif

    }
  }

  last_function = NULL;

  // Se non è la prima volta che entro nella funzione, è il callback che mi ha fatto rientrare
  // qui. Quindi raccolgo la risposta e continuo con la macchina a stati.
  if(nodeId != 0)
  {
    if(machine_state_index[nodeId] != 0)
    {
      int sdo_result;
      UNS8 line;

      UNS8 i;

      for(i = 0; i < SDO_MAX_SIMULTANEOUS_TRANSFERS; i++)
      {
        if((d->transfers[i].state != SDO_RESET) &&
            (d->transfers[i].CliServNbr == nodeId))
        {
          line = i;
          break;
        }
      }

      switch(d->transfers[line].state)
      {
        case SDO_ABORTED_INTERNAL:
          #ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf(
                "ERR[%d on node %x state %d]: %s (Aborted internal code: %x)\n",
                InternalError, nodeId, machine_state_index[nodeId],
                next_machine[nodeId][0]->error[1],
                d->transfers[line].abortCode);
          }
#endif

          sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            if((sdo_result != 0) && (sdo_result != 0xFF))
            {
              printf("ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                  InternalError, nodeId, machine_state_index[nodeId],
                  next_machine[nodeId][0]->error[1], sdo_result);
            }
          }
#endif

          result_value = 1;
          goto finalize;
          break;

        default:
          if(next_machine[nodeId][0]->function[machine_state_index[nodeId]
              - 1]
              == &writeNetworkDictCallBack)
          {
            function_call_number[nodeId]--;
            last_function = &writeNetworkDictCallBack;

            sdo_result = getWriteResultNetworkDict(d, nodeId,
                &canopen_abort_code);
            switch(sdo_result)
            {
              case SDO_DOWNLOAD_IN_PROGRESS:
                case SDO_UPLOAD_IN_PROGRESS:
                while((sdo_result = getWriteResultNetworkDict(d, nodeId,
                    &canopen_abort_code)) != SDO_DOWNLOAD_IN_PROGRESS)
                  printf(
                      "ERR[%d on node %x state %d]: Download in progress (getWrite)\n",
                      InternalError, nodeId, machine_state_index[nodeId]);
                ;
                break;

              case SDO_FINISHED:
                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

#ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  if((sdo_result != 0) && (sdo_result != 0xFF))
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                        InternalError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], sdo_result);
                  }
                }
#endif

                if(canopen_abort_code > 0)
                {
#ifdef CANOPENSHELL_VERBOSE
                  if(verbose_flag)
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (Canopen abort code %x)\n",
                        CANOpenError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], canopen_abort_code);
                  }
#endif
                  result_value = 1;
                  goto finalize;
                }
                break;

              case SDO_ABORTED_INTERNAL:
                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);
#ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  if((sdo_result != 0) && (sdo_result != 0xFF))
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                        InternalError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], sdo_result);
                  }
                }
#endif
                break;

              case SDO_ABORTED_RCV:
                #ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO getWriteResult error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }
#endif

                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

#ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  if((sdo_result != 0) && (sdo_result != 0xFF))
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                        InternalError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], sdo_result);
                  }
                }
#endif

                //machine_state_index[nodeId]--; // provo a rieseguire la stessa funzione
                //machine_state_param_index[nodeId] -= 5;
                result_value = 1;
                goto finalize;
                break;

              case SDO_RESET:
                #ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO getWriteResult error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);

                  printf("Il numero di chiamate a questa funzione è: %d\n",
                      function_call_number[nodeId]);

                  fflush(stdout);
                }
#endif
                return 1;
                break;

              default:
                #ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO getWriteResult error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);

                  printf("Il numero 2 di chiamate a questa funzione è: %d\n",
                      function_call_number[nodeId]);
                }
#endif

                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

#ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  if((sdo_result != 0) && (sdo_result != 0xFF))
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                        InternalError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], sdo_result);
                  }
                }
#endif

                result_value = 1;
                goto finalize;
                break;
            }
          }
          else if(next_machine[nodeId][0]->function[machine_state_index[nodeId]
              - 1] == &readNetworkDictCallback)
          {
            function_call_number[nodeId]--;
            last_function = &readNetworkDictCallback;

            UNS32 size = 64;

            sdo_result = getReadResultNetworkDict(d, nodeId,
                &readNetworkDictCallback_result, &size,
                &canopen_abort_code);

            switch(sdo_result)
            {
              case SDO_DOWNLOAD_IN_PROGRESS:
                case SDO_UPLOAD_IN_PROGRESS:
                while((sdo_result = getReadResultNetworkDict(d, nodeId,
                    &readNetworkDictCallback_result,
                    &size, &canopen_abort_code)) != SDO_DOWNLOAD_IN_PROGRESS)
                  printf(
                      "ERR[%d on node %x state %d]: Download in progress (getRead)\n",
                      InternalError, nodeId, machine_state_index[nodeId]);
                ;
                break;

              case SDO_FINISHED:
                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

#ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  if((sdo_result != 0) && (sdo_result != 0xFF))
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                        InternalError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], sdo_result);
                  }
                }
#endif

                if(canopen_abort_code > 0)
                {
#ifdef CANOPENSHELL_VERBOSE
                  if(verbose_flag)
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (Canopen abort code %x)\n",
                        CANOpenError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], canopen_abort_code);
                  }
#endif

                  result_value = 1;
                  goto finalize;
                }

                if(callback_user[nodeId] != NULL)
                {
                  // Nel caso in cui non è l'ultima funzione oppure ci sono altre
                  // macchine accodate, eseguo ora la funzione

                  if((machine_state_index[nodeId]
                      != next_machine[nodeId][0]->function_size)
                      ||
                      ((next_machine[nodeId] != NULL)
                          && (next_machine_size[nodeId] > 1)))
                    callback_user[nodeId](d, nodeId,
                        machine_state_index[nodeId],
                        readNetworkDictCallback_result);
                }
#ifdef CANOPENSHELL_VERBOSE
                else
                {
                  if(verbose_flag)
                  {
                    printf(
                        "WARN[%d on node %x state %d]: Lettura senza funzione di callback \n",
                        InternalError, nodeId, machine_state_index[nodeId]);
                  }
                }
#endif

                break;

              case SDO_ABORTED_RCV:
                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

#ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  if((sdo_result != 0) && (sdo_result != 0xFF))
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                        InternalError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], sdo_result);
                  }

                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO getReadResult error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }
#endif

                machine_state_index[nodeId]--; // provo a rieseguire la stessa funzione
                machine_state_param_index[nodeId] -= 5;
                break;

              default:
                #ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO getReadResult error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }
#endif

                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

#ifdef CANOPENSHELL_VERBOSE
                if(verbose_flag)
                {
                  if((sdo_result != 0) && (sdo_result != 0xFF))
                  {
                    printf(
                        "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                        InternalError, nodeId, machine_state_index[nodeId],
                        next_machine[nodeId][0]->error[1], sdo_result);
                  }
                }
#endif

                result_value = 1;
                goto finalize;
                break;
            }
          }
          break;
      } //switch(d->transfers[line].state)
    }
    else
      last_function = next_machine[nodeId][0]->function[0];
  }

  next_machine_entry:
  // Eseguo la prossima funzione

// questa funzione non prevede un callback
  if((machine_state_index[nodeId] < next_machine[nodeId][0]->function_size)
      && (next_machine[nodeId][0]->function[machine_state_index[nodeId]]
          == &masterSendNMTstateChange))
  {
    while(next_machine[nodeId][0]->function[machine_state_index[nodeId]]
        == &masterSendNMTstateChange)
    {
      masterSendNMTstateChange_t machine_function =
          next_machine[nodeId][0]->function[machine_state_index[nodeId]];

      last_function =
          next_machine[nodeId][0]->function[machine_state_index[nodeId]];

      // chimao EnterMutex e LeaveMutex solo se non sono in un callback
      if(!from_callback)
      {
        EnterMutex();
      }

      machine_function(d, nodeId,
          next_machine[nodeId][0]->param[machine_state_param_index[nodeId]++]);

      // chimao EnterMutex e LeaveMutex solo se non sono in un callback
      if(!from_callback)
      {
        LeaveMutex();
      }

      machine_state_index[nodeId]++;

#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        if(machine_state_index[nodeId]
            > next_machine[nodeId][0]->function_size)
        {
          printf(
              "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
              InternalError, nodeId, machine_state_index[nodeId]);
        }
      }
#endif

      if(machine_state_index[nodeId]
          == next_machine[nodeId][0]->function_size)
        break;
    }
  }

  if(machine_state_index[nodeId] < next_machine[nodeId][0]->function_size)
  {
    if(next_machine[nodeId][0]->function[machine_state_index[nodeId]]
        == &writeNetworkDictCallBack)
    {
      writeNetworkDictCallBack_t machine_function =
          next_machine[nodeId][0]->function[machine_state_index[nodeId]];

      UNS8 machine_function_return = 0;

      UNS32 user_param[5];

      for(i = 0; i < 5; i++)
      {
        // se il parametro è pari a 0xFFFFFFFF allora significa che devo prenderlo da quelli
        // variabili
        if(next_machine[nodeId][0]->param[machine_state_param_index[nodeId]]
            == 0xFFFFFFFF)
        {
          if(next_var_count[nodeId] < 1)
          {
            if(var_count_total[nodeId] == 0)
              user_param[i] =
                  next_machine[nodeId][0]->param[machine_state_param_index[nodeId]];
            else
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf(
                    "ERR[%d on node %x state %d]: %s (-1 insieme a parametri utente)\n",
                    InternalError, nodeId, machine_state_index[nodeId],
                    next_machine[nodeId][0]->error[1]);
              }
#endif

              result_value = 1;
              goto finalize;
            }
          }
          else
          {
            next_var_count[nodeId]--;

            user_param[i] = *next_args[nodeId];
            next_args[nodeId]++;
          }
        }
        else
          user_param[i] =
              next_machine[nodeId][0]->param[machine_state_param_index[nodeId]];

        machine_state_param_index[nodeId]++;
      }

      if(nodeId != 0)
      {
        function_call_number[nodeId]++;

        // delle volte vengono ricevuti degli SDO timeout. Lo sleep serve ad evitare
        // il sovraccarico del bus dovuta ai molti messaggi. Non è un problema di banda,
        // ma del numero di messaggi continui di tipo SDO che un motore può sostenere
        usleep(2000);
        // chimao EnterMutex e LeaveMutex solo se non sono in un callback
        if(!from_callback)
          EnterMutex();

        machine_function_return = machine_function(d, nodeId, user_param[0],
            user_param[1], user_param[2], user_param[3], &user_param[4],
            _machine_callback, 0);

        // chiamo EnterMutex e LeaveMutex solo se non sono in un callback
        if(!from_callback)
          LeaveMutex();

        if(machine_function_return != 0xFF)
        {
          machine_state_index[nodeId]++;

#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            if(machine_state_index[nodeId]
                > next_machine[nodeId][0]->function_size)
            {
              printf(
                  "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
                  InternalError, nodeId, machine_state_index[nodeId]);
            }

            fflush(stdout);
          }
#endif

          return 0;
        }
        else
        {
#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf(
                "ERR[%d on node %x state %d]: %s (writeNetwork error)\n",
                InternalError, nodeId, machine_state_index[nodeId],
                next_machine[nodeId][0]->error[1]);
          }
#endif

          result_value = 1;
          goto finalize;
        }
      }
      else //if(nodeId != 0)
      {
        for(i = 1; i < CANOPEN_NODE_NUMBER; i++)
        {
#ifndef SDO_SYNC
          lock_value = pthread_mutex_lock(&machine_mux[i]);

          if((motor_active[i] > 0) && machine_run[i] == 0)
          {
            machine_run[i] = 1;
            lock_value = pthread_mutex_unlock(&machine_mux[i]);
#else
            if(motor_active[i] > 0)
            {
              lock_value = pthread_mutex_lock(&machine_mux);

              if(machine_run == 1)
              pthread_cond_wait(&machine_run_cond, &machine_mux);
              else
              machine_run = 1;

              printf("writenetworkdick machine %d\n", nodeId);

              lock_value = pthread_mutex_unlock(&machine_mux);
#endif
            machine_state_param_index[i] =
                machine_state_param_index[nodeId];
            function_call_number[i]++;

            if(!from_callback)
            {
              EnterMutex();
            }

            machine_function_return = machine_function(d, i,
                user_param[0],
                user_param[1],
                user_param[2],
                user_param[3],
                &user_param[4],
                _machine_callback,
                0
                );

            if(!from_callback)
            {
              LeaveMutex();
            }

            if(machine_function_return != 0xFF)
            {
              machine_state_index[i]++;

#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                if(machine_state_index[i]
                    > next_machine[i][0]->function_size)
                {
                  printf(
                      "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
                      InternalError, nodeId, machine_state_index[i]);
                }
              }
#endif
            }
            else
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf(
                    "ERR[%d on node %x state %d]: %s (writeNetwork error)\n",
                    InternalError, i, machine_state_index[i],
                    next_machine[i][0]->error[1]);
              }
#endif
            }
          }
#ifndef SDO_SYNC
          else
            lock_value = pthread_mutex_unlock(&machine_mux[i]);
#endif
        }

        result_value = 0;
        goto finalize;
      }
    }
    else if(next_machine[nodeId][0]->function[machine_state_index[nodeId]]
        == &readNetworkDictCallback)
    {
      readNetworkDictCallback_t machine_function =
          next_machine[nodeId][0]->function[machine_state_index[nodeId]];

      UNS8 machine_function_return = 0;

      UNS32 user_param[3];

      for(i = 0; i < 3; i++)
      {
        // se il parametro è pari a 0xFFFFFFFF allora significa che devo prenderlo da quelli
        // variabili
        if(next_machine[nodeId][0]->param[machine_state_param_index[nodeId]]
            == 0xFFFFFFFF)
        {
          if(next_var_count[nodeId] < 1)
          {
            if(var_count_total[nodeId] == 0)
              user_param[i] =
                  next_machine[nodeId][0]->param[machine_state_param_index[nodeId]];
            else
            {
#ifdef CANOPENSHELL_VERBOSE
              if(verbose_flag)
              {
                printf(
                    "ERR[%d on node %x state %d]: %s (-1 insieme a parametri utente)\n",
                    InternalError, nodeId, machine_state_index[nodeId],
                    next_machine[nodeId][0]->error[1]);
              }
#endif

              result_value = 1;
              goto finalize;
            }
          }
          else
          {
            next_var_count[nodeId]--;

            user_param[i] = *next_args[nodeId];
            next_args[nodeId]++;
          }
        }
        else
          user_param[i] =
              next_machine[nodeId][0]->param[machine_state_param_index[nodeId]];

        machine_state_param_index[nodeId]++;
      }

      if(nodeId != 0)
      {
        function_call_number[nodeId]++;

        // chimao EnterMutex e LeaveMutex solo se non sono in un callback
        if(!from_callback)
          EnterMutex();

        machine_function_return = machine_function(d, nodeId, user_param[0],
            user_param[1], user_param[2], _machine_callback, 0);

        // chiamo EnterMutex e LeaveMutex solo se non sono in un callback
        if(!from_callback)
          LeaveMutex();

        if(machine_function_return != 0xFF)
        {
          machine_state_index[nodeId]++;

#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            if(machine_state_index[nodeId]
                > next_machine[nodeId][0]->function_size)
            {
              printf(
                  "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
                  InternalError, nodeId, machine_state_index[nodeId]);
            }

            fflush(stdout);
          }
#endif

          return 0;
        }
        else
        {
          result_value = 1;
          goto finalize;
        }
      }
      else
      {
        for(i = 1; i < CANOPEN_NODE_NUMBER; i++)
        {
#ifndef SDO_SYNC
          lock_value = pthread_mutex_lock(&machine_mux[i]);

          if((motor_active[i] > 0) && machine_run[i] == 0)
          {
            machine_run[i] = 1;

            lock_value = pthread_mutex_unlock(&machine_mux[i]);
#else
            if(motor_active[i] > 0)
            {
              lock_value = pthread_mutex_lock(&machine_mux);

              if(machine_run == 1)
              pthread_cond_wait(&machine_run_cond, &machine_mux);
              else
              machine_run = 1;

              printf("writenetworkdick machine %d\n", nodeId);

              lock_value = pthread_mutex_unlock(&machine_mux);
#endif
            machine_state_param_index[i] =
                machine_state_param_index[nodeId];
            function_call_number[i]++;

            if(!from_callback)
              EnterMutex();

            machine_function_return = machine_function(d, i, user_param[0],
                user_param[1], user_param[2], _machine_callback, 0);

            if(!from_callback)
              LeaveMutex();

            if(machine_function_return != 0xFF)
              machine_state_index[i]++;
          }
#ifndef SDO_SYNC
          else
            lock_value = pthread_mutex_unlock(&machine_mux[i]);
#endif
        }

        result_value = 0;
        goto finalize;
      }
    }
    else
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("WARN[%d on node %x state %d]: Funzione utente inaspettata\n",
            InternalError, nodeId, machine_state_index[nodeId]);
      }
#endif

      CustomFunction_t machine_function =
          next_machine[nodeId][0]->function[machine_state_index[nodeId]];
      last_function =
          next_machine[nodeId][0]->function[machine_state_index[nodeId]];

      if(nodeId != 0)
      {
        machine_state_index[nodeId]++;

#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          if(machine_state_index[nodeId]
              > next_machine[nodeId][0]->function_size)
          {
            printf(
                "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
                InternalError, nodeId, machine_state_index[nodeId]);
          }
        }
#endif

        function_call_number[nodeId]++;

        machine_function(d, nodeId, next_machine[nodeId][0]->param,
            next_machine[nodeId][0]->param_size);

        fflush(stdout);

        return 0;
      }
      else
      {
        for(i = 1; i < CANOPEN_NODE_NUMBER; i++)
        {
          if(motor_active[i] > 0)
          {
            machine_state_index[i]++;
            function_call_number[i]++;

            machine_function(d, i, next_machine[i][0]->param,
                next_machine[i][0]->param_size);
          }
        }

        result_value = 0;
        goto finalize;
      }
    }
  }

// Nel caso in cui le funzioni siano finite
  if(machine_state_index[nodeId] == next_machine[nodeId][0]->function_size)
  {

#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
    {
      if(next_machine[nodeId][0]->error[0] != NULL)
        printf("SUCC[node %x]: %s\n", nodeId,
            next_machine[nodeId][0]->error[0]);
    }
#endif

    machine_state_param_index[nodeId] = 0;

    next_machine_size[nodeId]--;

    // se è accodata un'altra macchina, allora l'avvio
    if((next_machine[nodeId] != NULL) && (next_machine_size[nodeId] > 0))
    {
      machine_state_index[nodeId] = 0;

      for(i = 0; i < next_machine_size[nodeId]; i++)
        next_machine[nodeId][i] = next_machine[nodeId][i + 1];

      goto next_machine_entry;
    }
    else
    {
      result_value = 0;

      goto finalize;
    }
  }
#ifdef CANOPENSHELL_VERBOSE
  else
  {
    if(verbose_flag)
    {

      printf(
          "ERR[%d on node %x state %d]: Non dovresti essere arrivato qui...mmmh\n",
          InternalError, nodeId, machine_state_index[nodeId]);
      printf(
          "Vediamo...\n Il numero di funzioni da eseguire era: %d. A quale sei arrivato?\n",
          next_machine[nodeId][0]->function_size);
    }
  }

  fflush(stdout);
#endif

  return 0;

  finalize:
  // nel caso in cui la funzione broadcast chiudesse la macchina con una funzione che non
  // prevedesse un callback, allora devo deallocare tutte le variabili precedenti.
  if((machine_state_index[nodeId] == next_machine[nodeId][0]->function_size)
      && (nodeId == 0))
  {
    for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
    {
      if(motor_active[i] > 0)
      {
        next_machine_size[i] = 0;
        next_var_count[i] = 0;
        var_count_total[i] = 0;
        machine_state_index[i] = 0;
        machine_state_param_index[i] = 0;

        if(args_ptr[i] != NULL)
        {
          free(args_ptr[i]);
          args_ptr[i] = NULL;
          next_args[i] = NULL;
        }

        if(next_machine[i] != NULL)
        {
          free(next_machine[i]);
          next_machine[i] = NULL;
        }

#ifndef SDO_SYNC
        // se c'è stato un errore, allora devo resettare tutte le variabili
        // machine_run
        if(result_value)
        {
          lock_value = pthread_mutex_lock(&machine_mux[i]);
          machine_run[i] = 0;
          lock_value = pthread_mutex_unlock(&machine_mux[i]);
        }

#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          if(lock_value != 0)
            printf(
                "ERR[%d on node %x state %d]: Impossibile sbloccare il mutex 6: %d\n",
                InternalError,
                nodeId, machine_state_index[nodeId], lock_value);
        }
#endif
#endif
      }
    }

#ifdef SDO_SYNC
    lock_value = pthread_mutex_lock(&machine_mux);
    machine_run = 0;
    printf("end machine %d\n", nodeId);
    pthread_cond_signal(&machine_run_cond);
    lock_value = pthread_mutex_unlock(&machine_mux);
#endif

    // se non ci sono stati problemi, posso considerare chiusa la
    // richiesta broadcast
    if(result_value == 0)
      machine_run[0] = 0;
  }
  else
  {
    int machine_index = machine_state_index[nodeId];

    next_machine_size[nodeId] = 0;
    next_var_count[nodeId] = 0;
    var_count_total[nodeId] = 0;
    machine_state_index[nodeId] = 0;
    machine_state_param_index[nodeId] = 0;

    if(args_ptr[nodeId] != NULL)
    {
      free(args_ptr[nodeId]);
      args_ptr[nodeId] = NULL;
      next_args[nodeId] = NULL;
    }

    if(next_machine[nodeId] != NULL)
    {
      free(next_machine[nodeId]);
      next_machine[nodeId] = NULL;
    }

#ifndef SDO_SYNC
    lock_value = pthread_mutex_lock(&machine_mux[nodeId]);
    machine_run[nodeId] = 0;
    lock_value = pthread_mutex_unlock(&machine_mux[nodeId]);

#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
    {
      if(lock_value != 0)
        printf(
            "ERR[%d on node %x state %d]: Impossibile sbloccare il mutex 7: %d\n",
            InternalError,
            nodeId, machine_state_index[nodeId], lock_value);
    }
#endif
#else
    lock_value = pthread_mutex_lock(&machine_mux);
    machine_run = 0;
    printf("end machine %d\n", nodeId);
    pthread_cond_signal(&machine_run_cond);
    lock_value = pthread_mutex_unlock(&machine_mux);
#endif

    // Call Callback function
    if((callback_user[nodeId] != NULL) && (nodeId != 0))
    {
      if(last_function == &readNetworkDictCallback)
        callback_user[nodeId](d, nodeId, machine_index,
            readNetworkDictCallback_result);
      else
        callback_user[nodeId](d, nodeId, machine_index, result_value);
    }
  }

  fflush(stdout);

  return result_value;
}
