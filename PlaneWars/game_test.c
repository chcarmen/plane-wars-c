#include "game.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
static  UTIL_Sleep(int time_ms)
{
    Sleep(time_ms);
}
#else
static  UTIL_Sleep(int time_ms)
{
    usleep(time_ms * 1000);
}
#endif

static void print_game_info(void)
{
    GAME_INFO_T * game_info = NULL;
    
    game_info = game_get_game_info();
    if (!game_info) {
        printf("game_get_game_info failed\n");
        return;
    }

    printf("\n********** game_info **********\n");
    printf("status: %d\n", game_info->status);
    printf("board_size_x: %d\n", game_info->board_size_x);
    printf("board_size_y: %d\n", game_info->board_size_y);
    printf("plane_type_num: %d\n", game_info->plane_type_num);
    printf("plane_num: %d\n", game_info->plane_num);
    printf("event_count: %d\n", game_info->event_count);
    printf("score: %d\n", game_info->score);
    printf("********** game_info **********\n\n");
}

static void print_plane_type_info(void)
{
    PLANE_TYPE_INFO_T * plane_type = NULL;
    int i = 0;
 
    printf("\n********** plane_type_info **********\n");
    printf("type\tsize_x\tsize_y\tmax_hp\tspeed\n");
    for (i=0; i<10; i++) {
        plane_type = game_get_plane_type_info(i);
        if (plane_type) {
            printf("%d\t%d\t%d\t%d\t%d\n", 
                plane_type->type,
                plane_type->size_x,
                plane_type->size_y,
                plane_type->max_hp,
                plane_type->speed);
        }
    }
    printf("********** plane_type_info **********\n\n");
}

static void print_plane_info(void)
{
    PLANE_INFO_T * plane = NULL;
    PLANE_TYPE_INFO_T * plane_type = NULL;
    int i = 0;

    printf("\n********** plane_info **********\n");
    printf("index\ttype\tpos_x1\tpos_x2\tpos_y1\tpos_y2\tmax_hp\thp\tspeed\n");
    for (i=0; i<20; i++) {
        plane = game_get_plane_info(i);
        if (!plane)
            continue;
        plane_type = game_get_plane_type_info(plane->type);
        if (!plane_type)
            continue;

        if (plane) {
            printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                i,
                plane->type,
                plane->pos_x,
                plane->pos_x + plane_type->size_x,
                plane->pos_y,
                plane->pos_y + plane_type->size_y,
                plane_type->max_hp,
                plane->hp,
                plane_type->speed);
        }
    }
    printf("********** plane_info **********\n\n");
}

static void simu_hit(void)
{
    static int n = 0;
    int ret;
    int i = 0;
    int max_x = 0;
    int max_y = 0;
    PLANE_INFO_T * plane = NULL;
    PLANE_TYPE_INFO_T * plane_type = NULL;

    n++;

    if ((n % 10) != 0)
        return;

    for (i=0; i<20; i++) {
        plane = game_get_plane_info(i);
        if (!plane)
            continue;
        plane_type = game_get_plane_type_info(plane->type);
        if (!plane_type)
            continue;
        if (plane->pos_y + plane_type->size_y > max_y) {
            max_x = plane->pos_x + plane_type->size_x;
            max_y = plane->pos_y + plane_type->size_y;
        }
    }

    EVENT_INFO_T event;
    event.event = HIT;
    event.arg1 = max_x - 10;
    event.arg2 = max_y - 20;
    ret = game_set_event(&event);
    if (ret < 0) {
        printf("game_set_event failed\n");
        return;
    }
    printf("hit point (%d, %d)\n", event.arg1, event.arg2);
}

int main(int argc, char * argv[])
{
    int ret;

    game_init();

    ret = game_set_board_size(480, 640);
    if (ret < 0) {
        printf("game_set_board_size failed\n");
        goto main_exit;
    }

    ret = game_set_plane_num(10);
    if (ret < 0) {
        printf("game_set_plane_num failed\n");
        goto main_exit;
    }

    PLANE_TYPE_INFO_T plane_type;
    plane_type.type = SMALL;
    plane_type.size_x = 40;
    plane_type.size_y = 35;
    plane_type.max_hp = 1;
    plane_type.speed = 2;
    ret = game_set_plane_type_info(&plane_type);
    if (ret < 0) {
        printf("game_set_plane_type_info failed\n");
        goto main_exit;
    }

    plane_type.type = MIDDLE;
    plane_type.size_x = 80;
    plane_type.size_y = 70;
    plane_type.max_hp = 2;
    plane_type.speed = 1;
    ret = game_set_plane_type_info(&plane_type);
    if (ret < 0) {
        printf("game_set_plane_type_info failed\n");
        goto main_exit;
    }

    plane_type.type = BIG;
    plane_type.size_x = 120;
    plane_type.size_y = 105;
    plane_type.max_hp = 3;
    plane_type.speed = 1;
    ret = game_set_plane_type_info(&plane_type);
    if (ret < 0) {
        printf("game_set_plane_type_info failed\n");
        goto main_exit;
    }

    printf("=== GAME INITED ===\n");

    print_plane_type_info();
    print_game_info();   
    print_plane_info();

    ret = game_start();
    if (ret < 0) {
        printf("game_start failed\n");
        goto main_exit;
    }

    printf("=== GAME STARTED ===\n");

    GAME_INFO_T * game;

    do {
        game_tick_handler();
        
        print_game_info();
        print_plane_info();

        simu_hit();
        
        UTIL_Sleep(500);
        game = game_get_game_info();
    } while (game->status == RUN);

main_exit:
    system("pause");
    return ret;
}
