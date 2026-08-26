#include "punto.h"

/**
 * \brief   Fill the instance of Punto_t Object scene elements.
 * \warning Let scale at 1, you can't scroll to get to the end of the board...
 *
 * \param   game   The game to print.
 * \param   player The number of player (2 or 4).
 * \param   scale  The scale of the game (default 1), make the board & decks bigger.
 */
static void setPuntoScene(Punto_t *game, size_t player, size_t scale)
{
    game->scene.size = 6 * scale;
    game->maxPlayer = player;
    game->player = setPuntoPlayerGroup(game->data.font, (size_t[2]){player, (6 * scale) * 1.5 }, (char *[4]){"J1", "J2", "J3", "j4"}, (sfColor[4]){sfRed, sfYellow, sfCyan, sfGreen});
    game->scene.board = setPuntoCardBoard(game->data.font, game->scene.size);
    game->scene.text = setText(game->data.font, "", sfWhite);
    sfText_setPosition(game->scene.text->core, (v2f_t){120, 0});
    game->scene.card = game->player[0]->deck[game->player[0]->size-1];
    game->scene.max = (Tuple_t){sfWhite, 0, 0};

}

/**
 * \brief   Create an instance of Punto_t Object.
 * \warning Let scale at 1, you can't scroll to get to the end of the board...
 
 * \param   player The number of player (2 or 4).
 * \param   scale  The scale of the game (default 1), make the board & decks bigger.
 * \return  A pointer to an instance of the Punto_t Object or NULL.
 */
static Punto_t *setPunto(size_t player, size_t scale)
{
    Punto_t *game = (player == 2 || player == 4) ? malloc(sizeof(Punto_t)) : NULL;
    
    if (game) {
        // Player elements
        game->current = 0;
        game->match = (player == 2) ? scale * 5 : scale * 4;
        game->round = 1;
        game->turn = 1;
        // Data elements
        game->data.select = setSoundBuffer("data/sound/menu/select.ogg");
        game->data.cancel = setSoundBuffer("data/sound/menu/cancel.ogg");
        game->data.hover = setSoundBuffer("data/sound/menu/hover.ogg");
        game->data.move = setSoundBuffer("data/sound/menu/move.ogg");
        game->data.font = setFont("data/text/font/Minecraft.ttf", 50, sfTextRegular);
        // Scene elements
        setPuntoScene(game, player, scale);
        // Sys elements
        game->frame = setWindow(&(WdIni_t){ sfVideoMode_getDesktopMode(), "Ornsolot's Punto Board Game", sfFullscreen,  144, sfFalse, sfFalse, sfFalse, sfTrue, sfTrue, "data/icon/p.png" });
        sfRenderWindow_setPosition(game->frame->core, (v2i_t){0, 0});
        sfRenderWindow_requestFocus(game->frame->core);
        game->cancel = setSound(game->data.cancel, sfFalse);
        game->select = setSound(game->data.select, sfFalse);
        game->loop = setMusic("data/sound/track/Luigi Casino.ogg", sfTrue);
    }
    return (game);
}

/**
 * \brief   Free the memory allocated to the game.
 *
 * \param   game The game to free.
 */
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
        unsetPuntoPlayerGroup(game->player, game->maxPlayer);
        unsetText(game->scene.text);
        unsetPuntoCardBoard(game->scene.board, game->scene.size);
        // System
        unsetWindow(game->frame);
        unsetSound(game->select);
        unsetSound(game->cancel);
        unsetMusic(game->loop);
        free(game);
    }
}

/**
 * \brief   Print the game board and hud.
 *
 * \param   game The game to print.
 */
static void printPunto(Punto_t *game)
{
    char str[200];

    if (updateClock(game->frame->clock) > 0) {
        // CLEAR WINDOW PREVIOUS FRAME
        sfRenderWindow_clear(game->frame->core, sfBlack);
        // PRINT THE BOARD
        printPuntoCardBoard(game->frame, game->scene.board, game->scene.size);
        // PRINT THE HUD
        printPuntoCard(game->frame, game->scene.card);
        sprintf(str, "%s turn with %zu card in deck !\nround: %lu move: %lu turn: %lu", game->player[game->current]->name, game->player[game->current]->size, game->round, game->player[game->current]->move, game->turn);
        sfText_setString(game->scene.text->core, str);
        sfRenderWindow_drawText(game->frame->core, game->scene.text->core, NULL);
        // DRAW NEW FRAME
        sfRenderWindow_display(game->frame->core);
    }
}

/**
 * \brief   The game loop.
 * \warning Let scale at 1, you can't scroll to get to the end of the board...
 *
 * \param   player The number of player.
 * \param   scale  The scale of the board.
 * \return  Error code.
 */
static int loopPunto(size_t player, size_t scale)
{
    Punto_t *game = setPunto(player, scale);
    int result = EXIT_FAILURE;

    if (game) { // CHECK IF THE GAME CAN IS BUILD
        sfMusic_play(game->loop->core); // START THE MUSIC LOOP
        while (eventPunto(game))  // MANAGE THE EVENT
            printPunto(game); // DISPLAY THE GAME BOARD
        unsetPunto(game); // FREE MEMORY
        result = EXIT_SUCCESS;
    }
    return (result);
}

/**
 * \brief   The main function with parameters verification.
 *
 * \param   av Parameters send to the function.
 * \param   ac Number of parameters send to the function.
 * \return  Error code.
 */
int main(int ac, char *av[])
{
    int result = EXIT_FAILURE;

    setRNG(); // SET RNG SEED
    if (ac == 5 && strcmp(av[1], "auto") == 0) { // Fill the database
        setDataBase(av[2], NULL);
        for (int i = 0; i < atoi(av[4]); i++)
            result = fillPuntoDatabase(atoi(av[3]), 1);
    } else if (ac == 4 && strcmp(av[1], "migrate") == 0) { // Data migration
        setDataBase(av[3], av[2]);
        result = migrateDatabase();
    } else if (ac == 4 && strcmp(av[1], "play") == 0) { // Game loop
        setDataBase(av[2], NULL);
        result = loopPunto(atoi(av[3]), 1);
    }  else if (ac == 3 && strcmp(av[1], "score") == 0) { // Score board
        setDataBase(av[2], NULL);
        result = printHighScore(); 
    }
    unsetDataBase();
    return (result);
}
