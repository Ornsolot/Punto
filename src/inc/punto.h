#pragma once

// DEPENDENCIES HEADER
#include <stdio.h>
#include "ocml.h"
#include "ocl.h"

// DATABASE HEADER
#include <mysql/mysql.h>
#include <sqlite3.h>

#define SQLHIGHSCORE "SELECT player, move, turn, score FROM Highscore;"
#define SQLEVENT "SELECT player, turn, action, end FROM Event;"

#include <mongoc/mongoc.h>
#include <bson.h>

#include <neo4j-client.h>

// CHECK IF TWO SFCOLOR ARE DIFFERENT.
#define CMPCOLOR(c1, c2) (sfColor_toInteger(c1) != sfColor_toInteger(c2))

// CHECK THE TYPE OF CONNEXION
typedef enum DATABSE { NONE = 0, MY_SQL = 1, SQLITE = 2, MONGODB = 3, NEO4J = 4 } db_t;
#define DB(a) (a == NULL ? NONE : (strcmp(a, "mysql") == 0) ? MY_SQL : (strcmp(a, "sqlite") == 0) ? SQLITE : (strcmp(a, "mongodb") == 0) ? MONGODB : (strcmp(a, "neo4j") == 0) ? NEO4J : NONE)

// CHECK IF YOU CAN INSERT IN A CASE
#define VALID(y, x, b, c, s, d) (b[y][x]->nbr < c->nbr && CMPCOLOR(b[y][x]->color, c->color) && ((x == s/2 && y == s/2) || (x == s/2-1 && y == s/2-1) || (x == s/2-1 && y == s/2) || (x == s/2 && y == s/2-1) || (x > 0 && CMPCOLOR(b[y][x-1]->color, d)) || (x < s-1 && CMPCOLOR(b[y][x+1]->color, d)) || (y > 0 && CMPCOLOR(b[y-1][x]->color, d)) || (y < s-1 && CMPCOLOR(b[y+1][x]->color, d))  || (x > 0 && y > 0 && CMPCOLOR(b[y-1][x-1]->color, d)) || (x < s-1 && y < s-1 && CMPCOLOR(b[y+1][x+1]->color, b[y][x]->color)) || (y > 0 && x < s-1 && CMPCOLOR(b[y-1][x+1]->color, d)) || (x > 0 && y < s-1 && CMPCOLOR(b[y+1][x-1]->color, d)) ))

/**
 * \brief   The object reresenting the carde of the game.
 */
typedef struct osPuntoCard_s {
    sfRectangleShape *rect;
    Text_t           *text;
    sfColor           color;
    unsigned short    nbr;
} Card_t;

// FUCTION TO USE THE CARDE / BOARD ELEMENTS
Card_t *setPuntoCard(Font_t *font, unsigned short number, sfColor color, v2f_t coord);
Card_t ***setPuntoCardBoard(Font_t *font, size_t size);
void printPuntoCard(Window_t *frame, Card_t *card);
void printPuntoCardBoard(Window_t *frame, Card_t ***board, size_t size);
void unsetPuntoCard(Card_t *card);
void unsetPuntoCardBoard(Card_t ***board, size_t size);

/**
 * \brief   The object reresenting the player of the game.
 */
typedef struct osPuntoPlayer_s {
    size_t   size;
    Card_t **deck;
    char    *name;
    size_t   move;
} Player_t;

// FUCTION TO USE THE PLAYER ELEMENTS
Player_t **setPuntoPlayerGroup(Font_t *font, size_t *size, char **name, sfColor *color);
void unsetPuntoPlayerGroup(Player_t **player, size_t length);

/**
 * \brief   The object reresenting the asset of the game.
 */
typedef struct osPuntoData_s {
    SndBuffer_t *select;
    SndBuffer_t *cancel;
    SndBuffer_t *hover;
    SndBuffer_t *move;
    Font_t      *font;
} PuntoData_t;

/**
 * \brief   The object reresenting a Tuple of the best score monitored.
 */
typedef struct osPuntoTuple_s {
    sfColor color;
    size_t length;
    size_t score;
} Tuple_t;

/**
 * \brief   The object reresenting the scene (board + hud) of the game.
 */
typedef struct osPuntoScene_s {
    size_t      size;
    Card_t   ***board;
    Text_t     *text;
    Card_t     *card;
    Tuple_t     max;
} PuntoScene_t;

/**
 * \brief   The object reresenting the game.
 */
typedef struct osPuntoGame_s {
    PuntoData_t  data;
    PuntoScene_t scene;
    Player_t    **player;
    Window_t    *frame;
    Sound_t     *cancel;
    Sound_t     *select;
    Music_t     *loop;
    size_t       round;
    size_t       turn;
    size_t       current;
    size_t       maxPlayer;
    size_t       match;
} Punto_t;

// EVENT FUCTION
bool eventPunto(Punto_t *game);

// DATABASE SUB FUCTION
void insertHighscore(const char *player, size_t move, size_t turn, size_t score);
void insertEvent(const char *player, size_t turn, const char *action, bool end);
// DATABASE MAIN FUNCTION
int fillPuntoDatabase(size_t size, size_t scale);
int migrateDatabase();
int printHighScore();
// DATABASE DRIVER FUCTION
void setDataBase(char *dbName, char *dbConv);
void unsetDataBase();
