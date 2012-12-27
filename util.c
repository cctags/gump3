#include "main.h"

static int _is_space(char c)
{
    switch (c)
    {
        case ' ':
        case '\n':
        case '\t':
        case '\f':
        case '\r':
            return 1;

        default:
            return 0;
    }
}

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

char* trim(char* input)
{
    char *start = input;
    char *ptr, *end;

    /* trim left */
    while (_is_space(*start))
    {
        start++;
    }

    /* totally empty */
    if (0 == *start)
    {
        start = NULL;
        goto Exit;
    }

    /* trim right */
    ptr = start;
    end = start;

    while (*ptr != 0)
    {
        /* only move end pointer if char isn't a space */
        if (!_is_space(*ptr))
        {
            end = ptr;
        }

        ptr++;
    }

    if (end == start)
    {
        start = NULL;
        goto Exit;
    }

    /* terminate the trimmed string with a null */
    *(++end) = 0;

Exit:
    return start;
}
