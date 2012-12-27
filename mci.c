#include "main.h"

static int _mci_send(char *cmd, char *value)
{
    int ec;
    char buf[1024];

    assert(NULL != cmd);
    assert(NULL != value);

    ec = mciSendString(
            cmd,                /* an MCI command string */
            buf,                /* a buffer that receives return information */
            COUNTOF(buf) - 1,   /* size of the return buffer */
            0);                 /* Handle to a callback window */
    if (ec != 0)
    {
        mciGetErrorString(ec, buf, COUNTOF(buf) - 1);
    }

    strcpy(value, buf);

#ifdef DEBUG
    printf("%s, line:%d: cmd='%s'\n", __FUNCTION__, __LINE__, cmd);
    printf("%s, line:%d: ec=%d, value='%s'\n", __FUNCTION__, __LINE__, ec, value);
#endif /* DEBUG */

    return ec;
}

static int _mci_mode(MciNode *pNode, char *value)
{
    char cmd[1024];

    if (NULL == pNode)
    {
        assert(0);
        return 1;
    }

    assert(NULL != value);

    /* status */
    sprintf(cmd, "status %s mode", pNode->m_szAlias);
    _mci_send(cmd, value);

    return 0;
}

MciNode *mci_open(char *filename)
{
    MciNode *pNode = NULL;
    char cmd[1024];
    char value[1024];

    assert(NULL != filename);

    /* Allocate a node structure. */
    pNode = (MciNode *)malloc(sizeof(*pNode));
    if (NULL == pNode)
    {
        panic("mci_open: malloc failed!");
        return NULL;
    }

    memset(pNode, 0, sizeof(*pNode));

    strcpy(pNode->m_szFileName, filename);
    sprintf(pNode->m_szAlias, "mp3_%d", generate_random_number());

    /* open */
    sprintf(cmd, "open \"%s\" alias %s", pNode->m_szFileName, pNode->m_szAlias);
    _mci_send(cmd, value);

    /* set */
    sprintf(cmd, "set %s time format milliseconds", pNode->m_szAlias);
    _mci_send(cmd, value);

    /* status */
    sprintf(cmd, "status %s length", pNode->m_szAlias);
    _mci_send(cmd, value);
    pNode->m_nLengthMs = atoi(value);

    return pNode;
}

/**
 * Set the volume between 0 and 100.
 */
void mci_set_volume(MciNode *pNode, int level)
{
    char cmd[1024];
    char value[1024];

    if (NULL == pNode)
    {
        assert(0);
        return;
    }

    /* setaudio */
    sprintf(cmd, "setaudio %s volume to %d", pNode->m_szAlias, level * 10);
    _mci_send(cmd, value);
}

void mci_play(MciNode *pNode)
{
    char cmd[1024];
    char value[1024];

    if (NULL == pNode)
    {
        assert(0);
        return;
    }

    /* play */
    sprintf(cmd, "play %s from %d to %d", pNode->m_szAlias, 0, pNode->m_nLengthMs);
    _mci_send(cmd, value);
}

void mci_close(MciNode *pNode)
{
    char cmd[1024];
    char value[1024];

    if (NULL == pNode)
    {
        assert(0);
        return;
    }

    /* play */
    sprintf(cmd, "close %s", pNode->m_szAlias);
    _mci_send(cmd, value);
}

int mci_is_playing(MciNode *pNode)
{
    char value[1024];

    if (NULL == pNode)
    {
        assert(0);
        return 0;
    }

    _mci_mode(pNode, value);

    if (0 == strcmp(value, "playing"))
    {
        return 1;
    }

    return 0;
}

int mci_is_paused(MciNode *pNode)
{
    char value[1024];

    if (NULL == pNode)
    {
        assert(0);
        return 0;
    }

    _mci_mode(pNode, value);

    if (0 == strcmp(value, "paused"))
    {
        return 1;
    }

    return 0;
}

void mci_pause(MciNode *pNode)
{
    char cmd[1024];
    char value[1024];

    if (NULL == pNode)
    {
        assert(0);
        return;
    }

    /* pause */
    sprintf(cmd, "pause %s", pNode->m_szAlias);
    _mci_send(cmd, value);
}

void mci_resume(MciNode *pNode)
{
    char cmd[1024];
    char value[1024];

    if (NULL == pNode)
    {
        assert(0);
        return;
    }

    /* resume */
    sprintf(cmd, "resume %s", pNode->m_szAlias);
    _mci_send(cmd, value);
}

void mci_stop(MciNode *pNode)
{
    char cmd[1024];
    char value[1024];

    if (NULL == pNode)
    {
        assert(0);
        return;
    }

    /* stop */
    sprintf(cmd, "stop %s", pNode->m_szAlias);
    _mci_send(cmd, value);

    /* seek */
    sprintf(cmd, "seek %s to start", pNode->m_szAlias);
    _mci_send(cmd, value);
}
