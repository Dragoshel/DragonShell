#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define DSH_RL_BUFSIZE 1024
#define DSH_TOK_BUFSIZE 64
#define DSH_TOK_DELIM " \t\r\n\a"

char **dsh_split_line(char *line);
char *dsh_read_line();

int dsh_execute(char **args);
int dsh_launch(char **args);
int dsh_cd(char **args);
int dsh_help(char **args);
int dsh_exit(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit"};

int (*builtin_func[])(char **) = {
    &dsh_cd,
    &dsh_help,
    &dsh_exit};

int dsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

int dsh_exit(char **args)
{
    return 0;
}

int dsh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "dsh: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("lsh");
        }
    }
    return 1;
}

int dsh_help(char **args)
{
    printf("Dragos DSH\n");
    printf("Type command name and arguments, and hit enter.\n");
    printf("The following commands are build in: \n");

    printf("%s", builtin_str[0]);

    for (int i = 1, length = dsh_num_builtins(); i < length; i++)
    {
        printf(", %s", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int dsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid < 0) //error
    {
        perror("dsh");
    }
    else if (pid == 0) //child
    {
        if (execvp(args[0], args) == -1)
        {
            perror("dsh");
        }
        exit(EXIT_FAILURE);
    }
    else //parent
    {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}
char **dsh_split_line(char *line)
{
    int buff_size = DSH_TOK_BUFSIZE;
    int index = 0;

    char **tokens = (char **)malloc(sizeof(char *) * buff_size);
    char *token;

    if (tokens == NULL)
    {
        fprintf(stderr, "dsh: The memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, DSH_TOK_DELIM);
    while (token != NULL)
    {
        tokens[index++] = token;

        if (index >= buff_size)
        {
            buff_size += DSH_TOK_BUFSIZE;
            char **tokens = realloc(tokens, sizeof(char *) * buff_size);
            if (tokens == NULL)
            {
                fprintf(stderr, "dsh: The memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, DSH_TOK_DELIM);
    }
    tokens[index] = NULL;
    return tokens;
}
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
        args = dsh_split_line(line);
        status = dsh_execute(args);

        free(line);
        free(args);
    } while (status);
}
int dsh_execute(char **args)
{
    if (args[0] == NULL)
    {
        // An empty command was entered.
        return 1;
    }

    for (int i = 0, length = dsh_num_builtins(); i < length; i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    return dsh_launch(args);
}

int main(int argc, char **args)
{
    dsh_loop(); // basic loop
    return EXIT_SUCCESS;
}