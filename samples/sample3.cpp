//Sample 3: Test pcl
//
#include"std_header.h"
#include"pcl_header.h"
using namespace pcl;
using namespace std;
int main() {
	string path = "D:/data_set/test_data/20190801_test_vlp16point/10hz/0.pcd";
	PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);
	PCDReader reader;
	reader.read(path, *cloud);
	pcl::visualization::CloudViewer viewer("pcd viewer");
	viewer.showCloud(cloud);
	system("pause");
	return 0;
}