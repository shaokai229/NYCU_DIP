/**************************************************************************/
// FILE NAME: Low_luminosity_Enhancement.c
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
//Gamma correction
int Gamme_correction(int in,float Gamma_coef){ //a = input , b = bits 
	float temp;
	int o;
	temp = in;
	temp = pow((temp / 255),Gamma_coef) *255 ; 
	o = (uint8_t)temp;
	return o;
}
int main() {
	FILE *fp_in;
	FILE *fp_out;
	FILE *fp_out_2;
	FILE *fp_out_3;
	FILE *fp2;  //  creating a file
	FILE *fp3;  //  creating a file	

	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab2\\2023DIPHW2\\input1.bmp", "rb");//讀取檔案FILE
	fp_out = fopen("C:\\Users\\shaokai\\DIP\\lab2\\output1.bmp", "wb");//寫入檔案FILE
	fp_out_2 = fopen("C:\\Users\\shaokai\\DIP\\lab2\\test_RGB2YCBCR2RGB.bmp", "wb");//寫入檔案FILE
	fp_out_3 = fopen("C:\\Users\\shaokai\\DIP\\lab2\\Ygamma.bmp", "wb");//寫入檔案FILE

	fp2 = fopen("C:\\Users\\shaokai\\DIP\\lab2\\output_file_hiscount.txt","w");   //  creating an output file with writing permissions
	fp3 = fopen("C:\\Users\\shaokai\\DIP\\lab2\\output_file_hiscount_2.txt","w");   //  creating an output file with writing permissions


	if (!fp_in) printf("bmp fail...\n");
	if (!fp_out) printf("output.bmp fail...\n"); // 確認讀檔寫檔
	if (!fp_out_2) printf("output.bmp fail...\n");

	fread(&fileHeader,sizeof(BITMAPFILEHEADER),1,fp_in);//讀取檔案頭
	fread(&infoHeader,sizeof(BITMAPINFOHEADER),1,fp_in);//讀取資訊頭

	unsigned int W = infoHeader.biWidth; // 寬
	unsigned int H = infoHeader.biHeight; // 長
	unsigned int P = infoHeader.biBitCount ; 
	int isRGBA = (P == 32); // 1 = RGBA; 0 = RGB
	int scan;


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
	// 統計histogram
	int hist[256];
	for (i = 0; i < 256; i++) {
        hist[i]=0;
    }
    for (i = 0; i < H; i++) {
        for (j = 0; j < W; j++) {
			struct RGB data = {color[i][j].rgbtRed, color[i][j].rgbtGreen, color[i][j].rgbtBlue };
			struct YCbCr value = RGBToYCbCr(data);  // value.Y,  value.Cb,  value.Cr
			hist[(int)value.Y]++;
        }
    }
	// print 出來 做檢查
	for(scan = 0; scan < 256; scan++)
	{
		fprintf(fp2," %d\n", hist[scan]);    //  printing frequencies with their respective intensities in output file
	}
	// 重新做map
	int totalPixels = W * H;
	int cumulativeHistogram[256] = {0};
	cumulativeHistogram[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cumulativeHistogram[i] = cumulativeHistogram[i - 1] + hist[i];
    }

  	for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
			// calculate YCbCr
			struct RGB data = {color[i][j].rgbtRed, color[i][j].rgbtGreen, color[i][j].rgbtBlue };
			struct YCbCr value = RGBToYCbCr(data);  // value.Y,  value.Cb,  value.Cr 
            int intensity = (int)value.Y;
			// new Y
            int newIntensity = (int)(((float)cumulativeHistogram[intensity] / totalPixels) * 255.0);
			value.Y = newIntensity;
			struct YCbCr new_YCbCr = {value.Y , value.Cb, value.Cr};
			struct RGB new_RGB = YCbCrToRGB(new_YCbCr);
			rgb.rgbtBlue = new_RGB.B;
			rgb.rgbtGreen = new_RGB.G;
			rgb.rgbtRed = new_RGB.R;

			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out);        // 將 color matrix 寫入輸出圖檔
        }
    }
	fclose(fp_out);
	// test RGB to YCbCr to RGB 
	/*
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_2); //輸出檔案
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_2);
	for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
			// calculate YCbCr
			struct RGB data_2 = {color[i][j].rgbtRed, color[i][j].rgbtGreen, color[i][j].rgbtBlue };
			struct YCbCr value_2 = RGBToYCbCr(data_2);  // value.Y,  value.Cb,  value.Cr 

			struct RGB new_RGB_2 = YCbCrToRGB(value_2);

			rgb.rgbtBlue = new_RGB_2.B;
			rgb.rgbtGreen = new_RGB_2.G;
			rgb.rgbtRed = new_RGB_2.R;

			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out_2);        // 將 color matrix 寫入輸出圖檔
        }
    }
	fclose(fp_out_2);
	*/
	// Gamma
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_3); //輸出檔案
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_3);
	for(int i=0; i<H; i++ )       
	{
		for(int j=0; j < W; j++ )  
		{
		struct RGB data_G = {color[i][j].rgbtRed, color[i][j].rgbtGreen, color[i][j].rgbtBlue };
		struct YCbCr value_G = RGBToYCbCr(data_G);  // value.Y,  value.Cb,  value.Cr 
		int intensity_Gamma = (int)value_G.Y;
		int newIntensity_Gamma;
        newIntensity_Gamma = Gamme_correction (intensity_Gamma, 0.4); //Gamma correction
		value_G.Y = newIntensity_Gamma;
		struct YCbCr new_YCbCr_Gamma = {value_G.Y , value_G.Cb, value_G.Cr};
		struct RGB new_RGB_Gamma = YCbCrToRGB(new_YCbCr_Gamma);
		rgb.rgbtBlue = new_RGB_Gamma.B;
		rgb.rgbtGreen = new_RGB_Gamma.G;
		rgb.rgbtRed = new_RGB_Gamma.R;

		fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out_3);        // 將 color matrix 寫入輸出圖檔中
		}
	}

	fclose(fp_out_3);

    return 0;

}