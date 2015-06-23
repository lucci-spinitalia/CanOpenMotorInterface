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
#include "CANOpenShellMasterError.h"
#include "file_parser.h"
#include "CANOpenShell.h"

static int row_read[127];

int QueuePut(struct table_data *data, int line_number);

void *QueueRefiller(void *args)
{
  struct table_data *data = args;
  int data_refilled = 0;

  while(1)
  {
    pthread_mutex_lock(&data->table_mutex);

    if(data->count < POSITION_DATA_NUM_MAX)
    {
      pthread_mutex_unlock(&data->table_mutex);

      data_refilled = QueuePut(data, POSITION_DATA_NUM_MAX - data->count);

      if(data_refilled == -1)
      {
        printf("ERR[%d on node %x]: Errore nel file.\n", InternalError,
            data->nodeId);

        break;
      }
      /*else if(data_refilled > 0)
       {
       printf("Refilled node %d: %d\n", data->nodeId, data_refilled);
       fflush(stdout);
       }*/
    }
    else
      pthread_mutex_unlock(&data->table_mutex);
  }

  return NULL;
}

void QueueInit(int nodeid, struct table_data *data)
{
  pthread_mutex_lock(&data->table_mutex);

  data->nodeId = nodeid;
  data->write_pointer = 0;
  data->read_pointer = 0;
  data->count = 0;
  data->cursor_position = 0;
  data->end_reached = 0;

  row_read[nodeid] = 0;

  if(data->table_refiller == 0)
  {
    int err;
    err = pthread_create(&data->table_refiller, NULL, &QueueRefiller, data);

    if(err != 0)
      printf("can't create thread:[%s]", strerror(err));
  }

  pthread_mutex_unlock(&data->table_mutex);
}

/**
 * Legge dalla coda e restituisce i valori nella struttura passata.
 *
 * Questa funzione si occupa anche di seguire il buffer circolare a seconda dell'offset
 * impostato. L'aggiornamento dei puntatori viene lasciato alla funzione QueueUpdate: in
 * questo modo è possibile leggere nuovamente i dati in caso di errore.
 */
int QueueGet(struct table_data *data_in, struct table_data_read *data_out,
    int offset)
{
  int read_pointer;

  // Controllo che l'offset non sia più grande dei dati che ho scritto nel buffer
  if(offset > data_in->count)
    return -1;

  if((data_in->read_pointer + offset) < POSITION_DATA_NUM_MAX)
    read_pointer = data_in->read_pointer + offset;
  else
    read_pointer = (data_in->read_pointer + offset) % POSITION_DATA_NUM_MAX;

  data_out->position = data_in->position[read_pointer];
  data_out->time_ms = data_in->time_ms[read_pointer];

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
  pthread_mutex_lock(&data->table_mutex);

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
      printf(
          "ERR[%d on node %x]: punti incongruenti con il buffer (QueueUpdate).\n",
          InternalError, data->nodeId);
  }

  pthread_mutex_unlock(&data->table_mutex);
}

/**
 * Ad ogni chiamata a questa funzione viene letta una nuova riga del file indicato
 * restituendo il comando letto e la realtiva descizione.
 *
 * @return >= 0: letta una nuova riga; -1: errore file -2: buffer pieno
 *
 * @remark: La funzione deve essere letta in modo ricorsivo per ottenere i comandi da
 * inviare ai motori. Tra il comando ed un eventuale commento, possono essere presenti
 * i seguenti caratteri: " ", "\t", "'". Il carattere "\n" rappresenta la fine della
 * riga.
 * Se non interessa prendere l'eventuale commento presente nel file, impostare la
 * variabile description a NULL. Se la dimensione della stringa description, indicata dal parametro
 * max_description_size, è troppo piccola per farci entrare la descrizione, quest'ultima verrà
 * troncata.
 */
int QueuePut(struct table_data *data, int line_number)
{
  FILE *file = NULL;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char *token;
  char line_copy[256];
  char position[12];
  char time[12];
  int line_count = 0;

  pthread_mutex_lock(&data->table_mutex);

  if(data->end_reached == 1)
  {
    pthread_mutex_unlock(&data->table_mutex);
    return 0;
  }

  if(data->count == POSITION_DATA_NUM_MAX)
  {
    pthread_mutex_unlock(&data->table_mutex);
    return -2;
  }

  char file_path[256];
  sprintf(file_path, "%s%d.mot", FILE_DIR, data->nodeId);

  file = fopen(file_path, "r");

  if(file == NULL)
  {
    perror("file");
    pthread_mutex_unlock(&data->table_mutex);

    return -1;
  }

  if(fseek(file, data->cursor_position, SEEK_SET) != 0)
  {
    fclose(file);

    pthread_mutex_unlock(&data->table_mutex);
    return -1;
  }

  for(line_count = 0; line_count < line_number; line_count++)
  {
    // Ogni informazione è delimitata da uno spazio
    if((read = getline(&line, &len, file)) != -1)
    {
      // controllo se la riga entra nel buffer
      if(len < sizeof(line_copy))
        strcpy(line_copy, line);
      else
        strncpy(line_copy, line, sizeof(line_copy) - 1);

      //printf("line: %s on %d\n", line_copy, data->nodeId);
      // copio la posizione
      token = strtok(line_copy, " ");
      if(token != NULL)
        strcpy(position, token);
      else
      {
        printf("WARN[%d on node %x]: Riga %d non valida.\n",
            InternalError, data->nodeId, row_read[data->nodeId] + line_count);

        continue;
      }

      // copio il tempo
      token = strtok(NULL, " \n\r\a");
      if(token != NULL)
        strcpy(time, token);
      else
      {
        printf("WARN[%d on node %x]: Riga %d non valida.\n",
            InternalError, data->nodeId, row_read[data->nodeId] + line_count);

        continue;
      }

      data->cursor_position += read;

      long lposition = atol(position);
      long ltime = atol(time);
      // aggiorno il buffer circolare
      memcpy(&data->position[data->write_pointer], &lposition,
          sizeof(&data->position[data->write_pointer]));

      memcpy(&data->time_ms[data->write_pointer], &ltime,
          sizeof(&data->time_ms[data->write_pointer]));

      data->write_pointer++;

      if(data->write_pointer >= POSITION_DATA_NUM_MAX)
        data->write_pointer = 0;

      data->count++;

      if(data->count == POSITION_DATA_NUM_MAX)
        break;
    }
    else
    {
      //data->cursor_position = 0;
      data->end_reached = 1;

      break;
    }
  }

  row_read[data->nodeId] += line_count + 1;

  pthread_mutex_unlock(&data->table_mutex);

  free(line);
  fclose(file);

  return line_count;
}
