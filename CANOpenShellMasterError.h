/*
 * CANOpenShellMasterError.h
 *
 *  Created on: 12/mag/2015
 *      Author: luca
 */

#ifndef CANOPENSHELLMASTERERROR_H_
#define CANOPENSHELLMASTERERROR_H_

#include "canfestival.h"

/**
 * Enumeratore degli errori
 */
enum enum_nodeError
{
  CANOpenError  = 0x00, /**< Errore da un oggetto sdo o pdo */
  InternalError = 0x01,  /**< Errore nel codice */
  SmartMotorError = 0x02 /**< Errore smart motor */
};

extern UNS32 canopen_abort_code;  /**< Codice dell'ultimo errore del tipo CANOpenError */

#endif /* CANOPENSHELLMASTERERROR_H_ */
