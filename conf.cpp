#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <cstdlib>

int main(int argc, char *argv[]) 
{
	//mapを開く
	cv::Mat ori_Map = cv::imread("../Map/Origin_Map/EE1F/occMap.png", cv::IMREAD_COLOR);
	cv::Mat img_rgba = cv::imread("../Map/Acc_Map/EE1F/map_RGBA.png", -1);
	cv::Mat img_Map;
	cv::cvtColor(ori_Map, img_Map, cv::COLOR_BGR2BGRA);		//BGRからBGRAに変換
	
	std::cerr << "Channel: " << img_rgba.channels() << std::endl;

	int x, y, A;
	std::cerr << "Making now..." << std::endl;

	for(size_t i = 0; i < img_rgba.cols; i++){
		for(size_t n = 0; n < img_rgba.cols; n++){
			x = i;
			y = n;
			A = img_rgba.at<cv::Vec4b>(n, i)[3];

			if (A == 255){
				continue;
			}
			
			if (A > 126){
				for(int i = -7; i < 7; i++){
					for(int n = -7; n < 7; n++){
						img_Map.at<cv::Vec4b>(y + n, x + i)[0] = 0;
						img_Map.at<cv::Vec4b>(y + n, x + i)[1] = 0;
						img_Map.at<cv::Vec4b>(y + n, x + i)[2] = 250;
					}	
				}
				continue;
			}
			
			if (A < 64){
				for(int i = -7; i < 7; i++){
					for(int n = -7; n < 7; n++){
						img_Map.at<cv::Vec4b>(y + n, x + i)[0] = 250;
						img_Map.at<cv::Vec4b>(y + n, x + i)[1] = 0;
						img_Map.at<cv::Vec4b>(y + n, x + i)[2] = 0;
					}	
				}
				continue;
			}
			
			if (A < 127){
				for(int i = -7; i < 7; i++){
					for(int n = -7; n < 7; n++){
						img_Map.at<cv::Vec4b>(y + n, x + i)[0] = 0;
						img_Map.at<cv::Vec4b>(y + n, x + i)[1] = 250;
						img_Map.at<cv::Vec4b>(y + n, x + i)[2] = 0;
					}	
				}
			}
		}
	}

	//mapを表示
	cv::imshow("TEST", img_Map);
	std::cout << " " << std::endl;
	std::cout << "Done." << std::endl;
	cv::waitKey(0);
	return 0;
}
