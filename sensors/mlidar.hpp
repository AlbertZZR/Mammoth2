#pragma once

#include "core.hpp"
#include "std_header.h"
#include "pcl_header.h"
#include "pcap_util.hpp"

namespace mammoth{
    enum MLIDAR_TYPE{VEL_HDL32 = 0, VEL_VLP16};

    class M3DLidar{
    private:
        pcap_t * _lidar_device;
        MLIDAR_TYPE _lidar_type;
        static double * hdl32_angles;
        static uint8_t * hdl32_ids;
        static double * vlp16_angles;
        static uint8_t * vlp16_ids;
    public:
        M3DLidar(MLIDAR_TYPE _in_lidar_type){
            _lidar_type = _in_lidar_type;
        }
        MLIDAR_TYPE get_type(){
            return _lidar_type;
        }
        void open(int _in_device_num = 1){
            _lidar_device = MPcapUtil::get_device(_in_device_num);
        }
        void open(std::string path){
            _lidar_device = MPcapUtil::get_device(path);
        }
        void get_frame(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr & scene);
    };
};

void mammoth::M3DLidar::get_frame(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr & scene){
	scene->clear();
	pcap_pkthdr *pkthdr = 0;
	const u_char *pktdata = 0;
    int count = 0;
	int maxFlectivity = 0;
	int block_count = 12;
	int channel_count = 32;
	int flag_size = 2;
	int head_size = 42;
	int block_size = 100;
	int angle_address = 2;
	int angle_size = 2;
	int unit_distance_size = 2;
	int unit_reflectivity_size = 1;
	int channel_size = unit_distance_size + unit_reflectivity_size;
	float * angles = new float[block_count];
	int * distance_mm = new int[channel_count * block_count];
	int * flectivity = new int[channel_count * block_count];
    while(pcap_next_ex(_lidar_device, &pkthdr, &pktdata) >= 0){
		if (pkthdr->caplen != 1248) {
			continue;
		}
        memset(angles, 0, sizeof(float) * block_count);
        memset(distance_mm, 0, sizeof(int) * channel_count * block_count);
        memset(flectivity, 0, sizeof(int) * channel_count * block_count);
        for (int i = 0; i < block_count; i++) {
            for (int k = angle_size - 1; k >= 0; k--) {
                int index = head_size + i * block_size + angle_address + k;
                float data = pktdata[head_size + i * block_size + angle_address + k];
                angles[i] = angles[i] * 256 + data;
            }
            angles[i] = angles[i] / 100;
            for (int j = 0; j < channel_count; j++) {
                float distance = 0;
                for (int k = unit_distance_size - 1; k >= 0; k--) {
                    distance_mm[i * channel_count + j] = distance_mm[i * channel_count + j] * 256 + pktdata[head_size + flag_size + i * block_size + angle_size + j * channel_size + k];
                }
                for (int k = unit_reflectivity_size - 1; k >= 0; k--) {
                    flectivity[i * channel_count + j] = flectivity[i * channel_count + j] * 256 + pktdata[head_size + flag_size + i * block_size + angle_size + j * channel_size + unit_distance_size + k];
                    if (maxFlectivity < flectivity[i * channel_count + j]) {
                        maxFlectivity = flectivity[i * channel_count + j];
                    }
                }
                distance = distance_mm[i * channel_count + j] / 1000.0;
                int flectivity_value = flectivity[i * channel_count + j];
                float horizontal_angle = angles[i] * 3.1415926 / 180;

                MPoint3D point;
                float vertical_angle = 0;
                if (_lidar_type == VEL_HDL32) {
                    vertical_angle = hdl32_angles[j % 32] * 3.1415926 / 180;
                } else if (_lidar_type == VEL_VLP16) {
                    vertical_angle = vlp16_angles[j % 32] * 3.1415926 / 180;
                }
                point.z = distance * sin(vertical_angle);
                point.y = distance * cos(vertical_angle) * sin(-horizontal_angle);
                point.x = distance * cos(vertical_angle) * cos(-horizontal_angle);
                pcl::PointXYZRGBA pclPoint;
                pclPoint.x = 2 * point.x;
                pclPoint.y = 2 * point.y;
                pclPoint.z = 2 * point.z;
                if (_lidar_type == VEL_HDL32) {
                    pclPoint.r = flectivity_value;
                    pclPoint.b = 200;
                    pclPoint.g = hdl32_ids[j % 32];
                } else if (_lidar_type == VEL_VLP16) {
                    pclPoint.r = flectivity_value;
                    pclPoint.b = vlp16_ids[j % 32];
                    pclPoint.g = 200;
                }
                scene->push_back(pclPoint);
            }
        }
		if ((_lidar_type == VEL_HDL32 && count >= 180) || (_lidar_type == VEL_VLP16 && count >= 75)) {
			//if (last_pcd_signal_count != pcd_signal_count) { //177  180
			delete angles;
			delete distance_mm;
			delete flectivity;
			count = 0;
			break;
		}
        ++count;
    }
}


double * mammoth::M3DLidar::hdl32_angles = new double[32]{
    -30.67, -9.33, -29.33, -8.00, -28.00, -6.66, -26.66, -5.33, -25.33, -4.00, -24.00, -2.67, -22.67, -1.33, -21.33, 0, -20, 1.33, -18.67, 2.67, -17.33, 4, -16, 5.33, -14.67, 6.67, -13.33, 8, -12, 9.33, -10.67, 10.67
};

uint8_t * mammoth::M3DLidar::hdl32_ids = new uint8_t[32]{
    -23 * -1 + 9, -7 * -1 + 9, -22 * -1 + 9, -6 * -1 + 9, -21 * -1 + 9, -5 * -1 + 9, -20 * -1 + 9, -4 * -1 + 9, -19 * -1 + 9, -3 * -1 + 9, -18 * -1 + 9, -2 * -1 + 9, -17 * -1 + 9, -1 * -1 + 9, -16 * -1 + 9, 0 * -1 + 9, -15 * -1 + 9, 1 * -1 + 9, -14 * -1 + 9, 2 * -1 + 9, -13 * -1 + 9, 3 * -1 + 9, -12 * -1 + 9, 4 * -1 + 9, -11 * -1 + 9, 5 * -1 + 9, -10 * -1 + 9, 6 * -1 + 9, -9 * -1 + 9, 7 * -1 + 9, -8 * -1 + 9, 8 * -1 + 9
};

double * mammoth::M3DLidar::vlp16_angles = new double[32]{
    -15, 1, -13, 3, -11, 5, -9, 7, -7, 9, -5, 11, -3, 13, -1, 15, -15, 1, -13, 3, -11, 5, -9, 7, -7, 9, -5, 11, -3, 13, -1, 15
};

uint8_t * mammoth::M3DLidar::vlp16_ids = new uint8_t[32]{
    0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15, 0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15
};