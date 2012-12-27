#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>
#include <windows.h>

#ifndef COUNTOF
#define COUNTOF(_x)         (sizeof(_x) / sizeof((_x)[0]))
#endif

typedef struct
{
    char    m_szFileName[1024];
    char    m_szAlias[32];
    int     m_nLengthMs;
} MciNode;

void panic(char *msg);
int generate_random_number();

int keyboard_init();
int keyboard_request_ctrl_key(int key);
void keyboard_get_the_triggered_key(int *buf, int *sz);

#endif /* __MAIN_H__ */
