#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DSH_RL_BUFSIZE 1024

char *dsh_read_line();
int dsh_execute(char **args);

char *dsh_read_line(void)
{
    int buff_size = DSH_RL_BUFSIZE;
    int index = 0;
    int c;

    char *buff = (char *)malloc(sizeof(char) * buff_size);

    if (buff == NULL)
    {
        fprintf(stderr, "dsh: The memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = getchar();

        if (c == EOF || c == '\n')
        {
            buff[index] = '\0';
            return buff;
        }
        else
        {
            buff[index++] = c;
        }

        if (index >= buff_size)
        {
            buff_size += DSH_RL_BUFSIZE;
            buff = realloc(buff, buff_size);
            if (buff == NULL)
            {
                fprintf(stderr, "dsh: The memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void dsh_loop()
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("> ");
        line = dsh_read_line();

        printf("%s\n", line);
        free(line);
    } while (1);
}

int main(int argc, char **args)
{
    dsh_loop(); // basic loop
    return EXIT_SUCCESS;
}