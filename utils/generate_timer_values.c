#define CLIB_IMPLEMENTATION
#include "clib.h"
#include <stdlib.h>

#define MAX_TIMER_VALUE_16 65536
#define MAX_TIMER_VALUE_8 256

float calculate_initial_value(int max_timer_value, int time_ms, int prescaler){
    double mc = 83.333; // nano

    return (max_timer_value - ((time_ms * 1000000) / (mc * prescaler)));
}


int main(int argc, char** argv){
    int time_ms = 0;
    if(argc == 1) {
        ERRO("An argument is needed!");
        exit(1);
    }
    CliArguments args = clib_make_cli_arguments(
        3, 
        clib_create_argument('h', "help", "Prints this message and exits", no_argument),
        clib_create_argument('v', "version", "Prints the version of the program and exits", no_argument),
        clib_create_argument('t', "time", "Specify the time (ms) to use", required_argument)
    );
    
    struct option* options = clib_get_options(args);
    
    int opt;
    while ((opt = getopt_long(argc, argv, "hvt:", options, NULL)) != -1) {
        switch (opt){
            case 'h':
                clib_cli_help(args, "", "Made by KDesp73");
                exit(0);
            case 'v':
                printf("v0.0.1\n");
                exit(1);
            case 't':
                time_ms = atoi(optarg);
                break;
            default:
                exit(1);    
        }
    }

    INFO("time_ms: %d", time_ms);

    for(size_t i = 1; i <= 256; i = i*2){
        printf("prescaler %d -> %.2f\n", i, calculate_initial_value(MAX_TIMER_VALUE_16, time_ms, i));
    }

    return 0;    
}