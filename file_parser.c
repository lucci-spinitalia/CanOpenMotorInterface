/*
 * file_parser.c
 *
 *  Created on: 16/giu/2015
 *      Author: luca
 *
 * Per ogni motore viene genarato un file con i punti da raggiungere ed il tempo
 * in cui raggiungerli. Sarà cura del modulo di conversione rpy-punti effettuare
 * tutti i controlli per quanto riguarda sia le velocità massime raggiunte,
 * il rispetto dei tempi minimi e che la somma di tutti i tempi per ogni motore
 * sia uguale rispetto a tutti gli altri.
 * La directory di default dove trovare i file sarà ./table
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "CANOpenShellMasterError.h"
#include "file_parser.h"
#include "CANOpenShell.h"

long row_read[127];
long row_total[127];
float compleate_percent;

int QueuePutPositionPipe(struct table_data *data);

float FileCompleteGet(int nodeId, int point_in_table)
{
  if(row_total[nodeId] != 0)
  {
//    printf("nodeid %d row %ld row total %ld point %d\n", nodeId, row_read[nodeId], row_total[nodeId], point_in_table);
    return (((row_read[nodeId] - point_in_table) * 100) / row_total[nodeId]);
  }
  else
    return 0;
}

long FileLineCount(int nodeId)
{
  FILE *file = NULL;
  char *line = NULL;
  size_t len = 0;

  char file_path[256];
  long line_count = 0;

  if(fake_flag == 0)
    sprintf(file_path, "%s%d.mot", FILE_DIR, nodeId);
  else
    sprintf(file_path, "%s%d.mot.fake", FILE_DIR, nodeId);

  file = fopen(file_path, "r");

  if(file == NULL)
  {
#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
      perror("file");
#endif
    return -1;
  }

  // Ogni informazione è delimitata da uno spazio
  while(getline(&line, &len, file) != -1)
    line_count++;

  free(line);
  fclose(file);

  return line_count;
}

void *QueueRefiller(void *args)
{
  struct table_data *data = args;
  int data_refilled = 0;
  int err;

  if(data->is_pipe)
  {
    if(QueueOpenFile(data) < 0)
    {
      printf("QueueRefiller open error\n");
      data->end_reached = 1;
      return NULL;
    }

    printf("Refilled opened\n");
  }

  err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if(err != 0)
    printf("can't set thread as cancellable\n");

  err = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  if(err != 0)
    printf("can't set thread as cancellable deferred\n");

  if(data->is_pipe == 0)
    row_total[data->nodeId] = FileLineCount(data->nodeId);

  data->end_reached = 0;

  while(1)
  {
    pthread_testcancel();

    if(data->is_pipe == 0)
    {
      if(data->count < POSITION_DATA_NUM_MAX)
        data_refilled = QueuePut(data, POSITION_DATA_NUM_MAX - data->count);
      else
        goto go_sleep;
    }
    else
      data_refilled = QueuePutPositionPipe(data);

    //printf("[%d] Refilled %d points\n", data->nodeId, data_refilled);
    if(data_refilled == -1)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("ERR[%d on node %x]: Errore nel file.\n", InternalError, data->nodeId);
      }
#endif

      break;
    }
    else if((data_refilled == 0) && (data->end_reached))
    {
      break;
    }

    go_sleep: if(data->is_pipe == 0)
      usleep(10000);
  }

  data->table_refiller = 0;

  return NULL;
}

int QueueOpenFile(struct table_data *data)
{
  char file_path[256];
  if(data->position_file == NULL)
  {
    if(fake_flag == 0)
      sprintf(file_path, "%s%d.mot", FILE_DIR, data->nodeId);
    else
      sprintf(file_path, "%s%d.mot.fake", FILE_DIR, data->nodeId);

    data->position_file = fopen(file_path, "r");

    if(data->position_file == NULL)
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
        perror("file");
#endif
      return -1;
    }
  }

  return 0;
}

void QueueInit(int nodeid, struct table_data *data)
{
  void *res;

  // chiude eventuali processi in esecuzione
  if(data->table_refiller != 0)
  {
    if(pthread_cancel(data->table_refiller) == 0)
    {
      pthread_join(data->table_refiller, &res);

      if(res == PTHREAD_CANCELED)
        data->table_refiller = 0;
    }
  }

  if(data->position_file != NULL)
  {
    fclose(data->position_file);
    data->position_file = NULL;
  }

  data->nodeId = nodeid;
  data->write_pointer = 0;
  data->read_pointer = 0;
  data->count = 0;
  data->cursor_position = 0;
  data->end_reached = 0;
  data->is_pipe = 0;

  row_read[nodeid] = 0;

}

int QueueFill(struct table_data *data)
{
  row_read[data->nodeId] = 0;

  pthread_mutex_lock(&data->table_mutex);

  data->write_pointer = 0;
  data->read_pointer = 0;
  data->count = 0;
  data->cursor_position = 0;
  data->end_reached = 0;

  pthread_mutex_unlock(&data->table_mutex);

  if(data->is_pipe == 0)
  {
    if(QueueOpenFile(data) < 0)
    {
      data->end_reached = 1;
      return -1;
    }
  }

  if(data->table_refiller == 0)
  {
    int err;
    err = pthread_create(&data->table_refiller, NULL, &QueueRefiller, data);

    if(err != 0)
      printf("can't create thread:[%s]", strerror(err));

    //printf("[%d] Created process fill", data->nodeId);
  }

  return 0;
}

/**
 * Legge dalla coda e restituisce i valori nella struttura passata.
 *
 * @input data_in: struttura dati table_data
 * @input data_out: struttura dati table_data_read
 * @input offset: numero di posizioni da prelevare
 *
 * @return:  0 -> successo
 *          -1 -> sintassi della riga errata
 *          -2 -> non ci sono abbastanza dati per coprire l'offset
 *
 * @remark: Questa funzione si occupa anche di seguire il buffer circolare a seconda dell'offset
 * impostato. L'aggiornamento dei puntatori viene lasciato alla funzione QueueUpdate: in
 * questo modo è possibile leggere nuovamente i dati in caso di errore.
 */
int QueueGet(struct table_data *data_in, struct table_data_read *data_out, int offset)
{
  int read_pointer;

  //printf("[%d] Trying to get data. . .", data_in->nodeId);
  // Controllo che l'offset non sia più grande dei dati che ho scritto nel buffer
  if(offset > data_in->count)
  {
#ifdef CANOPENSHELL_VERBOSE

    if(verbose_flag)
    {
      printf("Superato il limite del file\n");
    }
#endif
    return -2;
  }

  if((data_in->read_pointer + offset) < POSITION_DATA_NUM_MAX)
    read_pointer = data_in->read_pointer + offset;
  else
    read_pointer = (data_in->read_pointer + offset) % POSITION_DATA_NUM_MAX;

  if(data_in->type != 'S')
  {
#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
    {
      printf("Errore nella riga di movimento: %d %c\n", data_in->nodeId, data_in->type);
    }
#endif

    return -1;
  }

  data_out->position = data_in->position[read_pointer];
  data_out->time_ms = data_in->time_ms[read_pointer];

  //printf("pointer: %d, position: %ld, time: %ld\n", read_pointer, data_out->position, data_out->time_ms);
  return 0;
}

/**
 * Legge dalla coda e restituisce i valori nella struttura passata.
 *
 * @input data_in: struttura dati table_data
 * @input data_out: struttura dati table_data_read
 * @input offset: numero di posizioni da prelevare
 *
 * @return:  0 -> successo
 *          -1 -> sintassi della riga errata
 *          -2 -> non ci sono abbastanza dati per coprire l'offset
 *
 * @remark: Questa funzione si occupa anche di seguire il buffer circolare a seconda dell'offset
 * impostato. A differenza di un buffer circolare classico, i dati letti sono sempre gli ultimi in
 * coda. Quindi, se si richiede la lettura di 10 posizioni, di partirà dalla posizione del puntatore
 * di scrittura meno 10 e si concluderà con l'ultimo dato.
 */
int QueueGetPipe(struct table_data *data_in, struct table_data_read *data_out, int offset)
{
  int read_pointer;

  // Controllo che l'offset non sia più grande dei dati che ho scritto nel buffer
  if(offset > data_in->count)
  {
#ifdef CANOPENSHELL_VERBOSE

    if(verbose_flag)
    {
      printf("Superato il limite del file\n");
    }
#endif
    return -2;
  }

  if((data_in->write_pointer - offset) < 0)
    read_pointer = POSITION_DATA_NUM_MAX - (offset - data_in->write_pointer);
  else
    read_pointer = (data_in->write_pointer - offset);

  data_out->position = data_in->position[read_pointer];
  data_out->time_ms = data_in->time_ms[read_pointer];

  //printf("pointer: %d, position: %ld, time: %ld\n", read_pointer, data_out->position, data_out->time_ms);
  return 0;
}

/**
 * Legge l'ultimo dato  nella coda.
 *
 * @attention: assicurarsi che la coda sia stata scritta precedentemente, altrimenti
 * verrà restituito un valore casuale.
 */
int QueueLast(struct table_data *data_in, struct table_data_read *data_out)
{
  int read_pointer;

  if((data_in->read_pointer) > 0)
    read_pointer = data_in->read_pointer - 1;
  else
    read_pointer = POSITION_DATA_NUM_MAX - 1;

  data_out->position = data_in->position[read_pointer];
  data_out->time_ms = data_in->time_ms[read_pointer];

  return 0;
}

/**
 * Aggiorna le variabili del buffer circolare.
 *
 * I valori del buffer vengono presi direttamente dalla struttura ed questa funzione
 * dovrebbe essere richiamata solo quando si è certi che i dati siano arrivati a
 * destinazione.
 *
 */
void QueueUpdate(struct table_data *data, int point_number)
{
  //printf("[%d] Queue update\n", data->nodeId);
  if(point_number <= data->count)
  {
    data->read_pointer += point_number;

    if(data->read_pointer >= POSITION_DATA_NUM_MAX)
      data->read_pointer = data->read_pointer % POSITION_DATA_NUM_MAX;

    data->count -= point_number;
  }
  else
  {
    if(data->end_reached == 0)
      printf("ERR[%d on node %x]: punti incongruenti con il buffer (QueueUpdate).\n", InternalError,
          data->nodeId);
  }
}

/**
 * Sposta il puntatore di lettura a point_number punti prima del puntatore di scrittura.
 */
int QueueSeek(struct table_data *data, int point_number)
{
  if(point_number > data->count)
    return -1;

  if((data->write_pointer - point_number) < 0)
    data->read_pointer = POSITION_DATA_NUM_MAX - (point_number - data->write_pointer);
  else
    data->read_pointer = data->write_pointer - point_number;

  return 0;
}

/**
 * Aggiunge alla coda i valori letti dal file di simulazione.
 *
 * @return >= 0: letta una nuova riga; -1: errore file -2: buffer pieno
 *
 * @remark: nella funzione è integrato un controllo sulla sintassi della riga letta.
 * Nel caso in cui quest'ultima non rispetti le regole prefisse, viene automaticamente
 * scartata e si passa alla prossima, fino al raggiungimento di numero di righe valide
 * passate come parametro oppure alla fine del file. Comunque, quando viene riscontrato
 * un errore, il parametro "type" viene sovrascritto con "E". Questo serve soprattutto
 * per le righe di homing, che sono sempre singole e che altrimenti non avrebbero nessun
 * parametro di errore da passare all'utilizzatore (le righe tabella hanno come indicatore
 * il numero di dati letti "count").
 * Ci sono i seguenti tipi di riga:
 *    "H": contiene i parametri per l'homing, in particolare l'offset dal limite di
 *         giunto e le velocità di andata e ritorno. La riga di homing deve essere
 *         la prima e l'unica del file, in questo questa operazione non può essere
 *         eseguita più volte con valori diversi.
 *
 *    "S": contiene i parametri da passare alla tabella dell'interpolatore.
 */
int QueuePut(struct table_data *data, int line_number)
{
  //FILE *file = NULL;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char *token;
  char *token_save;
  char line_copy[256];
  //char file_path[256];
  char position[12];
  char time[12];
  char vel_forw[12];
  char vel_backw[12];
  char nodeid[3];
  int line_count = 0;
  long value;

  if(data->end_reached == 1)
    return 0;

  if(data->count == POSITION_DATA_NUM_MAX)
    return -2;

  /*if(fake_flag == 0)
   sprintf(file_path, "%s%d.mot", FILE_DIR, data->nodeId);
   else
   sprintf(file_path, "%s%d.mot.fake", FILE_DIR, data->nodeId);

   file = fopen(file_path, "r");*/

  if(data->position_file == NULL)
  {
#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
      perror("file");
#endif

    data->end_reached = 1;
    return -1;
  }

  //if(fseek(file, data->cursor_position, SEEK_SET) != 0)
  if(fseek(data->position_file, data->cursor_position, SEEK_SET) != 0)
  {
    //fclose(file);
    return -1;
  }

  for(line_count = 0; line_count < line_number; line_count++)
  {
    // Ogni informazione è delimitata da uno spazio
    //if((read = getline(&line, &len, file)) != -1)
    if((read = getline(&line, &len, data->position_file)) != -1)
    {
      if(strcmp(line, "\n") == 0)
        continue;

      // controllo se la riga entra nel buffer
      if(len < sizeof(line_copy))
        strcpy(line_copy, line);
      else
        strncpy(line_copy, line, sizeof(line_copy));

      // Verifico la sintassi
      if(strncmp(line_copy, "CT1 M", strlen("CT1 M")))
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("WARN[%d on node %x]: Riga %ld non valida (sintassi CT1 M)\n", InternalError,
              data->nodeId, row_read[data->nodeId] + line_count);

          printf("line: %s, row read: %ld, line count: %d\n", line, row_read[data->nodeId],
              line_count);
          printf("cursor position on error: %ld\n", data->cursor_position);
        }
#endif
        data->type = 'E';

        data->cursor_position += read;

        continue;
      }

      // controllo che l'indirizzo del motore combaci
      token = strtok_r(line_copy + 5, " ", &token_save);
      if(token != NULL)
      {
        strcpy(nodeid, token);

        if(atoi(nodeid) != data->nodeId)
        {
#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf("WARN[%d on node %x]: Riga %ld non valida (nodeid)\n", InternalError,
                data->nodeId, row_read[data->nodeId] + line_count);

            printf("line: %s", line);
          }
#endif

          data->type = 'E';

          data->cursor_position += read;

          continue;
        }
      }
      else
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("WARN[%d on node %x]: Riga %ld non valida (sintassi nodeid)\n", InternalError,
              data->nodeId, row_read[data->nodeId] + line_count);

          printf("line: %s", line);
        }
#endif
        data->cursor_position += read;

        continue;
      }

      // determino il tipo di riga letta
      token = strtok_r(NULL, " ", &token_save);
      if(token != NULL)
      {
        data->type = *token;
        strcpy(position, token + 1);
      }
      else
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("WARN[%d on node %x]: Riga %ld non valida (sintassi tipo)\n", InternalError,
              data->nodeId, row_read[data->nodeId] + line_count);

          printf("line: %s", line);
        }
#endif

        data->type = 'E';

        data->cursor_position += read;
        continue;
      }

      // a seconda del tipo di dato, leggo i parametri successivi
      switch(data->type)
      {
        case 'S':
          // copio il tempo
          token = strtok_r(NULL, " \n\r\a", &token_save);
          if(token != NULL)
          {
            value = strtol((token + 1), NULL, 10);

            if((*token == 'T') && (*(token + 1) != '\0') && (value != ERANGE) && (value != 0))
              strcpy(time, token + 1);
            else
              goto fault;
          }
          else
            goto fault;

          long lposition = atol(position);
          long ltime = atol(time);

          memcpy(&data->position[data->write_pointer], &lposition,
              sizeof(data->position[data->write_pointer]));

          memcpy(&data->time_ms[data->write_pointer], &ltime,
              sizeof(data->time_ms[data->write_pointer]));

          break;

        case 'H':
          // se non è la prima riga letta, allora la devo ignoare
          if((row_read[data->nodeId] != 0) || line_count != 0)
            goto fault;

          // copio le velocità di andata e ritorno
          token = strtok_r(NULL, " ", &token_save);
          if(token != NULL)
          {
            if(strncmp(token, "VF", 2) == 0)
              strcpy(vel_forw, token + 2);
            else
              goto fault;
          }
          else
            goto fault;

          token = strtok_r(NULL, " \n\r\a", &token_save);
          if(token != NULL)
          {
            if(strncmp(token, "VB", 2) == 0)
              strcpy(vel_backw, token + 2);
            else
              goto fault;
          }
          else
            goto fault;

          data->offset = atol(position);
          data->forward_velocity = atol(vel_forw);
          data->backward_velocity = atol(vel_backw);

          //return 1;
          break;

        default:
          fault:

          data->type = 'E';

          data->cursor_position += read;

          sprintf(line_copy, "linea %ld", row_read[data->nodeId] + line_count + 1);
          add_event(CERR_FileError, data->nodeId, 0, line_copy);

          continue;
          break;
      }

      data->cursor_position += read;

      data->write_pointer++;

      if(data->write_pointer >= POSITION_DATA_NUM_MAX)
        data->write_pointer = 0;

      pthread_mutex_lock(&data->table_mutex);

      data->count++;

      if(data->count == POSITION_DATA_NUM_MAX)
      {
        pthread_mutex_unlock(&data->table_mutex);

        break;
      }
      else
        pthread_mutex_unlock(&data->table_mutex);
    }
    else
    {
      data->end_reached = 1;

      break;
    }
  }

  row_read[data->nodeId] += line_count + 1;

  free(line);
  //fclose(file);

  return line_count;
}

/**
 * Aggiunge alla coda i valori letti dal file di simulazione.
 *
 * @return >= 0: letta una nuova riga; -1: errore file -2: buffer pieno
 *
 * @remark: questa funzione, come QueuePut, legge dal file le posizioni. Visto
 * però che, in questo caso, si lavora con una pipe, la lettura viene fatta in
 * modo continuo.
 */
int QueuePutPipe(struct table_data *data, int line_number)
{
  //FILE *file = NULL;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char *token;
  char *token_save;
  char line_copy[256];
  //char file_path[256];
  char position[12];
  char time[12];
  char vel_forw[12];
  char vel_backw[12];
  char nodeid[3];
  int line_count = 0;
  long value;

  if(data->end_reached == 1)
    return 0;

  if(data->position_file == NULL)
  {
#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
      perror("file");
#endif
    return -1;
  }

  for(line_count = 0; line_count < line_number; line_count++)
  {
    // Ogni informazione è delimitata da uno spazio
    //if((read = getline(&line, &len, file)) != -1)
    if((read = getline(&line, &len, data->position_file)) != -1)
    {
      if(strcmp(line, "\n") == 0)
        continue;

      // controllo se la riga entra nel buffer
      if(len < sizeof(line_copy))
        strcpy(line_copy, line);
      else
        strncpy(line_copy, line, sizeof(line_copy));

      // Verifico la sintassi
      if(strncmp(line_copy, "CT1 M", strlen("CT1 M")))
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("WARN[%d on node %x]: Riga %ld non valida (sintassi CT1 M)\n", InternalError,
              data->nodeId, row_read[data->nodeId] + line_count);

          printf("line: %s", line);
        }
#endif
        data->type = 'E';

        continue;
      }

      // controllo che l'indirizzo del motore combaci
      token = strtok_r(line_copy + 5, " ", &token_save);
      if(token != NULL)
      {
        strcpy(nodeid, token);

        if(atoi(nodeid) != data->nodeId)
        {
#ifdef CANOPENSHELL_VERBOSE
          if(verbose_flag)
          {
            printf("WARN[%d on node %x]: Riga %ld non valida (nodeid)\n", InternalError,
                data->nodeId, row_read[data->nodeId] + line_count);

            printf("line: %s", line);
          }
#endif

          data->type = 'E';

          continue;
        }
      }
      else
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("WARN[%d on node %x]: Riga %ld non valida (sintassi nodeid)\n", InternalError,
              data->nodeId, row_read[data->nodeId] + line_count);

          printf("line: %s", line);
        }
#endif

        continue;
      }

      // determino il tipo di riga letta
      token = strtok_r(NULL, " ", &token_save);
      if(token != NULL)
      {
        data->type = *token;
        strcpy(position, token + 1);
      }
      else
      {
#ifdef CANOPENSHELL_VERBOSE
        if(verbose_flag)
        {
          printf("WARN[%d on node %x]: Riga %ld non valida (sintassi tipo)\n", InternalError,
              data->nodeId, row_read[data->nodeId] + line_count);

          printf("line: %s", line);
        }
#endif

        data->type = 'E';
        continue;
      }

      // a seconda del tipo di dato, leggo i parametri successivi
      switch(data->type)
      {
        case 'S':
          // copio il tempo
          token = strtok_r(NULL, " \n\r\a", &token_save);
          if(token != NULL)
          {
            value = strtol((token + 1), NULL, 10);

            if((*token == 'T') && (*(token + 1) != '\0') && (value != ERANGE) && (value != 0))
              strcpy(time, token + 1);
            else
              goto fault;
          }
          else
            goto fault;

          long lposition = atol(position);
          long ltime = atol(time);

          memcpy(&data->position[data->write_pointer], &lposition,
              sizeof(data->position[data->write_pointer]));

          memcpy(&data->time_ms[data->write_pointer], &ltime,
              sizeof(data->time_ms[data->write_pointer]));

          break;

        case 'H':
          // se non è la prima riga letta, allora la devo ignoare
          if((row_read[data->nodeId] != 0) || line_count != 0)
            goto fault;

          // copio le velocità di andata e ritorno
          token = strtok_r(NULL, " ", &token_save);
          if(token != NULL)
          {
            if(strncmp(token, "VF", 2) == 0)
              strcpy(vel_forw, token + 2);
            else
              goto fault;
          }
          else
            goto fault;

          token = strtok_r(NULL, " \n\r\a", &token_save);
          if(token != NULL)
          {
            if(strncmp(token, "VB", 2) == 0)
              strcpy(vel_backw, token + 2);
            else
              goto fault;
          }
          else
            goto fault;

          data->offset = atol(position);
          data->forward_velocity = atol(vel_forw);
          data->backward_velocity = atol(vel_backw);

          //return 1;
          break;

        default:
          fault:

          data->type = 'E';

          sprintf(line_copy, "linea %ld", row_read[data->nodeId] + line_count + 1);
          add_event(CERR_FileError, data->nodeId, 0, line_copy);

          continue;
          break;
      }

      // aggiorno il buffer circolare
      data->write_pointer++;

      if(data->write_pointer >= POSITION_DATA_NUM_MAX)
        data->write_pointer = 0;

      pthread_mutex_lock(&data->table_mutex);

      if(data->count < POSITION_DATA_NUM_MAX)
        data->count++;

      pthread_mutex_unlock(&data->table_mutex);
    }
    else
    {
      data->end_reached = 1;

      break;
    }
  }

  row_read[data->nodeId] += line_count + 1;

  free(line);
  //fclose(file);

  return line_count;
}

/**
 * Aggiunge alla coda i valori letti dal file di posizione.
 *
 * @return >= 0: letta una nuova riga; -1: errore file -2: buffer pieno
 *
 * @remark: questa funzione, come QueuePut, legge dal file le posizioni. Visto
 * però che, in questo caso, si lavora con una pipe, la lettura viene fatta in
 * modo continuo. Visto che non c'è un buffer circolare, la variabile write_pointer
 * è stata utilizzata per indicare quando è arrivato il comando di start, mentre
 * read_pointer indica quando è arrivata una stringa riguardante il motore.
 */
int QueuePutPositionPipe(struct table_data *data)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char line_copy[256];
  long lposition;
  long lvelocity;
  long lacceleration;
  int start_flag;
  int nodeid;
  int line_count = 0;

  if(data->end_reached == 1)
    return 0;

  if(data->position_file == NULL)
  {
#ifdef CANOPENSHELL_VERBOSE
    if(verbose_flag)
      perror("file");
#endif
    return -1;
  }

  int parse_num;
  // Ogni informazione è delimitata da uno spazio
  //if((read = getline(&line, &len, file)) != -1)
  if((read = getline(&line, &len, data->position_file)) != -1)
  {
    if(strcmp(line, "\n") == 0)
      return -1;

    // controllo se la riga entra nel buffer
    if(len < sizeof(line_copy))
      strcpy(line_copy, line);
    else
      strncpy(line_copy, line, sizeof(line_copy));

    parse_num = sscanf(line_copy, "CT1 M%d P%ld VM%ld AM%ld %d\n", &nodeid, &lposition, &lvelocity,
        &lacceleration, &start_flag);

    if(parse_num >= 4)
    {
      if(data->nodeId == 0)
      {
        int i;

        for(i = (TABLE_MAX_NUM + 1); i >= 0; i--)
        {
          if(data[i - TABLE_MAX_NUM + 1].nodeId == nodeid)
          {
            break;
          }
        }

        if(i == -1)
          goto fault;

        pthread_mutex_lock(&data->table_mutex);
        data[i - TABLE_MAX_NUM + 1].read_pointer = 1;
        data[i - TABLE_MAX_NUM + 1].position[0] = lposition;
        data[i - TABLE_MAX_NUM + 1].forward_velocity = lvelocity;
        pthread_mutex_unlock(&data->table_mutex);

        if((parse_num == 5) && (start_flag > 0))
        {
          data[i - TABLE_MAX_NUM + 1].write_pointer = 1;
        }
      }
      else
      {
        pthread_mutex_lock(&data->table_mutex);
        data->read_pointer = 1;
        data->position[0] = lposition;
        data->forward_velocity = lvelocity;
        pthread_mutex_unlock(&data->table_mutex);

        if((parse_num == 5) && (start_flag > 0))
        {
          data->write_pointer = 1;
        }
      }
    }
    else
    {
#ifdef CANOPENSHELL_VERBOSE
      if(verbose_flag)
      {
        printf("WARN[%d on node %x]: Riga %ld non valida (sintassi CT1 M)\n", InternalError,
            data->nodeId, row_read[data->nodeId] + line_count);

        printf("line: %s", line);
      }
#endif
      data->type = 'E';

      goto fault;
    }
  }
  else
  {
    data->end_reached = 1;
  }

  row_read[data->nodeId] += line_count + 1;

  free(line);
  //fclose(file);

  return line_count;

  fault:

  data->type = 'E';
  sprintf(line_copy, "linea %ld", row_read[data->nodeId] + line_count + 1);
  add_event(CERR_FileError, data->nodeId, 0, line_copy);

  return -1;
}
