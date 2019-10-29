//Sample 1: Create point cloud and use passthrough filter
//		  to select and delete points in point cloud.
//
#include"core.hpp"
#include"preprocess.hpp"
using namespace mammoth;
int main() {
	MBinaryPointCloud<MPoint3D> pointcloud_1(5, MPoint3D(0, 0, 0));
	pointcloud_1.push_back(MPoint3D(1, 0, 0));
	pointcloud_1.print();
	PassthroughFilter<MPoint3D> filter_3D;
	filter_3D(pointcloud_1, 0, 0, 0);
	pointcloud_1.print();
	MBinaryPointCloud<MPoint2D> pointcloud_2;
	for (int i = 0; i <= 20; ++i) {
		MPoint2D point(i, 20 - i);
		pointcloud_2.push_back(point);
	}
	pointcloud_2.print();
	PassthroughFilter<MPoint2D> filter_2D;
	filter_2D(pointcloud_2, 0, 8, -1);
	pointcloud_2.print();
	system("pause");
	return 0;
}