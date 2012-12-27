#include "main.h"

static int s_keyboard_thread_quit = 0;

static int s_request_ctrl_key_array[128];
static int s_request_ctrl_keys = 0;
static int s_triggered_key_array[1024];
static int s_triggered_keys = 0;
static CRITICAL_SECTION s_keyboard_lock;

static int _keyboard_request_has_key(int key)
{
    int i, ret = 0;

    EnterCriticalSection(&s_keyboard_lock);

    for (i = 0; i < s_request_ctrl_keys; i++)
    {
        if (s_request_ctrl_key_array[i] == key)
        {
            ret = 1;
            goto Exit;
        }
    }

Exit:
    LeaveCriticalSection(&s_keyboard_lock);

    return ret;
}

static void _keyboard_key_event_proc(KEY_EVENT_RECORD ker)
{
    int key;

    if ((ker.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) && ker.bKeyDown)
    {
        key = ker.wVirtualKeyCode;

        if (key == 'C')
        {
            s_keyboard_thread_quit = 1;
        }

        EnterCriticalSection(&s_keyboard_lock);

        if (_keyboard_request_has_key(key) && (s_triggered_keys < COUNTOF(s_triggered_key_array)))
        {
            s_triggered_key_array[s_triggered_keys++] = key;
        }

        LeaveCriticalSection(&s_keyboard_lock);
    }
}

static DWORD WINAPI _keyboard_thread_proc(LPVOID arg)
{
    HANDLE hStdin;
    DWORD cNumRead, fdwMode, fdwSaveOldMode, i;
    INPUT_RECORD irInBuf[256];

    /* Get the standard input handle. */
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
    {
        panic("GetStdHandle");
    }

    /* Save the current input mode, to be restored on exit. */
    if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
    {
        panic("GetConsoleMode");
    }

    /* Enable the window input events. */
    fdwMode = ENABLE_WINDOW_INPUT;
    if (!SetConsoleMode(hStdin, fdwMode))
    {
        panic("SetConsoleMode");
    }

    /* Loop to read and handle the input events. */
    while (1)
    {
        /* Wait for the events. */
        if (!ReadConsoleInput(
                    hStdin,             /* input buffer handle */
                    irInBuf,            /* buffer to read into */
                    COUNTOF(irInBuf),   /* size of read buffer */
                    &cNumRead))         /* number of records read */
        {
            panic("ReadConsoleInput");
        }

        /* Dispatch the events to the appropriate handler. */
        for (i = 0; i < cNumRead; i++)
        {
            switch (irInBuf[i].EventType)
            {
                case KEY_EVENT: /* keyboard input */
                    _keyboard_key_event_proc(irInBuf[i].Event.KeyEvent);
                    break;

                case MOUSE_EVENT:
                case WINDOW_BUFFER_SIZE_EVENT:
                case FOCUS_EVENT:
                case MENU_EVENT:
                default:
                    break;
            }
        }

        if (s_keyboard_thread_quit)
        {
            break;
        }
    }

    SetConsoleMode(hStdin, fdwSaveOldMode);

    return 0;
}

int keyboard_init()
{
    static HANDLE s_hThread = NULL;

    assert(NULL == s_hThread);

    /* Initialize the request ctrl key array. */
    memset(s_request_ctrl_key_array, 0, sizeof(s_request_ctrl_key_array));
    memset(s_triggered_key_array, 0, sizeof(s_triggered_key_array));
    InitializeCriticalSection(&s_keyboard_lock);

    /* Create the keyboard monitoring thread. */
    s_hThread = CreateThread(NULL, 0, _keyboard_thread_proc, NULL, 0, NULL);
    if (NULL == s_hThread)
    {
        return 1;
    }

    return 0;
}

int keyboard_request_ctrl_key(int key)
{
    int ret = 1;

    EnterCriticalSection(&s_keyboard_lock);

    if (_keyboard_request_has_key(key))
    {
        goto Exit;
    }

    if (s_request_ctrl_keys >= COUNTOF(s_request_ctrl_key_array))
    {
        goto Exit;
    }

    s_request_ctrl_key_array[s_request_ctrl_keys++] = key;
    ret = 0;

Exit:
    LeaveCriticalSection(&s_keyboard_lock);
    return ret;
}

void keyboard_get_the_triggered_key(
        /* [in] */ int *buf,
        /* [in, out] */ int *sz)
{
    assert(*sz >= COUNTOF(s_triggered_key_array));

    EnterCriticalSection(&s_keyboard_lock);

    *sz = s_triggered_keys;
    memcpy(buf, s_triggered_key_array, (*sz) * sizeof(int));

    s_triggered_keys = 0;

    LeaveCriticalSection(&s_keyboard_lock);
}
