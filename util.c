#include "main.h"

void panic(char *msg)
{
#ifdef DEBUG
    fprintf(stderr, "** FATAL ERROR **: %s\n", msg);
#endif
    exit(1);
}

int generate_random_number()
{
    static int s_srand_called = 0;

    /* Seed the random-number generator with the current time so that
     * the numbers will be different every time we run.
     */
    if (0 == s_srand_called)
    {
        s_srand_called = 1;
        srand((unsigned)time(NULL));
    }

    return ((double)rand()) / (RAND_MAX + 1) * 100000000;
}
