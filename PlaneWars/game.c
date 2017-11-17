/******************************************************
* File name:    game.c
* Auther:       RedMonkey
* Date:         2017/11/17
* Description:  Plane wars game source code
*****************************************************/

#include "game.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_PLANE_NUM       (20)
#define MAX_PLANE_TYPE_NUM  (3)
#define MAX_EVENT_NUM       (1)

static GAME_INFO_T g_tGame;
static PLANE_TYPE_INFO_T g_tPlaneTypes[MAX_PLANE_TYPE_NUM];
static PLANE_INFO_T g_tPlanes[MAX_PLANE_NUM];
static EVENT_INFO_T g_tEvents[MAX_EVENT_NUM];

//////////////////////////////////////////////////////////////////////

// TODO: to make it callback to allow user defined game policy
static void init_plane(PLANE_INFO_T * plane)
{
    int i = 0;
    int r = 0;
    int big_plane_cnt = 0;
    PLANE_TYPE_INFO_T * plane_type = NULL;

    do {
        r = rand() % 3;
        switch (r) {
        case 2:
            plane->type = BIG;
            break;
        case 1:
            plane->type = MIDDLE;
            break;
        case 0:
        default:
            plane->type = SMALL;
            break;
        }

        // only one BIG plane is permitted
        big_plane_cnt = 0;
        for (i=0; i<g_tGame.plane_num; i++) {
            if (g_tPlanes[i].type == BIG)
                big_plane_cnt++;
        }
        if (big_plane_cnt == 1)
            break;
    } while (1);

    plane_type = game_get_plane_type_info(plane->type);
    assert(plane_type != NULL);

    assert(g_tGame.board_size_x - plane_type->size_x != 0);
    assert(g_tGame.board_size_y / 3 != 0);
    plane->pos_x = rand() % (g_tGame.board_size_x - plane_type->size_x);
    plane->pos_y = 0 - plane_type->size_y - (rand() % (g_tGame.board_size_y / 3));

    plane->hp = plane_type->max_hp;
}

static int get_hit_plane_index(int hit_pos_x, int hit_pos_y)
{
    int i = 0;
    PLANE_TYPE_INFO_T * plane_type = NULL;

    if (hit_pos_x < 0 || hit_pos_y < 0)
        return -1;

    for (i=0; i<g_tGame.plane_num; i++) {
        plane_type = game_get_plane_type_info(g_tPlanes[i].type);
        assert(plane_type != NULL);

        if (hit_pos_x > g_tPlanes[i].pos_x 
            && hit_pos_x < g_tPlanes[i].pos_x + plane_type->size_x
            && hit_pos_y > g_tPlanes[i].pos_y 
            && hit_pos_y < g_tPlanes[i].pos_y + plane_type->size_y)
            return i;
    }
    
    return -1;
}

static void process_events(void)
{
    int i = 0;
    int hit_pos_x;
    int hit_pos_y;
    int hit_index;

    for (i=0; i<g_tGame.plane_num; i++) {
        if (g_tPlanes[i].hp == 0) {
            g_tGame.score++;
            init_plane(&g_tPlanes[i]);
            continue;
        }
    }

    if (g_tGame.event_count <= 0)
        return;

    for (i=0; i<g_tGame.event_count; i++) {
        switch (g_tEvents[i].event) {
        case HIT:
            hit_pos_x = g_tEvents[i].arg1;
            hit_pos_y = g_tEvents[i].arg2;

            hit_index = get_hit_plane_index(hit_pos_x, hit_pos_y);
            if (hit_index < 0)
                break;
            g_tPlanes[hit_index].hp--;
            break;
        default:
            break;
        }
    }

    g_tGame.event_count = 0;
}

static void move_planes_regularly(void)
{
    int i = 0;
    PLANE_TYPE_INFO_T * plane_type = NULL;

    for (i=0; i<g_tGame.plane_num; i++) {
        plane_type = game_get_plane_type_info(g_tPlanes[i].type);
        assert(plane_type != NULL);

        g_tPlanes[i].pos_y += plane_type->speed;
    }
}

static void check_game_over(void)
{
    int i = 0;
    PLANE_TYPE_INFO_T * plane_type = NULL;

    for (i=0; i<g_tGame.plane_num; i++) {
        plane_type = game_get_plane_type_info(g_tPlanes[i].type);
        assert(plane_type != NULL);

        if (g_tPlanes[i].pos_y + plane_type->size_y > g_tGame.board_size_y) {
            g_tGame.status = GAMEOVER;
        }
    }
}

//////////////////////////////////////////////////////////////////////
void game_init(void)
{
    int i = 0;

    // reset all
    memset(&g_tGame, 0, sizeof(g_tGame));
    memset(&g_tPlaneTypes, 0, sizeof(g_tPlaneTypes));
    memset(&g_tPlanes, 0, sizeof(g_tPlanes));
    memset(&g_tEvents, 0, sizeof(g_tEvents));
    
    // set random seed
    srand((int)time(NULL));
    
    // init game status
    g_tGame.status = WELCOME;
}

void game_tick_handler(void)
{
    if (g_tGame.status != RUN)
        return;

    // process events from ui
    process_events();

    // game over if plane reaches the bottom
    check_game_over();

    // move all planes regularly
    move_planes_regularly();
}

int  game_set_board_size(int width, int height)
{
    if (g_tGame.status != WELCOME)
        return -1;

    g_tGame.board_size_x = width;
    g_tGame.board_size_y = height;
    return 0;
}

int  game_set_plane_num(int num)
{
    if (g_tGame.status != WELCOME)
        return -1;

    if (num > MAX_PLANE_NUM)
        return -1;

    g_tGame.plane_num = num;
    return 0;
}

int  game_set_plane_type_info(PLANE_TYPE_INFO_T * pInfo)
{
    int i = 0;

    if (g_tGame.status != WELCOME)
        return -1;

    if (!pInfo)
        return -1;

    // find exist type
    for (i=0; i<g_tGame.plane_type_num; i++) {
        if (g_tPlaneTypes[i].type == pInfo->type) {
            g_tPlaneTypes[i] = *pInfo;
            return 0;
        }
    }

    // type array full
    if (g_tGame.plane_type_num >= MAX_PLANE_TYPE_NUM)
        return -1;

    // save new type
    g_tPlaneTypes[i] = *pInfo;
    g_tGame.plane_type_num++;
    return 0;
}

int  game_start(void)
{
    int i = 0;

    if (g_tGame.status != WELCOME)
        return -1;

    if (g_tGame.board_size_x <= 0 
        || g_tGame.board_size_y <= 0 
        || g_tGame.plane_type_num <= 0
        || g_tGame.plane_num <= 0)
        return -1;

    for (i=0; i<g_tGame.plane_type_num; i++) {
        if (g_tPlaneTypes[i].size_x <= 0 
            || g_tPlaneTypes[i].size_y <= 0 
            || g_tPlaneTypes[i].speed <= 0
            || g_tPlaneTypes[i].size_x >= g_tGame.board_size_x
            || g_tPlaneTypes[i].size_y >= g_tGame.board_size_y)
            return -1;
    }

    // init plane position
    for (i=0; i<g_tGame.plane_num; i++) {
        init_plane(&g_tPlanes[i]);
    }
    
    g_tGame.score  = 0;
    g_tGame.status = RUN;
    return 0;
}

int  game_stop(void)
{
    g_tGame.status = GAMEOVER;
    return 0;
}

int  game_set_event(EVENT_INFO_T * event)
{
    if (g_tGame.status != RUN)
        return -1;

    if (!event)
        return -1;

    if (g_tGame.event_count >= MAX_EVENT_NUM)
        return -1;

    g_tEvents[g_tGame.event_count] = *event;
    
    g_tGame.event_count++;
    return 0;
}

GAME_INFO_T * game_get_game_info(void)
{
    return &g_tGame;
}

PLANE_INFO_T * game_get_plane_info(int index)
{
    if (index >= g_tGame.plane_num)
        return NULL;

    return &g_tPlanes[index];
}

PLANE_TYPE_INFO_T * game_get_plane_type_info(PLANE_TYPE type)
{
    int i = 0;

    for (i=0; i<g_tGame.plane_type_num; i++) {
        if (g_tPlaneTypes[i].type == type)
            return &g_tPlaneTypes[i];
    }

    return NULL;
}
