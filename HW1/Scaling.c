#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <stdint.h>
int i,j,u,v;


typedef struct {
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
    BYTE rgbtAlpha; 
} RGBA;
int main() {
    FILE *fp_in;
	FILE *fp_out;
	FILE *fp_out_2;

	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab1\\input2.bmp", "rb");//讀取檔案FILE
	fp_out = fopen("C:\\Users\\shaokai\\DIP\\lab1\\output2_down.bmp", "wb");//寫入檔案FILE
	fp_out_2 = fopen("C:\\Users\\shaokai\\DIP\\lab1\\output2_up.bmp", "wb");//寫入檔案FILE

	if (!fp_in) printf("bmp fail...\n");
	if (!fp_out) printf("output.bmp fail...\n");

	fread(&fileHeader,sizeof(BITMAPFILEHEADER),1,fp_in);//讀取檔案頭
	fread(&infoHeader,sizeof(BITMAPINFOHEADER),1,fp_in);//讀取資訊頭

	unsigned int W = infoHeader.biWidth; // 寬
	unsigned int H = infoHeader.biHeight; // 長
	unsigned int P = infoHeader.biBitCount ;

	//calulate scaling down W
    double temp_down_W = W / 1.5; 
	unsigned int new_down_W = (int)temp_down_W;
	while(new_down_W % 4 != 0 ){
		new_down_W = new_down_W - 1;
	}
	//calulate scaling down H
	unsigned int new_down_H = H / 1.5; 

	//calulate scaling up W
	double temp_up_W = W * 1.5; 
	unsigned int new_up_W = (int)temp_up_W;
	while(new_up_W % 4 != 0 ){
		new_up_W = new_up_W - 1;
	}
	//calulate scaling up H
	unsigned int new_up_H = H * 1.5; 

	//decide RGBA or RGB
	int isRGBA = (P == 32); // 1 = RGBA; 0 = RGB

    RGBTRIPLE rgb;
	RGBTRIPLE(*color)[W] = calloc(H,W* sizeof(RGBTRIPLE)); // 創建memory

	RGBA rgba;
	RGBA(*color_4)[W] = calloc(H,W* sizeof(RGBA)); // 創建memory
    printf("W = %d \n",W );
	printf("H = %d \n",H );
	printf("RGBA or RGB bits:%d \n",P );

    printf("new down W = %d \n",new_down_W );
	printf("new down H = %d \n",new_down_H );

	printf("new up W = %d \n",new_up_W );
	printf("new up H = %d \n",new_up_H );


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
		// down
		infoHeader.biWidth = new_down_W;
		infoHeader.biHeight = new_down_H;	
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);
		
		for (int y = 0; y < new_down_H; y++) {
        	for (int x = 0; x < new_down_W; x++) {
			// 先計算出bilinear所需要的參數
            // origin
            double originalX = (double)x * 1.5; 
            double originalY = (double)y * 1.5;

            int x1 = floor(originalX);
            int y1 = floor(originalY);

            int x2 = ceil(originalX);
            int y2 = ceil(originalY);

			if (x2 >= W-1) x2 = W - 1;
            if (y2 >= H-1) y2 = H - 1;

            // bilinear
            double dx = originalX - x1;
            double dy = originalY - y1;
			double temp_blue,temp_green,temp_red;

            
			temp_blue = (1 - dx) * (1 - dy) * color[y1][x1].rgbtBlue
                + dx * (1 - dy) * color[y1][x2].rgbtBlue
                + (1 - dx) * dy * color[y2][x1].rgbtBlue
                + dx * dy * color[y2][x2].rgbtBlue;	
			rgb.rgbtBlue  = (uint8_t)temp_blue;

			 
			temp_green = (1 - dx) * (1 - dy) * color[y1][x1].rgbtGreen
                + dx * (1 - dy) * color[y1][x2].rgbtGreen
                + (1 - dx) * dy * color[y2][x1].rgbtGreen
                + dx * dy * color[y2][x2].rgbtGreen;
			rgb.rgbtGreen = (uint8_t)temp_green;

			
			temp_red= (1 - dx) * (1 - dy) * color[y1][x1].rgbtRed
                + dx * (1 - dy) * color[y1][x2].rgbtRed
                + (1 - dx) * dy * color[y2][x1].rgbtRed
                + dx * dy * color[y2][x2].rgbtRed;
			rgb.rgbtRed = (uint8_t)temp_red;

			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out);        // 將 color matrix 寫入輸出圖檔中
			}
        }
		fclose(fp_out);

		// up // 更新新的size
		infoHeader.biWidth = new_up_W;
		infoHeader.biHeight = new_up_H;	
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_2); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_2);
		
		for (int y = 0; y < new_up_H; y++) {
        	for (int x = 0; x < new_up_W; x++) {
			// 先計算出bilinear所需要的參數
            // origin
            double originalX = (double)x / 1.5; 
            double originalY = (double)y / 1.5;

            int x1 = floor(originalX);
            int y1 = floor(originalY);

            int x2 = ceil(originalX);
            int y2 = ceil(originalY);

			if (x2 >= W-1) x2 = W - 1;
            if (y2 >= H-1) y2 = H - 1;

            // bilinear
            double dx = originalX - x1;
            double dy = originalY - y1;
			double temp_blue,temp_green,temp_red;

            
			temp_blue = (1 - dx) * (1 - dy) * color[y1][x1].rgbtBlue
                + dx * (1 - dy) * color[y1][x2].rgbtBlue
                + (1 - dx) * dy * color[y2][x1].rgbtBlue
                + dx * dy * color[y2][x2].rgbtBlue;	
			rgb.rgbtBlue  = (uint8_t)temp_blue;

			 
			temp_green = (1 - dx) * (1 - dy) * color[y1][x1].rgbtGreen
                + dx * (1 - dy) * color[y1][x2].rgbtGreen
                + (1 - dx) * dy * color[y2][x1].rgbtGreen
                + dx * dy * color[y2][x2].rgbtGreen;
			rgb.rgbtGreen = (uint8_t)temp_green;

			
			temp_red= (1 - dx) * (1 - dy) * color[y1][x1].rgbtRed
                + dx * (1 - dy) * color[y1][x2].rgbtRed
                + (1 - dx) * dy * color[y2][x1].rgbtRed
                + dx * dy * color[y2][x2].rgbtRed;
			rgb.rgbtRed = (uint8_t)temp_red;

			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out_2);        // 將 color matrix 寫入輸出圖檔中
			}
        }
		fclose(fp_out_2);
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
		// down // 更新新的size
		infoHeader.biWidth = new_down_W;
		infoHeader.biHeight = new_down_H;	
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);
		
		for (int y = 0; y < new_down_H; y++) {
        	for (int x = 0; x < new_down_W; x++) {
			// 先計算出bilinear所需要的參數
            // origin
            double originalX = (double)x * 1.5; 
            double originalY = (double)y * 1.5;

            int x1 = floor(originalX);
            int y1 = floor(originalY);

            int x2 = ceil(originalX);
            int y2 = ceil(originalY);

			if (x2 >= W-1) x2 = W - 1;
            if (y2 >= H-1) y2 = H - 1;

            // bilinear
            double dx = originalX - x1;
            double dy = originalY - y1;
			double temp_blue,temp_green,temp_red, temp_Alpha;

            
			temp_blue = (1 - dx) * (1 - dy) * color_4[y1][x1].rgbtBlue
                + dx * (1 - dy) * color_4[y1][x2].rgbtBlue
                + (1 - dx) * dy * color_4[y2][x1].rgbtBlue
                + dx * dy * color_4[y2][x2].rgbtBlue;	
			rgba.rgbtBlue  = (uint8_t)temp_blue;

			 
			temp_green = (1 - dx) * (1 - dy) * color_4[y1][x1].rgbtGreen
                + dx * (1 - dy) * color_4[y1][x2].rgbtGreen
                + (1 - dx) * dy * color_4[y2][x1].rgbtGreen
                + dx * dy * color_4[y2][x2].rgbtGreen;
			rgba.rgbtGreen = (uint8_t)temp_green;

			
			temp_red= (1 - dx) * (1 - dy) * color_4[y1][x1].rgbtRed
                + dx * (1 - dy) * color_4[y1][x2].rgbtRed
                + (1 - dx) * dy * color_4[y2][x1].rgbtRed
                + dx * dy * color_4[y2][x2].rgbtRed;
			rgba.rgbtRed = (uint8_t)temp_red;

			temp_Alpha= (1 - dx) * (1 - dy) * color_4[y1][x1].rgbtAlpha
                + dx * (1 - dy) * color_4[y1][x2].rgbtAlpha
                + (1 - dx) * dy * color_4[y2][x1].rgbtAlpha
                + dx * dy * color_4[y2][x2].rgbtAlpha;
			rgba.rgbtAlpha = (uint8_t)temp_Alpha;

			fwrite(&rgba, sizeof(RGBA), 1, fp_out);        // 將 color matrix 寫入輸出圖檔中
			}
        }
		fclose(fp_out);

		// up // 更新新的size
		infoHeader.biWidth = new_up_W;
		infoHeader.biHeight = new_up_H;	
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out_2); //輸出檔案
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out_2);
		
		for (int y = 0; y < new_up_H; y++) {
        	for (int x = 0; x < new_up_W; x++) {
			// 先計算出bilinear所需要的參數
            // origin
            double originalX = (double)x / 1.5; 
            double originalY = (double)y / 1.5;

            int x1 = floor(originalX);
            int y1 = floor(originalY);

            int x2 = ceil(originalX);
            int y2 = ceil(originalY);

			if (x2 >= W-1) x2 = W - 1;
            if (y2 >= H-1) y2 = H - 1;

			// bilinear
            double dx = originalX - x1;
            double dy = originalY - y1;
			double temp_blue,temp_green,temp_red, temp_Alpha;

            
			temp_blue = (1 - dx) * (1 - dy) * color_4[y1][x1].rgbtBlue
                + dx * (1 - dy) * color_4[y1][x2].rgbtBlue
                + (1 - dx) * dy * color_4[y2][x1].rgbtBlue
                + dx * dy * color_4[y2][x2].rgbtBlue;	
			rgba.rgbtBlue  = (uint8_t)temp_blue;

			 
			temp_green = (1 - dx) * (1 - dy) * color_4[y1][x1].rgbtGreen
                + dx * (1 - dy) * color_4[y1][x2].rgbtGreen
                + (1 - dx) * dy * color_4[y2][x1].rgbtGreen
                + dx * dy * color_4[y2][x2].rgbtGreen;
			rgba.rgbtGreen = (uint8_t)temp_green;

			
			temp_red= (1 - dx) * (1 - dy) * color_4[y1][x1].rgbtRed
                + dx * (1 - dy) * color_4[y1][x2].rgbtRed
                + (1 - dx) * dy * color_4[y2][x1].rgbtRed
                + dx * dy * color_4[y2][x2].rgbtRed;
			rgba.rgbtRed = (uint8_t)temp_red;

			temp_Alpha= (1 - dx) * (1 - dy) * color_4[y1][x1].rgbtAlpha
                + dx * (1 - dy) * color_4[y1][x2].rgbtAlpha
                + (1 - dx) * dy * color_4[y2][x1].rgbtAlpha
                + dx * dy * color_4[y2][x2].rgbtAlpha;
			rgba.rgbtAlpha = (uint8_t)temp_Alpha;

			fwrite(&rgba, sizeof(RGBA), 1, fp_out_2);        // 將 color matrix 寫入輸出圖檔中
			}
        }
		fclose(fp_out_2);
	}
    return 0;
}