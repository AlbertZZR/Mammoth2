#pragma once

#include"std_header.h"

using std::auto_ptr;
using std::vector;
using std::cout;
using std::endl;

namespace mammoth {
	//mammoth vector base class
	class MVector {
	public:
		virtual int get_dimensions() = 0;
		float operator[](int _in_index) {
			if (_in_index < 0 || _in_index >= _dimensions) {
				return NAN;
			}
			return *(_field_array[_in_index]);
		}
		virtual void assgin(MVector& _other_vector) {};
	protected:
		int _dimensions;
		vector<float*> _field_array;
		virtual void _init_field(int _in_dimensions) = 0;
	};

	//mammoth 3D point class
	class MPoint3D : public MVector{
	public:
		float x;
		float y;
		float z;
		MPoint3D() { 
			x = y = z = 0; 
			_init_field(3);
		}
		MPoint3D(float _in_x, float _in_y, float _in_z) {
			x = _in_x;
			y = _in_y;
			z = _in_z;
			_init_field(3);
		}
		int get_dimensions() {
			return _dimensions;
		}
		void assgin(MPoint3D& _other_vector) {
			x = _other_vector.x;
			y = _other_vector.y;
			z = _other_vector.z;
		}
	protected:
		void _init_field(int _in_dimensions) {
			_dimensions = _in_dimensions;
			_field_array.resize(_dimensions);
			_field_array[0] = &x;
			_field_array[1] = &y;
			_field_array[2] = &z;
		}
	};

	//mammoth 2D point class
	class MPoint2D : public MVector{
	public:
		float x;
		float y;
		MPoint2D() { 
			x = 0;
			y = 0;
			_init_field(2);
		}
		MPoint2D(float _in_x, float _in_y) {
			x = _in_x;
			y = _in_y;
			_init_field(2);
		}
		int get_dimensions() {
			return _dimensions;
		}
		void assgin(MPoint2D& _other_vector) {
			x = _other_vector.x;
			y = _other_vector.y;
		}
	protected:
		void _init_field(int _in_dimensions) {
			_dimensions = _in_dimensions;
			_field_array.resize(_dimensions);
			_field_array[0] = &x;
			_field_array[1] = &y;
		}
	};

	//mammoth point cloud class
	template<class T>
	class MBinaryPointCloud{
	private:
		auto_ptr<vector<T>> _p_array;
	public:
		MBinaryPointCloud(int _in_size = 0){
			auto_ptr<vector<T>> _tp_array(new vector<T>(_in_size));
			_p_array = _tp_array;
		}
		MBinaryPointCloud(int _in_size, const T& _in_value){
			auto_ptr<vector<T>> _tp_array(new vector<T>(_in_size, _in_value));
			_p_array = _tp_array;
		}
		auto_ptr<vector<T>> get_data_ptr() {
			return _p_array;
		}
		//If use std::vector's erase() funcion, may need to update ptr's value,
		//or the "_p_array" ptr may become an empty auto_ptr.
		void update_array_ptr(auto_ptr<vector<T>>& _newptr) {
			_p_array = _newptr;
		}
		void push_back(T _in_value) {
			T* new_item = new T();
			new_item->assgin(_in_value);
			_p_array->push_back(*new_item);
		}
		void print() {
			cout << "------------------StartPrintPointCloud------------------" << endl;
			if ((*_p_array).size() > 0) {
				int dimension = ((*_p_array)[0]).get_dimensions();
				for (int i = 0; i < (*_p_array).size(); ++i) {
					T& item = (*_p_array)[i];
					for (int j = 0; j < dimension; ++j) {
						cout << item[j] << " ";
					}
					cout << endl;
				}
			}
			cout << "-------------------EndPrintPointCloud-------------------" << endl << endl;
		}
	};
}