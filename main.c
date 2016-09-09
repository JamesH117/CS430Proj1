#include <stdio.h>
#include <stdlib.h>


typedef struct PPMpixel{
    unsigned char r,g,b;
    } PPMpixel;

typedef struct PPMimage {
    int width, height;
    PPMpixel *buffer;
    } PPMimage;

PPMimage *image;

//Function to convert ASCII to Binary Bits and Binary Bits to ASCII
int ppm_convert(int output_type){
return 0;
}

//Does not work for P6 yet, put catcher so you open a Binary file if given P6 File
int ppm_read(int output_type, char *input_file){
    FILE *fh;
    int max_color, width, height;
    int c;
    //If output_type is 6 creat a wb file
    //If input file is P6 open a rb file
    if(output_type == 3){
        fh = fopen(input_file, "r");
        //printf("Opening P3");

    }
    if(output_type == 6){
        fh = fopen(input_file, "rb");
        //printf("Opening P6");
    }



    //If fh == 0 then file did not open, error out
    if(fh == 0){
        fprintf(stderr, "Error: Unable to open file '%s' \n", input_file);
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
    width = image->width;
    height = image->height;
    //printf("%d %d", image->width, image->height);

    //Scan next element, which is image max color, compare with 255, max image color for project
    fscanf(fh, "%d", &max_color);
    if(max_color != 255){
        fprintf(stderr, "'%s' is not formatted into 8-bit color, ie max colors of 255", input_file);
    }

    //Code to put picture data into image buffer
    image->buffer = (PPMpixel*)malloc(sizeof(PPMpixel) * width * height);
    //This code writes to the buffer of image to transfer to output file
    fread(image->buffer, 3*width, height, fh);
    fclose(fh);
    return 0;
}

int ppm_write(int output_type, char *output_file){
    FILE *fp;
    int width, height;
    int c;
    char magic_number[2];
    width = image->width;
    height = image->height;
    magic_number[0] = 'P';

    if(output_type == 3){
        magic_number[1] = '3';
    }else{
        magic_number[1] = '6';
    }
    //printf("%c%c", magic_number[0], magic_number[1]);


    //Open binary file to write to if 6 is given
    if(output_type == 3){
        fp = fopen(output_file, "w");
        //printf("Opening P3 to write to. \n");

    }
    if(output_type == 6){
        fp = fopen(output_file, "wb");
        //printf("Opening P6 to write to. \n");
    }

    //If fp == 0 then file did not create, error out
    if(fp == 0){
        fprintf(stderr, "Error: Unable to create file '%s' \n", output_file);
        return 1;
    }
    //Write appropriate magic number to file based on output_type
    fwrite(magic_number, sizeof(magic_number), sizeof(magic_number)-1, fp);
    //Write image dimensions next
    fprintf(fp,"\n%d %d", width, height);
    //Write max colors (always 255)
    fprintf(fp,"\n255");

    //Call Function Here to convert image->buffer to either ASCII or Raw Bits

    //This code writes to output image, but only the pixels
    fwrite(image->buffer, 3*width, height, fp);
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Error: Not enough arguments, need image magic number, input file name, and output filename. \n");
    }

    //Grab variable that is output type
    char *output_type = argv[1];
    int type = atoi(output_type);
    //printf("%d", type);
    //Put Error checker to make sure output_type is 3 or 6.

    //Grab input file to convert
    char *input_file = argv[2];

    //Grab name of output file to create
    char *output_file = argv[3];

    ppm_read(type, input_file);
    ppm_write(type, output_file);
    return 0;
}
