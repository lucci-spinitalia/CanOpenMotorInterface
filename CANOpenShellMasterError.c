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

/**
 * buffer must be 80 characte minimum
 */
void AbortCodeTranslate(UNS32 abortCode, char *buffer)
{
  switch(abortCode)
  {
    case 0x05030000:
      sprintf(buffer, "Toggle bit not altered");
      break;
    case 0x05040000:
      sprintf(buffer, "SDO protocol timed out");
      break;
    case 0x05040001:
      sprintf(buffer, "Command specified not valid");
      break;
    case 0x05040002:
      sprintf(buffer, "Invalid block size (block mode only, see DS301)");
      break;
    case 0x05040003:
      sprintf(buffer, "Invalid sequence number (block mode only, see DS301)");
      break;
    case 0x05040004:
      sprintf(buffer, "CRC error (block mode only, see DS301)");
      break;
    case 0x05040005:
      sprintf(buffer, "Out of memory");
      break;
    case 0x06010000:
      sprintf(buffer, "Unsupported access to an object");
      break;
    case 0x06010001:
      sprintf(buffer, "Attempt to read a write only object");
      break;
    case 0x06010002:
      sprintf(buffer, "Attempt to write a read only object");
      break;
    case 0x06020000:
      sprintf(buffer, "Object does not exist in object dictionary");
      break;
    case 0x06040041:
      sprintf(buffer, "Object cannot be mapped to the PDO");
      break;
    case 0x06040042:
      sprintf(buffer, "The number and length of the object to be mapped would exceed PDO length");
      break;
    case 0x06040043:
      sprintf(buffer, "General parameter incompatibility reason");
      break;
    case 0x06040047:
      sprintf(buffer, "General internal incompatibility in the device");
      break;
    case 0x06060000:
      sprintf(buffer, "Access failed due to a hardware error");
      break;
    case 0x06070010:
      sprintf(buffer, "Data type does not match, length of service parameter does not match");
      break;
    case 0x06070012:
      sprintf(buffer, "Data type does not match, length of service parameter too high");
      break;
    case 0x06070013:
      sprintf(buffer, "Data type does not match, length of service parameter too low");
      break;
    case 0x06090011:
      sprintf(buffer, "Sub-index does not exist");
      break;
    case 0x06090030:
      sprintf(buffer, "Value range of parameter exceeded (only for write access)");
      break;
    case 0x06090031:
      sprintf(buffer, "Value of parameter written too high");
      break;
    case 0x06090032:
      sprintf(buffer, "Value of parameter written too low");
      break;
    case 0x06090036:
      sprintf(buffer, "Maximum value is less than minimum value");
      break;
    case 0x08000000:
      sprintf(buffer, "General error");
      break;
    case 0x08000020:
      sprintf(buffer, "Data cannot be transferred or stored to the application");
      break;
    case 0x08000021:
      sprintf(buffer,
          "Data cannot be transferred or stored to the application because of local control");
      break;
    case 0x08000022:
      sprintf(buffer,
          "Data cannot be transferred or stored to the application because of present device state");
      break;
    case 0x08000023:
      sprintf(buffer,
          "Object dictionary dynamic generation fails or no object dictionary is present");
      break;

    default:
      buffer[0] = '\0';
      break;
  }
}
