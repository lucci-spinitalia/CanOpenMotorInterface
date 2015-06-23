#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "CANOpenShellStateMachine.h"
#include "CANOpenShellMasterError.h"

int motor_active[CANOPEN_NODE_NUMBER]; /**< indica se un motore si è dichiarato */
int motor_homing[CANOPEN_NODE_NUMBER]; /**< indica se un motore si è dichiarato */
long motor_position[CANOPEN_NODE_NUMBER]; /**< ultima posizione del motore */

long next_machine_size[CANOPEN_NODE_NUMBER];
struct state_machine_struct **next_machine[CANOPEN_NODE_NUMBER];

int next_var_count[CANOPEN_NODE_NUMBER];
int var_count_total[CANOPEN_NODE_NUMBER];
UNS32 *args_ptr[CANOPEN_NODE_NUMBER];
UNS32 *next_args[CANOPEN_NODE_NUMBER];

int machine_state_index[CANOPEN_NODE_NUMBER];
int machine_state_param_index[CANOPEN_NODE_NUMBER];
volatile int motor_started[CANOPEN_NODE_NUMBER];

MachineCallback_t callback_user[CANOPEN_NODE_NUMBER];

void **sin_interpolation_function;
UNS32 *sin_interpolation_param;

char *sin_interpolation_error[2] =
    {
        NULL,
        "Cannot write smartmotor table in interpolation mode"
    };

struct state_machine_struct sin_interpolation_machine =
    {NULL, 0, NULL, 0, sin_interpolation_error};

volatile int machine_run[CANOPEN_NODE_NUMBER];

void *smart_start[8] =
    {
        &masterSendNMTstateChange, // Start canopen node
        &writeNetworkDictCallBack, // Reset status word
        &writeNetworkDictCallBack, // Set mode velocity
        &writeNetworkDictCallBack, // Set velocity in pv mode
        &writeNetworkDictCallBack, // Set acceleration
        &writeNetworkDictCallBack, // Set deceleration
        &writeNetworkDictCallBack, // Change state: ready to switch on
        &writeNetworkDictCallBack // Change state: switched on
    };
UNS32 smart_start_param[36] =
    {
        NMT_Start_Node,  // Start canopen node
        0x6040, 0x0, 2, 0, 0x80,  // Reset status word
        0x6060, 0x0, 1, 0, 0x3,  // Set mode velocity
        0x60FF, 0x0, 4, 0, 0xC350,  // Set velocity in pv mode
        0x6083, 0x0, 4, 0, 0x64,  // Set acceleration
        0x6084, 0x0, 4, 0, 0x64,  // Set deceleration
        0x6040, 0x0, 2, 0, 0x6,  // Change state: ready to switch on
        0x6040, 0x0, 2, 0, 0x7  // Change state: switched on
    };

char *smart_start_error[2] =
    {
        "smartmotor started",
        "Cannot start smartmotor"
    };

struct state_machine_struct smart_start_machine =
    {smart_start, 8, smart_start_param, 36, smart_start_error};

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

void *smart_stop[2] =
    {
        &writeNetworkDictCallBack,  // Change state: switched off
        &writeNetworkDictCallBack, // Set mode velocity
    };
UNS32 smart_stop_param[10] =
    {
        0x6040, 0x0, 2, 0, 0x0b,  // Change state: switched off
        0x6060, 0x0, 1, 0, 0x3,  // Set mode velocity
    };

char *smart_stop_error[2] =
    {
        "smart motor stopped",
        "cannost stop smartmotor"
    };

struct state_machine_struct smart_stop_machine =
    {smart_stop, 2, smart_stop_param, 10, smart_stop_error};

void *smart_position_zero[10] =
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
        &writeNetworkDictCallBack // Begin motion to target position
    };
UNS32 smart_position_zero_param[50] =
    {
        0x6040, 0x0, 2, 0, 0x80, // Reset the status word
        0x6060, 0x0, 1, 0, 0x1, // Set mode position
        0x6081, 0x0, 4, 0, 0xC3550, // Set profile speed
        0x607A, 0x0, 4, 0, 0x0, // Set target position to zero
        0x6083, 0x0, 4, 0, 0x64, // Set acceleration
        0x6084, 0x0, 4, 0, 0x64, // Set deceleration
        0x6040, 0x0, 2, 0, 0x6, // Change state: ready to switch on
        0x6040, 0x0, 2, 0, 0x7, // Change state: switched on
        0x6040, 0x0, 2, 0, 0x2F, // Enable command, single setpoint (motion not actually started yet)
        0x6040, 0x0, 2, 0, 0x3F // Begin motion to target position
    };

char *smart_position_zero_error[2] =
    {
        "smart motor at origin",
        "cannot reach origin"
    };

struct state_machine_struct smart_position_zero_machine =
    {smart_position_zero, 10, smart_position_zero_param, 50,
        smart_position_zero_error};

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

void *init_interpolation_function[7] =
    {
        &writeNetworkDictCallBack, // Enable command (motion not actually started yet)
        &writeNetworkDictCallBack, // Clear buffer
        &writeNetworkDictCallBack, // Enable buffer
        &writeNetworkDictCallBack, // Set time period to seconds
        &writeNetworkDictCallBack, // Set time period to 1 (second)
        &writeNetworkDictCallBack, // Set interpolation mode
        &writeNetworkDictCallBack // Set first point to zero
    };
/*
 * Param
 *   current_position
 */UNS32 init_interpolation_param[35] =
    {
        0x6040, 0x0, 2, 0, 0xF, // Enable command (motion not actually started yet)
        0x60C4, 0x6, 1, 0, 0x0, // Clear buffer
        0x60C4, 0x6, 1, 0, 0x1, // Enable buffer
        0x60C2, 0x2, 1, 0, 0xFD, // Set time period to milliseconds
        0x60C2, 0x1, 1, 0, 0x1, // Set time period to 1 (second)
        0x6060, 0x0, 1, 0, 0x7, // Set interpolation mode
        0x60C1, 0x1, 4, 0, 0x00000000 // Set first point to current position
    };

char *init_interpolation_error[2] =
    {
        "smartmotor interpolation mode init",
        "Cannot init smartmotor in interpolation mode"
    };

struct state_machine_struct init_interpolation_machine =
    {
        init_interpolation_function, 7, init_interpolation_param, 35,
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

void *stop_interpolation_function[3] =
    {
        &writeNetworkDictCallBack, // Write zero-length segment
        &writeNetworkDictCallBack, // Repeat final data
        &writeNetworkDictCallBack // leave drive on but holding at the ending position
    };
UNS32 stop_interpolation_param[15] =
    {
        0x60C2, 0x1, 1, 0, 0x0, // Write zero-length segment
        0x60C1, 0x1, 4, 0, 0xFFFFFFFF, // Repeat final data
        0x6040, 0x0, 2, 0, 0x0F, // leave drive on but holding at the ending position
    };

char *stop_interpolation_error[2] =
    {
        "smartmotor interpolation mode end",
        "Cannot stop smartmotor in interpolation mode"
    };

struct state_machine_struct stop_interpolation_machine =
    {stop_interpolation_function, 3, stop_interpolation_param, 15,
        stop_interpolation_error};

void *resume_interpolation_function[3] =
    {
        &writeNetworkDictCallBack, // Enable command (motion not actually started yet)
        &writeNetworkDictCallBack,
        &writeNetworkDictCallBack
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

void *smart_homing_function[14] =
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
        &writeNetworkDictCallBack, // Enable command (motion not actually started yet)
        &writeNetworkDictCallBack, // Begin motion
    };
/*
 * param:
 *   homing offset
 */UNS32 smart_homing_param[70] =
    {
        0x6040, 0x0, 2, 0, 0x0, // Reset status word
        0x6040, 0x0, 2, 0, 0x80, // Reset status word
        0x6040, 0x0, 2, 0, 0x0, // Reset status word
        0x2309, 0x0, 2, 0, -4, // Enable positive limit switch
        0x2309, 0x0, 2, 0, -5, // Enable negative limit switch
        0x6060, 0x0, 1, 0, 0x6, // Set mode of operation to homing
        0x6098, 0x0, 1, 0, 0x1, // Set homing method
        0x6099, 0x1, 4, 0, 10000, // Set homing speed during search for switch
        0x6099, 0x2, 4, 0, 10000, // Set homing speed during search for zero
        0x609a, 0x0, 4, 0, 100, // Set homing acceleration
        0x607c, 0x0, 4, 0, 0xFFFFFFFF, // Set homing offset
        0x6040, 0x0, 2, 0, 0x6, // This is required to satisfy cia 402 drive state machine
        0x6040, 0x0, 2, 0, 0xF, // Enable command (motion not actually started yet)
        0x6040, 0x0, 2, 0, 0x1F, // Begin motion

    };

char *smart_homing_error[2] =
    {
        "smartmotor homing. . .",
        "Cannot run smart motor in homing mode"
    };

struct state_machine_struct smart_homing_machine =
    {smart_homing_function, 14, smart_homing_param, 70,
        smart_homing_error};

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
        NULL,
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
    motor_homing[i] = 0;

    next_machine_size[i] = 0;
    next_var_count[i] = 0;
    var_count_total[i] = 0;
    machine_state_index[i] = 0;
    machine_state_param_index[i] = 0;

    pthread_mutex_init(&machine_mux[i], &Attr);
  }

  motor_active[0] = 1;
}

void _machine_destroy()
{
  int i = 0;

  for(i = 0; i < CANOPEN_NODE_NUMBER; i++)
  {
    pthread_mutex_destroy(&machine_mux[i]);
  }
}

void _machine_reset(CO_Data* d, UNS8 nodeId)
{
  int sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

  if((sdo_result != 0) && (sdo_result != 0xFF))
  {
    printf("ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
        InternalError, nodeId, machine_state_index[nodeId],
        next_machine[nodeId][0]->error[1], sdo_result);
  }

  motor_active[nodeId] = 0;
  motor_homing[nodeId] = 0;

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

  machine_run[nodeId] = 0;
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
  void *last_function;
  int result_value = 0;

  int lock_value;
  int i, j;

  static int function_call_number[CANOPEN_NODE_NUMBER];

  if(callback_num != 0) // se la funzione non è stata richiamata dal callback
  {
    lock_value = pthread_mutex_lock(&machine_mux[nodeId]);

    if(machine_run[nodeId] == 1)
    {
      printf(
          "ERR[%d on node %x state %d]: %s (Operazione in corso)\n",
          InternalError, nodeId, machine_state_index[nodeId],
          next_machine[nodeId][0]->error[1]);

      lock_value = pthread_mutex_unlock(&machine_mux[nodeId]);

      return 1;
    }

    if(machine_state_index[nodeId] > 0)
    {
      printf(
          "WARN[%d on node %x state %d]: Qualcuno è entrato senza bussare...\n",
          InternalError, nodeId, machine_state_index[nodeId]);
    }

    // se non è una richiesta broadcast
    if(nodeId != 0)
    {
      machine_run[nodeId] = 1;
      callback_user[nodeId] = machine_callback;
    }
    else
    {
      for(i = 1; i < CANOPEN_NODE_NUMBER; i++)
      {
        if(motor_active[i] > 0)
          callback_user[i] = machine_callback;
      }
    }

    lock_value = pthread_mutex_unlock(&machine_mux[nodeId]);

    // Memorizzo il vettore con le prossime funzioni da eseguire
    if(callback_num == 0)
    {
      printf("ERR[%d on node %x state %d]: Nessuna funzione\n", InternalError,
          nodeId, machine_state_index[nodeId]);
      fflush(stdout);
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
            next_machine_size[nodeId] * sizeof(struct state_machine_struct *));

        if(next_machine[nodeId] == NULL)
        {
          printf(
              "ERR[%d on node %x state %d]: %s (memoria finita per allocare next_machine[%d])\n",
              InternalError, nodeId, machine_state_index[nodeId],
              next_machine[nodeId][0]->error[1], nodeId);

          result_value = 1;
          goto finalize;
        }
      }
      else
      {
        free(next_machine[nodeId]);
        next_machine[nodeId] = NULL;
        next_machine[nodeId] = malloc(
            next_machine_size[nodeId] * sizeof(struct state_machine_struct *));

        if(next_machine[nodeId] == NULL)
        {
          printf(
              "ERR[%d on node %x state %d]: %s (memoria finita per allocare next_machine[%d])\n",
              InternalError, nodeId, machine_state_index[nodeId],
              next_machine[nodeId][0]->error[1], nodeId);

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
              printf(
                  "ERR[%d on node %x state %d]: %s (memoria finita per allocare next_machine[%d])\n",
                  InternalError, i, machine_state_index[i],
                  next_machine[i][0]->error[1], i);

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
              printf(
                  "ERR[%d on node %x state %d]: %s (memoria finita per allocare next_machine[%d])\n",
                  InternalError, i, machine_state_index[i],
                  next_machine[i][0]->error[1], i);

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
            printf(
                "ERR[%d on node %x state %d]: %s (memoria finita per allocare args_ptr[%d])\n",
                InternalError, nodeId, machine_state_index[nodeId],
                next_machine[nodeId][0]->error[1], nodeId);

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
                printf(
                    "ERR[%d on node %x state %d]: %s (memoria finita per allocare args_ptr[%d])\n",
                    InternalError, i, machine_state_index[i],
                    next_machine[i][0]->error[1], i);

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
      printf("ERR[%d on node %x state %d]: Errore al ritorno dal callback\n",
          InternalError,
          nodeId, machine_state_index[nodeId]);

      return 1;
    }
    else
    {
      if(machine_state_index[nodeId]
          > next_machine[nodeId][0]->function_size)
      {
        printf(
            "WARN[%d on node %x state %d]: Qualcuno si è intrufolato dalla porta posteriore...\n",
            InternalError, nodeId, machine_state_index[nodeId]);

        printf("Il numero di chiamate a questa funzione è: %d\n",
            function_call_number[nodeId]);
      }

      if(function_call_number[nodeId] < 1)
        printf(
            "WARN[%d on node %x state %d]: Sei andato un po' troppo veloce...\n",
            InternalError, nodeId, machine_state_index[nodeId]);

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
          printf(
              "ERR[%d on node %x state %d]: %s (Aborted internal code: %x)\n",
              InternalError, nodeId, machine_state_index[nodeId],
              next_machine[nodeId][0]->error[1], d->transfers[line].abortCode);

          sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);
          if((sdo_result != 0) && (sdo_result != 0xFF))
          {
            printf("ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                InternalError, nodeId, machine_state_index[nodeId],
                next_machine[nodeId][0]->error[1], sdo_result);
          }

          result_value = 1;
          goto finalize;
          break;

        default:
          if(next_machine[nodeId][0]->function[machine_state_index[nodeId] - 1]
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
                  printf("Download in progress\n");
                ;
                break;

              case SDO_FINISHED:
                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);
                if((sdo_result != 0) && (sdo_result != 0xFF))
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }

                if(canopen_abort_code > 0)
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (Canopen abort code %x)\n",
                      CANOpenError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], canopen_abort_code);

                  result_value = 1;
                  goto finalize;
                }
                break;

              case SDO_ABORTED_INTERNAL:
                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);
                if((sdo_result != 0) && (sdo_result != 0xFF))
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }
                break;

              case SDO_ABORTED_RCV:
                printf(
                    "ERR[%d on node %x state %d]: %s (SDO getWriteResult error %d)\n",
                    InternalError, nodeId, machine_state_index[nodeId],
                    next_machine[nodeId][0]->error[1], sdo_result);

                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);
                if((sdo_result != 0) && (sdo_result != 0xFF))
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }

                machine_state_index[nodeId]--; // provo a rieseguire la stessa funzione
                machine_state_param_index[nodeId] -= 5;
                break;

              case SDO_RESET:
                printf(
                    "ERR[%d on node %x state %d]: %s (SDO getWriteResult error %d)\n",
                    InternalError, nodeId, machine_state_index[nodeId],
                    next_machine[nodeId][0]->error[1], sdo_result);

                printf("Il numero di chiamate a questa funzione è: %d\n",
                    function_call_number[nodeId]);

                return 1;
                break;

              default:
                printf(
                    "ERR[%d on node %x state %d]: %s (SDO getWriteResult error %d)\n",
                    InternalError, nodeId, machine_state_index[nodeId],
                    next_machine[nodeId][0]->error[1], sdo_result);

                printf("Il numero 2 di chiamate a questa funzione è: %d\n",
                    function_call_number[nodeId]);

                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);
                if((sdo_result != 0) && (sdo_result != 0xFF))
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }

                result_value = 1;
                goto finalize;
                break;
            }
          }
          else if(next_machine[nodeId][0]->function[machine_state_index[nodeId]
              - 1]
              == &readNetworkDictCallback)
          {
            function_call_number[nodeId]--;
            last_function = &readNetworkDictCallback;

            UNS32 data = 0;
            UNS32 size = 64;

            sdo_result = getReadResultNetworkDict(d, nodeId, &data, &size,
                &canopen_abort_code);

            switch(sdo_result)
            {
              case SDO_DOWNLOAD_IN_PROGRESS:
                case SDO_UPLOAD_IN_PROGRESS:
                while((sdo_result = getReadResultNetworkDict(d, nodeId, &data,
                    &size,
                    &canopen_abort_code)) != SDO_DOWNLOAD_IN_PROGRESS)
                  printf("Download in progress\n");
                ;
                break;

              case SDO_FINISHED:
                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);

                if((sdo_result != 0) && (sdo_result != 0xFF))
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }

                if(canopen_abort_code > 0)
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (Canopen abort code %x)\n",
                      CANOpenError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], canopen_abort_code);

                  result_value = 1;
                  goto finalize;
                }

                if(callback_user[nodeId] != NULL)
                {
                  callback_user[nodeId](d, nodeId, machine_state_index[nodeId],
                      data);
                }
                else
                {
                  printf(
                      "WARN[%d on node %x state %d]: Lettura senza funzione di callback \n",
                      InternalError, nodeId, machine_state_index[nodeId]);
                }

                break;

              case SDO_ABORTED_RCV:
                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);
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

                machine_state_index[nodeId]--; // provo a rieseguire la stessa funzione
                machine_state_param_index[nodeId] -= 5;
                break;

              default:
                printf(
                    "ERR[%d on node %x state %d]: %s (SDO getReadResult error %d)\n",
                    InternalError, nodeId, machine_state_index[nodeId],
                    next_machine[nodeId][0]->error[1], sdo_result);

                sdo_result = closeSDOtransfer(d, nodeId, SDO_CLIENT);
                if((sdo_result != 0) && (sdo_result != 0xFF))
                {
                  printf(
                      "ERR[%d on node %x state %d]: %s (SDO close error %d)\n",
                      InternalError, nodeId, machine_state_index[nodeId],
                      next_machine[nodeId][0]->error[1], sdo_result);
                }

                result_value = 1;
                goto finalize;
                break;
            }
          }
          break;
      }
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

      if(machine_state_index[nodeId]
          > next_machine[nodeId][0]->function_size)
      {
        printf(
            "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
            InternalError, nodeId, machine_state_index[nodeId]);
      }

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
              printf(
                  "ERR[%d on node %x state %d]: %s (-1 insieme a parametri utente)\n",
                  InternalError, nodeId, machine_state_index[nodeId],
                  next_machine[nodeId][0]->error[1]);

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
        {
          //if(nodeId == 0x77)
          //  printf("EnterMutex\n");
          EnterMutex();
        }

        machine_function_return = machine_function(d, nodeId, user_param[0],
            user_param[1], user_param[2], user_param[3], &user_param[4],
            _machine_callback, 0);

        // chiamo EnterMutex e LeaveMutex solo se non sono in un callback
        if(!from_callback)
        {
          //if(nodeId == 0x77)
          // printf("LeaveMutex\n");
          LeaveMutex();
        }

        if(machine_function_return != 0xFF)
        {
          machine_state_index[nodeId]++;

          if(machine_state_index[nodeId]
              > next_machine[nodeId][0]->function_size)
          {
            printf(
                "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
                InternalError, nodeId, machine_state_index[nodeId]);
          }

          return 0;
        }
        else
        {
          printf(
              "ERR[%d on node %x state %d]: %s (writeNetwork error)\n",
              InternalError, nodeId, machine_state_index[nodeId],
              next_machine[nodeId][0]->error[1]);

          result_value = 1;
          goto finalize;
        }
      }
      else //if(nodeId != 0)
      {
        for(i = 1; i < CANOPEN_NODE_NUMBER; i++)
        {
          lock_value = pthread_mutex_lock(&machine_mux[i]);

          if((motor_active[i] > 0) && machine_run[i] == 0)
          {
            machine_state_param_index[i] = machine_state_param_index[nodeId];
            machine_run[i] = 1;
            function_call_number[i]++;
            lock_value = pthread_mutex_unlock(&machine_mux[i]);

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

              if(machine_state_index[i]
                  > next_machine[i][0]->function_size)
              {
                printf(
                    "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
                    InternalError, nodeId, machine_state_index[i]);
              }
            }
          }
          else
            lock_value = pthread_mutex_unlock(&machine_mux[i]);
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
              printf(
                  "ERR[%d on node %x state %d]: %s (-1 insieme a parametri utente)\n",
                  InternalError, nodeId, machine_state_index[nodeId],
                  next_machine[nodeId][0]->error[1]);

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

          if(machine_state_index[nodeId]
              > next_machine[nodeId][0]->function_size)
          {
            printf(
                "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
                InternalError, nodeId, machine_state_index[nodeId]);
          }

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
          if(motor_active[i] > 0)
          {
            machine_state_param_index[i] = machine_state_param_index[nodeId];
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
        }

        result_value = 0;
        goto finalize;
      }
    }
    else
    {
      printf("WARN[%d on node %x state %d]: Funzione utente inaspettata\n",
          InternalError, nodeId, machine_state_index[nodeId]);

      CustomFunction_t machine_function =
          next_machine[nodeId][0]->function[machine_state_index[nodeId]];
      last_function =
          next_machine[nodeId][0]->function[machine_state_index[nodeId]];

      if(nodeId != 0)
      {
        machine_state_index[nodeId]++;

        if(machine_state_index[nodeId]
            > next_machine[nodeId][0]->function_size)
        {
          printf(
              "WARN[%d on node %x state %d]: Qualcuno uscito senza salutare...\n",
              InternalError, nodeId, machine_state_index[nodeId]);
        }
        function_call_number[nodeId]++;

        machine_function(d, nodeId, next_machine[nodeId][0]->param,
            next_machine[nodeId][0]->param_size);

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
    if(next_machine[nodeId][0]->error[0] != NULL)
      printf("SUCC[node %x]: %s\n", nodeId,
          next_machine[nodeId][0]->error[0]);

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
  else
  {
    printf(
        "ERR[%d on node %x state %d]: Non dovresti essere arrivato qui...mmmh\n",
        InternalError, nodeId, machine_state_index[nodeId]);
    printf(
        "Vediamo...\n Il numero di funzioni da eseguire era: %d. A quale sei arrivato?\n",
        next_machine[nodeId][0]->function_size);
  }

  return 0;

  finalize:
  // nel caso in cui la funzione broadcast chiudesse la macchina con una funzione che non
  // prevede un callback, allora devo deallocare tutte le variabili precedenti.
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

        lock_value = pthread_mutex_lock(&machine_mux[i]);
        machine_run[i] = 0;
        lock_value = pthread_mutex_unlock(&machine_mux[i]);

        //if(nodeId == 0x77)
        //  printf("Mutex exit 4\n");

        if(lock_value != 0)
          printf(
              "ERR[%d on node %x state %d]: Impossibile sbloccare il mutex 6: %d\n",
              InternalError,
              nodeId, machine_state_index[nodeId], lock_value);

      }
    }

    // Call Callback function
    if(callback_user[nodeId] != NULL)
      callback_user[nodeId](d, nodeId, machine_state_index[nodeId],
          result_value);
  }
  else
  {
    // Call Callback function
    if((callback_user[nodeId] != NULL) && (nodeId != 0)
        && (last_function != &readNetworkDictCallback))
      callback_user[nodeId](d, nodeId, machine_state_index[nodeId],
          result_value);

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

    lock_value = pthread_mutex_lock(&machine_mux[nodeId]);
    machine_run[nodeId] = 0;
    lock_value = pthread_mutex_unlock(&machine_mux[nodeId]);

    //if(nodeId == 0x77)
    //  printf("Mutex exit 5\n");

    if(lock_value != 0)
      printf(
          "ERR[%d on node %x state %d]: Impossibile sbloccare il mutex 7: %d\n",
          InternalError,
          nodeId, machine_state_index[nodeId], lock_value);
  }

  return result_value;
}
