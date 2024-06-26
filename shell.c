#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef enum
{
    shell_builtin,
    executable_or_error,
} input_type;

void on_child_exit(){
    reap_child_zombie();
    write_to_log_file("Child terminated");
}

void setup_environment(){
    chdir(getenv("HOME"));
}

void execute_shell_bultin(char *args[])
{
    static char curdir[200];
    char tmpdir[200];

    if (!strcmp(args[0], "cd"))
    {
        if (!args[1] || !strcmp(args[1], "~"))
        {
            char *dir = getenv("HOME");
            strcpy(curdir, dir);
        }
        else if (!strcmp(args[1], "."))
        {
            getcwd(tmpdir, sizeof(tmpdir));
            strcpy(curdir, tmpdir);
        }
        else
        {
            strcpy(curdir, args[1]);
        }
        chdir(curdir);
        getcwd(tmpdir, sizeof(tmpdir));
    }
    else if (!strcmp(args[0], "echo"))
    {
        for (int i = 1; args[i] != NULL; i++)
        {
            printf("%s ", args[i]);
        }
        printf("\n");
    }
    // review that place
    else if (!strcmp(args[0], "export"))
    {
        char var[200];
        char *value;
        char finalval[200];
        int found = 0;
        for (int i = 1; args[i] != NULL; i++)
        {
            int size = strlen(args[i]);
            int indx = strcspn(args[i], "=");
            if (indx != size)
            {
                value = strchr(args[i], '=');
                value++;
                strncpy(var, args[i], indx);
                var[indx] = '\0';
                strcpy(finalval, value);
                found = i;
            }
            if (found > 0 && found != i)
            {
                strcat(finalval, " ");
                strcat(finalval, args[i]);
            }
        }
        setenv(var, finalval, 1);
    }
}

void execute_command(char *args[])
{
    int found_flag = 0;
    for (int i = 1; args[i] != NULL; i++)
    {
        if (!strcmp(args[i], "&"))
        {
            found_flag = 1;
            args[i] = NULL;
        }
    }
    int child_id = fork();
    if (child_id < 0)
    {
        // error
    }
    else if (!child_id)
    {
        // child part execute the process here
        // print("Error)
        execvp(args[0], args);
    }
    else
    {
        if (!found_flag)
            waitpid(child_id, NULL, 0);

    }
}

// take input from user
char *read_input()
{
    static char line[200] = {};
    fgets(line, sizeof(line), stdin);
    // remove the new line
    int indx = strcspn(line, "\n");
    line[indx] = '\0';
    return line;
}
// take the string and divide it into 2d array
void parse_input(char *line, char *args[])
{
    args[0] = strtok(line, " ");
    for (int i = 1; args[i - 1] != NULL; i++)
    {
        args[i] = strtok(NULL, " ");
    }
    for (int i = 1; args[i] != NULL; i++)
    {
        // remove first " and last "
        int indx = strcspn(args[i], "\"");
        int tempsize = strlen(args[i]);
        if (indx == 0 && tempsize != 0)
        {
            memmove(args[i], args[i] + 1, strlen(args[i] + 1) + 1);
        }
        indx = strcspn(args[i], "\"");
        tempsize = strlen(args[i]) - 1;
        if (indx == tempsize)
        {
            args[i][tempsize] = '\0';
        }
    }
}
// if there is no export replace the variable with its value  ($x) replace it with the value 5
void evaluate_expression(char *args[])
{
    for (int i = 1; args[i] != NULL; i++)
    {
        int indx = strcspn(args[i], "$");
        if (indx != strlen(args[i]))
        {
            char *var;
            var = strchr(args[i], '$') + 1;
            char *value = getenv(var);
            if (indx == 0)
            {
                if (value)
                {
                    strcpy(args[i], value);
                }
                else
                {
                    args[i] = "\0";
                }
            }
            else
            {
                args[i][indx] = '\0';
                if (value)
                    strcat(args[i], value);
            }
        }
    }
}
input_type find_type(char *arg)
{
    if (strcmp(arg, "cd") == 0 || strcmp(arg, "echo") == 0 || strcmp(arg, "export") == 0)
        return shell_builtin;
    // handle else
    return executable_or_error;
}
void shell()
{
    int exit_now = 0;
    while (exit_now == 0)
    {
        char *args[100] = {};
        char *username = getlogin();
        // char dir[200];
        // getcwd(dir,sizeof(dir));
        // char *home = getenv("HOME");
        // if (!strcmp(home ,dir))
        //     strcpy(dir,"~");
        printf("%s@Ubuntu: ",username);
        parse_input(read_input(), args);

        if (strcmp(args[0], "export"))
            evaluate_expression(args);

        if (args[0] && strcmp(args[0], "exit") == 0)
        {
            exit_now = 1;
            break;
        }

        input_type type = find_type(args[0]);

        switch (type)
        {
        case shell_builtin:
            execute_shell_bultin(args);
            break;
        case executable_or_error:
            execute_command(args);
            break;
        }
    }
}

int main()
{

    register_child_signal(on_child_exit());
    setup_environment();
    shell();
    // what we need to do

    // first read the input and divide it by spaces into array or strings

    // Now we have many possiblities
    //  first check if the input == exit then exit the whole program
    //  second it is not we have many choices but first we need to check if it has any variables like that command ls $x  where x = -l
    // we are doing that by 1.check if there are any $ sign if there aren't lucky duck go direct to next steps
    // if we found it we will search for each variable and replace it with its value

    // before execute that commands we need to consider if it has & or not
    //  found &  >> do not wait
    //  do not found it >> do wait for child process
    // first it could be a command with no arguments
    // second if it has arguments

    // when start executing any process we basically make a fork then wait or not according to the command and then finish executing and start all over again

    return 0;
}