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

void command_help()
{
    printf(
        "todo\n"
        "\n"
        "  A 'simple' CLI app to keep track of a list of todos stored\n"
        "  as a regular text file in $HOME/.todo\n"
        "\n"
        "  EXAMPLES:\n"
        "    todo add \"buy oat milk\"      --- add a new item\n"
        "    todo complete 3              --- mark the 3rd item as completed\n"
        "    todo search \"mikl\"           --- fuzzy searches all items\n"
        "    todo list                    --- lists all items\n"
        "    todo help                    --- these docs\n");
}

void command_add(Item *items, int argc, char *argv[])
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
    // make a new item
    Item a = Item_edit(Item_new(), text);
    // print new item
    Item_write(a, stdout);
    // add it to the list
    int n = 0;
    while (items[n++] != NULL)
        ;
    printf("%d items found\n", n);
    items[n - 1] = a;
    Item_sort(items);
}

void command_complete(Item *items, int argc, char **argv)
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
    Item_sort(items);
}

void command_search(Item *items, int argc, char **argv)
{
    if (items[0] == NULL)
        return;
    assert(argc == 3);
    char *fuzzy_needle = argv[2];
    int n = 0;
    while (items[n] != NULL)
        n++;
    int *values = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++)
        values[i] = Item_fuzzy_search(items[i], fuzzy_needle, strlen(fuzzy_needle));
    int min = values[0];
    for (int i = 0; i < n; i++)
        min = values[i] < min ? values[i] : min;
    for (int i = 0; i < n; i++)
        if (values[i] == min)
            Item_write(items[i], stdout);
}

void command_edit(Item *items, int argc, char **argv)
{
    assert(argc == 3);
    int i = 0;
    i = atoi(argv[2]);
    assert(i > 0);
    int j = 0;
    while (items[j++] != NULL)
        if (j == i)
        {
            Item a = items[j - 1];
            const char *editor = getenv("EDITOR");
            if (editor == NULL)
                editor = "nano";
            char cmd[100] = {};
            sprintf(cmd, "%s /tmp/todo.txt", editor);
            FILE *f = fopen("/tmp/todo.txt", "wb");
            Item_write(a, f);
            Item_free(a);
            fclose(f);
            assert(system(cmd) == 0);
            f = fopen("/tmp/todo.txt", "rb");
            a = Item_read(f);
            remove("/tmp/todo.txt");
            fclose(f);
            assert(a != NULL);
            items[j - 1] = a;
            Item_write(a, stdout);
        }
    Item_sort(items);
}

void command_list(Item *items)
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
    // read items
    int n = 0;
    while (Item_read(file) != NULL)
        n++;
    Item *items = calloc(n + 2, sizeof(Item)); // leave an extra space at the end
    rewind(file);
    for (int i = 0; i < n; i++)
        items[i] = Item_read(file);
    if (argc < 2)
        command_help();
    else if ((strcmp(argv[1], "add") == 0) || (strcmp(argv[1], "a") == 0))
        command_add(items, argc, argv);
    else if ((strcmp(argv[1], "complete") == 0) || (strcmp(argv[1], "c") == 0))
        command_complete(items, argc, argv);
    else if ((strcmp(argv[1], "search") == 0) || (strcmp(argv[1], "s") == 0))
        command_search(items, argc, argv);
    else if ((strcmp(argv[1], "edit") == 0) || (strcmp(argv[1], "e") == 0))
        command_edit(items, argc, argv);
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