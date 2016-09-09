#include <stdio.h>
#include <stdlib.h>


typedef struct PPMpixel{
    unsigned char r,g,b;
    } PPMpixel;

typedef struct PPMimage {
    int width, height;
    PPMpixel *buffer;
    } PPMimage;


//Does not work for P6 yet
int ppm_read(char *output_type, char *input_file, char *output_file){
    //If output_type is 6 creat a wb file
    //If input file is P6 open a rb file
    FILE *fh = fopen(input_file, "r");
    FILE *fp = fopen(output_file, "w");

    int max_color;
    int c;
    PPMimage *image;

    //Open file to read into buffer

    //If fh == 0 then file did not open, error out
    if(fh == 0){
        fprintf(stderr, "Error: Unable to open file '%s' \n", input_file);
        return 1;
    }
    //If fh == 0 then file did not create, error out
    if(fp == 0){
        fprintf(stderr, "Error: Unable to create file '%s' \n", output_file);
        return 1;
    }

    //Check the first character of the opened file to make sure it is a PPM file, otherwise error out
    c = fgetc(fh);
    if (c!= 'P'){
        fprintf(stderr, "Error: This is not a PPM file. \n");
        return 1;
    }
    ungetc(c, fh);


    //Allocate memory for image
    image = (PPMimage *)malloc(sizeof(PPMimage));
    if(image == 0){
        fprintf(stderr, "Unable to allocate memory \n");
        return 1;
    }

    //Skip over magic Number
    c = fgetc(fh);
    //putc(c, stderr);
    while((c = fgetc(fh)) != '\n'){
        //strcat(image->buffer, c);
        //putc(c, fp);
        //putc(c, stderr);
    }

    //Skip over the Comments
    c = getc(fh);
    //putc(c, stderr);
    while (c=='#'){
        while (getc(fh) != '\n');
            c=getc(fh);
            //putc(c, stderr);
    }
    ungetc(c, fh);

    //Get image dimensions
    //Add Error checking later
    fscanf(fh, "%d %d", &image->width, &image->height);
    //printf("%d %d", image->width, image->height);

    //Scan next element, which is image max color, compare with 255, max image color for project
    fscanf(fh, "%d", &max_color);
    if(max_color != 255){
        fprintf(stderr, "'%s' is not formatted into 8-bit color, ie max colors of 255", input_file);
    }

    //Code to put picture data into image buffer





    fclose(fp);
    fclose(fh);
    return 0;
}

int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Error: Not enough arguments, need image magic number, input file name, and output filename. \n");
    }

    //Grab variable that is output type
    char *output_type = argv[1];

    //Grab input file to convert
    char *input_file = argv[2];

    //Grab name of output file to create
    char *output_file = argv[3];

    ppm_read(output_type, input_file, output_file);
    return 0;
}
