


void dsh_loop()
{
    char *line;
    char **args;
    int status;

    do
    {
        print("> ");
        line = dsh_read_line();
        args = dsh_split_line(line);
        status = lsh_execute(args);
    } while (status);
}

int main(int argc, char **args)
{
    dsh_loop(); // basic loop
    return 0;   //return EXIT_SUCCESS;
}