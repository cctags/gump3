#include "main.h"

/*
 * TODO: Add the support for the playlist to be updated at runtime
 */

enum
{
    MODE_NORMAL     = 0,
    MODE_RANDOM,
    MODE_SHUFFLE,
};

struct _PlayNode
{
    char                m_szFileName[1024];
    struct list_head    m_node;
};

typedef struct _PlayNode PlayNode;

static int s_mode = MODE_NORMAL;

static struct list_head s_list;
static int s_list_size = 0;

static struct list_head *_playlist_get_next_normal(struct list_head *list)
{
    static struct list_head *pCurrent = NULL;

    struct list_head *p = NULL;

    if (list_empty(list))
    {
        goto Exit;
    }

    pCurrent = (NULL == pCurrent) ? list->next : pCurrent->next;
    if (list != pCurrent)
    {
        p = pCurrent;
    }

Exit:
    return p;
}

static struct list_head *_playlist_get_next_random(struct list_head *list)
{
    static struct list_head *pCurrent = NULL;

    struct list_head *p = NULL;
    int n;

    if (list_empty(list))
    {
        goto Exit;
    }

    if (NULL == pCurrent)
    {
        pCurrent = list;
    }

    n = (generate_random_number() % s_list_size) + 1;

    while (n--)
    {
        pCurrent = pCurrent->next;
    }

    if (list == pCurrent)
    {
        pCurrent = pCurrent->next;
    }

    p = pCurrent;

Exit:
    return p;
}

static struct list_head *_playlist_get_next_shuffle(struct list_head *list)
{
    static struct list_head *pCurrent = NULL;

    struct list_head tmpList;
    struct list_head *p = NULL;
    int n;

    if (list_empty(list))
    {
        goto Exit;
    }

    /* Shuffle the list. */
    if (NULL == pCurrent)
    {
        INIT_LIST_HEAD(&tmpList);

        while (!list_empty(list))
        {
            n = (generate_random_number() % s_list_size) + 1;

            p = list;

            while (n--)
            {
                p = p->next;
            }

            if (p == list)
            {
                p = p->next;
            }

            assert(NULL != p);

            list_del(p);

            list_add_tail(p, &tmpList);
        }

        while (!list_empty(&tmpList))
        {
            p = tmpList.next;
            list_del(p);
            list_add_tail(p, list);
        }

        pCurrent = list;
    }

    /* Get the next one. */
    pCurrent = pCurrent->next;

    if (pCurrent == list)
    {
        pCurrent = pCurrent->next;
    }

    p = pCurrent;

Exit:
    return p;
}

static void _playlist_add_item(char *filename)
{
    PlayNode *pNode;

    assert(NULL != filename);

    pNode = (PlayNode *)malloc(sizeof(*pNode));
    if (NULL == pNode)
    {
        assert(0);
        goto Exit;
    }

    memset(pNode, 0, sizeof(*pNode));

    strcpy(pNode->m_szFileName, filename);

    list_add_tail(&pNode->m_node, &s_list);
    s_list_size++;

Exit:
    return;
}

void playlist_add(char *filename, int recursive)
{
    char szCWD[1024];
    char szFullPath[1024];
    WIN32_FIND_DATA wfd;
    HANDLE hFind;

    assert(NULL != filename);

    GetCurrentDirectory(COUNTOF(szCWD), szCWD);

    /* Loop the normal files. */
    if (NULL == strchr(filename, '*'))
    {
        if (NULL == strchr(filename, ':'))
        {
            sprintf(szFullPath, "%s\\%s", szCWD, filename);
            _playlist_add_item(szFullPath);
        }
        else
        {
            _playlist_add_item(filename);
        }

        /* We don't have to loop more files. */
        recursive = 1;
        goto Exit;
    }

    /* For the wildcard characters. */
    hFind = FindFirstFile(filename, &wfd);
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                sprintf(szFullPath, "%s\\%s", szCWD, wfd.cFileName);
                _playlist_add_item(szFullPath);
            }
        }
        while (FindNextFile(hFind, &wfd));

        FindClose(hFind);
    }

    if (!recursive)
    {
        goto Exit;
    }

    /* Loop the sub directories. */
    hFind = FindFirstFile("*", &wfd);
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((0 != strcmp(wfd.cFileName, ".")) && (0 != strcmp(wfd.cFileName, "..")))
                {
                    sprintf(szFullPath, "%s\\%s", szCWD, wfd.cFileName);
                    SetCurrentDirectory(szFullPath);
                    playlist_add(filename, recursive);
                }
            }
        }
        while (FindNextFile(hFind, &wfd));

        FindClose(hFind);
    }

Exit:
    SetCurrentDirectory(szCWD);

    return;
}

void playlist_add_from_list(char *filename)
{
    FILE *fp;
    char buffer[1024], *p;

    fp = fopen(filename, "r");
    if (NULL == fp)
    {
        goto Exit;
    }

    while (fgets(buffer, COUNTOF(buffer), fp) != NULL)
    {
        p = trim(buffer);
        if (strlen(p) != 0)
        {
            playlist_add(p, 0);
        }
    }

    fclose(fp);

Exit:
    return;
}

int playlist_is_empty()
{
    return list_empty(&s_list);
}

void playlist_init()
{
    INIT_LIST_HEAD(&s_list);
}

void playlist_deinit()
{
    struct list_head *p;

    while (!list_empty(&s_list))
    {
        p = s_list.next;
        list_del(p);
        free(p);
    }
}

char *playlist_get_next()
{
    struct list_head *p = NULL;
    PlayNode *pNode;

    switch (s_mode)
    {
        case MODE_NORMAL:
            p = _playlist_get_next_normal(&s_list);
            break;

        case MODE_RANDOM:
            p = _playlist_get_next_random(&s_list);
            break;

        case MODE_SHUFFLE:
            p = _playlist_get_next_shuffle(&s_list);
            break;

        default:
            assert(0 && "unreachable here!");
            break;
    }

    if (NULL != p)
    {
        pNode = list_entry(p, PlayNode, m_node);
        return pNode->m_szFileName;
    }
    else
    {
        return NULL;
    }
}

void playlist_shuffle()
{
    s_mode = MODE_SHUFFLE;
}

void playlist_random()
{
    s_mode = MODE_RANDOM;
}
