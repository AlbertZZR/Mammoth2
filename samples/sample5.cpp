//Sample 5: Get online lidar-data and show
//
#include "std_header.h"
#include "lidar.hpp"
#include "mviewer.hpp"

using namespace mammoth;
using namespace pcl;

int main(){
    PointCloud<PointXYZRGBA>::Ptr cloud(new PointCloud<PointXYZRGBA>());
	M3DLidar lidar(MLIDAR_TYPE::VEL_VLP16);
	MPointViewer<PointXYZRGBA> viewer1;
	viewer1.init();
	lidar.open();
	while (true) {
		lidar.get_frame(cloud);
		viewer1.set_cloud(cloud);
	}
    return 0;
}