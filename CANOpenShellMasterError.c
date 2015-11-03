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
        "Errore motore",
        "Formato del file non corretto"
    };

void add_event(int error, int nodeid, int type, char *message)
{
  pthread_mutex_lock(&event_buffer.error_mux);
  if(event_buffer.count < sizeof(event_buffer.event))
  {
    event_buffer.event[event_buffer.count] = error;
    event_buffer.nodeid[event_buffer.count] = nodeid;
    event_buffer.type[event_buffer.count] = type;

    if(message != NULL)
      strcpy(event_buffer.message[event_buffer.count], message);

    event_buffer.count++;
  }

  pthread_mutex_unlock(&event_buffer.error_mux);
}

void return_event()
{
  int error_count;

  pthread_mutex_lock(&event_buffer.error_mux);
  for(error_count = 0; error_count < event_buffer.count; error_count++)
  {
    switch(event_buffer.type[error_count])
    {
      case 0: /* errore aisncrono */
        AERR(event_buffer.event[error_count], event_buffer.nodeid[error_count],
            event_buffer.message[error_count]);
        break;

      case 1: /* evento */
        EVENT(event_buffer.event[error_count],
            event_buffer.nodeid[error_count],
            event_buffer.message[error_count]);
        break;
    }
  }

  event_buffer.count = 0;
  pthread_mutex_unlock(&event_buffer.error_mux);
}
