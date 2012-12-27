#include "main.h"

/**
 * Global options.
 */
static int s_nPrintPlayList = 0;
static int s_nRecursiveSearch = 0;
static int s_nShuffle = 0;
static int s_nRandom = 0;
static int s_nPrintHelp = 0;
static char *s_szPlayList = NULL;

/**
 * Control options.
 */
static int control_flag_next = 0;
static int control_flag_play_or_pause = 0;
static int control_flag_forward = 0;
static int control_flag_backward = 0;

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

static void _print_version_info()
{
    printf(s_version_info);
}

static void _print_usage_info()
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
static int _init_parameters(int argc, char * const argv[])
{
    int ch;

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

    return 0;
}

static int _keyboard_handler()
{
    int buf[1024];
    int i, sz = COUNTOF(buf);

    keyboard_get_the_triggered_key(buf, &sz);

    for (i = 0; i < sz; i++)
    {
        switch (buf[i])
        {
            case 'P': control_flag_play_or_pause = 1;   break;
            case 'N': control_flag_next = 1;            break;
            case 'F': control_flag_forward = 1;         break;
            case 'B': control_flag_backward = 1;        break;
            case 'C': return 1;                         break;
            default:                                    break;
        }
    }

    return 0;
}

static void _update_prompt_string(int status, int position, int length)
{
    static char *s_str[] =
    {
        "[|]       ",
        "[/]       ",
        "[-]       ",
        "[\\]       ",
        "[Pause   ]",
        "[Pause.  ]",
        "[Pause.. ]",
        "[Pause...]",
    };
    static int s_index = 0;

    int index = 0;

    char buffer[256];

    assert(length >= 0);
    assert(position >= 0);

    if (position > length)
    {
        assert(0);
        position = length;
    }

    position = position / 1000 + 1;
    length = length / 1000 + 1;

    index = (STATUS_PLAYING == status) ? (s_index / 5) : (4 + s_index / 5);
    s_index = (s_index + 1) % (4 * 5);

    sprintf(buffer, "--> %d:%02d:%02d / %d:%02d:%02d [%02d%%], %s",
            position / 3600, (position % 3600) / 60, (position % 3600) % 60,
            length / 3600, (length % 3600) / 60, (length % 3600) % 60,
            (int)((double)position * 100 / length), s_str[index]);
    printf("\r%s", buffer);
}

int main(int argc, char * const argv[])
{
    int i;
    char *filename;
    MciNode *p;
    int ret = 0;
    int quit_flag = 0;

    _print_version_info();

    /* Parse the parameters. */
    ret = _init_parameters(argc, argv);
    if (0 != ret)
    {
        _print_usage_info();
        ret = 1;
        goto Exit;
    }

    if (s_nPrintHelp)
    {
        _print_usage_info();
        goto Exit;
    }

    /* Init the playlist. */
    playlist_init();

    for (i = optind; i < argc; i++)
    {
        playlist_add(argv[i], 1);
    }

    if (NULL != s_szPlayList)
    {
        playlist_add_from_list(s_szPlayList);
    }

    /* Output the playlist. */
    if (s_nPrintPlayList)
    {
        while (NULL != (filename = playlist_get_next()))
        {
            printf("%s\n", filename);
        }
        goto Exit;
    }

    /* No audio files found. */
    if (playlist_is_empty())
    {
        _print_usage_info();
        ret = 1;
        goto Exit;
    }

    /* Set the play mode. */
    if (s_nShuffle)
    {
        playlist_shuffle();
    }
    else if (s_nRandom)
    {
        playlist_random();
    }

    /* Init the keyboard and add the request keys. */
    keyboard_init();
    keyboard_request_ctrl_key('P');
    keyboard_request_ctrl_key('C');
    keyboard_request_ctrl_key('N');
    keyboard_request_ctrl_key('F');
    keyboard_request_ctrl_key('B');
    keyboard_request_ctrl_key('L');
    keyboard_request_ctrl_key('H');

    /* Loop the playlist. */
    while (1)
    {
        int due_time, current_time, position = 0;
        int status = STATUS_STOPPED;

        filename = playlist_get_next();

        if (NULL == filename)
        {
            break;
        }

        p = mci_open(filename);
        if (NULL == p)
        {
            goto Continue;
        }

        current_time = GetTickCount();
        due_time = current_time + p->m_nLengthMs;

        mci_play(p, 0);

        printf("\n\n>>> %s\n", filename);
        status = STATUS_PLAYING;

        while (1)
        {
            if (STATUS_PAUSED == status)
            {
                /* play. */
                if (control_flag_play_or_pause)
                {
                    current_time = GetTickCount();
                    due_time = p->m_nLengthMs - position + current_time;

                    mci_resume(p);

                    status = STATUS_PLAYING;
                    control_flag_play_or_pause = 0;
                }

                _update_prompt_string(STATUS_PAUSED, position, p->m_nLengthMs);

                goto NextFragment;
            }

            /* pause. */
            if (control_flag_play_or_pause)
            {
                position = mci_position(p);

                mci_pause(p);

                status = STATUS_PAUSED;
                control_flag_play_or_pause = 0;

                goto NextFragment;
            }

            /* next. */
            if (control_flag_next)
            {
                control_flag_next = 0;
                goto NextOne;
            }

            /* << / >> */
            if (control_flag_forward || control_flag_backward)
            {
                if (control_flag_forward)
                {
                    if (current_time + SEEK_OFFSET >= due_time)
                    {
                        goto NextOne;
                    }

                    due_time -= SEEK_OFFSET;
                    position += SEEK_OFFSET;

                    control_flag_forward = 0;
                }
                else if (control_flag_backward)
                {
                    if (position <= SEEK_OFFSET)
                    {
                        due_time += position;
                        position = 0;
                    }
                    else
                    {
                        due_time += SEEK_OFFSET;
                        position -= SEEK_OFFSET;
                    }

                    control_flag_backward = 0;
                }

                mci_play(p, position);
            }

            current_time = GetTickCount();

            if (current_time >= due_time)
            {
                goto NextOne;
            }

            position = p->m_nLengthMs - (due_time - current_time);

            _update_prompt_string(STATUS_PLAYING, position, p->m_nLengthMs);

NextFragment:
            Sleep(PLAY_INTERVAL);

            quit_flag = _keyboard_handler();
            if (quit_flag)
            {
                goto NextOne;
            }
        }

        goto Continue;

NextOne:
        mci_stop(p);
        mci_close(p);

Continue:
        Sleep(PLAY_INTERVAL);

        if (quit_flag)
        {
            break;
        }
    }

    playlist_deinit();

Exit:
    return ret;
}
