

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <stdint.h>

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int i,j,u,v,x,y,a;

FILE *fin1, *fin2;

string IN1_PATH, IN2_PATH;

int width, height, channel;
int size;

struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};


vector<double> MSE(vector<vector<RGB>> img1, vector<vector<RGB>> img2){
	vector<double> mse;
	double mse_r, mse_g, mse_b = 0.0;
	double N = 3*height * width;

	for( i = 0; i < height; i++){
		for( j = 0; j < width; j++){
			mse_r += pow(abs(img1[i][j].R - img2[i][j].R), 2);
			mse_g += pow(abs(img1[i][j].G - img2[i][j].G), 2);
			mse_b += pow(abs(img1[i][j].B - img2[i][j].B), 2);
		}
	}
		
	mse_r = mse_r / N;
	mse_g = mse_g / N;
	mse_b = mse_b / N;


	mse.push_back(mse_r);
	mse.push_back(mse_g);
	mse.push_back(mse_b);

	return mse;
}
double PSNR(vector<double> mse){
	double psnr = 0.0;

	for(auto i = 0; i < 3; i++)
		psnr += 10 * log10((255 * 255) / mse[i]);

	return psnr;
}


int main(int argc, char **argv){
	FILE *fp_in;
	FILE *fp_ori;


	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	//
	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab4_test\\output1.bmp", "rb");//讀取input1
	fp_ori = fopen("C:\\Users\\shaokai\\DIP\\lab4_test\\2023 DIP hw4\\input1_ori.bmp", "rb");//讀取input1
 	// 將檔案頭讀入
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_in);
    fread(&infoHeader, sizeof(BITMAPFILEHEADER), 1, fp_in);

    width = infoHeader.biWidth;   // 寬
    height = infoHeader.biHeight; // 長
    channel = infoHeader.biBitCount / 8; // 通道數

    // 讀取圖片數據
    vector<vector<RGB>> img1(height, vector<RGB>(width));
    vector<vector<RGB>> img2(height, vector<RGB>(width));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fread(&img1[i][j], sizeof(RGB), 1, fp_in);
            fread(&img2[i][j], sizeof(RGB), 1, fp_ori);
        }
    }

    // 關閉檔案
    fclose(fp_in);
    fclose(fp_ori);


	vector<double> mse;
	cout << "========= MSE =========" << endl;
	mse = MSE(img1, img2);
	cout << "MSE_r = " << mse[0] << endl;
	cout << "MSE_g = " << mse[1] << endl;
	cout << "MSE_b = " << mse[2] << endl;
	double psnr;
	cout << "========= PSNR ========" << endl;
	psnr = PSNR(mse);
	cout << "PSNR = " << psnr << " dB"<<endl;


	return 0;
}