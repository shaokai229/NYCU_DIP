/**************************************************************************/
// FILE NAME: Sharpness_Enhancement.c
// VERSRION: 1.0
// DATE: 2023/11/03
// AUTHOR: Shao-Kai,Lu
// CODE TYPE: C
// DESCRIPTION: 2023 Fall Digital Image Procssing / HW2
/**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <stdint.h>
int i,j,u,v,x,y,a;
// Laplace filter
int laplace1[3][3] = { {0, 1, 0}, {1, -4, 1}, {0, 1, 0} };
int laplace2[3][3] = { {1, 1, 1}, {1, -8, 1}, {1, 1, 1} };
int laplace3[3][3] = { {0, -1, 0}, {-1, 4, -1}, {0, -1, 0} };
int laplace4[3][3] = { {-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1} };

int laplace5[3][3] = { {0, -1, 0}, {-1, 5, -1}, {0, -1, 0} };
int laplace6[3][3] = { {-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1} };
struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

struct YCbCr
{
	float Y;
	float Cb;
	float Cr;
};
// RGB 轉 YCbCr
struct YCbCr RGBToYCbCr(struct RGB rgb) {
	float fr = (float)rgb.R ;
	float fg = (float)rgb.G ;
	float fb = (float)rgb.B ;

	struct YCbCr ycbcr;
	ycbcr.Y = (float)(0.2989 * fr + 0.5866 * fg + 0.1145 * fb);
	ycbcr.Cb = (float)(-0.1687 * fr - 0.3313 * fg + 0.5000 * fb) + 128;
	ycbcr.Cr = (float)(0.5000 * fr - 0.4184 * fg - 0.0816 * fb) + 128;

	return ycbcr;
}
// YCbCr 轉 RGB
struct RGB YCbCrToRGB(struct YCbCr ycbcr) {
    struct RGB rgb;

    float Y = ycbcr.Y;
    float Cb = ycbcr.Cb;
    float Cr = ycbcr.Cr;

    int red = Y + (int)(1.402 * (Cr - 128));
    int green = Y - (int)(0.344136 * (Cb - 128) + 0.714136 * (Cr - 128));
    int blue = Y + (int)(1.772 * (Cb - 128));


    rgb.R = (int)(red < 0 ? 0 : red > 255 ? 255 : red);
    rgb.G = (int)(green < 0 ? 0 : green > 255 ? 255 : green);
    rgb.B = (int)(blue < 0 ? 0 : blue > 255 ? 255 : blue);

    return rgb;
}

int main() {
	FILE *fp_in;
	FILE *fp_out;
	FILE *fp_out_2;

	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab2\\2023DIPHW2\\input2.bmp", "rb");//讀取檔案FILE
	fp_out = fopen("C:\\Users\\shaokai\\DIP\\lab2\\output2_1.bmp", "wb");//寫入檔案FILE
	fp_out_2 = fopen("C:\\Users\\shaokai\\DIP\\lab2\\output2_2.bmp", "wb");//寫入檔案FILE

	if (!fp_in) printf("bmp fail...\n");
	if (!fp_out) printf("output.bmp fail...\n"); // 確認讀檔寫檔
	if (!fp_out_2) printf("output.bmp fail...\n");

	fread(&fileHeader,sizeof(BITMAPFILEHEADER),1,fp_in);//讀取檔案頭
	fread(&infoHeader,sizeof(BITMAPINFOHEADER),1,fp_in);//讀取資訊頭

	unsigned int W = infoHeader.biWidth; // 寬
	unsigned int H = infoHeader.biHeight; // 長
	unsigned int P = infoHeader.biBitCount ; 
	int isRGBA = (P == 32); // 1 = RGBA; 0 = RGB

	RGBTRIPLE rgb;
	RGBTRIPLE(*color)[W] = calloc(H,W* sizeof(RGBTRIPLE)); // 創建memory

	printf("%d \n",W );
	printf("%d \n",H );
	printf("%d \n",P );
	//--------------------------- RGB --------------------------------

	for(i=0; i<H; i++ ) {      
		for( j=0; j<W; j++  ) {  
			fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in);
			color[i][j].rgbtBlue=rgb.rgbtBlue;
			color[i][j].rgbtGreen=rgb.rgbtGreen;
			color[i][j].rgbtRed=rgb.rgbtRed;
		}
	}
	fclose(fp_in);     //檔案fp讀取完成，關閉

	/** ##### 輸出圖檔 ##### */
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out); //輸出檔案
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);
	struct YCbCr value;
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			int sum_Y = 0;
			if(i == 0 || j == 0|| i== H-1|| j == W-1){
				struct RGB data = {color[i][j].rgbtRed, color[i][j].rgbtGreen, color[i][j].rgbtBlue};
				value = RGBToYCbCr(data);  // value.Y, value.Cb, value.Cr
				sum_Y = value.Y;
			}
			else{
				// convolution
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
					int new_i = i + x;
					int new_j = j + y;

					int new_i_padded = new_i;
					int new_j_padded = new_j;
					// zero padding
					if (new_i_padded < 0 || new_i_padded >= H || new_j_padded < 0 || new_j_padded >= W) {
						value.Y = 0;
					} else {
						struct RGB data = {color[new_i_padded][new_j_padded].rgbtRed, color[new_i_padded][new_j_padded].rgbtGreen, color[new_i_padded][new_j_padded].rgbtBlue};
						value = RGBToYCbCr(data);  // value.Y, value.Cb, value.Cr
					}
					// mult and sum
					int intensity = (int)value.Y;
					sum_Y += intensity * laplace5[x + 1][y + 1];
					}
				}
			}
			// condition 0~255

			int newIntensity = sum_Y;
			
			if(newIntensity>255)
				newIntensity = 255;
			else if(newIntensity <0)
				newIntensity = 0;
			struct YCbCr new_YCbCr = {newIntensity , value.Cb, value.Cr};
			struct RGB new_RGB = YCbCrToRGB(new_YCbCr);
			rgb.rgbtBlue = new_RGB.B;
			rgb.rgbtGreen = new_RGB.G;
			rgb.rgbtRed = new_RGB.R;

			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out);
		}
	}
	fclose(fp_out);
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_2); //輸出檔案
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_2);

	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			int sum_Y = 0;
			if(i == 0 || j == 0|| i== H-1|| j == W-1){
				struct RGB data = {color[i][j].rgbtRed, color[i][j].rgbtGreen, color[i][j].rgbtBlue};
				value = RGBToYCbCr(data);  // value.Y, value.Cb, value.Cr
				sum_Y = value.Y;
			}
			else{
				// convolution
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
					int new_i = i + x;
					int new_j = j + y;

					int new_i_padded = new_i;
					int new_j_padded = new_j;
					// zero padding
					if (new_i_padded < 0 || new_i_padded >= H || new_j_padded < 0 || new_j_padded >= W) {
						value.Y = 0;
					} else {
						struct RGB data = {color[new_i_padded][new_j_padded].rgbtRed, color[new_i_padded][new_j_padded].rgbtGreen, color[new_i_padded][new_j_padded].rgbtBlue};
						value = RGBToYCbCr(data);  // value.Y, value.Cb, value.Cr
					}
					// mult and sum
					int intensity = (int)value.Y;
					sum_Y += intensity * laplace6[x + 1][y + 1];
					}
				}
			}
			// condition 0~255

			int newIntensity = sum_Y;
			
			if(newIntensity>255)
				newIntensity = 255;
			else if(newIntensity <0)
				newIntensity = 0;
			struct YCbCr new_YCbCr = {newIntensity , value.Cb, value.Cr};
			struct RGB new_RGB = YCbCrToRGB(new_YCbCr);
			rgb.rgbtBlue = new_RGB.B;
			rgb.rgbtGreen = new_RGB.G;
			rgb.rgbtRed = new_RGB.R;

			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out_2);
		}
	}

	fclose(fp_out_2);

    return 0;

}