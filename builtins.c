#include "minishell.h"

// cd
int cd(char *path) 
{
    return chdir(path);
}