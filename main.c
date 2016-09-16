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
//Function to reset my temp buffer that I use to put ascii characters into image buffer
void *memset(void *str, int c, size_t n);
//Function I use to help grab all ascii characters but not spaces
int isspace(int c);

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
        exit(1);
    }
    //Check the first character of the opened file to make sure it is a PPM file, otherwise error out
    c = fgetc(fh);
    if (c!= 'P'){
        fprintf(stderr, "Error: This is not a PPM file. \n");
        exit(1);
    }
    ungetc(c, fh);
    //Allocate memory for image
    image = (PPMimage*)malloc(sizeof(PPMimage));
    if(image == 0){
        fprintf(stderr, "Unable to allocate memory \n");
        exit(1);
    }
    c = fgetc(fh);
    c = fgetc(fh);
    //Put original Input File type # into struct for convert reference
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
        exit(1);
    }
    width = image->width;
    height = image->height;
    //Scan next element, which is image max color, compare with 255, max image color for 8-bit pictures
    fscanf(fh, "%d", &max_color);
    if(max_color != MAX_COLORS){
        fprintf(stderr, "'%s' is not formatted into 8-bit color, ie max colors of %d", input_file, MAX_COLORS);
        exit(1);
    }
    //Code allocate room for pixel data
    image->buffer = (PPMpixel*)malloc((MAX_COLORS+1)*width*height);
    if(image->buffer == 0){
        fprintf(stderr, "Error: Memory could not be allocated.");
        exit(1);
    }
    //Remove unnecessary character still in file
    fgetc(fh);
    if(image->input_filetype == '3'){
        j=0;
        i=0;
        while ((c = fgetc(fh)) != EOF){
            //If character I grab is a space, then I have a full rgb component, add to buffer
            if(isspace(c)){

                //Get rid of extra spaces in case there is more than one space between each pixel data
                while(isspace(c = fgetc(fh))){
                }
                ungetc(c, fh);
                //Convert Pixel data to an int
                k = atoi(temp_buffer);
                //Check if pixel data is too large
                if(k > MAX_COLORS){
                    fprintf(stderr, "Some pixel data is larger than Max color size allowed.");
                    exit(1);
                }
                //Put pixel data into corresponding RGB
                tracker++;
                if(tracker == 1){
                    image->buffer[j].r = k;
                }
                if(tracker == 2){
                    image->buffer[j].g = k;
                }
                if(tracker == 3){
                    image->buffer[j].b = k;

                    tracker = 0;
                }
                j++;
                i = 0;
                //Empty out temp buffer since some numbers may only be 1 digit or 2 digit instead of max 3
                memset(temp_buffer, '\0', sizeof(temp_buffer));
            }
            else{
                //Put part of full number into temp_buffer until I grab entire number
                temp_buffer[i++] = c;
            }
        }
    ungetc(c, fh);
    }
    //If a P6 file, read the entire thing into buffer, need to fix minor bug if possible
    if(image->input_filetype == '6'){
        fread(image->buffer, (sizeof(&image->buffer)), height*width, fh);
    }
    fclose(fh);
    return 0;

}

//Take image buffer and put it into corresponding output file
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
    //Open correct type of file to write to
    if(output_type == 3){
        fp = fopen(output_file, "w");
    }
    if(output_type == 6){
        fp = fopen(output_file, "wb");
    }

    //If fp == 0 then file did not create, error out
    if(fp == 0){
        fprintf(stderr, "Error: Unable to create file '%s' \n", output_file);
        exit(1);
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
    if(magic_number[1] == '3' && image->input_filetype == '6'){
            fprintf(fp,"\n");
                for (j=0; j<width*height; j++){
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
    //Working P6 to P6
    if(magic_number[1] == '6' && image->input_filetype == '6'){
            fprintf(fp,"\n");
            for (j=0; j<width*height; j++){
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

    //Put Error checker to make sure output_type is 3 or 6.
    if(type != 6 && type != 3){
        fprintf(stderr, "Image output type given is not valid, must be 3 or 6.\n");
        exit(1);
    }
    //Grab input file to convert
    char *input_file = argv[2];

    //Grab name of output file to create
    char *output_file = argv[3];
    ppm_read(input_file);
    ppm_write(type, output_file);
    return 0;
}
