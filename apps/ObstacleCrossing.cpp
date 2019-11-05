#include "std_header.h"
#include "preprocess.hpp"
#include "lidar.hpp"
#include "mviewer.hpp"

using namespace mammoth;
using namespace pcl;
using namespace std;

void main_progress();

int main(){
	main_progress();
    return 0;
}

void main_progress() {
	PointCloud<PointXYZRGBA>::Ptr cloud(new PointCloud<PointXYZRGBA>());
	M3DLidar lidar(MLIDAR_TYPE::VEL_VLP16);
	MPointViewer<PointXYZRGBA> viewer1;
	viewer1.init();
	lidar.open();
	int frame_count = 0;
	string point_path = "";
	string result_path = "";
	while (true) {
		lidar.get_frame(cloud);
		//filtering
		pcl::PassThrough<PointXYZRGBA> passThrough;
		passThrough.setInputCloud(cloud);
		passThrough.setFilterLimitsNegative(false);
		passThrough.setFilterFieldName("x");
		passThrough.setFilterLimits(-1, 1); //100
		passThrough.filter(*cloud);
		passThrough.setFilterFieldName("y");
		passThrough.setFilterLimits(4, 8); //100
		passThrough.filter(*cloud);
		//transforming
		float angle = -0.2; // -2
		angle = angle * 3.141592657 / 180;
		for (int i = 0; i < cloud->size(); i++) {
			(*cloud)[i].x = -1 * (*cloud)[i].x;
			float temp = (*cloud)[i].y;
			(*cloud)[i].y = temp * cos(angle) - (*cloud)[i].z * sin(angle);
			(*cloud)[i].z = (*cloud)[i].z * cos(angle) + (*cloud)[i].y * sin(angle);
			(*cloud)[i].r = 0;
			(*cloud)[i].g = 255;
			(*cloud)[i].b = 0;
		}
		//ground altitude
		float consult_ground_average_altitude = -2.45;
		//= get_ground_average_altitude(cloud);
		float threshold1 = consult_ground_average_altitude - 0.1;
		float max_x = -10000;
		float max_y = -10000;
		float max_z = -10000;
		float min_x = 10000;
		float min_y = 10000;
		float min_z = 10000;
		float flag = max_x - min_x;
		for (int i = 0; i < cloud->size(); i++) {
			PointXYZRGBA point = (*cloud)[i];
			if (point.z >= threshold1) {
				if (max_z < point.z) {
					max_z = point.z;
				}
				if (min_z > point.z) {
					min_z = point.z;
				}
			}
		}
		float height = max_z - min_z;
		if (height > 0.15) {
			height -= 0.04;
			if (height >= 0.2) {
				height += 0.015;
			}
		}
		if (height <= 0.03) {
			height += 0.02;
		}
		float offset = 0.04;
		float threshold2 = min_z + offset + height * 0.25;//0.03
		for (int i = 0; i < cloud->size(); i++) {
			PointXYZRGBA point = (*cloud)[i];
			if (point.z >= threshold2) {
				if (max_x < point.x) {
					max_x = point.x;
				}
				if (min_x > point.x) {
					min_x = point.x;
				}
				if (max_y < point.y) {
					max_y = point.y;
				}
				if (min_y > point.y) {
					min_y = point.y;
				}
			}
		}

		//±£´æ½á¹û
		char pointfile_name[50];
		memset(pointfile_name, 0, 50);
		sprintf(pointfile_name, "%d.pcd", frame_count);
		string temp_str = pointfile_name;
		string pointfile_name_str = point_path;
		pointfile_name_str += temp_str;
		//PcdUtil::save_pcd_file(pointfile_name_str, cloud);
		pointfile_name_str = result_path;
		memset(pointfile_name, 0, 50);
		sprintf(pointfile_name, "%d.txt", frame_count);
		temp_str = pointfile_name;
		pointfile_name_str += temp_str;
		//std::ofstream result_file(pointfile_name_str);

		//////////////////
		float threshold = 0.16;//get_height_threshold(min_x, max_x);
		float long_edge = max_x - min_x;
		float width_edge = max_y - min_y;
		char temp[100];
		if (max_x - min_x != flag && !(long_edge >= 1.6 && height <= 0.15)
			//&& !(long_edge >= 0.8 && height <= 0.03)
			) {
			if (height < threshold || (long_edge <= 0.03) || (height >= threshold && height <= 0.2 && long_edge >= 0.65)) {
				cout << "YES ";
				//result_file << "YES" <<std::endl;
				viewer1.set_text(0, "YES", 0, 40, 0.3f, { 0.0f, 1.0f, 0.0f, 5.0f });
			}
			else {
				cout << "NO ";
				//result_file << "NO" << std::endl;
				viewer1.set_text(0, "NO", 0, 40, 0.3f, { 1.0f, 0.0f, 0.0f, 5.0f });
			}
			cout << "long:" << long_edge << " width:" << width_edge << " height:" << height;
			//result_file << long_edge << " " << width_edge << " " << height << std::endl;
			memset(temp, 0, 100);
			sprintf(temp, "LONG %3.4f", long_edge);
			//viewer1.set_text(1, temp, 0, 70, 0.3f, { 1.0f, 1.0f, 1.0f, 5.0f });
			memset(temp, 0, 100);
			sprintf(temp, "WIDTH %3.4f", width_edge);
			//viewer1.set_text(2, temp, 0, 100, 0.3f, { 1.0f, 1.0f, 1.0f, 5.0f });
			memset(temp, 0, 100);
			sprintf(temp, "HEIGHT %3.4f", height);
			//viewer1.set_text(3, temp, 0, 130, 0.3f, { 1.0f, 1.0f, 1.0f, 5.0f });
		} else {
			//no obstacles
			//result_file << "YES" << std::endl;
			//result_file << "0 0 0" << std::endl;
			cout << "YES ";
			viewer1.set_text(0, "YES", 0, 40, 0.3f, { 0.0f, 1.0f, 0.0f, 5.0f });
			memset(temp, 0, 100);
			sprintf(temp, "LONG %3.4f", 0);
			//viewer1.set_text(1, temp, 0, 70, 0.3f, { 1.0f, 1.0f, 1.0f, 5.0f });
			memset(temp, 0, 100);
			sprintf(temp, "WIDTH %3.4f", 0);
			//viewer1.set_text(2, temp, 0, 100, 0.3f, { 1.0f, 1.0f, 1.0f, 5.0f });
			memset(temp, 0, 100);
			sprintf(temp, "HEIGHT %3.4f", 0);
			//viewer1.set_text(3, temp, 0, 130, 0.15f, { 1.0f, 1.0f, 1.0f, 5.0f });
		}
		cout << endl;
		viewer1.set_cloud(cloud);
		++frame_count;
	}
}

