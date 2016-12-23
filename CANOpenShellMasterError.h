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

#define AERR(num, nodeid, msg)            \
          printf("AERR %d @M%d: %s %s\n", num, nodeid, cerr_descr[num], msg)

#define EVENT(num, nodeid, msg)            \
          printf("EVENT %d @M%d: %s %s\n", num, nodeid, cerr_descr[num], msg)

#define OK(cmd)            \
          printf("OK %s\n", cmd)

#define CANOPENSHELL_VERBOSE

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
  CERR_ConfigError = 0x03, /**< Errore di configurazione */
  CERR_BusVoltageFault = 0x04, /**< Tensione motore troppo bassa */
  CERR_OverCurrentFault = 0x05, /**< Picco di corrente eccessivo */
  CERR_TemperatureFault = 0x06,  /**< Raggiunta temperatura limite */
  CERR_PositionFault = 0x07, /**< Errore di inseguimento posizione */
  CERR_VelocityFault = 0x08, /**< Errore in velocità */
  CERR_DerivativeFault = 0x09, /**< Errore nella derivata prima dell'errore in posizione */
  CERR_RightLimitFault = 0x0a, /**< Raggiunto limite destro */
  CERR_LeftLimitFault = 0x0b,  /**< Raggiunto limite destro */
  CERR_MoveError = 0x0c,  /**< Errore generico di movimento */
  CERR_QuickStop = 0x0d, /**< Quick stop */
  CERR_InterpPositionError = 0x0e, /**< Errore d'inseguimento di posizione nella modalità interpolazione */
  CERR_InterpInvalidTimeError = 0x0f, /**< Intervallo temporale impostato non valido */
  CERR_InterpInvalidPositionError = 0x10, /**< Posizione impostata non valida */
  CERR_InterpFIFOUnderError = 0x11, /**< FIFO underflow */
  CERR_InterpFIFOOverError = 0x12,  /**< FIFO overflow */
  CERR_PermissionDenied = 0x13, /**< Inviato comando dallo stato sbalgiato */
  CERR_SimulationError = 0x14, /**< Errore nella simulazione */
  CERR_MotorFault = 0x15, /**< Canopen entrato nello stato "Fault" */
  CERR_FileError = 0x16 /**< Errore nella linea del file .mot */
};

struct
{
  int count;
  int event[100];
  int type[100]; /* 0: error async 1: event */
  int nodeid[100];
  char message[100][100];

  pthread_mutex_t error_mux;
} event_buffer;


extern char *cerr_descr[];
extern volatile int verbose_flag;

extern UNS32 canopen_abort_code;  /**< Codice dell'ultimo errore del tipo CANOpenError */

void add_event(int error, int nodeid, int type, char *message);
void return_event();
void AbortCodeTranslate(UNS32 abortCode, char *buffer);

#endif /* CANOPENSHELLMASTERERROR_H_ */
