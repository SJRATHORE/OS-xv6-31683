#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char *separators = " -\r\t\n./,";

int
is_separator(char c)
{
    return strchr(separators, c) != 0;
}

void
sixfive(int fd)
{
    char c;
    int number = 0;
    int in_number = 0;
    int valid_start = 1;

    while(read(fd, &c, 1) == 1){

        if(c >= '0' && c <= '9'){
            if(!in_number){
                in_number = 1;
                number = 0;
            }

            if(valid_start){
                number = number * 10 + (c - '0');
            }
        }
        else{
            if(in_number){
                if(valid_start &&
                   (number % 5 == 0 || number % 6 == 0)){
                    printf("%d\n", number);
                }

                in_number = 0;
            }

            valid_start = is_separator(c);
        }
    }

    // EOF behaves like a separator.
    if(in_number && valid_start){
        if(number % 5 == 0 || number % 6 == 0){
            printf("%d\n", number);
        }
    }
}

int
main(int argc, char *argv[])
{
    int fd;

    if(argc == 1){
        sixfive(0);
        exit(0);
    }

    for(int i = 1; i < argc; i++){
        fd = open(argv[i], O_RDONLY);

        if(fd < 0){
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            continue;
        }

        sixfive(fd);
        close(fd);
    }

    exit(0);
}
