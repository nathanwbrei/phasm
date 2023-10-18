
#pragma once
#include "tensor.hpp"
#include <stddef.h>
#include <vector>
#include <iostream>
#include <map>
#include <variant>
#include <array>
#include <initializer_list>


namespace phasm {


class TensorIndices {
    std::vector<size_t> m_data;
public:
    inline TensorIndices(std::vector<size_t> indices) : m_data(indices) {};
    inline TensorIndices(std::initializer_list<size_t> indices) : m_data(indices) {};
    inline size_t get_dim_count() const { return m_data.size(); }
    inline size_t get_index(size_t dim) const { return m_data[dim]; }
    inline void set_index(size_t dim, size_t index) { m_data[dim] = index; }
};


namespace detail {

class NestedListV1 {
    std::map<TensorIndices, std::pair<size_t, size_t>> m_leaves;
public:
    void insert(const TensorIndices& indices, size_t offset, size_t count);
    std::pair<size_t,size_t> get(const TensorIndices& index);
};

class NestedListV2 {
    std::vector<size_t> m_contents;
    size_t m_depth;
public:
    inline NestedListV2(size_t depth) : m_depth(depth) {};
    void append(const TensorIndices& indices, size_t offset, size_t count);
    void insert(const TensorIndices& indices, size_t offset, size_t count);
    std::pair<size_t,size_t> get(const TensorIndices& indices);
};


class NestedListV3 {
    std::vector<size_t> m_contents;
    size_t m_depth;
public:
    inline NestedListV3(size_t depth) : m_depth(depth) {}
    void reserve(size_t dim, size_t length);
    void append(const TensorIndices& indices, size_t offset, size_t count);
    std::pair<size_t,size_t> get(const TensorIndices& index);
};

} // namespace detail



class omnitensor {

public:
    enum class DimType { Array, Vector, List };

    void* m_data;
    size_t* m_sizes;
    size_t m_capacity;
    size_t m_length = 0;
    size_t m_dim_count;
    std::vector<int64_t> m_shape; // 0 for variably-sized dimensions
    std::vector<DimType> m_dimtypes;
    std::vector<size_t> m_dim_offsets;
    DType m_dtype;

public:
    omnitensor(DType dtype, std::vector<int64_t> shapes, size_t capacity=1024) {

        assert(capacity > 0);
        m_capacity = capacity;
        m_shape = shapes;
        m_dim_count = shapes.size();
        m_dtype = dtype;
        m_sizes = new size_t[capacity];

        switch(dtype) {
            case DType::UI8: m_data = new uint8_t[capacity]; break;
            case DType::I16: m_data = new int16_t[capacity]; break;
            case DType::I32: m_data = new int32_t[capacity]; break;
            case DType::I64: m_data = new int64_t[capacity]; break;
            case DType::F32: m_data = new float[capacity];   break;
            case DType::F64: m_data = new double[capacity];  break;
            default: throw std::runtime_error("Bad dtype");
        }
        
        m_length = 1;
        for (size_t i = 0; i < m_dim_count; ++i) {
            m_length *= m_shape[i]; // If we have any zero-dimension dims, length will be 0
                                    
            if (i == m_dim_count - 1) {
                if (m_shape[i] == 0) {
                    m_dimtypes.push_back(DimType::Vector);
                }
                else {
                    m_dimtypes.push_back(DimType::Array);
                }
            }
            else {
                if (m_shape[i] == 0 && m_shape[i+1] == 0) {
                    m_dimtypes.push_back(DimType::List);
                }
                else if (m_shape[i] == 0 && m_shape[i+1] != 0) {
                    m_dimtypes.push_back(DimType::Vector);
                }
                else if (m_shape[i] != 0 && m_shape[i+1] == 0) {
                    assert(false);
                }
                else if (m_shape[i] != 0 && m_shape[i+1] != 0) {
                    m_dimtypes.push_back(DimType::Array);
                }
            }
        }

        size_t offset = 1;
        m_dim_offsets.resize(m_dim_count);
        for (size_t dim = 0; dim < m_dim_count; ++dim) {
            size_t rdim = m_dim_count - 1 - dim;
            m_dim_offsets[rdim] = offset;
            offset *= m_shape[rdim];
        }

    };
    size_t length() {
        return m_length;
    }

    ~omnitensor();

    inline size_t offset(const TensorIndices& indices) {
        size_t offset = 0;
        for (size_t dim=0; dim<indices.get_dim_count(); ++dim) {
            if (m_dimtypes[dim] == DimType::Array) {
                offset += m_dim_offsets[dim] * indices.get_index(dim);
            }
        }
        return offset;
    }

    inline size_t length(const TensorIndices& current) {
        if (current.get_dim_count() == 0) {
            return m_length;
        }
        TensorIndices next = current;
        size_t last_dim = next.get_dim_count() - 1;
        next.set_index(last_dim, current.get_index(last_dim) + 1);
        return offset(next) - offset(current);
    }

    template <typename T>
    std::pair<T*, size_t> data(const TensorIndices& indices) {
        return { static_cast<T*>(m_data)+offset(indices), length(indices)};
    }
};

}; // namespace phasm
