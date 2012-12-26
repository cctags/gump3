#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

/**
 * Global options.
 */
static int s_nPrintPlayList = 0;
static int s_nRecursiveSearch = 0;
static int s_nShuffle = 0;
static int s_nRandom = 0;
static int s_nPrintHelp = 0;
static char *s_szPlayList = NULL;

static char *s_version_info = \
    "Gump3 audio player. Version 0.4 (December, 2012).\n" \
    "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! USE AT YOUR OWN RISK!\n\n";

static char *s_usage_info = \
    "Usage:\n" \
    "   gump3 [option(s)] [file(s)]\n" \
    "\n" \
    "Supported options [defaults in brackets]:\n" \
    "   -@ f read filenames from f\n" \
    "   -c   disable control keys [default enabled]\n" \
    "   -m   show what is playing now in MSN messenger [default disabled]\n" \
    "   -q   quiet (dont't print title)\n" \
    "   -p   print the audio file(s)\n" \
    "   -s   search for files in the specified directory and all sub-directories\n" \
    "   -z   shuffle play\n" \
    "   -Z   random play\n" \
    "   -h   print help message\n" \
    "\n" \
    "Control keys:\n" \
    "   C-P   play / pause              C-C   stop\n" \
    "   C-N   play next audio file\n" \
    "   C-F   foward 5 seconds          C-B   backward 5 seconds\n" \
    "   C-L   turn down the volume      C-H   turn up the volume\n";

static void print_version_info()
{
    printf(s_version_info);
}

static void print_usage_info()
{
    printf(s_usage_info);
}

/**
 * Init the parameters.
 *
 * \return
 *      0 - OK
 *      1 - error
 */
static int init_parameters(int argc, char * const argv[])
{
    int ch;
    int i;

    optind = 1;
    while ((ch = getopt(argc, argv, ":cmqpszZh@:")) != -1)
    {
        switch (ch)
        {
            case 'c':
            case 'm':
            case 'q':
                /* TODO: not supported now. */
                break;

            case 'p': s_nPrintPlayList = 1;     break;
            case 's': s_nRecursiveSearch = 1;   break;
            case 'z': s_nShuffle = 1;           break;
            case 'Z': s_nRandom = 1;            break;
            case 'h': s_nPrintHelp = 1;         break;
            case '@': s_szPlayList = optarg;    break;

            default: break;
        }
    }

#ifdef DEBUG
    printf("%s, line:%d: s_nPrintPlayList=%d.\n", __FUNCTION__, __LINE__, s_nPrintPlayList);
    printf("%s, line:%d: s_nRecursiveSearch=%d.\n", __FUNCTION__, __LINE__, s_nRecursiveSearch);
    printf("%s, line:%d: s_nShuffle=%d.\n", __FUNCTION__, __LINE__, s_nShuffle);
    printf("%s, line:%d: s_nRandom=%d.\n", __FUNCTION__, __LINE__, s_nRandom);
    printf("%s, line:%d: s_nPrintHelp=%d.\n", __FUNCTION__, __LINE__, s_nPrintHelp);
    printf("%s, line:%d: s_szPlayList='%s'\n", __FUNCTION__, __LINE__, s_szPlayList);
#endif

    for (i = optind; i < argc; i++)
    {
        printf("%s, line:%d: [%d]: '%s'\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    return 0;
}

int main(int argc, char * const argv[])
{
    int ret = 0;

    print_version_info();

    ret = init_parameters(argc, argv);
    if (0 != ret)
    {
        print_usage_info();
        ret = 1;
        goto Exit;
    }

    if (0 != s_nPrintHelp)
    {
        print_usage_info();
        goto Exit;
    }

Exit:
    return ret;
}
