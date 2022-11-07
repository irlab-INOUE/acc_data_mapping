#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <unistd.h>

int main(int argc, char *argv[]) 
{

	/*===========================マップによる変数定義===========================*/

	/*
	//EE1F
	std::string filename = "EE1F_sta_data";
	int IMG_ORIGIN_X = 530;
	int IMG_ORIGIN_Y = 70; 
	int lim = 200;
	int count = 1;
	cv::Mat imgMap = cv::imread("../Map/Origin_Map/EE1F/occMap.png", cv::IMREAD_COLOR);
	*/

	/*
	//RWRC2021
	std::string filename = "RWRC2021_sta_data";
	int IMG_ORIGIN_X = 1046;
	int IMG_ORIGIN_Y = 576;
	int lim = 3000;
	int count = 10;
	cv::Mat imgMap = cv::imread("../Map/Origin_Map/RWRC2021/occMap.png", cv::IMREAD_COLOR);
	*/

	//RWRC2022
	std::string filename = "RWRC2022_sta_data";
	int IMG_ORIGIN_X = 1305;
	int IMG_ORIGIN_Y = 385;
	int lim = 3000;
	int count = 10;
	cv::Mat imgMap = cv::imread("../Map/Origin_Map/RWRC2022/occMap.png", cv::IMREAD_COLOR);
	

	/*===========================================================================*/

	std::string str = "y";
	std::string ans;
	std::cerr << "Sequential display of map (y/n):";
	std::cin >> ans;
	int dis_ans = ans.compare(str);

	int base = 1000;
	int hight = 500;
	int width = 2200;
	int center_h = hight / 2;
	int center_w = width / 2;
	
	//mapを開く
	cv::Mat img_cnv;
	cv::cvtColor(imgMap, img_cnv, cv::COLOR_BGR2BGRA);		//BGRからBGRAに変換

	cv::Mat imgAcc = cv::Mat(cv::Size(width, hight), CV_8UC3, cv::Scalar(182, 182, 182));
	cv::line(imgAcc, cv::Point(100,50),cv::Point(100,hight-50), cv::Scalar(255,255,255),2,cv::LINE_8);
	cv::line(imgAcc, cv::Point(50,center_h),cv::Point(width-50,center_h), cv::Scalar(255,255,255),2,cv::LINE_8);

	//判別結果のファイルを開く
	std::string path = "../data/Sta_data/";
	std::ifstream file(path + filename);
	//ファイルが開けなかった場合
	if (!file){
		std::cout << "データファイルが開けませんでした" << std::endl;
		std::exit(1);
	}

	float TS, lab, F, z;
	double x, y, deg, qx, qy, v;
	int i, lab2;
	int N = 0;
	int I = 0;
	int n = 0;
	int k = 0;
	int f = 0;
	int c = 0;
	int color_num;
	std::string name;

	//読み込んだ数値を一時保存するためのベクトル
	std::vector<float> map_lab;
	std::vector<float> map_x;
	std::vector<float> map_y;
	std::vector<float> map_deg;
	std::vector<float> map_z;


	//データの読み込み
	while(!file.eof()) {
		file >> TS >> F >> lab >> lab2 >> z >> x >> y >> deg >> v;
		map_lab.emplace_back(lab);		//分散値
		map_z.emplace_back(z);			//Z軸の加速度
		map_x.emplace_back(x);			//自己位置(x)
		map_y.emplace_back(y);			//自己位置(y)
		map_deg.emplace_back(deg);		//自己位置(deg)
	}

	//データの総数
	I = map_x.size() - 1;
	
	double csize = 0.05;  // 1pixelは0.05m に相当（これも聞く）

	std::cerr << "Map Showing..." << std::endl;

	//指定座標にプロット
	while (N < I){
		//mapでの座標値
		qx = (map_x[N] + 8) / csize + IMG_ORIGIN_X;
		qy =-map_y[N] / csize + IMG_ORIGIN_Y;

		//地図へRGBの色付けとA値を付与
		//上限設定
		if (map_lab[N] > lim){
			map_lab[N] = lim;
		}
		color_num = (map_lab[N] / lim) * 255;	//分散値を0~255の値に変換
		
		//分散値（小）ならばBlueで表示をしA値を付与
		if (color_num < 64){
			for(int i = -7; i < 7; i++){
				for(int n = -7; n < 7; n++){
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[0] = 255;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[1] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[2] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[3] = color_num;	//A値を付与 
				}
			}
			cv::circle(imgAcc, cv::Point(c + 101, center_h + map_z[N] - base), 1, cv::Scalar(255,0,0), -1);
			cv::line(imgAcc, cv::Point(c + 100, center_h + map_z[N] - base),cv::Point(c + 101,center_h + map_z[N+10] - base), cv::Scalar(255,0,0),2,cv::LINE_8);
		}

		//分散値（中）ならばGreenで表示をしA値を付与
		else if (color_num < 127){
			for(int i = -7; i < 7; i++){
				for(int n = -7; n < 7; n++){
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[0] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[1] = 255;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[2] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[3] = color_num;	//A値を付与
				}
			}
			cv::circle(imgAcc, cv::Point(c + 101, center_h + map_z[N] - base), 1, cv::Scalar(0,255,0), -1);
			cv::line(imgAcc, cv::Point(c + 100, center_h + map_z[N] - base),cv::Point(c + 101,center_h + map_z[N+10] - base), cv::Scalar(0,255,0),2,cv::LINE_8);
		}

		//分散値（大）ならばRedで表示をしA値を付与
		else if (color_num > 126){
			for(int i = -7; i < 7; i++){
				for(int n = -7; n < 7; n++){
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[0] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[1] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[2] = 255;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[3] = color_num;	//A値を付与
				}
			}
			cv::circle(imgAcc, cv::Point(c + 101, center_h + map_z[N] - base), 1, cv::Scalar(0,0,255), -1);
			cv::line(imgAcc, cv::Point(c + 100, center_h + map_z[N] - base),cv::Point(c + 101,center_h + map_z[N+10] - base), cv::Scalar(0,0,255),2,cv::LINE_8);
		}

		N = N + count;
		c = c + 1;

		//std::cerr << n << std::endl; 

		//mapを逐次表示
		if (dis_ans == 0){
			cv::Mat display_img;
			cv::resize(img_cnv, display_img, cv::Size(), 0.4, 0.4);
			cv::Mat display_Acc;
			cv::resize(imgAcc, display_Acc, cv::Size(), 0.8, 0.8);
			cv::imshow("Map", display_img);
			//cv::imshow("Map", img_cnv);
			cv::imshow("Acc", display_Acc);
			//key = cv::waitKey(42);
			cv::waitKey(1);
		}
	}

	//完成mapを表示＆保存
	cv::Mat display_img;
	cv::resize(img_cnv, display_img, cv::Size(), 0.4, 0.4);
	cv::Mat display_Acc;
	cv::resize(imgAcc, display_Acc, cv::Size(), 0.8, 0.8);
	cv::imshow("Map", display_img);
	cv::imshow("Acc", display_Acc);
	std::cerr << " " << std::endl;
	std::cerr << "Done." << std::endl;
	cv::imwrite("../Map/Acc_Map/map_RGBA.png", img_cnv);
	cv::waitKey(0);
	return 0;
}
