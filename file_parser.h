/*
 * file_parser.h
 *
 *  Created on: 16/giu/2015
 *      Author: luca
 */

#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_

#define FILE_DIR "/tmp/spinitalia/motor_data/"
#define POSITION_DATA_NUM_MAX 450

struct table_data
{
  int nodeId;
  long position[POSITION_DATA_NUM_MAX]; /**< posizione da raggiungere in passi encoder */
  long time_ms[POSITION_DATA_NUM_MAX]; /**< tempo per arrivare alla posizione in ms (solo per comando posizione/interpolazione */

  long offset; /**< offset dal limite di giunto (solo per comando homing) */
  long forward_velocity; /**< velocità di ricerca limite di giunto (solo per comando di homing) */
  long backward_velocity; /**< velocità di spostamento nello zero dal limite di giunto (solo per comando homing) */

  char type; /**< tipo di dato letto "H": riga di homing, "S": riga tabella */

  int write_pointer; /**< dove sono arrivato a scrivere */
  int read_pointer; /**< dove sono arrivato a leggere */
  int count; /**< numero di elementi presenti nel buffer */

  FILE *position_file; /**< file da cui leggere le posizioni per la simulazione */
  long cursor_position; /**< dove sono arrivato nel file delle posizioni */
  int end_reached; /**< indica se non ci sono più punti da inserire nella tabella */
  int is_pipe;

  pthread_mutex_t table_mutex; /**< sincro tra diversi thread */
  pthread_t table_refiller; /**< thread per tenere la tabella piena */
};

struct table_data_read
{
  long position; /**< posizione da raggiungere in passi encoder */
  long time_ms; /**< tempo per arrivare alla posizione in ms */
};


void QueueInit(int nodeid, struct table_data *data);
void QueueUpdate(struct table_data *data, int point_number);
int QueueGet(struct table_data *data_in, struct table_data_read *data_out,
    int offset);
int QueueLast(struct table_data *data_in, struct table_data_read *data_out);
int QueueFill(struct table_data *data);
int QueuePut(struct table_data *data, int line_number);
int QueuePutPipe(struct table_data *data, int line_number);
float FileCompleteGet(int nodeId, int point_in_table);
int QueueOpenFile(struct table_data *data);
int QueueSeek(struct table_data *data, int point_number);

#endif /* FILE_PARSER_H_ */
