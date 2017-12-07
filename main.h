#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>
#include <windows.h>

#include "list.h"

#ifndef COUNTOF
#define COUNTOF(_x)         (sizeof(_x) / sizeof((_x)[0]))
#endif /* COUNTOF */

#ifndef SEEK_OFFSET
#define SEEK_OFFSET         (5 * 1000)
#endif /* SEEK_OFFSET */

#ifndef SEEK_OFFSET_MINUTES
#define SEEK_OFFSET_MINUTES (60 * 1000)
#endif /* SEEK_OFFSET_MINUTES */

#ifndef VOLUME_OFFSET
#define VOLUME_OFFSET       100
#endif /* VOLUME_OFFSET */

#define INTERVAL_FRAGMENT   250
#define INTERVAL_AUDIO_FILE 2000

#define STATUS_PLAYING      0x1
#define STATUS_PAUSED       0x2
#define STATUS_STOPPED      0x4

#define BUFFER_SIZE         (1 << 10)

typedef struct
{
    char    m_szFileName[BUFFER_SIZE];
    char    m_szAlias[32];
    int     m_nLengthMs;
} MciNode;

void panic(char *msg);
int generate_random_number();
char* trim(char* input);

int keyboard_init();
int keyboard_request_ctrl_key(int key);
void keyboard_get_the_triggered_key(int *buf, int *sz);

void playlist_add(char *filename, int recursive);
void playlist_add_from_list(char *filename);
int playlist_is_empty();
void playlist_init();
void playlist_deinit();
char *playlist_get_next();
void playlist_shuffle();
void playlist_random();

MciNode *mci_open(char *filename);
int mci_position(MciNode *pNode);
void mci_set_volume(MciNode *pNode, int level);
int mci_get_volume(MciNode *pNode);
void mci_play(MciNode *pNode, int startms);
void mci_close(MciNode *pNode);
int mci_is_playing(MciNode *pNode);
int mci_is_paused(MciNode *pNode);
int mci_is_stopped(MciNode *pNode);
void mci_pause(MciNode *pNode);
void mci_resume(MciNode *pNode);
void mci_stop(MciNode *pNode);

#endif /* __MAIN_H__ */
