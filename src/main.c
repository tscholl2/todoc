#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include "item.h"

int file_size(FILE *f)
{
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    rewind(f);
    return size;
}

void trim(char *s)
{
    char *p = s;
    int l = strlen(p);
    while (isspace(p[l - 1]))
        p[--l] = 0;
    while (*p && isspace(*p))
        ++p, --l;
    memmove(s, p, l + 1);
}

void command_help()
{
    printf(
        "todo app cli\n"
        "    todo add \"buy oat milk\"      --- add a new item\n"
        "    todo complete 3              --- mark the 3rd item as done\n"
        "    todo search \"mikl\"           --- fuzzy searches all items\n"
        "    todo list                    --- lists all items\n"
        "    todo help                    --- these docs\n");
}
void command_add(Item ***items, int argc, char *argv[])
{
    char *text = NULL;
    // try to read from option
    int c = getopt(argc, argv, "m:");
    if (c == 'm')
        text = optarg;
    // try to read from argument
    if (text == NULL && argc == 3)
    {
        text = argv[2];
    }
    // try to read from stdin
    fd_set fds;
    struct timeval tv = {0, 1};
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    int ok = select(1, &fds, NULL, NULL, &tv);
    if (text == NULL && ok)
    {
        char buf[1000] = {};
        if (fread(buf, 1, sizeof(buf), stdin) > 0)
            text = buf;
    }
    // try to read from editor
    if (text == NULL)
    {
        const char *editor = getenv("EDITOR");
        if (editor == NULL)
            editor = "nano";
        char cmd[100] = {};
        sprintf(cmd, "%s /tmp/todo.txt", editor);
        assert(system(cmd) == 0);
        FILE *f = fopen("/tmp/todo.txt", "rb+");
        int n = file_size(f);
        text = calloc(n, 1);
        fread(text, 1, n, f);
        fclose(f);
        remove("/tmp/todo.txt");
    }
    // trim result
    assert(text != NULL);
    trim(text);
    assert(strlen(text) > 0);
    // make a new item
    Item *a = Item_edit(Item_new(), text);
    // print new item
    Item_write(a, stdout);
    // add it to the list
    int n = 0;
    while ((*items)[n++] != NULL)
        ;
    Item **new_items = calloc(n + 1, sizeof(Item *));
    for (int i = 0; i < n - 1; i++)
    {
        new_items[i] = (*items)[i];
    }
    new_items[n - 1] = a;
    new_items[n] = NULL;
    free(*items);
    *items = new_items;
}
void command_complete(Item **items, int argc, char **argv)
{
    assert(argc == 3);
    int i = 0;
    i = atoi(argv[2]);
    assert(i > 0);
    int j = 0;
    while (items[j++] != NULL)
        if (j == i)
            // TODO: skip if already completed
            Item_write(Item_complete(items[j - 1]), stdout);
}
void command_search()
{
    printf("TODO search\n");
}
void command_edit()
{
    printf("TODO edit\n");
}
void command_list(Item **items)
{
    while (items[0] != NULL)
        Item_write((items++)[0], stdout);
}

int main(int argc, char *argv[])
{
    const char *home = getenv("HOME");
    char file_path[strlen(home) + 10];
    sprintf(file_path, "%s/.todo", home);
    FILE *file = fopen(file_path, "rb+");
    if (file == NULL)
        file = fopen(file_path, "ab+");
    assert(file != NULL);
    int n = 0;
    while (Item_read(file) != NULL)
        n++;
    Item **items = calloc(n + 1, sizeof(Item *));
    rewind(file);
    for (int i = 0; i < n; i++)
        items[i] = Item_read(file);
    items[n] = NULL;
    if (argc < 2)
        command_help();
    else if ((strcmp(argv[1], "add") == 0) || (strcmp(argv[1], "a") == 0))
        command_add(&items, argc, argv);
    else if ((strcmp(argv[1], "complete") == 0) || (strcmp(argv[1], "c") == 0))
        command_complete(items, argc, argv);
    else if ((strcmp(argv[1], "search") == 0) || (strcmp(argv[1], "s") == 0))
        command_search();
    else if ((strcmp(argv[1], "edit") == 0) || (strcmp(argv[1], "e") == 0))
        command_edit();
    else if ((strcmp(argv[1], "list") == 0) || (strcmp(argv[1], "l") == 0))
        command_list(items);
    else
        command_help();
    // save items
    rewind(file);
    while (items[0] != NULL)
        Item_write((items++)[0], file);
    fclose(file);
}