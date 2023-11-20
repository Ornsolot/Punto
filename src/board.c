#include "punto.h"

static char *dec(unsigned x, char *s)
{
    *--s = 0;
    if (!x) *--s = '0';
    for (; x; x/=10) *--s = '0'+x%10;
    return s;
}

Card_t *setPuntoCard(Font_t *font, unsigned short number, sfColor color, v2f_t coord)
{
    Card_t *card = (font) ? malloc(sizeof(Card_t)) : NULL;
    char buffer[3*sizeof(int)+1];
    sfFloatRect rb, td;
    float thick = 5;
    
    if (card) {
        card->nbr = number;
        card->color = color;
        card->rect = sfRectangleShape_create();
        sfRectangleShape_setSize(card->rect, (v2f_t){100, 100});
        sfRectangleShape_setFillColor(card->rect, sfBlack);
        sfRectangleShape_setOutlineColor(card->rect, card->color);
        sfRectangleShape_setOutlineThickness(card->rect, thick);
        sfRectangleShape_setPosition(card->rect, coord);
        rb = sfRectangleShape_getGlobalBounds(card->rect);
        card->text = setText(font, dec(number, buffer), card->color);
        td = sfText_getGlobalBounds(card->text->core);
        sfText_setPosition(card->text->core, (v2f_t){coord.x + (rb.width / 2 - td.width / 2 - thick), coord.y + (rb.height / 2 - td.height / 2 - (thick * 4))});
    }
    return (card);
}

Card_t ***setPuntoCardBoard(Font_t *font, size_t size)
{
    Card_t ***board = (font) ? malloc(sizeof(Card_t **) * size) : NULL;

    if (board) {
        for (size_t y = 0; y < size; y++) {
            board[y] = malloc(sizeof(Card_t *) * size);
            for (size_t x = 0; x < size; x++)
                board[y][x] = setPuntoCard(font, 0, sfWhite, (v2f_t){115 * x + 10, 115 * (y+1) + 10});
        }
    }
    return (board);
}

void printPuntoCard(Window_t *frame, Card_t *card)
{
    if (card) {
        sfRenderWindow_drawRectangleShape(frame->core, card->rect, NULL);
        sfRenderWindow_drawText(frame->core, card->text->core, NULL);
    }
}

void printPuntoCardBoard(Window_t *frame, Card_t ***board, size_t size)
{
    if (board) {
        for (size_t y = 0; y < size; y++)
            for (size_t x = 0; x < size; x++)
                printPuntoCard(frame, board[y][x]);
    }
}

void unsetPuntoCard(Card_t *card)
{
    if (card) {
        sfRectangleShape_destroy(card->rect);
        unsetText(card->text);
        free(card);
    }
}

void unsetPuntoCardBoard(Card_t ***board, size_t size)
{
    if (board) {
        for (size_t y = 0; y < size; y++) {
            for (size_t x = 0; x < size; x++)
                unsetPuntoCard(board[y][x]);
            free(board[y]);
        }
        free(board);
    }
}