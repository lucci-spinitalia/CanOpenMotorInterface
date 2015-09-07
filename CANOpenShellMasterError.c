/*
 * CANOpenShellMasterError.c
 *
 *  Created on: 12/mag/2015
 *      Author: luca
 */
#include "CANOpenShellMasterError.h"
UNS32 canopen_abort_code = 0; /**< Codice dell'ultimo errore del tipo CANOpenError */
volatile int verbose_flag = 0;

char *cerr_descr[] =
{
    "Comando non riconosciuto",
    "Errore interno",
    "Parametri errati",
    "Errore di configurazione",
    "Servo bus voltage fault",
    "Peak over-current occurred",
    "Excessive temperature",
    "Excessive position error",
    "Errore nell'inseguimento della velocità",
    "First derivative (DE/Dt) of position error over limit",
    "Right(+) over travel limit",
    "Left(-) over travel limit",
    "Move error",
    "Quick stop active",
    "Position error tolerance exceeded (IP mode)",
    "Invalid time range (IP mode)",
    "Invalid position increment (IP mode)",
    "FIFO underflow (IP mode)",
    "FIFO overflow (IP mode)",
    "Operazione non permessa",
    "Simulazone interrotta",
    "Errore motore"
};
