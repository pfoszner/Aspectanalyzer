#ifndef ARRAY_H
#define ARRAY_H

#pragma once

#include <vector>

template <class T>
class Array
{

public:
    std::size_t size(int dim) const { if (dim == 0) return width; else if (dim == 1) return height; else return -1; }
	const std::vector<T>& getData() const { return v; } 
	std::vector<T>& getData() { return v; } 

    Array(int dim1, int dim2) : width(dim1), height(dim2), v(dim1 * dim2) {}
	Array(int size) : width(size), height(size), v(size * size) {}
	Array() : width(0), height(0) {}
	Array(const Array<T>& ref) : width(ref.width), height(ref.height), v(ref.v) {}
    //Array(Array&& rhs) : width(rhs.width), height(rhs.height), v(rhs.v)
    //{
        //v(rhs.v);
        //std::copy(v.begin(), v.end(), rhs.v.begin());
    //}

    T* operator[](const int row) { return v.data() + row * height; }
    const T* operator[](const int row) const { return v.data() + row * height; }


    void Initialize(int dim1, int dim2, T const& start)
    {
        width = dim1;
        height = dim2;
        v = std::vector<T>(dim1 * dim2, start);
    }

    void Initialize(const Array<T> &copy)
    {
        width = copy.width;
        height = copy.height;
        v = std::vector<T>(copy.v);
    }

//protected:
	int width;
	int height;
	std::vector<T> v; 
};


#endif // ARRAY_H
