#include <omnitensor.hpp>
#include <iostream>
#include <stdexcept>

namespace phasm {


namespace detail {

void NestedListV3::reserve(size_t dim, size_t length) {

    size_t current_offset = 0;
    if (dim > m_depth) {
        throw std::runtime_error("Dimension is too deep!");
    }
    for (size_t i=0; i<dim-1; ++i) {
        // Walk to _latest_ node at depth dim
        size_t capacity = m_contents[current_offset];
        if (capacity == 0) {
            throw std::runtime_error("Outer dimension needs to be reserved first!");
        }
        size_t size = m_contents[current_offset+1];
        size_t pointer_to_last = m_contents[current_offset+1];
        current_offset = pointer_to_last;
    }
    
    size_t capacity = m_contents[current_offset];
    size_t size = m_contents[current_offset+1];
    if (size >= capacity) {
        throw std::runtime_error("Outer dimension is already full!");
    }
    m_contents[current_offset+1] = size+1;
    m_contents[current_offset+size+1] = m_contents.size();
    m_contents.push_back(length);
    m_contents.push_back(0);
    for (int i=0; i<length; ++i) {
        m_contents.push_back(0);
    }
}

void NestedListV3::append(const TensorIndices& indices, size_t offset, size_t count) {

}

std::pair<size_t,size_t> NestedListV3::get(const TensorIndices& indices) {

    size_t current_offset = 0;
    for (size_t i=0; i<m_depth; ++i) {
        // Walk to _latest_ node at depth dim
        size_t index = (indices.get_dim_count() > m_depth) ? indices.get_index(dim) : 0;
        size_t capacity = m_contents[current_offset];
        if (capacity == 0) {
            throw std::runtime_error("Outer dimension needs to be reserved first!");
        }
        size_t size = m_contents[current_offset+1];
        size_t pointer_to_last = m_contents[current_offset+1];
        current_offset = pointer_to_last;
    }
    
    size_t capacity = m_contents[current_offset];
    size_t size = m_contents[current_offset+1];
    if (size >= capacity) {
        throw std::runtime_error("Outer dimension is already full!");
    }
    m_contents[current_offset+1] = size+1;
    m_contents[current_offset+size+1] = m_contents.size();
    m_contents.push_back(length);
    m_contents.push_back(0);
    for (int i=0; i<length; ++i) {
        m_contents.push_back(0);
    }
    size_t current_offset = 0;
    for (size_t dim=0; dim<m_depth; ++dim) {
        size_t index = (indices.get_dim_count() > dim) ? indices.get_index(dim) : 0;
        // In case the user provided an index _shorter_ than m_depth, "pad" index with extra '0's. 


    }
    return {};
}


} // namespace detail


omnitensor::~omnitensor() {
    switch (m_dtype) {
        case DType::UI8: delete[] static_cast<uint8_t*>(m_data); break;
        case DType::I16: delete[] static_cast<int16_t*>(m_data); break;
        case DType::I32: delete[] static_cast<int32_t*>(m_data); break;
        case DType::I64: delete[] static_cast<int64_t*>(m_data); break;
        case DType::F32: delete[] static_cast<float*>(m_data); break;
        case DType::F64: delete[] static_cast<double*>(m_data); break;
        default:
            if (m_length > 0) {
                std::cout << "PHASM: Memory leak due to invalid (corrupt?) tensor dtype" << std::endl;
                std::terminate();
            }
            break;
    }
};



} // namespace phasm
    
