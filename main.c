#include <stdio.h>
#include <stdlib.h>


typedef struct PPMpixel{
    unsigned char r,g,b;
    } PPMpixel;

typedef struct PPMimage {
    int width, height, input_filetype;
    PPMpixel *buffer;
    } PPMimage;

PPMimage *image;
//Function to convert ASCII to Binary Bits and Binary Bits to ASCII
int ppm_convert(int output_type, int input_filetype);

//Read input_file and put file type, image dimensions, and pixel data into image struct.
int ppm_read(char *input_file){
    FILE *fh;
    int max_color, width, height;
    int c;

    //Try to open as a P3 File, if unable try to open as a P6 file
    fh = fopen(input_file, "r");
    if(fh == 0){
       fh = fopen(input_file, "rb");
    }
    //If fh == 0 then file did not open, error out
    if(fh == 0){
        fclose(fh);
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
    //fprintf("%d", image->input_filetype);

    //Allocate memory for image
    image = (PPMimage *)malloc(sizeof(PPMimage));
    if(image == 0){
        fprintf(stderr, "Unable to allocate memory \n");
        return 1;
    }

    //Put original Input File type # into struct for convert reference
    c = fgetc(fh);
    c = fgetc(fh);
    image->input_filetype = c;
    ungetc(c, fh);
    //printf("%c", image->input_filetype);

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

    //Scan next element, which is image max color, compare with 255, max image color for 8-bit pictures
    fscanf(fh, "%d", &max_color);
    if(max_color != 255){
        fprintf(stderr, "'%s' is not formatted into 8-bit color, ie max colors of 255", input_file);
    }

    //Code allocate room for pixel data
    image->buffer = (PPMpixel*)malloc(sizeof(PPMpixel) * width * height);
    if(image->buffer == 0){
        fprintf(stderr, "Error: Memory could not be allocated.");
    }
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
    ppm_convert(magic_number[1], image->input_filetype);
    //This code writes to output image, but only the pixels
    fwrite(image->buffer, 3*width, height, fp);
    fclose(fp);
    return 0;
}

int ppm_convert(int output_type, int input_filetype){
    int i, width, height;
    width = image->width;
    height = image->height;
    //Increment through the buffer and change each RGB
    //printf("We have a P%c file converting to a P%c", image->input_filetype, output_type);
    if(input_filetype == '3' && output_type == '6'){
        //Code to go from ASCII to Raw Bits
        printf("\nInput type 3 and output type 6");
        for(i=0; i< width*height; i++){

        }
        return 0;

    }
    else if(input_filetype == '6' && output_type == '3'){
        printf("\nInput type 6 and output type 3");
        //Code to go from Raw Bits to ASCII
        for(i=0; i< width*height; i++){

        }
        return 0;
    }
    else{
        //Converting P3 to P3 or P6 to P6
        printf("\n P3 to P3 or P6 to P6");
        return 0;
    }

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

    ppm_read(input_file);
    ppm_write(type, output_file);
    return 0;
}
