#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>

#include <stdint.h> // *should* typedef uint8_t
// Check that uint8_t type exists
#ifndef UINT8_MAX
#error "No support for uint8_t"
#endif

#define arraylen(x) (sizeof x / sizeof *x) // from example given in disc
//  #define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
// same code from man page

#ifndef WRAP
#define WRAP 76 // for wrapping
#endif

static char const alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/=";

int main(int argc, char *argv[])
{

    FILE *input = stdin;

    // for input stream selection
    if (argc == 1)
    {
        // input = stdin;
        // do nothing
    }
    // open a file if given
    else if (argc == 2)
    {
        if (strcmp("-", argv[1]))
        {
            input = fopen(argv[1], "r");
            if (!input)
                err(errno, "fopen");
        }
    }
    else
    {

        err(errno = EINVAL, "Usage: %s [FILE]", argv[0]);
    }
    /*
    if( argc > 2){
        err(errno=EINVAL, "Too many arguments provided\n ");
    }

    //input = stdin;
    if (argc == 2 && strcmp(argv[1],"-") == 0){     //if - is given as a file name then get input from keyboard
        input = stdin;  //get input from the user

    }

    //size_t temp = 0;

*/
    size_t ccount = 0;

    while (1)
    {
        // read three bytes into the buf var "in"
        uint8_t in[3] = {0};

        size_t ret = fread(in, sizeof(*in), arraylen(in), input);

        // fread retrun value is a short item count (or zero)
        if (ret < arraylen(in))
        {
            if (ferror(stdin))
                err(errno, "fread");
        }
        if (ret == 0 && feof(input))
            break;

        // bit shifting minpulations
        uint8_t out_idx[4] = {0};

        // Upper 6 bits of byte 0
        out_idx[0] = in[0] >> 2;
        // Lower 2 bits of byte 0, shift left and or with the upper 4 bits of byte 1
        out_idx[1] = (in[0] << 4 | in[1] >> 4) & 0x3F; // 0x3f == 00111111

        // Lower 4 bits of byte 1, shift left and or with ???
        out_idx[2] = (in[1] << 2 | in[2] >> 6) & 0x3F; // my addtion  (in[2] >> 6 );
        out_idx[3] = in[2] & 0x3F;                     // 0x3F= 00111111

        // padding case
        if (ret < 3)
        {
            out_idx[3] = 64;
            out_idx[2] &= 0x3C; // ox3C ==00111100
        }

        if (ret < 2)
        {
            out_idx[2] = 64;
            out_idx[1] &= 0x30; // 0x30 == 00110000
        }

        // for output print and wrap chars
        for (size_t i = 0; i < sizeof out_idx / sizeof *out_idx; ++i)
        {
            if (putchar(alphabet[out_idx[i]]) == EOF)
                err(errno, "putchar()");
            if (++ccount == WRAP)
            {
                if (putchar('\n') == EOF)
                    err(errno, "putchar()");
                ccount = 0;
            }
        }
        if (ret < arraylen(in) && feof(input))
            break;
    }

    if (ccount != 0)
        if (putchar('\n') == EOF)
            err(errno, "putchar()");

    if (fflush(stdout) == EOF)
        err(errno, "fflush()");
    if (input != stdin && fclose(input) == EOF)
        err(errno, "fclose()");

    return EXIT_SUCCESS;
}

/*
        //create outputs chars using the output_index inot an array of alphabets
        char output[arraylen(out_idx)];



        //padding
        for(int i = 0; i < arraylen(output); ++i){

            if(i*3 +1 >ret * 4) output[i] = '=';
            else output[i] = alphabet[out_idx[i]];

        }

        //wrapping
        for(int i = 0; i < arraylen(output); ++i){

        }

        fwrite(output, sizeof *output, arraylen(output), stdout);


        if (ret < arraylen(in)) break;

    }

    //putchar('\n');

    //fclose(input);

    */
