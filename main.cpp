#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <stdint.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

using namespace std;

//ENUMS----------------------------------------------------------------------
//compression methods
enum Compression {
	BI_RGB,
	BI_RLE8,
	BI_RLE4,
	BI_BITFIELDS,
	BI_JPEG,
	BI_PNG,
	BI_ALPHABITFIELDS
};


//Structs--------------------------------------------------------------------
struct bmpfile_magic
{
	unsigned char magic[2];
};
struct bmpfile_header
{
	uint32_t size;
	uint16_t creator1;
	uint16_t creator2;
	uint32_t bmp_offset;
};
typedef struct {
  uint32_t header_sz;
  int32_t width;
  int32_t height;
  uint16_t nplanes;
  uint16_t bitspp;
  uint32_t compress_type;
  uint32_t bmp_bytesz;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors;
  uint32_t nimpcolors;
} BITMAP_INFO_HEADER;

//main()----------------------------------------------------------------------
void renderFunction()
{
  string filename;

	ifstream bmpFile("apple.bmp", ios::in | ios::binary);
	ofstream outFile("output.alp", ios::out | ios::binary);

	if (bmpFile == 0)
		cout<<"File not found";
	else
	{
		bmpfile_magic bmMagic;
		bmpfile_header bmHeader;

		//extract file header data
		bmpFile.read((char*)&bmMagic,sizeof(bmpfile_magic));
		bmpFile.read((char*)&bmHeader, sizeof(bmpfile_header));

		//display file header data
		cout << "Bitmap File Header:\n";
		cout << "\tMagic Number: " << bmMagic.magic[0] << bmMagic.magic[1] << "\n";
		cout << "\tSize in bytes: " << bmHeader.size << "\n";
		cout << "\tCreator1 field: " << bmHeader.creator1 << "\n";
		cout << "\tCreator2 field: " << bmHeader.creator2 << "\n";
		cout << "\tBitmap data offset: " << bmHeader.bmp_offset << "\n\n";

		if (bmMagic.magic[0] == 'B' && bmMagic.magic[1] == 'M')
		{
			BITMAP_INFO_HEADER bmpInfoHeader;

			//extract bitmap info header data:
			bmpFile.read((char*)&bmpInfoHeader, sizeof(BITMAP_INFO_HEADER));

			//display bitmap info header data:
			cout << "Bitmap Info Header:\n";
			cout << "\tHeader size: " << bmpInfoHeader.header_sz << "\n";
			cout << "\tBitmap width in pixels: " << bmpInfoHeader.width << "\n";
			cout << "\tBitmap height in pixels: " << bmpInfoHeader.height << "\n";
			cout << "\tNumber of Color Planes used: " << bmpInfoHeader.nplanes << "\n";
			cout << "\tNumber of bits per pixel (color depth): " << bmpInfoHeader.bitspp << "\n";
			cout << "\tCompression Method Used: " << bmpInfoHeader.compress_type << "\n";
			cout << "\tBitmap image size: " << bmpInfoHeader.bmp_bytesz << "\n";
			cout << "\tHorizontal resolution: " << bmpInfoHeader.hres << "\n";
			cout << "\tVertical resolution: " << bmpInfoHeader.vres << "\n";
			cout << "\tNumber of colors in the color palette: " << bmpInfoHeader.ncolors << "\n";
			cout << "\tNumber of important colors used: " << bmpInfoHeader.nimpcolors << "\n";


			if (bmpInfoHeader.header_sz == 40)
			{

				//calculate the bitmap data size and check it against the value in the info header
				double ceilArg = (double)bmpInfoHeader.bitspp*(double)bmpInfoHeader.width / 32.0;
				double rowSize = ceil(ceilArg)*4;
				//pixelArraySize:
				double absHeight = fabs((double)bmpInfoHeader.height);
				long pixelArraySize = rowSize * absHeight;
				cout <<"Calculcated pixel array size: " << pixelArraySize << "\n";

				//should insert check here to make sure calculated size = header suggested size

				//position the file seek pointer
				bmpFile.seekg(bmHeader.bmp_offset);

				//read the bitmap data
				unsigned char* byteArray = new unsigned char[pixelArraySize];
				bmpFile.read((char*)byteArray,sizeof(char)*pixelArraySize);

				//extract bitmap data from the byte array, use it to calculate a greyscale value
				//at each x y coordinate, and then save the resulting greyscale value in a 2D array
				long x = 0;
				long y = 0;
				long rowSizeL = (long)rowSize;
				long width = bmpInfoHeader.width;
				long height = (long)absHeight;
				unsigned char tempBlue = 0;
				unsigned char tempGreen = 0;
				unsigned char tempRed = 0;

				//initialize the greyscale 2D array
				unsigned char* outputArray = new unsigned char[width * height * 3];
				//iterate through byteArray and copy the bgr values to colorArray
				while (y < height)
				{
					while (x < width)
					{
						tempBlue = byteArray[y * rowSizeL + x * 3];
						tempGreen = byteArray[y * rowSizeL + x * 3 + 1];
						tempRed = byteArray[y *rowSizeL + x * 3 + 2];
						outputArray[y * rowSizeL + x * 3] = tempRed;
            outputArray[y * rowSizeL + x * 3 + 1] = tempGreen;
            outputArray[y * rowSizeL + x * 3 + 2] = tempBlue;
						x++;
					}
					x=0;
					y++;
				}
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawPixels(width , height, GL_RGB, GL_UNSIGNED_BYTE, outputArray );

        glutSwapBuffers();

				//write the output to a file
				outFile.write((char *)outputArray, sizeof(char)*width*height*3);
			}
			else
			{
				cout<<"BitmapInfoHeader Version not supported";
			}
		}
		else
		{
			cout<<"File is not a bitmap image.  Magic Number not equal to ascii code \"BM\"";
		}
	}
	bmpFile.close();
	outFile.close();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(100,100);
    glutCreateWindow("OpenGL - First window demo");
    glutDisplayFunc(renderFunction);
    glutMainLoop();
    return 0;
}
