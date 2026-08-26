#include "punto.h"

/**
 * \brief   Convert an integer into a string.
 * \param   nbr The number to convert into a string.
 * \param   str The string that will be filled with the number.
 * \return  The string containing the number.
 * \warning OBSCURE ARCANE OF BLACK WIZARDRY COMPUTER MAGIC.
 */
static char *utoa(size_t nbr, char *str)
{
    *--str = '\0';
    if (!nbr) *--str = '0';
    for (; nbr; nbr /= 10) *--str = '0' + nbr % 10;
    return (str);
}

/**
 * \brief   Create an Card_t Object.
 * \param   font   The font of the number on the card.
 * \param   number The number on the card.
 * \param   color  The color of the card (number and outline).
 * \param   coord  The coordinate (v2f_t){x, y} of the card.
 * \return  An instance of the Card_t Object.
 */
Card_t *setPuntoCard(Font_t *font, unsigned short number, sfColor color, v2f_t coord)
{
    Card_t *card = (font) ? malloc(sizeof(Card_t)) : NULL;
    char buffer[3 * sizeof(int) + 1];
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
        card->text = setText(font, utoa(number, buffer), card->color);
        td = sfText_getGlobalBounds(card->text->core);
        sfText_setPosition(card->text->core, (v2f_t){coord.x + (rb.width / 2 - td.width / 2 - thick), coord.y + (rb.height / 2 - td.height / 2 - (thick * 4))});
    }
    return (card);
}

/**
 * \brief   Create an matix of pointer to Card_t Object.
 * \param   font The font of the board's card.
 * \param   size The number of row & col of the matrix.
 * \return  An matrix of pointer to Card_t Object.
 */
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

/**
 * \brief   Draw a Card_t Object.
 * \param   frame The window to draw into.
 * \param   card  The card to draw.
 */
void printPuntoCard(Window_t *frame, Card_t *card)
{
    if (card) {
        sfRenderWindow_drawRectangleShape(frame->core, card->rect, NULL);
        sfRenderWindow_drawText(frame->core, card->text->core, NULL);
    }
}

/**
 * \brief   Draw an matix of pointer to Card_t Object.
 * \param   frame The window to draw into.
 * \param   board The board (matrix of pointer to Card_t Object) to draw.
 * \param   size The number of row & col of the matrix.
 */
void printPuntoCardBoard(Window_t *frame, Card_t ***board, size_t size)
{
    if (board) {
        for (size_t y = 0; y < size; y++)
            for (size_t x = 0; x < size; x++)
                printPuntoCard(frame, board[y][x]);
    }
}

/**
 * \brief   Free the memory allocated to an Card_t Object.
 * \param   card The Card_t Object to free.
 */
void unsetPuntoCard(Card_t *card)
{
    if (card) {
        sfRectangleShape_destroy(card->rect);
        unsetText(card->text);
        free(card);
    }
}

/**
 * \brief   Free the memory allocated to an matix of pointer to Card_t Object.
 * \param   board The board (matrix of pointer to Card_t Object) to free.
 * \param   size  The number of row & col of the matrix.
 */
void unsetPuntoCardBoard(Card_t ***board, size_t size)
{
    if (board && size > 0) {
        for (size_t y = 0; y < size; y++) {
            for (size_t x = 0; x < size; x++)
                unsetPuntoCard(board[y][x]);
            free(board[y]);
        }
        free(board);
    }
}
