/*
 * CANOpenShellMasterError.h
 *
 *  Created on: 12/mag/2015
 *      Author: luca
 */

#ifndef CANOPENSHELLMASTERERROR_H_
#define CANOPENSHELLMASTERERROR_H_

#include "canfestival.h"

#define CERR(cmd, num)            \
          printf("CERR %s %d: %s\n", cmd, num, cerr_descr[num])

#define OK(cmd)            \
          printf("OK %s\n", cmd)

//#define CANOPENSHELL_VERBOSE

/**
 * Enumeratore degli errori
 */
enum enum_nodeError
{
  CANOpenError  = 0x00, /**< Errore da un oggetto sdo o pdo */
  InternalError = 0x01,  /**< Errore nel codice */
  SmartMotorError = 0x02, /**< Errore smart motor */
};

enum enum_cerr
{
  CERR_NotFound  = 0x00, /**< Comando non riconosciuto */
  CERR_InternalError = 0x01, /**< Errore interno */
  CERR_ParamError = 0x02, /**< Errore nei parametri passati */
  CERR_ConfigError = 0x03 /**< Errore di configurazione */
};

extern char *cerr_descr[];

extern UNS32 canopen_abort_code;  /**< Codice dell'ultimo errore del tipo CANOpenError */

#endif /* CANOPENSHELLMASTERERROR_H_ */
