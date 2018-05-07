#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <ulfius.h>
#include <jansson.h>

#define PORT 1234 /* port number */
#define BOARDSIZE 3 /* board size */
#define MAXMESSAGELENGTH 50 /* maximum message length */

/* structure of position */
typedef struct {
  int x;
  int y;
} posn;

static int board[BOARDSIZE][BOARDSIZE]; /* the game board, used to parse the status for calculation, not to store the status */
static int free_pos_index; /* the maximum index of the free position */

static char message[MAXMESSAGELENGTH]; /* message buffer */
static posn *free_posn[BOARDSIZE*BOARDSIZE]; /* store the position availalbe for the server to make moves */

/* function prototypes */
json_t *encode_board(); /* convert board to json */
json_t *encode_message();/* encode message */
json_t *encode_response(); /* encode response for the json information */
void initalize_board(); /* initalizae the board, re-set all points to 0 for start-up */
void make_move(); /* make a move by the server, best first move */
int make_decision(int target); /* make a decision by the server against a threshold */
void free_posns(); /* free the memory allocated */
int check_win(); /* check the current status of the game */
int callback_startgame (const struct _u_request *request, struct _u_response *response, void *user_data); /* call back functions */
int callback_playgame (const struct _u_request *request, struct _u_response *response, void *user_data); /* call back functions */

/* implementations */
json_t *encode_board()
{
  json_t *json_board = json_array();
  json_t *row;
  for (int i = 0; i < BOARDSIZE; i++) {
    row = json_array();
    for (int j = 0; j < BOARDSIZE; j++)
      json_array_append(row, json_integer(board[i][j]));
    json_array_append(json_board, row);
    json_decref(row);
  }

  return json_board;
}

json_t *encode_message()
{
  json_t *json_message = json_string(message);
  return json_message;
}


json_t *encode_response() {

  json_t *json_board = encode_board();
  json_t *json_message = encode_message();
  json_t *final_response = json_object();
  json_object_set_new(final_response, "message", json_message);
  json_object_set_new(final_response, "board", json_board);
  
  return final_response;
  
}

void initalize_board()
{
  for (int i = 0; i < BOARDSIZE; i++)
    for (int j = 0; j < BOARDSIZE; j++)
      board[i][j] = 0;
}

void make_move()
{
  int row, col, lose_target, win_target;

  lose_target = -BOARDSIZE + 1; /* target/threshold value for make a decision against lose */
  win_target = BOARDSIZE - 1; /* target/threshold value for make a decision to win */
  
  /* greedy approach to take the first win */
  if (make_decision(win_target))
    return;

  /* to counter the potential point for lose */
  if (make_decision(lose_target))
    return;
  
  /* if no decision made, make a move on the center otherwise the first one */
  if (board[BOARDSIZE/2][BOARDSIZE/2] == 0) {
    row = BOARDSIZE/2;
    col = BOARDSIZE/2;
  }

  else {
  row = free_posn[0]->x;
  col = free_posn[0]->y;
  }
  
  board[row][col] = 1;
}


int make_decision(int target)
{
  int row, col, sum;
  
    for (int i = 0; i < free_pos_index; i++) {
      row = free_posn[i]->x;
      col = free_posn[i]->y;

      /* check row */
      sum = 0;
      for (int j = 0; j < BOARDSIZE; j++) {
	if (j == col)
	  continue;
	sum += board[row][j];
      }

      if (sum == target) {
	board[row][col] = 1;
	return 1;
      }

      /* check column */
      sum = 0;
      for (int j = 0; j < BOARDSIZE; j++) {
	if (j == row)
	  continue;
	sum += board[j][col];
      }

      if (sum == target) {
	board[row][col] = 1;
	return 1;
      }
   
      /* check diagonal */
      if (row == col) {
	sum = 0;
	for (int m = 0, n = 0; m < BOARDSIZE && n < BOARDSIZE; m++, n++) {
	  if ((m == row) && (n == col))
	    continue;
	  sum += board[m][n];
	}

	if (sum == target) {
	  board[row][col] = 1;
	  return 1;
	}
      }
  
      /* check anti-diagonal */
      if (row + col == BOARDSIZE - 1) {
	sum = 0;
	for (int m = BOARDSIZE - 1, n = 0; m >= 0 && n <BOARDSIZE; m--, n++) {
	  if ((m == row) && (n == col))
	    continue;
	  sum += board[m][n];
	}

	if (sum == target) {
	  board[row][col] = 1;
	  return 1;
	}
      }
    }

  return 0;
 
}

void free_posns()
{
  for (int i = 0; i < free_pos_index; i++)
    free(free_posn[i]);
}

int callback_startgame (const struct _u_request *request, struct _u_response *response, void *user_data) {
 
  /* Generate random order to start the game */
  initalize_board();
  srand(time(0));
  int rn = rand() % 2;
  board[BOARDSIZE/2][BOARDSIZE/2] = rn;

  strcpy(message, "New Game! Make a Move!");
  
  /* JSON response */
  json_t *json_response = encode_response();
  
  ulfius_set_json_body_response(response, 200, json_response);
  json_decref(json_response);

  return U_CALLBACK_CONTINUE;
}

int check_win()
{
  int rt; /* record row result */
  int ct; /* record column result */
  int dt = 0; /* diagonal */
  int adt = 0; /* anti-diagonal */
  free_pos_index = 0; /* free-position index */
  
  /* check for each row */
  for (int i = 0; i < BOARDSIZE; i++) {
    rt = 0;
    ct = 0;
    for (int j = 0; j < BOARDSIZE; j++) {
      
      /* check if the board has free space and record their */
      if (board[i][j] == 0) {
	posn *tposn = (posn *) malloc(sizeof(posn));
        tposn->x = i;
	tposn->y = j;
	free_posn[free_pos_index++] = tposn; 
      }
      
      rt += board[i][j];
      ct += board[j][i];
      if (i == j)
	dt += board[i][j];
      if (i + j == BOARDSIZE - 1)
	adt += board[i][j];
    }

    if (abs(rt) == BOARDSIZE || abs(dt) == BOARDSIZE || abs(ct) == BOARDSIZE || abs(adt) == BOARDSIZE)
      return 1;
  }

  return 0;

}

int callback_playgame (const struct _u_request *request, struct _u_response *response, void *user_data) {


  json_t *json_array = ulfius_get_json_body_request(request, NULL); /* get array from request */

  int array_size = json_array_size(json_array); /* get array size */

  /* Parse the JSON file into the board (all information about the game status comes from the JSON file) */
  for (int i = 0; i < array_size; i++) {
    json_t *json_sub_array = json_array_get(json_array, i);
    int sub_array_size = json_array_size(json_sub_array);
    for (int j = 0; j < sub_array_size; j++)
      board[i][j] = (int) json_integer_value(json_array_get(json_sub_array, j));
    json_decref(json_sub_array);
  }

  json_decref(json_array);
 
  /* check the result first to see if the client has won or draw already */

  /* client already won, shouldn't happen for this server becuase it never loses */
  if (check_win())
    strcpy(message, "Game Ends! You Win!");

  /* already draw due to no space, not necessary to continue */
  else if (!free_pos_index)
    strcpy(message, "Game Ends! Tie!");

  /* server needs to make a move */
  else {
    make_move();
    /* make a move by the client*/
    if (check_win())
      strcpy(message, "Game Ends! You Lose!");
    /* draw */
    else if (!free_pos_index)
      strcpy(message, "Game Ends! Tie!");
    /* continue */
    else
      strcpy(message, "Game Continues! Make a Move!");
  }

  json_t *json_response = encode_response();

  ulfius_set_json_body_response(response, 200, json_response);
  
  json_decref(json_response);
 
  free_posns();
   		   
  return U_CALLBACK_CONTINUE;
}


int main()
{

  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return(1);
  }

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", "/startgame", NULL, 0, &callback_startgame, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", "/playgame", NULL, 0, &callback_playgame, NULL);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);
    // Wait for the user to press <enter> on the console to quit the application
    getchar();
  } else {
    fprintf(stderr, "Error starting framework\n");
  }
  printf("End framework\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
  
}


