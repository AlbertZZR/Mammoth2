#include "std_header.h"
#include "preprocess.hpp"
#include "glviewer.h"
#include "mthread.hpp"
#include "lidar.hpp"
#include "pcl_header.h"
#include "time_util.hpp"
using namespace pcl;


using namespace std;
using namespace mammoth;

void test1();
void test2();
void test3();
void test4();

int main(){
	//test1();
	//test2();
	//test3();
	test4();
    return 0;
}

void test1() {
	string path = "D:/data_set/test_data/20190801_test_vlp16point/10hz/0.pcd";
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
	pcl::io::loadPCDFile(path, *cloud);
	MPassthroughFilter<pcl::PointXYZRGBA> filter_3D;
	int count1 = 0, count2 = 0;
	for (int i = 0; i < cloud->size(); ++i) {
		if ((*cloud)[i].x < 0 || (*cloud)[i].x > 10) {
			++count1;
		}
	}
	cout << "before filter: " << cloud->size() << " count1: " << count1 << endl;
	filter_3D(cloud, 0, 0, 10);
	for (int i = 0; i < cloud->size(); ++i) {
		if ((*cloud)[i].x < 0 || (*cloud)[i].x > 10) {
			++count2;
		}
	}
	cout << "after filter: " << cloud->size() << " count2: " << count2 << endl;
};


void set_point_cloud(glviewer::GLDevice * p_glviewer, const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr & cloud) {
	p_glviewer->SetPointCloud(&((*cloud)[0]), cloud->size());
}

void test2() {
	static glviewer::GLDevice* p_glviewer = nullptr;
	if (p_glviewer == nullptr) {
		p_glviewer = GetGLDevice(0, NULL, "Point Cloud Window");
	}
	p_glviewer->SetParam(glviewer::DeviceParams::POINT_SIZE, 1.0f);
	float camera_data[9] = { 0.074241, 0.990729, -0.113753, 0.008500, 0.113435,0.993508, 0.015301,2.194579, -1.845818 };

	string path = "D:/data_set/test_data/20190801_test_vlp16point/10hz/0.pcd";
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
	pcl::io::loadPCDFile(path, *cloud);
	set_point_cloud(p_glviewer, cloud);
}

void func1() {
	cout << "hello" << endl;
	Sleep(1000);
}

void func2() {
	cout << "world" << endl;
	Sleep(1000);
}

void test3() {
	MThreadPool::create_thread("pic_thread", func1);
	MThreadPool::create_thread("pcd_thread", func2);
	Sleep(3000);
	MThreadPool::join_thread("pic_thread");
	MThreadPool::join_thread("pcd_thread");
	MThreadPool::stop_thread("pcd_thread");
	getchar();
}

void test4() {
	string path = "C:\\20191105_test_vlp16point\\";
	static glviewer::GLDevice* p_glviewer = nullptr;
	if (p_glviewer == nullptr) {
		p_glviewer = GetGLDevice(0, NULL, "Point Cloud Window");
	}
	p_glviewer->SetParam(glviewer::DeviceParams::POINT_SIZE, 1.0f);
	PointCloud<PointXYZRGBA>::Ptr cloud(new PointCloud<PointXYZRGBA>());
	M3DLidar lidar(MLIDAR_TYPE::VEL_VLP16);
	int count = 0;
	lidar.open();
	while (true) {
		char temp[10] = { 0 };
		sprintf(temp, "%d.pcd", count);
		std::string name = path + std::string(temp);
		++count;
		//cout << MTimeUtil::get_time_code_millsecond() << endl;
		lidar.get_frame(cloud);
		//cout << MTimeUtil::get_time_code_millsecond() << endl;
		//pcl::io::savePCDFileBinary(name, *cloud);
		p_glviewer->SetPointCloud(&((*cloud)[0]), cloud->size());
		//cout << MTimeUtil::get_time_code_millsecond() << endl << endl;
	}
}