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
// 將0~255個數字表現，分別去降成64個數字, 16個數字, 4個數字來表現整個0~255
// 先右移再左移，目的是為了將比較小的成分扣掉，我們自己手洞補這個基礎值
// 所以有了+1, +7, +31，讓數字可以映射至我們希望的數值。
int quant_resolution(int a,int b){ //a = input , b = bits 
	int o;
	if(b == 6) // bit = 6
		o = ((a>>2)<<2) + 1 ;
	else if(b == 4) // bit = 4
		o= ((a>>4)<<4) + 7 ;
	else // bit = 2
		o= ((a>>6)<<6) + 31;
	return o;
}
int main() {
	FILE *fp_in;
	FILE *fp_out_1;
	FILE *fp_out_2;
	FILE *fp_out_3;

	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab1\\input2.bmp", "rb");//讀取檔案FILE
	fp_out_1 = fopen("C:\\Users\\shaokai\\DIP\\lab1\\output2_1.bmp", "wb");//寫入檔案FILE
	fp_out_2 = fopen("C:\\Users\\shaokai\\DIP\\lab1\\output2_2.bmp", "wb");//寫入檔案FILE
	fp_out_3 = fopen("C:\\Users\\shaokai\\DIP\\lab1\\output2_3.bmp", "wb");//寫入檔案FILE

	if (!fp_in) printf("bmp fail...\n");
	if (!fp_out_1) printf("output.bmp fail...\n");
	if (!fp_out_2) printf("output.bmp fail...\n");
	if (!fp_out_3) printf("output.bmp fail...\n");// 確認讀檔寫檔

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
		// ------------------------------------------6bit
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_1); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_1);
        
		for(int i=0; i<H; i++ )       
		{
			for(int j=0; j < W; j++ )  
			{	
				
				rgb.rgbtBlue = quant_resolution(color[i][j].rgbtBlue,6);
				rgb.rgbtGreen = quant_resolution(color[i][j].rgbtGreen,6);
				rgb.rgbtRed = quant_resolution(color[i][j].rgbtRed,6);

				fwrite(&rgb , sizeof(RGBTRIPLE), 1, fp_out_1);        // 將 color matrix 寫入輸出圖檔中
				 
			}
		}
       
		fclose(fp_out_1);
		// ------------------------------------------4bit
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_2); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_2);
        
		for(int i=0; i<H; i++ )       
		{
			for(int j=0; j < W; j++ )  
			{			
				rgb.rgbtBlue = quant_resolution(color[i][j].rgbtBlue,4);
				rgb.rgbtGreen = quant_resolution(color[i][j].rgbtGreen,4);
				rgb.rgbtRed = quant_resolution(color[i][j].rgbtRed,4);

				fwrite(&rgb , sizeof(RGBTRIPLE), 1, fp_out_2);        // 將 color matrix 寫入輸出圖檔中
				 
			}
		}
       
		fclose(fp_out_2);
		// 2bit
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_3); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_3);
        
		for(int i=0; i<H; i++ )       
		{
			for(int j=0; j < W; j++ )  
			{		
				rgb.rgbtBlue = quant_resolution(color[i][j].rgbtBlue,2);
				rgb.rgbtGreen = quant_resolution(color[i][j].rgbtGreen,2);
				rgb.rgbtRed = quant_resolution(color[i][j].rgbtRed,2);

				fwrite(&rgb , sizeof(RGBTRIPLE), 1, fp_out_3);        // 將 color matrix 寫入輸出圖檔中
				 
			}
		}
       
		fclose(fp_out_3);
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
		// ------------------------------------------6bit
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_1); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_1);
        
		for(int i=0; i<H; i++ )       
		{
			for(int j=0; j < W; j++ )  
			{	
				
				rgba.rgbtBlue = quant_resolution(color_4[i][j].rgbtBlue,6);
				rgba.rgbtGreen = quant_resolution(color_4[i][j].rgbtGreen,6);
				rgba.rgbtRed = quant_resolution(color_4[i][j].rgbtRed,6);
				rgba.rgbtAlpha = quant_resolution(color_4[i][j].rgbtAlpha,6);

				fwrite(&rgba , sizeof(RGBA), 1, fp_out_1);        // 將 color matrix 寫入輸出圖檔中
				 
			}
		}
       
		fclose(fp_out_1);
		// ------------------------------------------4bit
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_2); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_2);
        
		for(int i=0; i<H; i++ )       
		{
			for(int j=0; j < W; j++ )  
			{			
				rgba.rgbtBlue = quant_resolution(color_4[i][j].rgbtBlue,4);
				rgba.rgbtGreen = quant_resolution(color_4[i][j].rgbtGreen,4);
				rgba.rgbtRed = quant_resolution(color_4[i][j].rgbtRed,4);
				rgba.rgbtAlpha = quant_resolution(color_4[i][j].rgbtAlpha,4);

				fwrite(&rgba , sizeof(RGBA), 1, fp_out_2);        // 將 color matrix 寫入輸出圖檔中
				 
			}
		}
       
		fclose(fp_out_2);
		// 2bit
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_3); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_3);
        
		for(int i=0; i<H; i++ )       
		{
			for(int j=0; j < W; j++ )  
			{		
				rgba.rgbtBlue = quant_resolution(color_4[i][j].rgbtBlue,2);
				rgba.rgbtGreen = quant_resolution(color_4[i][j].rgbtGreen,2);
				rgba.rgbtRed = quant_resolution(color_4[i][j].rgbtRed,2);
				rgba.rgbtAlpha = quant_resolution(color_4[i][j].rgbtAlpha,2);

				fwrite(&rgba , sizeof(RGBA), 1, fp_out_3);        // 將 color matrix 寫入輸出圖檔中
				 
			}
		}
       
		fclose(fp_out_3);
	}

    return 0;

}