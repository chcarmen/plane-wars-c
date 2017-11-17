/******************************************************
* File name:    game.h
* Auther:       RedMonkey
* Date:         2017/11/17
* Description:  Plane wars game source code
*****************************************************/

#ifndef __GAME_H__
#define __GAME_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WELCOME = 1,
    RUN,
    GAMEOVER,
} GAME_STATUS;

typedef struct {
    GAME_STATUS status;
    int board_size_x;
    int board_size_y;
    int plane_type_num;
    int plane_num;
    int event_count;
    int score;
} GAME_INFO_T;

typedef enum {
    SMALL = 1,
    MIDDLE,
    BIG,
} PLANE_TYPE;

typedef struct {
    PLANE_TYPE type;
    int size_x;
    int size_y;
    int max_hp;
    int speed;
} PLANE_TYPE_INFO_T;

typedef struct {
    PLANE_TYPE type;
    int hp;
    int pos_x;
    int pos_y;
} PLANE_INFO_T;

typedef enum {
    HIT = 1,
} EVENT_TYPE;

typedef struct {
    EVENT_TYPE event;
    int arg1;
    int arg2;
} EVENT_INFO_T;


void game_init(void);
void game_tick_handler(void);

int  game_set_board_size(int width, int height);
int  game_set_plane_num(int num);
int  game_set_plane_type_info(PLANE_TYPE_INFO_T * pInfo);

int  game_start(void);
int  game_stop(void);
int  game_set_event(EVENT_INFO_T * event);

GAME_INFO_T       * game_get_game_info(void);
PLANE_INFO_T      * game_get_plane_info(int index);
PLANE_TYPE_INFO_T * game_get_plane_type_info(PLANE_TYPE type);

#ifdef __cplusplus
}
#endif

#endif // __GAME_H__