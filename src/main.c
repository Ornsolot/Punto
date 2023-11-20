#include "punto.h"

static Punto_t *setPunto()
{
    Punto_t *game = malloc(sizeof(Punto_t));
    
    if (game) {
        // Data elements
        game->data.select = setSoundBuffer("data/sound/menu/select.ogg");
        game->data.cancel = setSoundBuffer("data/sound/menu/cancel.ogg");
        game->data.hover = setSoundBuffer("data/sound/menu/hover.ogg");
        game->data.move = setSoundBuffer("data/sound/menu/move.ogg");
        game->data.font = setFont("data/text/font/Minecraft.ttf", 50, sfTextRegular);
        // Scene elements
        game->scene.size = 7;
        game->scene.board = NULL;
        game->scene.board = setPuntoCardBoard(game->data.font, game->scene.size);
        game->scene.text = setText(game->data.font, "J1 turn", sfWhite);
        sfText_setPosition(game->scene.text->core, (v2f_t){120, 0});
        game->scene.card = setPuntoCard(game->data.font, 1, sfRed, (v2f_t){10, 10});
        // Sys elements
        game->frame = setWindow(&(WdIni_t){ sfVideoMode_getDesktopMode(), "Ornsolot's Punto Board Game", sfFullscreen,  144, sfFalse, sfFalse, sfFalse, sfTrue, sfTrue, "data/icon/p.png" });
        sfRenderWindow_setPosition(game->frame->core, (v2i_t){0, 0});
        game->loop = setMusic("data/sound/track/Luigi Casino.ogg", sfTrue);
    }
    return (game);
}

static void unsetPunto(Punto_t *game)
{
    if (game) {
        // Data
        unsetSoundBuffer(game->data.select);
        unsetSoundBuffer(game->data.cancel);
        unsetSoundBuffer(game->data.hover);
        unsetSoundBuffer(game->data.move);
        unsetFont(game->data.font);
        // Board
        unsetText(game->scene.text);
        unsetPuntoCard(game->scene.card);
        unsetPuntoCardBoard(game->scene.board, game->scene.size);
        // Sys
        unsetWindow(game->frame);
        unsetMusic(game->loop);
        free(game);
    }
}

bool eventPunto(Punto_t *game)
{
    v2i_t mouse = sfMouse_getPositionRenderWindow(game->frame->core);
    v2i_t click = {-1, -1};
    bool find = false;
    bool end = true;
    sfFloatRect rect;
    sfEvent event;
    for (size_t y = 0; y < game->scene.size; y++)
        for (size_t x = 0; x < game->scene.size; x++) {
            if (game->scene.card->nbr > game->scene.board[y][x]->nbr)
                end = false;
            rect = sfRectangleShape_getGlobalBounds(game->scene.board[y][x]->rect);
            if ((find = sfFloatRect_contains(&rect, mouse.x, mouse.y)) == true)
                click = (v2i_t){x, y};
            sfRectangleShape_setFillColor(game->scene.board[y][x]->rect, (find) ? sfWhite : sfBlack);
        }
    //sfKeyboard_setVirtualKeyboardVisible(sfTrue);
    while (sfRenderWindow_pollEvent(game->frame->core, &event)) {
        if (end || sfKeyboard_isKeyPressed(sfKeyEscape) || event.type == sfEvtClosed) {
            sfRenderWindow_close(game->frame->core);
            printf((end) ? "You can't play anymore %s win.\n" : "Game was ended.\n", "hello world !");
        }
        if (event.type == sfEvtMouseButtonPressed && click.y >= 0 && click.x >= 0 && game->scene.board[click.y][click.x]->nbr < game->scene.card->nbr && (
            (click.x == (int)(game->scene.size / 2) && click.y == (int)(game->scene.size / 2)) ||
            (click.x > 0 && CMPCOLOR(game->scene.board[click.y][click.x-1]->color, sfWhite)) || (click.x < (int)(game->scene.size -1) && CMPCOLOR(game->scene.board[click.y][click.x+1]->color, sfWhite)) ||
            (click.y > 0 && CMPCOLOR(game->scene.board[click.y-1][click.x]->color, sfWhite)) || (click.y < (int)(game->scene.size -1) && CMPCOLOR(game->scene.board[click.y+1][click.x]->color, sfWhite))
            )) {
            rect = sfRectangleShape_getGlobalBounds(game->scene.board[click.y][click.x]->rect);
            unsetPuntoCard(game->scene.board[click.y][click.x]);
            game->scene.board[click.y][click.x] = setPuntoCard(game->data.font, game->scene.card->nbr, game->scene.card->color, (v2f_t){rect.left + 5, rect.top + 5});
            printf("%i\n", 0%2);
        }

    }
    return (sfRenderWindow_isOpen(game->frame->core));
}

void printPunto(Punto_t *game)
{
    char str[200];

    if (updateClock(game->frame->clock) > 0) {
        sfRenderWindow_clear(game->frame->core, sfBlack);
        printPuntoCard(game->frame, game->scene.card);
        sprintf(str, "%s turn !\n round: %i move: %i turn: %i", "J1", 0, 0, 0);
        sfText_setString(game->scene.text->core, str);
        sfRenderWindow_drawText(game->frame->core, game->scene.text->core, NULL);
        printPuntoCardBoard(game->frame, game->scene.board, game->scene.size);
        sfRenderWindow_display(game->frame->core);
    }
}

int main(int argc, char *argv[], char *envp[])
{
    Punto_t *game = NULL;

    if (!(game = setPunto()))
        return (EXIT_FAILURE);
    sfMusic_play(game->loop->core);
    while (eventPunto(game)) {
        printPunto(game);
    }
    unsetPunto(game);
    return (EXIT_SUCCESS);
}
