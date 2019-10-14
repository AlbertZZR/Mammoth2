#pragma once

#include"core.hpp"

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
	class PassthroughFilter {
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
	};
}