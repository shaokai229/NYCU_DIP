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
struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};
// 生成高斯核
void generateGaussianKernel(double** kernel, int sizeW, int sizeH, double sigma) {
    int centerW = sizeW / 2;
    int centerH = sizeH / 2;

    for (int i = 0; i < sizeH; ++i) {
        for (int j = 0; j < sizeW; ++j) {
            double x = j - centerW;
            double y = i - centerH;
            kernel[i][j] = exp(-(x * x + y * y) / (2 * sigma * sigma));
        }
    }

    // 正規化，使總和為 1
    double sum = 0.0;
    for (int i = 0; i < sizeH; ++i) {
        for (int j = 0; j < sizeW; ++j) {
            sum += kernel[i][j];
        }
    }

    for (int i = 0; i < sizeH; ++i) {
        for (int j = 0; j < sizeW; ++j) {
            kernel[i][j] /= sum;
        }
    }
}
Mat convertToMat(double **kernel, int sizeW, int sizeH)
{
    Mat matKernel(sizeH, sizeW, CV_64F); // 將 CV_32F 改為 CV_64F

    double sum = 0.0;
    for (int i = 0; i < sizeH; ++i)
    {
        for (int j = 0; j < sizeW; ++j)
        {
            sum += kernel[i][j];
        }
    }

    for (int i = 0; i < sizeH; ++i)
    {
        for (int j = 0; j < sizeW; ++j)
        {
            matKernel.at<double>(i, j) = kernel[i][j] / sum;
        }
    }

    return matKernel;
}
void getPSF(Mat& PSF, Size filterSize, int len, double theta)
{
    // Create a black image for the PSF
    PSF = Mat::zeros(filterSize, CV_32F);

    // Convert the angle to radians
    double angleRad = theta * CV_PI / 180.0;

    // Calculate the center of the PSF
    Point center(filterSize.width / 2, filterSize.height / 2);

    // Calculate the endpoint of the motion line
    int x= center.x + 0.5*static_cast<int>(len * cos(angleRad));
    int y = center.y + 0.5*static_cast<int>(len * sin(angleRad));

	int o = center.x - 0.5*static_cast<int>(len * cos(angleRad));
	int k = center.y - 0.5*static_cast<int>(len * sin(angleRad));
    // int x2 = center.x + static_cast<int>(len/2 * cos(angleRad));
    // int y2 = center.y + static_cast<int>(len/2 * sin(angleRad));

    // Draw a white line on the PSF
    line(PSF,Point(o,k), Point(x, y), Scalar(255), 1, LINE_AA);
    
    // Normalize the PSF to have a sum of 1
    // imwrite("PSF2.jpg" ,PSF);
    PSF /= sum(PSF);
    
    
}
void fftshift(const Mat& inputImg, Mat& outputImg)
{
    outputImg = inputImg.clone();
    int cx = outputImg.cols / 2;
    int cy = outputImg.rows / 2;
    Mat q0(outputImg, Rect(0, 0, cx, cy));
    Mat q1(outputImg, Rect(cx, 0, cx, cy));
    Mat q2(outputImg, Rect(0, cy, cx, cy));
    Mat q3(outputImg, Rect(cx, cy, cx, cy));
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}
int main() {
    FILE *fp_in;

	FILE *fp_out;
	FILE *fp_out_2;

	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab4_test\\2023 DIP hw4\\input2.bmp", "rb");//讀取input1

	fp_out = fopen("C:\\Users\\shaokai\\DIP\\lab4_test\\output2.bmp", "wb");//寫入檔案FILE (Grey world)
	//fp_out_2 = fopen("C:\\Users\\shaokai\\DIP\\lab4_test\\2023 DIP hw4\\input2.bmp", "wb");//寫入input2
	

	if (!fp_in) printf("bmp fail...\n");
	if (!fp_out) printf("output.bmp fail...\n"); // 確認讀檔寫檔

	fread(&fileHeader,sizeof(BITMAPFILEHEADER),1,fp_in);//讀取檔案頭
	fread(&infoHeader,sizeof(BITMAPINFOHEADER),1,fp_in);//讀取資訊頭

	unsigned int W = infoHeader.biWidth; // 寬
	unsigned int H = infoHeader.biHeight; // 長
	unsigned int P = infoHeader.biBitCount ; 
	int isRGBA = (P == 32); // 1 = RGBA; 0 = RGB

	RGB rgb;
	vector<vector<RGB>> color(H, vector<RGB>(W));

    cout << W << endl;
    cout << H << endl;
    cout << P << endl;

	for(i=0; i<H; i++ ) {      
		for( j=0; j<W; j++  ) {  
			fread(&rgb, sizeof(RGB), 1, fp_in);
			color[i][j].R=rgb.R;
			color[i][j].G=rgb.G;
			color[i][j].B=rgb.B;
		}
	}
	fclose(fp_in);     //檔案fp讀取完成，關閉

	/** --------------------------------##### 輸出圖檔 ##### --------------------------------------**/
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out); //輸出檔案
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);
	 /** -------------------------------##### 製作Kernel ##### ----------------------------------**/
	const int kernelSizeW = W;
	const int kernelSizeH = H;
	double** gaussianKernel = new double*[kernelSizeH];
	for (int i = 0; i < kernelSizeH; ++i) {
		gaussianKernel[i] = new double[kernelSizeW];
	}

	// 生成高斯核
	double sigma = 100; // 標準差
	generateGaussianKernel(gaussianKernel, kernelSizeW, kernelSizeH, sigma);
	/** -------------------------------##### Generate Motion_blur  ##### ----------------------------**/
	Mat motion_blur;
	getPSF(motion_blur,Size(W,H),28,45); 
	Mat matGaussianKernel = convertToMat(gaussianKernel, W,H);
	matGaussianKernel.convertTo(matGaussianKernel, CV_32F); //將原圖轉換成可計算矩陣容器
	//cv::add(motion_blur, matGaussianKernel, motion_blur);
	/*
	std::cout << "motion_blur_new:" << std::endl;
	std::cout << motion_blur << endl;
	*/
	//fftshift(motion_blur,motion_blur);
	/** -------------------------------##### Motion_blur to Freq. ##### ----------------------------**/
	Mat dftInput1_motion_blur,dftImage1_motion_blur;

	motion_blur.convertTo(dftInput1_motion_blur, CV_32F); //將原圖轉換成可計算矩陣容器
	dft(dftInput1_motion_blur, dftImage1_motion_blur, DFT_COMPLEX_OUTPUT);    //進行 DFT
	
	/*
	std::cout << "motion_blur_DFT:" << std::endl;
	for (int i = 0; i < dftImage1_motion_blur.rows; ++i) {
		for (int j = 0; j < dftImage1_motion_blur.cols; ++j) {
			std::cout << dftImage1_motion_blur.at<Vec2f>(i, j)[0] << " + " << dftImage1_motion_blur.at<Vec2f>(i, j)[1] << "i   ";
		}
    std::cout << std::endl;
	}
	*/

	// -----------------------------------------------conjugate
	cv::Mat dftImage1_motion_blur_conj;
    cv::Mat planes[2];
    cv::split(dftImage1_motion_blur, planes);
    // 將虛部取反
    planes[1] = -planes[1];
    // 合併實部和虛部
    cv::merge(planes, 2, dftImage1_motion_blur_conj);
	// --------------------------------------------------| |^2
	cv::split(dftImage1_motion_blur, planes);
    // 計算絕對值
    cv::Mat magnitude;
    cv::magnitude(planes[0], planes[1], magnitude);
    // 對絕對值進行平方
    cv::Mat dftImage1_motion_blur_abs_square = magnitude.mul(magnitude);
	
	/** -------------------------------##### blur image to Freq. ##### ----------------------------**/

	Mat inputImage(H, W, CV_8UC3);
	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) {
			inputImage.at<Vec3b>(i, j) = Vec3b(color[i][j].R, color[i][j].G, color[i][j].B);
		}
	}
	Mat dftInputImage[3];
	split(inputImage, dftInputImage);

	for (int c = 0; c < 3; ++c) {
		dftInputImage[c].convertTo(dftInputImage[c], CV_32F);
	}
	// 進行 DFT
	Mat dftImage[3];
	for (int c = 0; c < 3; ++c) {
		dft(dftInputImage[c], dftImage[c], DFT_COMPLEX_OUTPUT);
	}
	/** -------------------------------##### Wiener ##### ----------------------------**/
	float SNR = 0.04;

	Mat add_result;
	cv::add(dftImage1_motion_blur_abs_square, SNR, add_result);

	Mat divide_result;
	cv::split(dftImage1_motion_blur_conj, planes);
	/*
	std::cout << "dftImage1_motion_blur_conj size: " << dftImage1_motion_blur_conj.size() << " type: " << dftImage1_motion_blur_conj.type() << std::endl;
	std::cout << "add_result size: " << add_result.size() << " type: " << add_result.type() << std::endl;
	*/
 	cv::Mat planes_temp[2];
	cv::divide(planes[0],add_result,planes_temp[0]);
	cv::divide(planes[1],add_result,planes_temp[1]);
	cv::merge(planes_temp, 2, divide_result);
	
	Mat multiply_result[3];

	// 逐通道相乘
	for (int c = 0; c < 3; ++c) {
		cv::Mat R_I[2];
		cv::split(dftImage[c], R_I);

		// Multiply real and imaginary parts with the corresponding values from the Wiener filter
		Mat ac,ad,bc,bd;
		cv::multiply(planes_temp[0], R_I[0], ac);
		cv::multiply(planes_temp[0], R_I[1], ad);
		cv::multiply(planes_temp[1], R_I[0], bc);
		cv::multiply(planes_temp[1], R_I[1], bd);

		// Merge real and imaginary parts to get the final result for this channel
		cv::Mat add_temp[2];
		cv::add(ac, -bd, add_temp[0]);
		cv::add(ad, bc, add_temp[1]);
		cv::merge(add_temp, 2, multiply_result[c]);
	}
	
	
	/** -------------------------------##### IDFT ##### ----------------------------**/
	
	Mat idft_result[3];

	// 逐通道進行IDFT
	for (int c = 0; c < 3; ++c) {
		cv::idft(multiply_result[c], idft_result[c], cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);
		fftshift(idft_result[c],idft_result[c]);
		//idft_result[c].convertTo(idft_result[c], CV_8U);
	}

	// 歸一化每個通道
	
	for (int c = 0; c < 3; ++c) {
		cv::normalize(idft_result[c], idft_result[c], 0, 255, cv::NORM_MINMAX, CV_8U);
		
	}
	
	
	// 合併三個通道
	Mat final_result;
	cv::merge(idft_result, 3, final_result);
	/*
	std::cout << "final_result:" << std::endl;
	std::cout << final_result << endl;
	*/

	// 將轉換後的像素值輸出到 BMP 檔案
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			RGB rgb;
			Vec3b intensity = final_result.at<Vec3b>(i, j);

			rgb.R = intensity[0];
			rgb.G = intensity[1];
			rgb.B = intensity[2];

			fwrite(&rgb, sizeof(RGB), 1, fp_out);
		}
	}	
	fclose(fp_out);
    return 0;
}