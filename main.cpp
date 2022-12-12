#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <unistd.h>

int main(int argc, char *argv[]) 
{

	std::string str_1 = "1";
	std::string str_2 = "2";
	std::string str_3 = "3";
	std::string str_4 = "4";
	std::string ans_loc;
	std::cerr	<< "1: EE1F\n" 
			<< "2: RWRC2021\n"
			<< "3: RWRC2022\n"
			<< "4: ONCT"
			<< std::endl;
	std::cout << "==================================================" << std::endl;
	std::cerr << "choose the location:";
	std::cin >> ans_loc;
	int dis_ans_1, dis_ans_2, dis_ans_3 ,dis_ans_4 = 1;
	dis_ans_1 = ans_loc.compare(str_1);
	dis_ans_2 = ans_loc.compare(str_2);
	dis_ans_3 = ans_loc.compare(str_3);
	dis_ans_4 = ans_loc.compare(str_4);


	/*===========================マップによる変数定義===========================*/

	std::string Name;
	std::string filename;
	int IMG_ORIGIN_X;
	int IMG_ORIGIN_Y; 
	int lim;
	int count;
	float size_cnv;
	float size_acc;
	cv::Mat imgMap;

	//EE1F
	if (dis_ans_1 == 0){
		Name = "EE1F";
		filename = "EE1F_sta_data";
		IMG_ORIGIN_X = 380;
		IMG_ORIGIN_Y = 70; 
		lim = 200;
		count = 1;
		size_cnv = 1;
		size_acc = 1;
		imgMap = cv::imread("../Map/Origin_Map/EE1F/occMap.png", cv::IMREAD_COLOR);
	}

	//RWRC2021
	if (dis_ans_2 == 0){
		Name = "RWRC2021";
		filename = "RWRC2021_sta_data";
		IMG_ORIGIN_X = 1046;
		IMG_ORIGIN_Y = 576;
		lim = 3000;
		count = 10;
		size_cnv = 0.4;
		size_acc = 0.8;
		imgMap = cv::imread("../Map/Origin_Map/RWRC2021/occMap.png", cv::IMREAD_COLOR);
	}

	//RWRC2022
	if (dis_ans_3 == 0){
		Name = "RWRC2022";
		filename = "RWRC2022_sta_data";
		IMG_ORIGIN_X = 1305;
		IMG_ORIGIN_Y = 385;
		lim = 3000;
		count = 10;
		size_cnv = 0.4;
		size_acc = 0.8;
		imgMap = cv::imread("../Map/Origin_Map/RWRC2022/occMap.png", cv::IMREAD_COLOR);
	}

	//ONCT
	if (dis_ans_4 == 0){
		Name = "ONCT";
		filename = "ONCT_sta_data";
		IMG_ORIGIN_X = 271;
		IMG_ORIGIN_Y = 1588;
		lim = 800;
		count = 10;
		size_cnv = 0.6;
		size_acc = 0.8;
		imgMap = cv::imread("../Map/Origin_Map/ONCT/occMap.png", cv::IMREAD_COLOR);
	}

	/*===========================================================================*/

	std::cerr << "→→→ Map to make: " << Name << std::endl;
	std::cout << "==================================================" << std::endl;
	std::string str = "y";
	std::string ans;
	std::cerr << "Sequential display of map (y/n):";
	std::cin >> ans;
	int dis_ans = ans.compare(str);

	int base = 1000;
	int hight = 800;
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
		std::cerr << "could not open the Sta data file" << std::endl;
		std::exit(1);
	}

	float TS, lab, F, z;
	double x, y, deg, qx, qy, v;
	int i, lab2, lab_F;
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
	std::vector<int> map_lab2;
	std::vector<float> map_x;
	std::vector<float> map_y;
	std::vector<float> map_deg;
	std::vector<float> map_z;


	//データの読み込み
	while(!file.eof()) {
		file >> TS >> F >> lab >> lab2 >> z >> x >> y >> deg;
		map_lab.emplace_back(lab);		//分散値
		map_lab2.emplace_back(lab2);		//分散値
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
		qx = ((map_x[N] + 8) / csize) + IMG_ORIGIN_X;
		qy =(-map_y[N] / csize) + IMG_ORIGIN_Y;

		//地図へRGBの色付けとA値を付与
		//上限設定
		if (map_lab[N] > lim){
			map_lab[N] = lim;
		}
		color_num = (map_lab[N] / lim) * 255;	//分散値を0~255の値に変換
		lab_F = map_lab2[N];	//分散値を0~255の値に変換
		
		/*
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
			cv::line(imgAcc, cv::Point(c + 100, center_h + map_z[N] - base),cv::Point(c + 101,center_h + map_z[N+count] - base), cv::Scalar(255,0,0),2,cv::LINE_8);
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
			cv::line(imgAcc, cv::Point(c + 100, center_h + map_z[N] - base),cv::Point(c + 101,center_h + map_z[N+count] - base), cv::Scalar(0,255,0),2,cv::LINE_8);
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
			cv::line(imgAcc, cv::Point(c + 100, center_h + map_z[N] - base),cv::Point(c + 101,center_h + map_z[N+count] - base), cv::Scalar(0,0,255),2,cv::LINE_8);
		}
		*/

		
		//std::cerr << lab_F << std::endl;
		//分散値（小）ならばBlueで表示をしA値を付与
		if (lab_F == 1){
			for(int i = -7; i < 7; i++){
				for(int n = -7; n < 7; n++){
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[0] = 255;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[1] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[2] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[3] = color_num;	//A値を付与 
				}
			}
			cv::circle(imgAcc, cv::Point(c + 101, center_h + map_z[N] - base), 1, cv::Scalar(255,0,0), -1);
			cv::line(imgAcc, cv::Point(c + 100, center_h + map_z[N] - base),cv::Point(c + 101,center_h + map_z[N+count] - base), cv::Scalar(255,0,0),2,cv::LINE_8);
		}

		//分散値（中）ならばGreenで表示をしA値を付与
		else if (lab_F == 0){
			for(int i = -7; i < 7; i++){
				for(int n = -7; n < 7; n++){
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[0] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[1] = 255;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[2] = 0;
					img_cnv.at<cv::Vec4b>(qy + n, qx + i)[3] = color_num;	//A値を付与
				}
			}
			cv::circle(imgAcc, cv::Point(c + 101, center_h + map_z[N] - base), 1, cv::Scalar(0,255,0), -1);
			cv::line(imgAcc, cv::Point(c + 100, center_h + map_z[N] - base),cv::Point(c + 101,center_h + map_z[N+count] - base), cv::Scalar(0,255,0),2,cv::LINE_8);
		}
		

		N = N + count;
		c = c + 1;

		//std::cerr << n << std::endl; 

		//mapを逐次表示
		if (dis_ans == 0){
			cv::Mat display_img;
			cv::resize(img_cnv, display_img, cv::Size(), size_cnv, size_cnv);
			cv::Mat display_Acc;
			cv::resize(imgAcc, display_Acc, cv::Size(), size_acc, size_acc);
			cv::imshow("Map", display_img);
			//cv::imshow("Map", img_cnv);
			cv::imshow("Acc", display_Acc);
			//key = cv::waitKey(42);
			cv::waitKey(1);
		}
	}

	//完成mapを表示＆保存
	cv::Mat display_img;
	cv::resize(img_cnv, display_img, cv::Size(), size_cnv, size_cnv);
	cv::Mat display_Acc;
	cv::resize(imgAcc, display_Acc, cv::Size(), size_acc, size_acc);
	cv::imshow("Map", display_img);
	cv::imshow("Acc", display_Acc);
	std::cerr << " " << std::endl;
	std::cerr << "Done." << std::endl;
	cv::imwrite("../Map/Acc_Map/map_RGBA.png", img_cnv);
	cv::waitKey(0);
	return 0;
}
