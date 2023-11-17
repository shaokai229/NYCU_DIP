/**************************************************************************/
// FILE NAME: denoise.c
// VERSRION: 1.0
// DATE: 2023/10/31
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
//Gaussian filter
int Gaussian[3][3] = { {1 , 2 , 1 }, {2 , 4 , 2 }, {1 , 2 , 1 } };
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
// Median filter
int median(int arr[3][3]) {
    int temp[9];
    int k = 0;
	// input matrix to temp metrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[k] = arr[i][j];
            k++;
        }
    }

    // bubble sort
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (temp[j] > temp[j + 1]) {
                // change
                int tmp = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = tmp;
            }
        }
    }

    // return median
    return temp[4];
}
int main() {
	FILE *fp_in;
	FILE *fp_out;
	FILE *fp_out_2;

	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab2\\2023DIPHW2\\input3.bmp", "rb");//讀取檔案FILE
	fp_out = fopen("C:\\Users\\shaokai\\DIP\\lab2\\output3_1.bmp", "wb");//寫入檔案FILE
	fp_out_2 = fopen("C:\\Users\\shaokai\\DIP\\lab2\\output3_2.bmp", "wb");//寫入檔案FILE

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
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
			int neighborhood_Y[3][3];
			int neighborhood_Cb[3][3];
			int neighborhood_Cr[3][3];
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    int ni = i + x;
                    int nj = j + y;
					// zero padding
                    if (ni < 0 || ni >= H || nj < 0 || nj >= W) {
                        neighborhood_Y[x + 1][y + 1] = 0;
						neighborhood_Cb[x + 1][y + 1] = 0;
						neighborhood_Cr[x + 1][y + 1] = 0;
                    } else {
						// fill in 
						struct RGB data = {color[ni][nj].rgbtRed, color[ni][nj].rgbtGreen, color[ni][nj].rgbtBlue};
						value = RGBToYCbCr(data);  // value.Y, value.Cb, value.Cr
                        neighborhood_Y[x + 1][y + 1] = value.Y;
						neighborhood_Cb[x + 1][y + 1] = value.Cb;
						neighborhood_Cr[x + 1][y + 1] = value.Cr;
                    }
                }
            }
			// median sort
			int new_R = median(neighborhood_Y);
			int newCb = median(neighborhood_Cb);
			int newCr =  median(neighborhood_Cr);
			//printf("%d \n", new_R );
			if(new_R>255)
				new_R = 255;
			else if(new_R <0)
				new_R = 0;
			if(newCb>255)
				newCb = 255;
			else if(newCb <0)
				newCb = 0;
			if(newCr>255)
				newCr = 255;
			else if(newCr <0)
				newCr = 0;				
			struct YCbCr new_YCbCr = {new_R , newCb, newCr};
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
	struct RGB data_RGB;
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			float sum_R = 0;
			float sum_G = 0;
			float sum_B = 0;
			if(i == 0 || j == 0|| i== H-1|| j == W-1){
				data_RGB.R = color[i][j].rgbtRed;
				data_RGB.G = color[i][j].rgbtGreen;
				data_RGB.B = color[i][j].rgbtBlue;
				sum_R = data_RGB.R;
				sum_G = data_RGB.G;
				sum_B = data_RGB.B;
			}
			else{
				
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
					int new_i = i + x;
					int new_j = j + y;

					int new_i_padded = new_i;
					int new_j_padded = new_j;
					// zero padding
					if (new_i_padded < 0 || new_i_padded >= H || new_j_padded < 0 || new_j_padded >= W) {
						data_RGB.R = 0;
						data_RGB.G = 0;
						data_RGB.B = 0;
					} else {
						data_RGB.R = color[new_i_padded][new_j_padded].rgbtRed;
						data_RGB.G = color[new_i_padded][new_j_padded].rgbtGreen;
						data_RGB.B = color[new_i_padded][new_j_padded].rgbtBlue;
					}

					int R_temp = data_RGB.R;
					int G_temp = data_RGB.G;
					int B_temp = data_RGB.B;
					// mult and sum
					sum_R += (1.0 / 16.0) * R_temp * Gaussian[x + 1][y + 1];
					sum_G += (1.0 / 16.0) * G_temp * Gaussian[x + 1][y + 1];
					sum_B += (1.0 / 16.0) * B_temp * Gaussian[x + 1][y + 1];
					}
				}
			}
			// condition 0~255

			int new_R = sum_R;
			int new_G = sum_G;
			int new_B = sum_B;
			if(new_R>255)
				new_R = 255;
			else if(new_R <0)
				new_R = 0;
			if(new_G>255)
				new_G = 255;
			else if(new_G <0)
				new_G = 0;
			if(new_B>255)
				new_B = 255;
			else if(new_B <0)
				new_B = 0;				
			rgb.rgbtBlue = new_B;
			rgb.rgbtGreen = new_G;
			rgb.rgbtRed = new_R;

			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out_2);
		}
	}
	fclose(fp_out_2);	
    return 0;

}