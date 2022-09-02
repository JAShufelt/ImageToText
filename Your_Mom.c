#include <stdio.h>
#include <stdlib.h>

typedef struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} Pixel;

void byte_skip_ahead(FILE* in, int i)   //Reads i bytes, skipping them. 
{
    char buffer;
    for (; i > 0; i--)
    {
        fread_s(&buffer, 1, 1, 1, in);
    }
}

int main()
{
    char filename[50];
    FILE* in;
    FILE* out;
    Pixel pixelBuffer;

    printf("*****************************************************************\n");
    printf("*                         Image to Text                         *\n");
    printf("*****************************************************************\n\n");

    printf("\t\tWelcome to \"Image to Text\"!\nPlease enter the name of the file you'd like to convert to text.\nOr type \"help\" for instructions, or \"exit\" to close the program.\n> ");

    while(1)
    {
        fgets(filename, 50, stdin); //Get filename from stdin
        for (int i = 0; i < 50; i++) {
            if (filename[i] == '\n') {
                filename[i] = '\0';
                break;
            }
        }

        if(strcmp(filename,"help") == 0)
        {
            printf("\n\"Image to Text\" is a tool which allows you to convert images into text.\n");
            printf("Images provided must be in 24 bpp .bmp and no larger than 250x250 pixels.\n");
            printf("The pixel data is converted into a greyscale text image constructed using\nonly five ASCII characters (_ %c %c %c %c).\n\n",176,177,178,219);

            printf("To begin, make sure an eligble image is located in the same path as\nthis program's executable.");
            printf(" Once converted, the output will be printed on\ncommandline as well as saved to a file,\"ImageToText.txt\"\n\n>");
        }
        else if(strcmp(filename, "exit") == 0)
        {
            return 0;
        }
        else
        {
            fopen_s(&in, filename, "rb");

            if (in != NULL) {
                char bm[2] = { 'A', 'B' };
                fread_s(&bm, 2, 2, 1, in);
                printf("\n\"%s\" was successfully opened.\n", filename);
                if(bm[0] == 'B' && bm[1] == 'M')
                {
                    break;
                }
                else
                {
                    printf("\"%s\" is not a 24 bpp .bpm file. Please convert it to the proper format and try again.\n\n>", filename);
                    fclose(in);
                }
            }
            else
            {
                printf("\n\"%s\" was not found, please check that the filename is spelt correctly and the file is in the correct path.\n\n>", filename);
            }
        }
    }
    
    char byteBuffer;    //Buffer to hold a single byte
    int arrayStartingByte;  //Contains the address of the pixel array data
    int width;  //Contains the width of the image
    int height; //Contains the height of the image
    int padding;    //Contains the number of bytes padded to the end of each row.
    
    byte_skip_ahead(in, 8);    //Skip to the pixel array starting address 0x0000000A (Skip to byte 10)
    
    fread_s(&arrayStartingByte, sizeof(arrayStartingByte), sizeof(arrayStartingByte), 1, in);   //Extract pixel array starting address
    byte_skip_ahead(in, 4); //Skip to the width 0x00000012

    fread_s(&width, sizeof(width), sizeof(width), 1, in);   //Extract width 
    fread_s(&height, sizeof(height), sizeof(height), 1, in);    //Extract height 
    padding = ((width * 3) % 4);    //Calculate padding amount

    byte_skip_ahead(in, (arrayStartingByte - 26));  //Skipping to the pixel array data = (Starting Address - 26)

    unsigned char avgColor;
    unsigned char** characters[250][250];
    
    
    //Scan the whole .bmp image and translate each pixel into one of five different ASCII characters stored in a corresponding location in 2D char array "characters".
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            fread_s(&pixelBuffer, sizeof(Pixel), sizeof(Pixel), 1, in); //Read in 3 bytes into pixelBuffer
            avgColor = (pixelBuffer.red + pixelBuffer.blue + pixelBuffer.green) / 3;    //Calculate average color saturation

            unsigned char newChar;
            if(0 <= avgColor && avgColor <= 60)
            {
                newChar = 219;
            }
            else if( 61 <= avgColor && avgColor <= 90)
            {
                newChar = 178;
            }
            else if( 91 <= avgColor && avgColor <= 125)
            {
                newChar = 177;
            }
            else if(126 <= avgColor && avgColor <= 160)
            {
                newChar = 176;
            }
            else
            {
                newChar = 95;
            }
            characters[i][j] = newChar;
        }
        byte_skip_ahead(in, padding);   //Skip past the padding before starting next line
    }
    fclose(in);

    //Write out the image to commandline, as well as ImageToText.txt using 2D char array "characters"
    fopen_s(&out, "ImageToText.txt", "w");
    printf("\n\n");
    unsigned char* temp[1];

    for(int i = height - 1 ; i > -1; i--)
    {
        for(int j = 0; j < width; j++)
        {
            fprintf(out, "%c%c", (unsigned char)characters[i][j], (unsigned char)characters[i][j]);
            printf("%c%c", (unsigned char)characters[i][j], (unsigned char)characters[i][j]);
        }
        temp[0] = '\n';
        fprintf(out,"%c",'\n');
        printf("\n");
    }

   
    fclose(out);
    return 0;
}



