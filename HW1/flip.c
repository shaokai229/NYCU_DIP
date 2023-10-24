#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
int i,j,u,v,x,y,a;
int temp_R,temp_G,temp_B;
typedef struct {
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
    BYTE rgbtAlpha; 
} RGBA;

int main() {
	FILE *fp_in;
	FILE *fp_out;

	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab1\\input2.bmp", "rb");//讀取檔案FILE
	fp_out = fopen("C:\\Users\\shaokai\\DIP\\lab1\\output2_flip.bmp", "wb");//寫入檔案FILE

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

	RGBA rgba;
	RGBA(*color_4)[W] = calloc(H,W* sizeof(RGBA)); // 創建memory

	printf("%d \n",W );
	printf("%d \n",H );
	printf("%d \n",P );
	//--------------------------- RGB --------------------------------
	if(!isRGBA){
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

		for(int i=0; i<H; i++ )       
		{
			for(int j=0; j < W; j++ )  
			{	//以每row為單位，進行左右交換
				rgb.rgbtBlue = color[i][W - j - 1].rgbtBlue;
				rgb.rgbtGreen = color[i][W - j - 1].rgbtGreen;
				rgb.rgbtRed = color[i][W - j - 1].rgbtRed;

				fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out);        // 將 color matrix 寫入輸出圖檔中
			}
		}
		fclose(fp_out);
	}
	//--------------------------- RGBA --------------------------------
	else{
		for(i=0; i<H; i++ ) {      
			for( j=0; j<W; j++  ) {  
				fread(&rgba, sizeof(RGBA), 1, fp_in);
				color_4[i][j].rgbtBlue=rgba.rgbtBlue;
				color_4[i][j].rgbtGreen=rgba.rgbtGreen;
				color_4[i][j].rgbtRed=rgba.rgbtRed;
				color_4[i][j].rgbtAlpha=rgba.rgbtAlpha;
			}
		}
		fclose(fp_in);     //檔案fp讀取完成，關閉

		/** ##### 輸出圖檔 ##### */
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);

		for(int i=0; i<H; i++ )       
		{
			for(int j=0; j < W; j++ )  
			{	//以每row為單位，進行左右交換		
				rgba.rgbtBlue = color_4[i][W - j - 1].rgbtBlue;
				rgba.rgbtGreen = color_4[i][W - j - 1].rgbtGreen;
				rgba.rgbtRed = color_4[i][W - j - 1].rgbtRed;
				rgba.rgbtAlpha = color_4[i][W - j - 1].rgbtAlpha;

				fwrite(&rgba, sizeof(RGBA), 1, fp_out);        // 將 color matrix 寫入輸出圖檔中
			}
		}
		fclose(fp_out);
	}

    return 0;

}