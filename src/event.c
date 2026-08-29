#include "punto.h"

/**
 * \brief   Convert a color to a string.
 *
 * \param   color The color to convert into a string.
 * \return  A string with the name of the color or NULL.
 */
static char *colorToString(sfColor color)
{
    switch(sfColor_toInteger(color)) {
        case INT_TRANSPARENT: return ("Invisible");
        case INT_MAGENTA: return ("Magenta");
        case INT_YELLOW: return ("Yellow");
        case INT_BLACK: return ("Black");
        case INT_WHITE: return ("White");
        case INT_GREEN: return ("Green");
        case INT_BLUE: return ("Blue");
        case INT_CYAN: return ("Cyan");
        case INT_RED: return ("Red");
        default: return ("ERROR: Undefined Color");
    }
}

/**
 * \brief   Find the biggest tuple of a column.
 *
 * \param   game  The game containing the board to parse.
 * \param   click The coordinate of the click.
 */
static void checkScoreCol(Punto_t *game, v2i_t click)
{
    Tuple_t tuple = (Tuple_t){sfBlack, 0, 0};
    sfUint32 buffer = 0;

    for (size_t y = 0;  y < game->scene.size; y++) {
        buffer = sfColor_toInteger(game->scene.board[y][click.x]->color);
        if (buffer == sfColor_toInteger(tuple.color)) {
            tuple.score += game->scene.board[y][click.x]->nbr;
            tuple.length++;
        } else if (buffer != INT_WHITE) {
            tuple = (Tuple_t){game->scene.board[y][click.x]->color, 1, game->scene.board[y][click.x]->nbr};
        }
        if (game->scene.max.length < tuple.length || (game->scene.max.score > tuple.score && game->scene.max.length == tuple.length))
            game->scene.max = (Tuple_t){tuple.color, tuple.length, tuple.score};
    }
}

/**
 * \brief   Find the biggest tuple of a row.
 *
 * \param   game  The game containing the board to parse.
 * \param   click The coordinate of the click.
 */
static void checkScoreRow(Punto_t *game, v2i_t click)
{
    Tuple_t tuple = (Tuple_t){sfBlack, 0, 0};
    sfUint32 buffer = 0;

    for (size_t x = 0;  x < game->scene.size; x++) {
        buffer = sfColor_toInteger(game->scene.board[click.y][x]->color);
        if (buffer == sfColor_toInteger(tuple.color)) {
            tuple.score += game->scene.board[click.y][x]->nbr;
            tuple.length++;
        } else if (buffer != INT_WHITE) {
            tuple = (Tuple_t){game->scene.board[click.y][x]->color, 1, game->scene.board[click.y][x]->nbr};
        }
        if (game->scene.max.length < tuple.length || (game->scene.max.score > tuple.score && game->scene.max.length == tuple.length))
            game->scene.max = (Tuple_t){tuple.color, tuple.length, tuple.score};
    }
}

/**
 * \brief   Find the biggest tuple of a diagonal from upper-left to lower-right.
 *
 * \param   game the game containing the board to parse.
 */
static void checkScoreDiagLeftToRight(Punto_t *game)
{
    Tuple_t tuple = (Tuple_t){sfBlack, 0, 0};
    sfUint32 buffer = 0;
    short maxDiag = (game->maxPlayer < 4) ? 3 : 5;
    short bound = game->scene.size - game->match;

    for (short i = 0; i < maxDiag; i++) {
        for (size_t y = (i > bound) ? i - bound : 0, size_t x = (i < bound) ? bound - i : 0;  y < game->scene.size && x < game->scene.size; y++, x++) {
            buffer = sfColor_toInteger(game->scene.board[y][x]->color);
            if (buffer == sfColor_toInteger(tuple.color)) {
                tuple.score += game->scene.board[y][x]->nbr;
                tuple.length++;
            } else if (buffer != INT_WHITE) {
                tuple = (Tuple_t){game->scene.board[y][x]->color, 1, game->scene.board[y][x]->nbr};
            }
            if (game->scene.max.length < tuple.length || (game->scene.max.score > tuple.score && game->scene.max.length == tuple.length))
                game->scene.max = (Tuple_t){tuple.color, tuple.length, tuple.score};
        }
    }
}

/**
 * \brief   Find the biggest tuple of a diagonal from upper-right to lower-left.
 *
 * \param   game The game containing the board to parse.
 */
static void checkScoreDiagRightToLeft(Punto_t *game)
{
    Tuple_t tuple = (Tuple_t){sfBlack, 0, 0};
    sfUint32 buffer = 0;
    short maxDiag = (game->maxPlayer < 4) ? 3 : 5;
    short bound = game->match - 1;

    for (short i = 0; i < maxDiag; i++) {
        for (size_t y = (i > bound) ? i + 1 - bound : 0, size_t x = (i < bound) ? bound - i : 0;  y < game->scene.size && x =< 0; y++, x--) {
            buffer = sfColor_toInteger(game->scene.board[y][x]->color);
            if (buffer == sfColor_toInteger(tuple.color)) {
                tuple.score += game->scene.board[y][x]->nbr;
                tuple.length++;
            } else if (buffer != INT_WHITE) {
                tuple = (Tuple_t){game->scene.board[y][x]->color, 1, game->scene.board[y][x]->nbr};
            }
            if (game->scene.max.length < tuple.length || (game->scene.max.score > tuple.score && game->scene.max.length == tuple.length))
                game->scene.max = (Tuple_t){tuple.color, tuple.length, tuple.score};
        }
    }
}

/**
 * \brief   Find the biggest tuple of the board, the win condition of the game.
 *
 * \param   game  The game containing the board to parse.
 * \param   click The coordinate of the click.
 * \return  True if the game is finished and False otherwise.
 */
static bool checkScore(Punto_t *game, v2i_t click)
{
    checkScoreCol(game, click);
    checkScoreRow(game, click);
    checkScoreDiagLeftToRight(game);
    checkScoreDiagRightToLeft(game);
    
    return ((game->scene.max.length == game->match) ? true : false);
}

/**
 * \brief   Manage the Click of the mouse events, change cards and players on the board.
 *
 * \param   game the game to monitor.
 * \param   click the coordinate of the click.
 * \return  True if the game is finished and false otherwise.
 */
static bool clickEvent(Punto_t *game, v2i_t click)
{
    sfFloatRect rect = sfRectangleShape_getGlobalBounds(game->scene.board[click.y][click.x]->rect);
    char action[100];
    bool end = false;

    // DATABASE INSERT
    sprintf(action, "insert %i %s card at {y:%i, x:%i}", game->scene.card->nbr, colorToString(game->scene.card->color), click.y, click.x);
    insertEvent(game->player[game->current]->name, game->turn, action, false);
    // CARD SWAPING
    unsetPuntoCard(game->scene.board[click.y][click.x]);
    game->scene.board[click.y][click.x] = setPuntoCard(game->data.font, game->scene.card->nbr, game->scene.card->color, (v2f_t){rect.left + 5, rect.top + 5});
    unsetPuntoCard(game->scene.card);
    game->player[game->current]->size--;
    game->player[game->current]->move++;
    // TURN MANAGEMENT
    if ((end = checkScore(game, click)) == false) {
        if (game->current == game->maxPlayer - 1) {
            game->current = 0;
            game->round++;
        } else {
            game->current++;
        }
        if (game->player[game->current]->size >= 1) {
            game->scene.card = game->player[game->current]->deck[game->player[game->current]->size-1];
        } else {
            end = true;
        }
    }
    game->turn++;
    return (end);
}

/**
 * \brief   Manage the Board events of the game, mainly the interactivity of the board.
 *
 * \param   game  The game to monitor.
 * \param   click The coordinate of the click.
 * \param   valid The coordinate of the click.
 * \return  True if the game is finished and False otherwise.
 */
static bool boardEvent(Punto_t *game, v2i_t *click, bool *valid)
{
    v2i_t mouse = sfMouse_getPositionRenderWindow(game->frame->core);
    bool find = false;
    bool end = true;
    sfFloatRect rect;

    for (size_t y = 0; y < game->scene.size; y++)
        for (size_t x = 0; x < game->scene.size; x++) {
            if (end || game->scene.card->nbr > game->scene.board[y][x]->nbr && CMPCOLOR(game->scene.board[y][x]->color, game->scene.card->color))
                end = false;
            rect = sfRectangleShape_getGlobalBounds(game->scene.board[y][x]->rect);
            if ((find = sfFloatRect_contains(&rect, mouse.x, mouse.y)) == true) {    
                *valid = VALID(y, x, game->scene.board, game->scene.card, game->scene.size, sfWhite);
                *click = (v2i_t) {x, y};
            }
            sfRectangleShape_setFillColor(game->scene.board[y][x]->rect, (find) ? (*valid) ? sfMagenta : sfWhite : sfBlack);
        }
    return (end);
}

/**
 * \brief   Monitor and manage player input and game event trigger.
 * \param   game The game to monitor.
 * \return  False in case of end of the game event and True otherwise.
 */
bool eventPunto(Punto_t *game)
{
    v2i_t click = {-1, -1};
    bool valid = false;
    bool end = true;
    char action[100];
    
    end = boardEvent(game, &click, &valid);
    while (sfRenderWindow_pollEvent(game->frame->core, &game->frame->event)) {
        if (game->frame->event.type == sfEvtMouseButtonPressed) {
            if (valid)
                end = clickEvent(game, click);
            sfSound_play((valid) ? game->select->core : game->cancel->core);
        }
        if (end || sfKeyboard_isKeyPressed(sfKeyEscape) || game->frame->event.type == sfEvtClosed) {
            sfSound_play(game->cancel->core);
            if (end) {
                sprintf(action, "Win with the score of %zu !", game->scene.max.score);
                insertEvent(game->player[game->current]->name, game->turn, action, true);
                insertHighscore(game->player[game->current]->name, game->player[game->current]->move, game->turn, game->scene.max.score);
            }
            sfRenderWindow_close(game->frame->core);
            printf((end) ? "%s win with the score of %zu !.\n" : "Game was ended.\n", game->player[game->current]->name, game->scene.max.score);
        }
    }
    return (sfRenderWindow_isOpen(game->frame->core));
}
