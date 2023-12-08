#include "punto.h"
#include <errno.h>

// MYSQL GLOBAL VARIABLE
MYSQL *mysql = NULL;
// SQLITE GLOBAL VARIABLE
sqlite3 *sqlite = NULL;
// MONGODB GLOBAL VARIABLES
mongoc_client_t *mongodb = NULL;
mongoc_collection_t *highscore = NULL;
mongoc_collection_t *event = NULL;
// NEO4J GLOBAL VARIABLE
neo4j_connection_t *neo4j = NULL;

// INDEX TO KNOW WICH DATABASE TO USE FOR EACH OPERATION
db_t type = NONE;
db_t conv = NONE;

static char *trimStr(char *str)
{
    str[strlen(str) - 1] = '\0';
    return (str + 1);
}

/**
 * \brief   Callback function to insert an element of the Table Event from MYSQL to another Database.
 * \param   NotUsed the callback force it's existence for niche application.
 * \param   ac the number of element in av.
 * \param   av the list of value of the query.
 * \param   env the list of key of the query.
 */
static int callbackEvent(void *NotUsed, int ac, char **av, char **env)
{
    if (!NotUsed && env &&  ac == 4)
        insertEvent(av[0], atoi(av[1]), av[2], atoi(av[3]));
    return (EXIT_SUCCESS);
}

/**
 * \brief   Callback function to insert an element of the Table Event from MONGODB to another Database.
 * \warning I had to use BSON_t file to do so, it's long ccode for what it's supposed to do (lots of conversion).
 */
static void callbackEventMongo()
{
    bson_t * query = bson_new();
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(event, query, NULL, NULL);
    const bson_t *doc = NULL;
    bson_iter_t iter;
    const char *player = NULL;
    const char *action = NULL;
    size_t turn = 0;
    bool end = false;
    const char *key = NULL;

    while (mongoc_cursor_next(cursor, &doc)) {
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                key = bson_iter_key(&iter);
                if (strcmp(key, "player") == 0)
                    player = bson_iter_utf8(&iter, NULL);
                if (strcmp(key, "turn") == 0)
                    turn = bson_iter_as_int64(&iter);
                if (strcmp(key, "action") == 0)
                    action = bson_iter_utf8(&iter, NULL);
                if (strcmp(key, "end") == 0)
                    end = bson_iter_as_bool(&iter);
            }
            insertEvent(player, turn, action, end);
        }
    }
    mongoc_cursor_destroy(cursor);
    bson_destroy(query);
}

/**
 * \brief   Callback function to print an element of the Table Highscore from MYSQL.
 * \param   NotUsed the callback force it's existence for niche application.
 * \param   ac the number of element in av.
 * \param   av the list of value of the query.
 * \param   env the list of key of the query.
 */
static int callbackPrintHighscore(void *NotUsed, int ac, char **av, char **env)
{
    if (!NotUsed && env && ac == 4)
        printf("%s\t%s\t%s\t%s\n", av[0], av[1], av[2], av[3]);
    return (EXIT_SUCCESS);
}

/**
 * \brief   Callback function to insert an element of the Table Highscore from MYSQL to another Database.
 * \param   NotUsed the callback force it's existence for niche application.
 * \param   ac the number of element in av.
 * \param   av the list of value of the query.
 * \param   env the list of key of the query.
 */
static int callbackHighscore(void *NotUsed, int ac, char **av, char **env)
{
    if (!NotUsed && env && ac == 4)
        insertHighscore(av[0], atoi(av[1]), atoi(av[2]), atoi(av[3]));
    return (EXIT_SUCCESS);
}

/**
 * \brief   Callback function to insert an element of the Table Highscore from MONGODB to another Database.
 * \warning I had to use BSON_t file to do so, it's long ccode for what it's supposed to do (lots of conversion).
 */
static void callbackPrintHighScoreMongo()
{
    bson_t * query = bson_new();
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(highscore, query, NULL, NULL);
    const bson_t *doc = NULL;
    bson_iter_t iter;
    const char *player = NULL;
    size_t move = 0;
    size_t turn = 0;
    size_t score = 0;
    const char *key = NULL;

    while (mongoc_cursor_next(cursor, &doc)) {
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                key = bson_iter_key(&iter);
                if (strcmp(key, "player") == 0)
                    player = bson_iter_utf8(&iter, NULL);
                if (strcmp(key, "move") == 0)
                    move = bson_iter_as_int64(&iter);
                if (strcmp(key, "turn") == 0)
                    turn = bson_iter_as_int64(&iter);
                if (strcmp(key, "score") == 0)
                    score = bson_iter_as_int64(&iter);
            }
            printf("%s\t%zu\t%zu\t%zu\n", player, move, turn, score);
        }
    }
    mongoc_cursor_destroy(cursor);
    bson_destroy(query);
}

/**
 * \brief   Callback function to insert an element of the Table Highscore from MONGODB to another Database.
 * \warning I had to use BSON_t file to do so, it's long ccode for what it's supposed to do (lots of conversion).
 */
static void callbackHighScoreMongo()
{
    bson_t * query = bson_new();
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(highscore, query, NULL, NULL);
    const bson_t *doc = NULL;
    bson_iter_t iter;
    const char *player = NULL;
    size_t move = 0;
    size_t turn = 0;
    size_t score = 0;
    const char *key = NULL;

    while (mongoc_cursor_next(cursor, &doc)) {
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                key = bson_iter_key(&iter);
                if (strcmp(key, "player") == 0)
                    player = bson_iter_utf8(&iter, NULL);
                if (strcmp(key, "move") == 0)
                    move = bson_iter_as_int64(&iter);
                if (strcmp(key, "turn") == 0)
                    turn = bson_iter_as_int64(&iter);
                if (strcmp(key, "score") == 0)
                    score = bson_iter_as_int64(&iter);
            }
            insertHighscore(player, move, turn, score);
        }
    }
    mongoc_cursor_destroy(cursor);
    bson_destroy(query);
}

/**
 * \brief   Insert an element in the table Highscore.
 * \param   palyer the name of the player.
 * \param   move the number of moves of the player (turn he played).
 * \param   turn the current turn of the game.
 * \param   score how muche the player scored.
 */
void insertHighscore(const char *player, size_t move, size_t turn, size_t score)
{
    neo4j_result_stream_t *buffer = NULL;
    bson_t *doc = bson_new();
    bson_oid_t oid;
    char query[300];

    sprintf(query, "INSERT INTO Highscore (player, move, turn, score) VALUES ('%s', %zu, %zu, %zu);", player, move, turn, score);
    if (type == MY_SQL && mysql && mysql_query(mysql, query) != 0)
        fprintf(stderr, "Query Failure\n");
    if (type == SQLITE && sqlite && sqlite3_exec(sqlite, query, 0, 0, NULL) != SQLITE_OK)
        fprintf(stderr, "Query Failure\n");
    if (type == MONGODB && mongodb && highscore) {
        bson_oid_init(&oid, NULL);
        BSON_APPEND_OID(doc, "_id", &oid);
        BSON_APPEND_UTF8(doc, "player", player);
        BSON_APPEND_INT32(doc, "move", move);
        BSON_APPEND_INT32(doc, "turn", turn);
        BSON_APPEND_INT32(doc, "score", score);
        bson_append_now_utc(doc, "at", -1);
        if (!mongoc_collection_insert(highscore, MONGOC_INSERT_NONE, doc, NULL, NULL))
            fprintf(stderr, "Query Failure\n");
    }
    if (type == NEO4J && neo4j) {
        sprintf(query, "CREATE (:Highscore {player: '%s',  move: %zu, turn: %zu, score: %zu, at: timestamp()})\n", player, move, turn, score);
        if ((buffer = neo4j_run(neo4j, query, neo4j_null)) == NULL)
            neo4j_perror(stderr, errno, "Failed to run statement");
        neo4j_close_results(buffer);
    }
    bson_destroy (doc);
}

/**
 * \brief   Insert an element in the table Event.
 * \param   palyer the name of the player.
 * \param   turn the current turn of the game.
 * \param   action the action (move or event triggered) made by the player.
 * \param   end is this an end of the game event ?
 */
void insertEvent(const char *player, size_t turn, const char *action, bool end)
{
    neo4j_result_stream_t *buffer = NULL;
    bson_t *doc = bson_new();
    bson_oid_t oid;
    char query[300];

    sprintf(query, "INSERT INTO Event (player, turn, action, end) VALUES ('%s', %zu, '%s', %s);", player, turn, action, (end) ? "TRUE" : "FALSE");
    if (type == MY_SQL && mysql && mysql_query(mysql, query) != 0)
        fprintf(stderr, "Query Failure\n");
    if (type == SQLITE && sqlite && sqlite3_exec(sqlite, query, 0, 0, NULL) != SQLITE_OK)
        fprintf(stderr, "Query Failure\n");
    if (type == MONGODB && mongodb && event) {
        bson_oid_init(&oid, NULL);
        BSON_APPEND_OID(doc, "_id", &oid);
        BSON_APPEND_UTF8(doc, "player", player);
        BSON_APPEND_INT32(doc, "turn", turn);
        BSON_APPEND_UTF8(doc, "action", action);
        BSON_APPEND_BOOL(doc, "end", end);
        bson_append_now_utc(doc, "at", -1);
        if (!mongoc_collection_insert(event, MONGOC_INSERT_NONE, doc, NULL, NULL))
            fprintf(stderr, "Query Failure\n");
    }
    if (type == NEO4J && neo4j) {
        sprintf(query, "CREATE (:Event {player: '%s', turn: %zu, action: '%s', end: %s, at: timestamp()})\n", player, turn, action, (end) ? "true" : "false");
        if ((buffer = neo4j_run(neo4j, query, neo4j_null)) == NULL)
            neo4j_perror(stderr, errno, "Failed to run statement");
        neo4j_close_results(buffer);
    }
    bson_destroy (doc);
}

/**
 * \brief   Function to print all elements from a Databse to the treminal.
 */
int printHighScore()
{
    neo4j_result_stream_t *buffer = NULL;
    neo4j_result_t *node = NULL;
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char array[4][200];

    printf("──────── SCORE BOARD ────────\n");
    printf("%s\t%s\t%s\t%s\n", "NAME", "MOVE", "TURN", "SCORE");
    if (type == MY_SQL && mysql) {
        if (mysql_query(mysql, SQLHIGHSCORE) == 0 && (result = mysql_store_result(mysql)) != NULL) {
            while ((row = mysql_fetch_row(result)) != NULL)
                printf("%s\t%s\t%s\t%s\n", row[0], row[1], row[2], row[3]);
        }
        mysql_free_result(result);
    }
    if (type == SQLITE && sqlite)
        sqlite3_exec(sqlite, SQLHIGHSCORE, callbackPrintHighscore, 0, NULL);
    if (type == MONGODB && mongodb && highscore)
        callbackPrintHighScoreMongo();
    if (type == NEO4J && neo4j) {
        if ((buffer = neo4j_run(neo4j, "MATCH (h:Highscore) RETURN h.player, h.move, h.turn, h.score", neo4j_null)) != NULL) {
            while ((node = neo4j_fetch_next(buffer)) != NULL) {
                for (size_t i = 0; i < neo4j_nfields(buffer); i++)
                    neo4j_tostring(neo4j_result_field(node, i), array[i], sizeof(array[i]));
                printf("%s\t%s\t%s\t%s\n", trimStr(array[0]), array[1], array[2], array[3]);

            }
        }
        neo4j_close_results(buffer);
    }
    return (EXIT_SUCCESS);
}

/**
 * \brief   Function to insert all elements from a Databse to another.
 */
int migrateDatabase()
{
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;  

    if (conv == MY_SQL && mysql) {
        if (mysql_query(mysql, SQLHIGHSCORE) == 0 && (result = mysql_store_result(mysql)) != NULL) {
            while ((row = mysql_fetch_row(result)) != NULL)
                insertHighscore(row[0], atoi(row[1]), atoi(row[2]), atoi(row[3]));
        }
        mysql_free_result(result);
        if (mysql_query(mysql, SQLEVENT) == 0 && (result = mysql_store_result(mysql)) != NULL) {
            while ((row = mysql_fetch_row(result)) != NULL)
                insertEvent(row[0], atoi(row[1]), row[2], atoi(row[3]));
        }
        mysql_free_result(result);
    }
    if (conv == SQLITE && sqlite) {
        sqlite3_exec(sqlite, SQLHIGHSCORE, callbackHighscore, 0, NULL);
        sqlite3_exec(sqlite, SQLEVENT, callbackEvent, 0, NULL);
    }
    if (conv == MONGODB && mongodb && highscore && event) {
        callbackHighScoreMongo();
        callbackEventMongo();
    }
    return (EXIT_SUCCESS);
}

/**
 * \brief   Populate a database with a false game of punto.
 * \param   size the size of the board (6 by default).
 * \param   scale the scale of the board.
 * \warning Let scale at 1, you can't scroll down so you will be stuck because you can't see the full board.
 * \warning It use rng to do so and the data might be a bit erratic
 */
int fillPuntoDatabase(size_t size, size_t scale)
{
    char *player[4] = {"J1", "J2", "J3", "J4"};
    char *color[4][2] = {{"Red", "Yellow"}, {"Cyan", "Green"}, {"Green", "Cyan"}, {"Yellow", "Red"}};
    size_t max = getNaturalRNG(((size == 2) ? 5 : 4) * scale * size, 6 * scale * 1.5 * size);
    size_t score = (size == 2) ? getNaturalRNG(1+2+3+4+5, 5+6+7+8+9) : getNaturalRNG(1+2+3+4, 6+7+8+9);
    int current = -1;
    char action[100];

    printf("Generating %zu move.\n", max);
    for (size_t i = 0; i < max; i++) {
        current = (current == (int)(size - 1)) ? 0 : current + 1;
        sprintf(action, "insert %zu %s card at {y:%zu, x:%zu}", getNaturalRNG(1, 9), color[current][(size == 2) ? 0 : getNaturalRNG(0, 1)], getNaturalRNG(0, 6), getNaturalRNG(0, 6));
        insertEvent(player[current], i+1, action, false);
    }
    sprintf(action, "Win with the score of %zu !", score);
    insertEvent(player[current], max, action, true);
    insertHighscore(player[current], max/size, max, score);
    return (EXIT_SUCCESS);
}

/**
 * \brief   Connect to the database asked by the user prompt
 * \param   dbName The prompt for the database that will elements inserted into.
 * \param   dbConv The prompt for the database that will elements taken from.
 */
void setDataBase(char *dbName, char *dbConv)
{
    type = DB(dbName);
    conv = DB(dbConv);
    
    if ((type == MY_SQL || conv == MY_SQL) && (mysql = mysql_init(mysql)) != NULL && mysql_real_connect(mysql, "127.0.1.1", "admin", "secret", "Punto", 21002, NULL, 0))
        printf("mysql connected !\n");
    if ((type == SQLITE || conv == SQLITE) && sqlite3_open("data/db/punto.db", &sqlite) == SQLITE_OK)
        printf("sqlite connected !\n");
    if ((type == MONGODB || conv == MONGODB)) {
        mongoc_init();
        if ((mongodb = mongoc_client_new("mongodb://admin:secret@127.0.1.1:21000")) != NULL && (event = mongoc_client_get_collection(mongodb, "Punto", "Event")) != NULL  && (highscore = mongoc_client_get_collection(mongodb, "Punto", "Highscore")) != NULL)
            printf("mongodb connected !\n");
    }
    if (type == NEO4J || conv == NEO4J) {
        neo4j_client_init();
        if (( neo4j = neo4j_connect("neo4j://neo4j:secret4j@127.0.1.1:21005", NULL, NEO4J_INSECURE)) == NULL)
            neo4j_perror(stderr, errno, "Connection failed");
    }
}

/**
 * \brief   Setup the database driver(s).
 */
void unsetDataBase()
{
    if ((type == MY_SQL || conv == MY_SQL) && mysql)
        mysql_close(mysql);
    if ((type == SQLITE || conv == SQLITE) && sqlite)
        sqlite3_close(sqlite);
    if ((type == MONGODB || conv == MONGODB) && mongodb) {
        mongoc_collection_destroy(highscore);
        mongoc_collection_destroy(event);
        mongoc_client_destroy(mongodb);
        mongoc_cleanup();
    }
    if ((type == NEO4J || conv == NEO4J) && neo4j) {
        neo4j_close(neo4j);
        neo4j_client_cleanup();
    }
}