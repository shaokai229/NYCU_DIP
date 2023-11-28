/**************************************************************************/
//##########################################################################
// FILE NAME: Chromatic Adaptation.c
// VERSRION: 1.0
// DATE: 2023/11/17
// AUTHOR: Shao-Kai,Lu
// CODE TYPE: C
// DESCRIPTION: 2023 Fall Digital Image Procssing / HW3
//##########################################################################
/**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <stdint.h>
int i,j,u,v,x,y,a;

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

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab3\\2023DIPHW3\\input2.bmp", "rb");//讀取檔案FILE
	fp_out = fopen("C:\\Users\\shaokai\\DIP\\lab3\\output2_1.bmp", "wb");//寫入檔案FILE (Grey world)
	fp_out_2 = fopen("C:\\Users\\shaokai\\DIP\\lab3\\output2_1_m2.bmp", "wb");//寫入檔案FILE (Max-RGB)
	
	if (!fp_in) printf("bmp fail...\n");
	if (!fp_out) printf("output.bmp fail...\n"); // 確認讀檔寫檔

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
	float total_R;
	float total_G;
	float total_B;
	float mean_R;
	float mean_G;
	float mean_B;
	float mean_gray;
	float R_coef, G_coef, B_coef;
	// step 1. calculate each RGB channel's average value
	for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
			total_R = color[i][j].rgbtRed + total_R;
			total_G = color[i][j].rgbtGreen + total_G;
			total_B = color[i][j].rgbtBlue + total_B;
		}
	}
	mean_R = total_R / (H*W);
	mean_G = total_G / (H*W);
	mean_B = total_B / (H*W);
	mean_gray = (mean_R+ mean_G +mean_B) / 3;
	// Step 2: calculate each channel's adjustment coefficient 
	R_coef = mean_gray / mean_R;
	G_coef = mean_gray / mean_G;	
	B_coef = mean_gray / mean_B;
	printf("Gray mean: %f \n",mean_gray );
	printf("R mean: %f \n",mean_R );
	printf("G mean: %f \n",mean_G );
	printf("B mean: %f \n",mean_B );
	// step 3. adjustment
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
		float temp_B, temp_G, temp_R;
		temp_B = color[i][j].rgbtBlue * B_coef;
		temp_B =(int)(temp_B < 0 ? 0 : temp_B > 255 ? 255 : temp_B);

		temp_G = color[i][j].rgbtGreen * G_coef;
		temp_G =(int)(temp_G < 0 ? 0 : temp_G > 255 ? 255 : temp_G);

		temp_R = color[i][j].rgbtRed * R_coef;
		temp_R =(int)(temp_R < 0 ? 0 : temp_R > 255 ? 255 : temp_R);		
		rgb.rgbtBlue = temp_B;
		rgb.rgbtGreen = temp_G;
		rgb.rgbtRed = temp_R;
		fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out);
        }
    }
	fclose(fp_out);
	// (White Patch) max RGB method
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_2); //輸出檔案
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_2);
	int max_R = 0 ,max_G = 0,max_B = 0, Max = 0;;
	int temp_max_R, temp_max_G, temp_max_B;
	float temp_B, temp_G, temp_R;
	// step 1. find max
	for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
			temp_max_R = color[i][j].rgbtRed;
			if(temp_max_R > max_R)
			{
				max_R = temp_max_R;
			}
			temp_max_G = color[i][j].rgbtGreen;
			if(temp_max_G > max_G)
			{
				max_G = temp_max_G;
			}
			temp_max_B = color[i][j].rgbtBlue;
			if(temp_max_B > max_B)
			{
				max_B = temp_max_B;
			}

			int currentMax = color[i][j].rgbtRed;
            if (color[i][j].rgbtGreen > currentMax) {
                currentMax = color[i][j].rgbtGreen;
            }
            if (color[i][j].rgbtBlue > currentMax) {
                currentMax = color[i][j].rgbtBlue;
            }
            if (currentMax > Max) {
                Max = currentMax;
            }
		}
	}
	float k[3] = { (float)max_R / Max, (float)max_G / Max, (float)max_B / Max };
	for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {

            // Step 2: Adjust each channel based on the maximum value
			//temp_R = (float)color[i][j].rgbtRed / k[0];
            temp_R = (float)color[i][j].rgbtRed / max_R * 255;
			temp_R = (int)(temp_R < 0 ? 0 : temp_R > 255 ? 255 : temp_R);
			
			//temp_G = (float)color[i][j].rgbtGreen / k[1];
            temp_G = (float)color[i][j].rgbtGreen / max_G * 255;
			temp_G = (int)(temp_G < 0 ? 0 : temp_G > 255 ? 255 : temp_G);

			//temp_B = (float)color[i][j].rgbtBlue / k[2];
            temp_B = (float)color[i][j].rgbtBlue / max_B * 255;
			temp_B = (int)(temp_B < 0 ? 0 : temp_B > 255 ? 255 : temp_B);

			rgb.rgbtBlue = temp_B;
			rgb.rgbtGreen = temp_G;
			rgb.rgbtRed = temp_R;
			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out_2);
        }
    }
    return 0;
}