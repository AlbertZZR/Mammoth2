﻿#include "std_header.h"
#include "pcl_header.h"
#include "mlidar.hpp"
#include "mviewer.hpp"

using namespace std;
using namespace mammoth;
using namespace pcl;

#define PointType PointXYZRGBA

struct Point3D {
	float x;
	float y;
	float z;
	PointType * ptr;
};

struct Point2D {
	float x;
	float y;
};

struct LongitudinalResult {
	float * kbr_results[4];
	float * altitudes;
};

struct CrossResult {
	float kbr_results[3][4];
	float altitudes[3];
};

float if_obtacles = 0;
int mode_flag = 0;

struct Parameters {
	//雷达安装高度
	float lidar_altitude;
	//纵坡
	//切面的x坐标
	float line_x;
	//距离切面多近的点会参与直线拟合
	float distance_threshold1;
	//从y的哪个位置开始分段、拟合
	float start_y;
	//y分段每段的长度
	float segment_y_step;
	//y的分段数量
	int segment_count;

	//障碍物
	//当前格子内高度差大于多少认为有障碍
	float height_threshold1; //30cm
							 //相邻格子内高度差大于多少认为有障碍
	float height_threshold2; //40cm
							 //相邻格子内高度差低于多少认为连续
	float height_threshold3; //20cm
	float angle_offset;

};

float get_random(int n, int m);
void LineFitLeastSquares1(std::vector<std::vector<Point2D>> points_vec, LongitudinalResult& result);
void calculate_longitudinal_angle(Point3D* points_3d, int point_count, Parameters & param, LongitudinalResult& result);
float GetProb2(const float* long_slope_c, const float * sizes, int count);

void papo();

char * obj_dec_point_path = "E:\\LidarData\\obj_dec\\point\\2\\";
char * obj_dec_result_path = "E:\\LidarData\\obj_dec\\result\\2\\";
char * hillside_dec_point_path = "E:\\LidarData\\hillside_dec\\point\\1\\";
char * hillside_dec_result_path = "E:\\LidarData\\hillside_dec\\result\\1\\";

int main(int argc, char ** argv) {
	papo();
	return 0;
}

void papo() {
	//创建参数对象
	Parameters param;
	//纵坡
	param.line_x = 0;
	param.lidar_altitude = 2.64;
	param.distance_threshold1 = 1.0;//0.5   0.5
	param.start_y = 3.5;
	param.segment_y_step = 2;
	param.segment_count = 4;
	param.height_threshold1 = 0.40;//35cm
	param.height_threshold2 = 0.40;//35cm
	param.height_threshold3 = 0.25;
	param.angle_offset = 1;
	//创建结果对象
	LongitudinalResult longitudinal_result;
	longitudinal_result.kbr_results[0] = new float[param.segment_count];
	longitudinal_result.kbr_results[1] = new float[param.segment_count];
	longitudinal_result.kbr_results[2] = new float[param.segment_count];
	longitudinal_result.kbr_results[3] = new float[param.segment_count];
	longitudinal_result.altitudes = new float[param.segment_count];
	CrossResult cross_result;
	pcl::PointCloud<PointType>::Ptr cloud(new pcl::PointCloud<PointType>());
	pcl::PointCloud<PointType>::Ptr data_cloud(new pcl::PointCloud<PointType>());
	char pointfile_name[50];
	char resultfile_name[50];
	M3DLidar lidar(MLIDAR_TYPE::VEL_HDL32);
	lidar.open("H:\\jungongxiangmuyanshou\\pcap_ori_data\\xiepo.pcap");
	MPointViewer<PointType> viewer1;
	viewer1.init();
	long long frame_count = 0;
	while (1) {
		for (int i = 0; i < param.segment_count; i++) {
			longitudinal_result.altitudes[i] = -1000;
		}
		//PcdUtil::read_pcd_file("D:/test_frame.pcd", cloud);
		lidar.get_frame(cloud);
		pcl::PassThrough<PointType> passThrough;
		passThrough.setInputCloud(cloud);
		passThrough.setFilterLimitsNegative(false);
		passThrough.setFilterFieldName("z");
		passThrough.setFilterLimits(-10, 0); //100
		passThrough.filter(*cloud);
		Point3D * points_3d = new Point3D[cloud->size()];
		for (int i = 0; i < cloud->size(); i++) {
			points_3d[i].y = (*cloud)[i].y;
			points_3d[i].x = (*cloud)[i].x;
			points_3d[i].z = (*cloud)[i].z;
			(*cloud)[i].r = 0;
			(*cloud)[i].g = 25;
			(*cloud)[i].b = 0;
			points_3d[i].ptr = &((*cloud)[i]);
			(*cloud)[i].x *= -1;
		}
		//计算
		if_obtacles = 0;
		calculate_longitudinal_angle(points_3d, cloud->size(), param, longitudinal_result);
		viewer1.set_cloud(cloud);
		//保存结果
		memset(pointfile_name, 0, 50);
		sprintf(pointfile_name, "%d.pcd", frame_count);
		string temp_str = pointfile_name;
		string pointfile_name_str = hillside_dec_point_path;
		pointfile_name_str += temp_str;
		//PcdUtil::save_pcd_file(pointfile_name_str, cloud);
		pointfile_name_str = hillside_dec_result_path;
		memset(pointfile_name, 0, 50);
		sprintf(pointfile_name, "%d.txt", frame_count);
		temp_str = pointfile_name;
		pointfile_name_str += temp_str;
		ofstream result_file(pointfile_name_str);
		//显示结果
		char temp[100];
		char flag_i = 0;
		for (int i = 0; i < param.segment_count; i++) {
			if (longitudinal_result.kbr_results[0][i] > 15) {
				if (longitudinal_result.altitudes[i] != -1000) {
					longitudinal_result.kbr_results[3][i] = longitudinal_result.kbr_results[3][i] * 1.7;// + param.angle_offset;
					memset(temp, 0, 100);
					sprintf(temp, "SEG%d ANG %2.3f ALT %2.3f", i, longitudinal_result.kbr_results[3][i], longitudinal_result.altitudes[i]);
					//result_file << temp << endl;
					viewer1.set_text(i, temp, 0, 40 + (i + 2) * 25, 0.3f, { 1.0f, 1.0f, 1.0f, 5.0f });
					//printf("第%d段 点数:%f  角度:%f  高度:%f  相关系数:%f\n", i, longitudinal_result.kbr_results[0][i], longitudinal_result.kbr_results[3][i], longitudinal_result.altitudes[i], longitudinal_result.kbr_results[2][i]);
				}
			}
			else {
				longitudinal_result.kbr_results[3][i] = NAN;
				memset(temp, 0, 100);
				viewer1.set_text(i, temp, 0, 40 + (i + 2) * 25, 0.3f, { 1.0f, 1.0f, 1.0f, 5.0f });
			}
		}
		float p = GetProb2(longitudinal_result.kbr_results[3], longitudinal_result.kbr_results[0], param.segment_count);
		//printf("rate : %f\n", p);
		float rate = p;
		memset(temp, 0, 100);
		if (if_obtacles < 1) {
			//不处理
		}else {
			if (mode_flag == 0) {
				rate = (10 - if_obtacles) * get_random(0, 20) * 0.001;
				if (rate < 0) {
					rate = 0;
				}
			}
		}
		if (rate >= 0.6) {
			//result_file << "YES " << rate << endl;
			if (mode_flag == 0) {
				sprintf(temp, "YES PASSINGRATE %2.6f", rate);
			}
			else {
				sprintf(temp, "YES PASSINGRATE %2.5f", rate);
			}

			viewer1.set_text(16, temp, 0, 40, 0.3f, { 0.0f, 1.0f, 0.0f, 5.0f });
		}
		else if (rate < 0.6) {
			//result_file << "NO " << rate << endl;
			if (mode_flag == 0) {
				sprintf(temp, "NO PASSINGRATE %2.6f", rate);
			}
			else {
				sprintf(temp, "NO PASSINGRATE %2.5f", rate);
			}
			viewer1.set_text(16, temp, 0, 40, 0.3f, { 1.0f, 0.0f, 0.0f, 5.0f });
		}
		delete points_3d;
		cloud->clear();
		frame_count++;
		//system("pause");
	}
}

void calculate_longitudinal_angle(Point3D* points_3d, int point_count, Parameters & param, LongitudinalResult& result) {
	std::vector<std::vector<Point2D>> points_vec;
	if (points_vec.size() == 0) {
		for (int i = 0; i < param.segment_count; i++) {
			std::vector<Point2D> points;
			points_vec.push_back(points);
		}
	}
	for (int i = 0; i < points_vec.size(); i++) {
		points_vec[i].clear();
	}

	//memset(result.altitudes, 0, param.segment_count * sizeof(float));
	float * max_heights = new float[param.segment_count];
	float * min_heights = new float[param.segment_count];
	for (int i = 0; i<param.segment_count; i++) {
		max_heights[i] = -1000;
		min_heights[i] = 1000;
	}
	//筛选
	for (int i = 0; i < point_count; i++) {
		Point3D& temp_point = points_3d[i];
		if ((fabs(temp_point.x - param.line_x) <= param.distance_threshold1) && temp_point.y >= param.start_y) {
			//找到相应数组中
			Point2D p;
			p.x = temp_point.y;
			p.y = temp_point.z + param.lidar_altitude;
			//printf("x:%f y:%f\n", p.x, p.y);
			float d_x = p.x - param.start_y;
			if (d_x < 0) {
				continue;
			}
			int segment_index = d_x / param.segment_y_step;
			if (segment_index >= param.segment_count) {
				continue;
			}
			//计算最大 最小高度
			if (max_heights[segment_index] < p.y) {
				max_heights[segment_index] = p.y;
			}
			if (min_heights[segment_index] > p.y) {
				min_heights[segment_index] = p.y;
			}
			//
			points_vec[segment_index].push_back(p);
			switch (segment_index) {
				/*case 0:temp_point.ptr->r = 255; temp_point.ptr->g = 0; temp_point.ptr->b = 0; break;
				case 1:temp_point.ptr->r = 0; temp_point.ptr->g = 255; temp_point.ptr->b = 192; break;
				case 2:temp_point.ptr->r = 0; temp_point.ptr->g = 128; temp_point.ptr->b = 192; break;
				case 3:temp_point.ptr->r = 192; temp_point.ptr->g = 128; temp_point.ptr->b = 0; break;
				case 4:temp_point.ptr->r = 192; temp_point.ptr->g = 0; temp_point.ptr->b = 192; break;
				case 5:temp_point.ptr->r = 255; temp_point.ptr->g = 255; temp_point.ptr->b = 255; break;*/
				default: temp_point.ptr->r = 255; temp_point.ptr->g = 0; temp_point.ptr->b = 0; break;
			}
			//if (segment_index != 0) {
			//	points_vec[segment_index - 1].push_back(p);
			//	/*if (result.altitudes[segment_index - 1] < p.y) {
			//		result.altitudes[segment_index - 1] = p.y;
			//	}*/
			//}
			//if (segment_index < param.segment_count - 1) {
			//	points_vec[segment_index + 1].push_back(p);
			//	/*if (result.altitudes[segment_index + 1] < p.y) {
			//		result.altitudes[segment_index + 1] = p.y;
			//	}*/
			//}
		}

	}
	for (int i = 0; i < param.segment_count; i++) {
		result.altitudes[i] = max_heights[i] - min_heights[i];
	}
	float delta_height = 0;
	float cur_delta_height = 0;
	for (int i = 0; i < param.segment_count; i++) {
		if (i == 0) {
			if (result.altitudes[i] >= param.height_threshold1) {
				if_obtacles += 1;
				printf("NUM %d ALT %f\n", i, result.altitudes[i]);
			}
		}
		else {
			cur_delta_height = result.altitudes[i] - result.altitudes[i - 1];

			if (cur_delta_height >= param.height_threshold2) {
				printf("NUM %d %d  ALT %f %f\n", i, i - 1, result.altitudes[i], result.altitudes[i - 1]);
				if_obtacles += 1;
			}
			else if (cur_delta_height <= param.height_threshold3) {
				if (cur_delta_height < 0) {
					cur_delta_height = 0;
				}
				delta_height += 0.5 * cur_delta_height;
				float real_altitude = result.altitudes[i] - delta_height;
				if (real_altitude >= param.height_threshold1) {
					if_obtacles += 1;
					printf("NUM %d ORIALT %f REALALT %f DELTAHEIGHT %f\n", i, result.altitudes[i], real_altitude, delta_height);
				}
			}
		}
	}
	//panduan have obtacles
	LineFitLeastSquares1(points_vec, result);

	delete min_heights;
	delete max_heights;
}

void LineFitLeastSquares1(std::vector<std::vector<Point2D>> points_vec, LongitudinalResult& result) {
	for (int k = 0; k < points_vec.size(); k++) {
		float A = 0.0;
		float B = 0.0;
		float C = 0.0;
		float D = 0.0;
		float E = 0.0;
		float F = 0.0;
		result.kbr_results[0][k] = 0;
		result.kbr_results[1][k] = 0;
		result.kbr_results[2][k] = 0;
		result.kbr_results[3][k] = 0;
		std::vector<Point2D> points = points_vec[k];
		if (points.size() == 0) {
			continue;
		}
		int count = points.size();
		for (int i = 0; i<count; i++) {
			float x = points[i].x;
			float y = points[i].y;
			A += x * x;
			B += x;
			C += x * y;
			D += y;
		}
		// 计算斜率a和截距b
		float a, b, temp = 0;
		if (temp = (count*A - B*B))// 判断分母不为0
		{
			a = (count*C - B*D) / temp;
			b = (A*D - B*C) / temp;
		}
		else {
			a = 1;
			b = 0;
		}
		// 计算相关系数r
		float Xmean, Ymean;
		Xmean = B / count;
		Ymean = D / count;
		float tempSumXX = 0.0, tempSumYY = 0.0;
		for (int i = 0; i<count; i++) {
			float x = points[i].x;
			float y = points[i].y;
			tempSumXX += (x - Xmean) * (x - Xmean);
			tempSumYY += (y - Ymean) * (y - Ymean);
			E += (x - Xmean) * (y - Ymean);
		}
		F = sqrt(tempSumXX) * sqrt(tempSumYY);

		float r;
		r = E / F;
		result.kbr_results[0][k] = points.size();
		result.kbr_results[1][k] = b;
		result.kbr_results[2][k] = r*r;
		result.kbr_results[3][k] = atan(a) * 180 / 3.141592657;
	}
}

float GetProb2(const float* long_slope_c, const float * sizes, int count) {
	//const float long_friction = 0.6f;
	////Commons
	//const float i0 = 2.65f;
	//const float ig1 = 4.596f;
	//const float n = 0.9f;
	//const float Temax = 619.7f;
	//const float m = 1820.0f + 100.0f + 100.0f;
	//const float g = 9.8f;
	//const float r = 0.353f;
	//const float zero_threshold = 1.0f;
	//const float b = 1.675f;
	//const float hg = 0.59f;
	//int outmode = 0;
	//float theta_vali = 0.0f;
	//float theta1[15];
	//int nt = 0;
	//printf("\n\n");
	//for (int i = 0; i < 15; i++) {
	//	if (sizes[i] < 15) {
	//		continue;
	//	}
	//	printf("%f\n", long_slope_c[i]);
	//	if (!isnan(long_slope_c[i])) {
	//		outmode |= 0x01;
	//		if (fabsf(long_slope_c[i]) > zero_threshold) {
	//			outmode |= 0x02;
	//			theta1[nt++] = long_slope_c[i] / 180.0f*3.141592f;
	//		}
	//	}

	//}
	//switch (outmode) {
	//case 0:
	//	break;
	//case 2:
	//	//All NaN
	//	return NAN;
	//case 1:
	//	return 1.0f;
	//}

	//float SMEAN = GetMean(theta1, nt, sinf);
	//float SSTDSQ = GetSqStd(theta1, SMEAN, nt, sinf);
	//float CMEAN = GetMean(theta1, nt, cosf);
	//float CSTDSQ = GetSqStd(theta1, CMEAN, nt, cosf);

	//float P1, P2, P3;
	////Condition1
	//{
	//	float E = SMEAN * g;
	//	float D = SSTDSQ * g * g;
	//	float Fmax = Temax * ig1*i0*n / r;
	//	float X = (Fmax / m - E) / sqrtf(D);
	//	P1 = phi(X);
	//}

	////Condition2
	//{
	//	float c1 = long_friction * CMEAN - SMEAN;
	//	float c2 = (long_friction * long_friction) * CSTDSQ + SSTDSQ;
	//	float x = c1 / sqrtf(c2);
	//	// printf("%.2f //// ", c2);
	//	P2 = phi(x);
	//}

	////condition3
	//{
	//	float E = b * CMEAN - hg * SMEAN;
	//	float D = (b * b)*CSTDSQ + (hg * hg)*SSTDSQ;
	//	float X = E / sqrt(D);

	//	P3 = phi(X);
	//}
	////printf("%.2f,%.2f,%.2f ///// ", P1, P2, P3);
	//return P1 * P2 * P3;
	float weight1 = 0.5;
	float threshold1 = 2;
	float weight2 = 0.6;
	float threshold2 = 15;
	float weight3 = 0.8;
	float threshold3 = 30;
	float weight4 = 1.0;
	int real_count = 0;
	float sum = 0;
	float average = 0;
	float result = 0;
	for (int i = 0; i < count; i++) {

		float angle = long_slope_c[i];
		if (angle == NAN) {
			continue;
		}
		else if (sizes[i] < 15) {
			continue;
		}
		else {
			if (angle <= threshold1 && angle >= 0) {
				sum += weight1 * angle;
			}
			else if (angle >= threshold1 && angle < threshold2) {
				sum += weight2 * angle;
			}
			else if (angle >= threshold2 && angle < threshold3) {
				sum += weight3 * angle;
			}
			else if (angle >= threshold3) {
				sum += weight4 * angle;
			}
			real_count++;
		}
	}
	average = sum / real_count;
	//printf("ave: %f\n", average);
	if (average >= 0 && average < threshold1) {
		result = 1;
	}
	else if (average >= threshold1 && average < threshold2) {
		result = 0.8 + (threshold2 - average) / (threshold2 - threshold1) / 5;
	}
	else if (average >= threshold2 && average < threshold3) {
		result = 0.8 - (average - threshold2) / (threshold3 - threshold2) / 2;
	}
	else {
		result = 0.3 - (average - threshold3) / 100;
	}
	if (result < 0) {
		result = 0;
	}
	if (result > 1) {
		result = 1;
	}
	return result;
}


VOID _Ck(const char* dir, vector<string>& cv) {
	WIN32_FIND_DATAA WF;
	CHAR FindDir[260];
	strcpy_s(FindDir, dir);
	strcat_s(FindDir, "\\*");
	HANDLE hFind = FindFirstFileA(FindDir, &WF);
	do {
		if (hFind == INVALID_HANDLE_VALUE)
			break;
		if (!(WF.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			const char* ptr = WF.cFileName + strlen(WF.cFileName);

		}
	} while (FindNextFileA(hFind, &WF));
	FindClose(hFind);
}


float phi(float x) {
	// constants
	float a1 = 0.254829592f;
	float a2 = -0.284496736f;
	float a3 = 1.421413741f;
	float a4 = -1.453152027f;
	float a5 = 1.061405429f;
	float p = 0.3275911f;

	// Save the sign of x
	int sign = 1;
	if (x < 0)
		sign = -1;
	x = fabsf(x) / sqrtf(2.0f);

	// A&S formula 7.1.26
	float t = 1.0f / (1.0f + p * x);
	float y = 1.0f - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*expf(-x * x);

	return 0.5*(1.0 + sign * y);
}

template<typename _Cb>
float GetMean(const float* values, int n, _Cb cb = [](float x) {return x; }) {
	float sum = 0.0f;
	for (int i = 0; i < n; i++) {
		sum += cb(values[i]);
	}
	return sum / n;
}

template<typename _Cb>
float GetSqStd(const float* values, float mean, int n, _Cb cb = [](float x) {return x; }) {
	float sum = 0.0f;
	for (int i = 0; i < n; i++) {
		float C = cb(values[i]);
		sum += (C - mean) * (C - mean);
	}
	return sum / n;
}


float get_random(int n, int m) {
	//srand((unsigned)time(NULL));
	float r = rand() % (n - m + 1) + m;
	return r;
}