#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <windows.h>

#ifndef COUNTOF
#define COUNTOF(_x)         (sizeof(_x) / sizeof((_x)[0]))
#endif

void panic(char *msg);

int keyboard_init();
int keyboard_request_ctrl_key(int key);
void keyboard_get_the_triggered_key(int *buf, int *sz);

#endif /* __MAIN_H__ */
