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

// 生成高斯核
void generateGaussianKernel(double** kernel, int size, double sigma) {
    int center = size / 2;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            double x = i - center;
            double y = j - center;
            kernel[i][j] = exp(-(x * x + y * y) / (2 * sigma * sigma));
        }
    }

    // 正規化，使總和為 1
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            sum += kernel[i][j];
        }
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            kernel[i][j] /= sum;
        }
    }
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
    int x= center.x + static_cast<int>(len * cos(angleRad));
    int y = center.y + static_cast<int>(len * sin(angleRad));
    // int x2 = center.x + static_cast<int>(len/2 * cos(angleRad));
    // int y2 = center.y + static_cast<int>(len/2 * sin(angleRad));

    // Draw a white line on the PSF
    line(PSF,center, Point(x, y), Scalar(255), 1, LINE_AA);
    
    // Normalize the PSF to have a sum of 1
    // imwrite("PSF2.jpg" ,PSF);
    PSF /= sum(PSF);
    
    
}

int main() {
    FILE *fp_in;

	FILE *fp_out;
	FILE *fp_out_2;

	BITMAPFILEHEADER fileHeader;//宣告BMP檔案標頭
	BITMAPINFOHEADER infoHeader;//宣告BMP資訊標頭

	fp_in = fopen("C:\\Users\\shaokai\\DIP\\lab4_test\\2023 DIP hw4\\input1.bmp", "rb");//讀取input1

	fp_out = fopen("C:\\Users\\shaokai\\DIP\\lab4_test\\output1_RL.bmp", "wb");//寫入檔案FILE (Grey world)
	fp_out_2 = fopen("C:\\Users\\shaokai\\DIP\\lab4_test\\2023 DIP hw4\\input2.bmp", "wb");//寫入input2
	
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

	int iterations = 3;
	//struct YCbCr value;
	const int kernelSize = 51;

	/** -------------------------------##### 製作Kernel ##### ----------------------------------**/
    double** gaussianKernel = new double*[kernelSize];
    for (int i = 0; i < kernelSize; ++i) {
        gaussianKernel[i] = new double[kernelSize];
    }

    // 生成高斯核
    double sigma = 100; // 標準差
    generateGaussianKernel(gaussianKernel, kernelSize, sigma);

    // 顯示生成的高斯核
    //std::cout << "Generated Gaussian Kernel:" << std::endl;
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            //std::cout << gaussianKernel[i][j] << " ";
        }
       //std::cout << std::endl;
    }
	/** -------------------------------##### 跟謝旻諺做conv ##### ----------------------------------**/
	double motion_blur[5][5] = {{0, 0, 0, 0.2, 0},
								{0, 0, 0, 0.2, 0},
								{0, 0, 0.2, 0, 0},
								{0, 0.2, 0, 0, 0},
								{0, 0.2, 0, 0, 0} };
	
	

	double new_matrix[51][51];	

	Mat motion_blur_new;
	getPSF(motion_blur_new,Size(51,51),9,45);
	/*
	std::cout << "motion_blur_new:" << std::endl;
	std::cout << motion_blur_new << endl;
	*/
	

	//std::cout << "new_matrix:" << std::endl;						
	for (int i = 0; i < kernelSize; ++i) {
		for (int j = 0; j < kernelSize; ++j) {
			double sum = 0.0;
			for (int x = -25; x <= 25; x++) {
				for (int y = -25; y <= 25; y++) {
				int ii = i + x;
				int jj = j + y;
					if (ii >= 0 && ii < kernelSize && jj >= 0 && jj < kernelSize) {
						sum += gaussianKernel[ii][jj] * motion_blur_new.at<double>(x + 25, y + 25);
					}
				}
			}
			new_matrix[i][j] = sum;
			//std::cout << new_matrix[i][j] << " ";
			
		}
		//std::cout << " "<< endl;
		//std::cout << "#i:"<< i << endl;
    }
	/** -------------------------------##### flip ##### ----------------------------------**/
	double flipped_psf[51][51];
	for (int i = 0; i < kernelSize; ++i) {
		for (int j = 0; j < kernelSize; ++j) {
			flipped_psf[i][j] = new_matrix[kernelSize - 1 - i][kernelSize - 1 - j];
		}
	}


	
	/** -------------------------------##### RGB to YCbCr ##### ----------------------------**/
	// for (i = 0; i < H; i++) {
    //     for (j = 0; j < W; j++) {
	// 		struct RGB data = {color[i][j].R, color[i][j].G, color[i][j].B};
	// 		value = RGBToYCbCr(data);  // value.Y,  value.Cb,  value.Cr
    //     }
    // }
	
	/** -------------------------------##### Richardson Lucy ##### ----------------------------**/
	
	double** deconvolvedImage = new double*[H];
	//std::cout << "deconvolvedImage:" << std::endl;
    for (int i = 0; i < H; ++i) {
        deconvolvedImage[i] = new double[W];
		//std::cout << *deconvolvedImage[i] << " ";
    }
	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {
			struct RGB data = {color[i][j].R, color[i][j].G, color[i][j].B};
			struct YCbCr value = RGBToYCbCr(data);
			deconvolvedImage[i][j] = value.Y;  // 或者使用其他適當的值
		}
	}
	// second conv2
	double** second_conv2_result = new double*[H];
    for (int i = 0; i < H; ++i) {
        second_conv2_result[i] = new double[W];
    }

	double** temp_first_conv_result = new double*[H];
	for (int i = 0; i < H; ++i) {
		temp_first_conv_result[i] = new double[W];
	}
    for (int iter = 0; iter < iterations; ++iter) {
        // 將當前估計值與反轉的卷積核進行卷積

		for (int i = 0; i < H; ++i) {
			for (int j = 0; j < W; ++j) {
				double sum = 0.0;
				for (int x = -25; x <= 25; x++) {
                	for (int y = -25; y <= 25; y++) {
                    int ii = i + x;
                    int jj = j + y;

                    if (ii >= 0 && ii < H && jj >= 0 && jj < W) {
                        sum += deconvolvedImage[ii][jj] * new_matrix[x + 25][y + 25];
                    }
                   }
           		}
				temp_first_conv_result[i][j] = sum;
				}
    	}


        // 計算觀察和估計影像之間的比率
        for (int i = 0; i < H; ++i) {
            for (int j = 0; j < W; ++j) {
				struct RGB data = {color[i][j].R, color[i][j].G, color[i][j].B};
				struct YCbCr value = RGBToYCbCr(data);  // value.Y,  value.Cb,  value.Cr
				double temp_Y = value.Y;
				double div_result;

				div_result = temp_Y / (temp_first_conv_result[i][j] + 1e-10);

				//second conv2
				double sum = 0.0;
				for (int x = -25; x <= 25; x++) {
                	for (int y = -25; y <= 25; y++) {
                    int ii = i + x;
                    int jj = j + y;

                    if (ii >= 0 && ii < H && jj >= 0 && jj < W) {
                        sum += div_result * flipped_psf[x + 25][y + 25];
                    }
                }
           		}
				second_conv2_result[i][j] = sum;

                double ratio = second_conv2_result[i][j]; 


                // 使用比率更新估計值
                deconvolvedImage[i][j] *= ratio;
            }
        }
    }
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			double  newIntensity = deconvolvedImage[i][j];
			struct RGB data_temp = {color[i][j].R, color[i][j].G, color[i][j].B};
			struct YCbCr value_temp = RGBToYCbCr(data_temp);

			if(newIntensity>255)
				newIntensity = 255;
			else if(newIntensity <0)
				newIntensity = 0;

			struct YCbCr new_YCbCr = {newIntensity , value_temp.Cb, value_temp.Cr};
			struct RGB new_RGB = YCbCrToRGB(new_YCbCr);
			rgb.B = new_RGB.B;
			rgb.G = new_RGB.G;
			rgb.R = new_RGB.R;

			fwrite(&rgb, sizeof(RGB), 1, fp_out);
		}
	}
	fclose(fp_out);
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out); //輸出檔案
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);
    return 0;
}