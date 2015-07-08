/*
 * CANOpenShellMasterError.c
 *
 *  Created on: 12/mag/2015
 *      Author: luca
 */

#include "CANOpenShellMasterError.h"

UNS32 canopen_abort_code = 0;  /**< Codice dell'ultimo errore del tipo CANOpenError */

char *cerr_descr[] = {
    "Comando non riconosciuto",
    "Errore intero",
    "Parametri errati",
    "Errore di configurazione"
};
