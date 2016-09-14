#include <stdio.h>
#include <stdlib.h>


typedef struct PPMpixel{
    //Test is '\n' at beginning of each line of rgb
    unsigned char r,g,b;
    } PPMpixel;

typedef struct PPMimage {
    int width, height;
    int input_filetype;
    PPMpixel *buffer;
    } PPMimage;

PPMimage *image;
//Definition for max colors, and also for buffer size usage
#define MAX_COLORS 255
//Function to convert ASCII to Binary Bits and Binary Bits to ASCII
int ppm_convert(int output_type, int input_filetype);

//Read input_file and put file type, image dimensions, and pixel data into image struct.
int ppm_read(char *input_file){
    FILE *fh;
    int max_color, width, height;
    int tracker = 0;
    int c, i,j,k;
    char temp_buffer[4];

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

    //Allocate memory for image
    image = (PPMimage*)malloc(sizeof(PPMimage));
    if(image == 0){
        fprintf(stderr, "Unable to allocate memory \n");
        return 1;
    }
    //Put original Input File type # into struct for convert reference
    c = fgetc(fh);
    c = fgetc(fh);
    image->input_filetype = c;

    //Skip over magic Number
    while((c = fgetc(fh)) != '\n'){
    }

    //Skip over the Comments
    c = fgetc(fh);
    while (c=='#'){
        while (fgetc(fh) != '\n');
            c=fgetc(fh);
    }
    ungetc(c, fh);

    //Get image dimensions
    if(fscanf(fh, "%d %d", &image->width, &image->height) !=2){
        fprintf(stderr, "A width or Height image dimensions is missing in the file.");
        return 1;
    }
    width = image->width;
    height = image->height;

    //Scan next element, which is image max color, compare with 255, max image color for 8-bit pictures
    fscanf(fh, "%d", &max_color);
    if(max_color != MAX_COLORS){
        fprintf(stderr, "'%s' is not formatted into 8-bit color, ie max colors of %d", input_file, MAX_COLORS);
        return 1;
    }
    //Code allocate room for pixel data
    image->buffer = (PPMpixel*)malloc((MAX_COLORS+1)*width*height);
    //printf("The size of PPMpixel is: %d", sizeof(image->buffer));
    //memset(image->buffer, '\0', sizeof(image->buffer));
    //printf("%d", sizeof(image->buffer));
    if(image->buffer == 0){
        fprintf(stderr, "Error: Memory could not be allocated.");
        return 1;
    }
    fgetc(fh);

    //printf("Addding to buffer");
    if(image->input_filetype == '3'){
        j=0;
        i=0;
        while ((c = fgetc(fh)) != EOF){
        //printf("Start of Loop %c\n", c);
            if(isspace(c)){
                //printf("Space Caught\n");
                //printf("%c", temp_buffer[0]);
                //printf("%c", temp_buffer[1]);
                //printf("%c", temp_buffer[2]);
                //printf("%c\n", temp_buffer[3]);
                k = atoi(temp_buffer);
                //printf("%d\n", k);
                tracker++;
                if(tracker == 1){
                    image->buffer[j].r = k;
                    //printf("k is %d\n", k);
                    //printf("r is %d\n", k);
                }
                if(tracker == 2){
                    image->buffer[j].g = k;
                    //printf("k is %d\n", k);
                    //printf("g is %d\n", k);
                }
                if(tracker == 3){
                    image->buffer[j].b = k;
                    //printf("k is %d\n", k);
                    //printf("b is %d\n", k);
                    tracker = 0;
                }
                j++;
                i = 0;
                //printf("%d\n", tracker);
                memset(temp_buffer, '\0', sizeof(temp_buffer));
            }
            else{
                //printf("Adding to buffer %c\n", c);
                temp_buffer[i++] = c;
            }
        }
    ungetc(c, fh);
    }
    //If a P6 file, read the entire thing into buffer, need to fix
    if(image->input_filetype == '6'){
        fread(image->buffer, (sizeof(&image->buffer)), height*width, fh);

    }

    //fread(image->buffer, (MAX_COLORS+1), width*height, fh);
    //printf("The size of PPMpixel is: %d", sizeof(PPMpixel));
    fclose(fh);
    return 0;

}

int ppm_write(int output_type, char *output_file){
    FILE *fp;
    int width, height;
    int j;
    int tracker = 0;
    char magic_number[2];
    width = image->width;
    height = image->height;
    magic_number[0] = 'P';

    if(output_type == 3){
        magic_number[1] = '3';
    }else{
        magic_number[1] = '6';
    }
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
    fprintf(fp,"\n%d", MAX_COLORS);

    //Converting a P3 to P6
    if(magic_number[1] == '6' && image->input_filetype == '3'){
            fprintf(fp,"\n");
            for (j=0; j< sizeof(PPMpixel)*width*height; j++){
                    tracker++;
                    if(tracker == 1){
                       fwrite(&image->buffer[j].r,1,1, fp);
                    }
                    if(tracker == 2){
                       fwrite(&image->buffer[j].g,1,1, fp);
                    }
                    if(tracker == 3){
                      fwrite(&image->buffer[j].b,1,1, fp);
                        tracker = 0;
                    }
            }
    }

    //Converting a P6 to P3
    //Working but bunch of extra zeros added to end
    if(magic_number[1] == '3' && image->input_filetype == '6'){
            fprintf(fp,"\n");
            //printf("Size of PPMpixel is: %d", sizeof(PPMpixel)*width*height);
                for (j=0; j< sizeof(PPMpixel)*width*height; j++){
                        fprintf(fp, "%d", image->buffer[j].r);
                        fprintf(fp, "\n");
                        fprintf(fp, "%d", image->buffer[j].g);
                        fprintf(fp, "\n");
                        fprintf(fp, "%d", image->buffer[j].b);
                        fprintf(fp, "\n");
                }
    }

    //Working P3 to P3
    if(magic_number[1] == '3' && image->input_filetype == '3'){
            fprintf(fp,"\n");
                for (j=0; j< sizeof(PPMpixel)*width*height; j++){
                    tracker++;
                    if(tracker == 1){
                       fprintf(fp, "%d", image->buffer[j].r);
                        fprintf(fp, "\n");
                    }
                    if(tracker == 2){
                       fprintf(fp, "%d", image->buffer[j].g);
                        fprintf(fp, "\n");
                    }
                    if(tracker == 3){
                       fprintf(fp, "%d", image->buffer[j].b);
                        fprintf(fp, "\n");
                        tracker = 0;
                    }
                }
    }
    //Working P6 to P6, extra 0's added to end of file
    if(magic_number[1] == '6' && image->input_filetype == '6'){
            fprintf(fp,"\n");
            for (j=0; j< sizeof(PPMpixel)*width*height; j++){
                    fwrite(&image->buffer[j].r,1,1, fp);
                    fwrite(&image->buffer[j].g,1,1, fp);
                    fwrite(&image->buffer[j].b,1,1, fp);
            }
    }
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
    if(type != 6 && type != 3){
        fprintf(stderr, "Image output type given is not valid, must be 3 or 6.\n");
        return 1;
    }
    //Grab input file to convert
    char *input_file = argv[2];

    //Grab name of output file to create
    char *output_file = argv[3];
    ppm_read(input_file);
    ppm_write(type, output_file);
    return 0;
}
