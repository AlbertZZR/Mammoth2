#pragma once

#include"core.hpp"
#include"pcl_header.h"

namespace mammoth {
	//_compare_type 0:equal -1:less-equal -2:less 1:greater-equal 2:greater
	template<class T>
	struct MCompare {
		bool operator ()(T& _in_data, int _in_fieldindex, float _in_value, int _compare_type) {
			switch (_compare_type) {
			case 0: {
				if (_in_data[_in_fieldindex] == _in_value) {
					return true;
				}
			}break;
			case -1: {
				if (_in_data[_in_fieldindex] <= _in_value) {
					return true;
				}
			}break;
			case -2: {
				if (_in_data[_in_fieldindex] < _in_value) {
					return true;
				}
			}break;
			case 1: {
				if (_in_data[_in_fieldindex] >= _in_value) {
					return true;
				}
			}break;
			case 2: {
				if (_in_data[_in_fieldindex] > _in_value) {
					return true;
				}
			}break;
			default: {
				return true;
			}
			}
			return false;
		}
	};

	//_compare_type 0:equal -1:less-equal -2:less 1:greater-equal 2:greater
	template<class T>
	class MPassthroughFilter {
	public:
		void operator()(MBinaryPointCloud<T>& _in_pointcloud, int _in_fieldindex, float _in_value, int _compare_type) {
			static MCompare<T> compare;
			auto data_ptr = _in_pointcloud.get_data_ptr();
			for (auto it = data_ptr->begin(), end = data_ptr->end(); it != end;) {
				if (compare(*it, _in_fieldindex, _in_value, _compare_type)) {
					it = data_ptr->erase(it);
					end = data_ptr->end();
				}
				else {
					++it;
				}
			}
			_in_pointcloud.update_array_ptr(data_ptr);
		}

		void operator()(typename pcl::PointCloud<T>::Ptr& _in_pointcloud, int _in_fieldindex, float _in_bottomvalue, float _in_topvalue, bool _in_negative = false){
			pcl::PassThrough<T> passThrough;
			passThrough.setInputCloud(_in_pointcloud);
			passThrough.setFilterLimitsNegative(_in_negative);
			//0: x  1: y  2: z
			if(_in_fieldindex == 0){
				passThrough.setFilterFieldName("x");
			}else if(_in_fieldindex == 1){
				passThrough.setFilterFieldName("y");
			}else if(_in_fieldindex == 2){
				passThrough.setFilterFieldName("z");
			}else{
				return;
			}
			passThrough.setFilterLimits(_in_bottomvalue, _in_topvalue);
			passThrough.filter(*_in_pointcloud);
		}
	};

}