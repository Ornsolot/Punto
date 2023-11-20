#pragma once

#include <stdio.h>
#include "ocml.h"
#include "ocl.h"

#define CMPCOLOR(c1, c2) (sfColor_toInteger(c1) != sfColor_toInteger(c2))

typedef struct osPuntoCard_s {
    sfRectangleShape *rect;
    Text_t           *text;
    sfColor           color;
    unsigned short    nbr;
} Card_t;

Card_t *setPuntoCard(Font_t *font, unsigned short number, sfColor color, v2f_t coord);
Card_t ***setPuntoCardBoard(Font_t *font, size_t size);
void printPuntoCard(Window_t *frame, Card_t *card);
void printPuntoCardBoard(Window_t *frame, Card_t ***board, size_t size);
void unsetPuntoCard(Card_t *card);
void unsetPuntoCardBoard(Card_t ***board, size_t size);

typedef struct osPuntoData_s {
    SndBuffer_t *select;
    SndBuffer_t *cancel;
    SndBuffer_t *hover;
    SndBuffer_t *move;
    Font_t      *font;
} PuntoData_t;

typedef struct osPuntoScene_s {
    size_t      size;
    Card_t   ***board;
    Text_t     *text;
    Card_t     *card;
} PuntoScene_t;

typedef struct osPuntoGame_s {
    PuntoData_t  data;
    PuntoScene_t scene;

    Window_t    *frame;
    Music_t     *loop;
} Punto_t;