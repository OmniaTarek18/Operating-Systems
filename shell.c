#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
typedef enum
{
    shell_builtin,
    executable_or_error,
} input_type;

// void parent_main(){
//     register_child_signal(on_child_exit());
//     setup_environment();
//     shell();
// }

// void on_child_exit()
//     reap_child_zombie();
//     write_to_log_file("Child terminated");
// }

// void setup_environment(){
//     cd(Current_Working_Directory);
// }

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
    //review that place 
    else if (!strcmp(args[0], "export"))
    {
        char var[200];
        char *value;
        int found=0;
        for (int i = 1; args[i] != NULL; i++)
        {
            int size = strlen(args[i]);
            int indx = strcspn(args[i], "=");
            if (indx != size)
            {
                value = strchr(args[i],'=');
                value++;
                args[i][indx]='\0';
                strcpy(var,args[i]);
                found =i;
                args[i][indx]='=';
                printf("values is %s\n",value);
            }
            if (found > 0 && found != i){
                strcat(value," ");
                strcat(value,args[i]);
                printf("value is %s\n",value);
            }
        }
        for (int i=0 ; args[i]!=NULL ; i++){
            printf("%s\n",args[i]);
        }

        setenv(var, value, 1);
    }
}

// void execute_command(){
//     child_id = fork();
//     if child:
//         execvp(command parsed)
//         print("Error)
//         exit()
//     else if parent and foreground:
//         waitpid(child)
// }

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
            var = strchr(args[i],'$')+1;
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
        char *args[50] = {};
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
            // execute_command();
            break;
        }
    }
}

int main()
{

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