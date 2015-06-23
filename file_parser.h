/*
 * file_parser.h
 *
 *  Created on: 16/giu/2015
 *      Author: luca
 */

#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_

#define FILE_DIR "SWP.33.05.02.0.0/tables/"
#define POSITION_DATA_NUM_MAX 45

struct table_data
{
  int nodeId;
  long position[POSITION_DATA_NUM_MAX]; /**< posizione da raggiungere in passi encoder */
  long time_ms[POSITION_DATA_NUM_MAX]; /**< tempo per arrivare alla posizione in ms */

  int write_pointer; /**< dove sono arrivato a scrivere */
  int read_pointer; /**< dove sono arrivato a leggere */
  int count; /**< numero di elementi presenti nel buffer */

  int cursor_position; /**< dove sono arrivato nel file delle posizioni */
  int end_reached; /**< indica se non ci sono più punti da inserire nella tabella */

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

#endif /* FILE_PARSER_H_ */
