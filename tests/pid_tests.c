#include <stdio.h>

int
pid_tests (int argc, char *argv[])
{
    const int default_choice = 1;
    int       choice         = default_choice;
    
    if (1 < argc)
    {
        if (sscanf(argv[1], "%d", &choice) != 1)
        {
            printf("Could not parse argument. Terminating.\n");
            return -1;
        }
    }
    return 0;
}