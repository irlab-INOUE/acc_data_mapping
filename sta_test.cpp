#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <math.h>
#include <random>

#define N 30	//分割した1データあたりのデータ点数
#define Q 1	//シフト数

float U(std::vector<float>);
float W(std::vector<float>, float);
float S(std::vector<float>, float);
int F_test(float, float, float, float, float);

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

	std::string filename;
	std::string filename2;

	if (dis_ans_1 == 0){
		filename = "EE1_flat_sub";
		filename2 = "2cb_EE1log.log";
	}

	if (dis_ans_2 == 0){
		filename = "RWRC2021.log";
		filename2 = "RWRC2021.log";
	}
	
	if (dis_ans_3 == 0){
		filename = "RWRC2022.log";
		filename2 = "RWRC2022.log";
	}

	if (dis_ans_4 == 0){
		filename = "ONCT.log";
		filename2 = "ONCT.log";
	}

	
	//マスターデータファイルを開く
	std::cout << "→→→ open the Master data file: \t\t" << filename << std::endl;
	std::string path = "../data/Master_data/";
	std::ifstream file(path + filename);
	if (!file){
		std::cerr << "Could not open Master Data file" << std::endl;
		std::exit(1);
	}

	//計測結果のファイル
	std::cout << "→→→ open the Measured data file: \t" << filename2 << std::endl;
	std::string path2 = "../data/Origin_data/";
	std::ifstream file2(path2 + filename2);
	if (!file2){
		std::cout << "Could not open Measured Data file" << std::endl;
		std::exit(1);
	}

	//出力ファイルの設定
	std::string outfile = "result_sta_data";
	std::cout << "==================================================" << std::endl;
	std::string outpath = "../data/";
	std::ofstream ofs(outpath + outfile);
	if (!ofs){
		std::cout << "Could not open Destination file" << std::endl;
		std::exit(1);
	}

	//変数定義
	int n, i = 0;
	int c, c_out, F_res;
	double x, y, deg;
	float ts_imu, ax, ay, wx, wy, wz, mx, my, mz;
	float TS, ts, az;
	float ave_all, dis_all;
	float ave_sep, dis_sep;
	float dis_sep_past;
	float F_u, F_l, F_num;
	float size, size_mes;
	double v, w;
	std::vector<float> acc_z;
	std::vector<float> acc_mes_z;
	std::vector<float> acc_sep;
	std::vector<float> result_ave;
	std::vector<float> result_dis;
	std::vector<int> result_F;
	std::vector<double> mes_x;
	std::vector<double> mes_y;
	std::vector<double> mes_deg;
	std::vector<float> mes_ts;
	std::vector<double> mes_v;

	//教師データの読み込み
	//file >> TS >> ts >> az >> x >> y >> deg;
	file >> TS >> x >> y >> deg >> ax >> ay >> az >> wx >> wy >> wz >> mx >> my >> mz >> v >> w;
	while(!file.eof()){
		//az = az * 1000;
		acc_z.emplace_back(az);
		//file >> TS >> ts >> az >> x >> y >> deg;
		file >> TS >> x >> y >> deg >> ax >> ay >> az >> wx >> wy >> wz >> mx >> my >> mz >> v >> w;
	}

	//計測データの読み込み
	file2 >> TS >> x >> y >> deg >> ax >> ay >> az >> wx >> wy >> wz >> mx >> my >> mz >> v >> w;
	//file2 >> TS >> ts >> az >> x >> y >> deg;
	while(!file2.eof()){
		//az = az * 1000;
		acc_mes_z.emplace_back(az);
		mes_x.emplace_back(x);
		mes_y.emplace_back(y);
		mes_deg.emplace_back(deg);
		mes_ts.emplace_back(TS);
		mes_v.emplace_back(v);
		file2 >> TS >> x >> y >> deg >> ax >> ay >> az >> wx >> wy >> wz >> mx >> my >> mz >> v >> w;
		//file2 >> TS >> ts >> az >> x >> y >> deg;
	}
	
	//データ数
	size = acc_z.size();
	size_mes = acc_mes_z.size();

	//データ数からの棄却領域設定
	std::cout << "母データ数:\t" << size << std::endl;
	std::cout << "標本データ数:\t" << N << std::endl;
	std::cout << "上側限界値:\t";
	std::cin >> F_u;
	std::cout << "下側限界値:\t";
	std::cin >> F_l;

	//教師データの平均
	ave_all = U(acc_z);		
	//std::cout << "母平均" << ave_all << std::endl;
	
	//教師データの分散
	dis_all = W(acc_z, ave_all);
	//std::cout << "母分散" << dis_all << std::endl;

	//区間ごとでのF検定
	for (c = 0; c < acc_mes_z.size(); c = c + Q){

		//std::cout << mes_x[c] << "," << mes_y[c] << std::endl;

		//データ出力のためのラベル保存
		c_out = c;
	
		//データの分割
		while ((i < N) && (c + i < size_mes)){
			acc_sep.emplace_back(acc_mes_z[c + i]);
			i = i + 1;
		}
		i = 0;

		//標本平均
		ave_sep = U(acc_sep);
		result_ave.emplace_back(ave_sep);

		//不偏分散
		dis_sep_past = dis_sep;
		dis_sep = S(acc_sep, ave_sep);
		//std::cout << "不偏分散" << " " << dis_sep << std::endl;
		result_dis.emplace_back(dis_sep);

		//F検定
		F_res = F_test(dis_all, dis_sep, dis_sep_past, F_l, F_u);
		F_num = dis_sep / dis_all;
		result_F.emplace_back(F_res);
		//std::cout << "result" << " " << F_res << std::endl;

		//ファイル出力
		while ((i < Q) && (c_out + i < size_mes)){
			ofs	<< mes_ts[c_out + i] << " "	//タイムスタンプ
				<< F_num << " "			//F値
				<< dis_sep << " "		//不偏分散
				<< F_res << " "			//検定結果
				<< acc_mes_z[c_out + i] << " "	//Acc_zデータ
				<< mes_x[c_out + i] << " "	//自己位置x
				<< mes_y[c_out + i] << " "	//自己位置y
				<< mes_deg[c_out + i] << " " 	//自己位置deg
				//<< mes_v[c_out + i] << " "	//速度v
				<< std::endl;
			i = i + 1;
		}
		i = 0;
		
		acc_sep.clear();	//分割データベクトルの全要素を消去
	}
	std::cerr << "Done." << std::endl;
}

/*ランダムにデータ生成（正規分布）---------------------------
int random(std::vector<float> ran_data, int size){

	std::vector<float> acc_sep;

	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_int_distribution<int> distr(0, (size-1));
	acc_sep.emplace_back(ran_data[distr(eng)]);

	return acc_sep;
}
*/

//平均-------------------------------------------------------
float U(std::vector<float> data){

	float sum = 0;
	float ave;
	int n = 0;
	int size = data.size();

	for (n = 0; n < size; n++){
		sum = sum + data[n];
	}
	
	ave = sum / size;
	
	return ave;
}

//分散-------------------------------------------------------
float W(std::vector<float> data, float ave){

	int i = 0;
	int size = data.size();
	float sub;
	float sub_2, sigma, dis;
	float sum = 0;

	for (i = 0; i < size; i++){
		sub = data[i] - ave;
		sub_2 = sub * sub;
		sum = sum + sub_2;
	}
	
	dis = sum / size;

	return dis;
}

//不偏分散---------------------------------------------------
float S(std::vector<float> data, float ave){

	int i = 0;
	int size = data.size();
	float sub = 0;
	float sub_2 = 0;
	float sigma = 0; 
	float dis = 0;
	float sum = 0;

	for (i = 0; i < size; i++){
		sub = data[i] - ave;
		sub_2 = sub * sub;
		sum = sum + sub_2;
	}
	
	dis = sum / (size - 1);

	return dis;
}
	
//F検定------------------------------------------------------
int F_test(float dis_all, float dis_sep, float dis_sep_past, float F_l, float F_u){

	float F;
	int res;
		
	/*F値の計算	
	if (dis_all > dis_sep){
		F = dis_all / dis_sep;
	}
	else{
		F = dis_sep / dis_all;
	}
	*/

	F = dis_sep / dis_all;

	//std::cout << F << std::endl;

	//棄却判定
	if ((F_l < F) && (F_u > F)){
		res = 1;
	}
	else if ((F < F_l) || (F_u < F)){
		res = 0;
	}

	/*
	dis_sep_past = dis_sep_past + 1;
	if (dis_sep > dis_sep_past){
		res = 0;
	}
	*/

	return res;
}


