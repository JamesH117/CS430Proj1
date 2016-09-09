#include <stdio.h>
#include <stdlib.h>


typedef struct PPMpixel{
    unsigned char r,g,b;
    } PPMpixel;

typedef struct PPMimage {
    int width, height;
    PPMpixel *buffer;
    } PPMimage;



int ppm_write(char output_type, char *input_file, char *output_file){
    //Open file to read into buffer
    FILE *fh = fopen(input_file, "r");

    //If fh == 0 then file did not open, error out
    if(fh == 0){
        fprintf(stderr, "Error: Unable to open file '%s' \n", input_file);
    }
    //Check the first character of the opened file to make sure it is a PPM file, otherwise error out
    int c = fgetc(fh);
    if (c!= 'P'){
        fprintf(stderr, "Error: This is not a PPM file. \n");
    }
    ungetc(c, fh);
    fclose(fh);
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc != 4){
        fprintf(stderr, "Error: Not enough arguments, need image magic number, input file name, and output filename. \n");
    }

    //Grab variable that is output type
    char *output_type = argv[1];


    //Grab input file to convert
    char *input_file = argv[2];

    //Grab name of output file to create
    char *output_file = argv[3];

    ppm_write(output_type, input_file, output_file);

    return 0;
}
