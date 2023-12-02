#include "punto.h"

/**
 * \brief   Free the memory allocated to the group of player.
 * \param   font the font of the cards of the deck.
 * \param   length the length of the deck (18).
 * \param   name the name of the player.
 * \param   color the colors of the cards.
 */
static Player_t *setPunto2Player(Font_t *font, size_t length, char *name, sfColor *color)
{
    Player_t *player = (font && name && length > 0) ? malloc(sizeof(Player_t)) : NULL;
    
    player->size = length * 2;
    if (player && (player->deck = malloc(sizeof(Card_t *) * player->size))) {
        for (size_t i = 0; i < length; i++) {
            player->deck[i+length] = setPuntoCard(font, i+1, color[1], (v2f_t){10, 10});
            player->deck[i] = setPuntoCard(font, i+1, color[0], (v2f_t){10, 10});
        }
        shuffle((void **) (player->deck), player->size);
        player->name = name;
        player->move = 1;
    }
    return (player);
}

/**
 * \brief   Free the memory allocated to the group of player.
 * \param   font the font of the cards of the deck.
 * \param   length the length of the deck (9).
 * \param   name the name of the player.
 * \param   color the color of the cards.
 */
static Player_t *setPunto4Player(Font_t *font, size_t length, char *name, sfColor color)
{
    Player_t *player = (font && name && length > 0) ? malloc(sizeof(Player_t)) : NULL;
    
    player->size = length;
    if (player && (player->deck = malloc(sizeof(Card_t *) * player->size))) {
        for (size_t i = 0; i < length; i++) {
            player->deck[i] = setPuntoCard(font, i+1, color, (v2f_t){10, 10});
        }
        shuffle((void **) (player->deck), player->size);
        player->name = name;
        player->move = 1;
    }
    return (player);
}

/**
 * \brief   Free the memory allocated to the group of player.
 * \param   font the font of the cards of the decks.
 * \param   size the length of the groupe (2 or 4).
 */
Player_t **setPuntoPlayerGroup(Font_t *font, size_t *size, char **name, sfColor *color)
{
    Player_t **player = (font && name && size && color) ? malloc(sizeof(Player_t *) * size[0]) : NULL;

    if (player) {
        if (size[0] == 4) {
            for (size_t i = 0; i < size[0]; i++)
                player[i] = setPunto4Player(font, size[1], name[i], color[i]);
        } else {
            player[0] = setPunto2Player(font, size[1], name[0], (sfColor[2]){color[0], color[1]});
            player[1] = setPunto2Player(font, size[1], name[1], (sfColor[2]){color[2], color[3]});
        }
    }
    return (player);
}

/**
 * \brief   Free the memory allocated to a player.
 * \param   player the player to free.
 */
static void unsetPuntoPlayer(Player_t *player)
{
    if (player) {
        for (size_t i = 0; i < player->size; i++)
            unsetPuntoCard(player->deck[i]);
        free(player->deck);
        free(player);
    }
}

/**
 * \brief   Free the memory allocated to the group of player.
 * \param   player the groupe of player to free.
 * \param   length the length of the groupe (2 or 4).
 */
void unsetPuntoPlayerGroup(Player_t **player, size_t length)
{
    if (player && length > 0) {
        for (size_t i = 0;  i < length; i++)
            unsetPuntoPlayer(player[i]);
        free(player);
    }
}