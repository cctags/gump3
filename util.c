#include "main.h"

void panic(char *msg)
{
#ifdef DEBUG
    fprintf(stderr, "** FATAL ERROR **: %s\n", msg);
#endif
    exit(1);
}
