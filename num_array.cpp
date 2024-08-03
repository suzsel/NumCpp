#include <vector>
#include <iostream>
#include <random>

template <typename T>
class Array {
private:
    std::vector<T> data;
    std::vector<size_t> shape;
    std::vector<size_t> strides;
    size_t total_size;

    void calculate_strides() {
        strides.resize(shape.size());
        size_t stride = 1;
        for (int i = shape.size() - 1; i >= 0; --i) {
            strides[i] = stride;
            stride *= shape[i];
        }
    }

    void print_recursive(size_t dim, size_t offset, const std::string& prefix) {
        if (dim == shape.size() - 1) {
            std::cout << prefix << "[";
            for (size_t i = 0; i < shape[dim]; ++i) {
                std::cout << data[offset + i * strides[dim]];
                if (i < shape[dim] - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        } else {
            std::cout << prefix << "[" << std::endl;
            for (size_t i = 0; i < shape[dim]; ++i) {
                print_recursive(dim + 1, offset + i * strides[dim], prefix + " ");
            }
            std::cout << prefix << "]" << std::endl;
        }
    }

public:
    Array(const std::vector<size_t>& shape, bool rand_init = false, T init_val = T()) : shape(shape), total_size(1) {
        for (auto& s : shape) {
            total_size *= s;
        }
        data.resize(total_size, init_val);
        calculate_strides();
        
        if (rand_init) {
            initialize_random();
        } 
    }

    struct Proxy {              // for arr[0][1][2] = 5;
        Array<T>& array;
        std::vector<size_t> indices;

        Proxy(Array<T>& array, size_t first_index) : array(array) {
            indices.push_back(first_index);
        }

        Proxy& operator[](size_t index) {
            indices.push_back(index);
            return *this;
        }

        Proxy& operator=(const T& value) {
            if (indices.size() == array.shape.size()) {
                size_t flat_index = 0;
                for (size_t i = 0; i < indices.size(); ++i) {
                    flat_index += indices[i] * array.strides[i];
                }
                array.data[flat_index] = value;
            } else {
                throw std::invalid_argument("Insufficient indices provided for assignment");
            }
            return *this;
        }

        operator T&() {
            size_t flat_index = 0;
            for (size_t i = 0; i < indices.size(); ++i) {
                flat_index += indices[i] * array.strides[i];
            }
            return array.data[flat_index];
        }
    };
    Proxy operator[](size_t index) {
        return Proxy(*this, index);
    }

    T& get(const std::vector<size_t>& idx) {
        size_t flat_index = 0;
        for (size_t i = 0, stride = 1; i < idx.size(); ++i) {
            flat_index += idx[i] * stride;
            stride *= shape[shape.size() - i - 1];
        }
        return data[flat_index];
    }

    T& operator()(const std::vector<size_t>& idx) {
        size_t index = 0;
        for (size_t i = 0; i < idx.size(); ++i) {
            index += idx[i] * strides[i];
        }
        return data[index];
    }

    void initialize_random() {
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_real_distribution<T> distr(0, 1);

        for(auto& elem : data) {
            elem = distr(eng);
        }
    }
    
    void print(const std::string& prefix = "") {
        print_recursive(0, 0, prefix);
    }

    std::vector<size_t> get_strides() {
        return strides;
    }

    std::vector<size_t> get_shape() {
        return shape;
    }
};




int main() {

    Array<double> ones({3, 3}, false, 1);
    ones[1][2] = 10.0;
    ones.print();
    Array<double> rand({3, 3, 3}, true);
    rand.print();

    return 0;
}