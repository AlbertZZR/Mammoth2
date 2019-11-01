#include "std_header.h"
#include "preprocess.hpp"
#include "glviewer.h"
#include "mthread.hpp"

using namespace std;
using namespace mammoth;

void test1();
void test2();
void test3();

int main(){
	//test1();
	//test2();
	test3();
	
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