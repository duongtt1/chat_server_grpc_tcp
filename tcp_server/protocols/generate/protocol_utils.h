#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>

template<typename T>
class Serializer {
public:
    static void serialize(const T& data, std::vector<char>& buffer) {
        const char* data_ptr = reinterpret_cast<const char*>(&data);
        buffer.insert(buffer.end(), data_ptr, data_ptr + sizeof(data));
    }
    
    static T deserialize(const std::vector<char>& buffer, size_t& offset) {
        if (offset + sizeof(T) > buffer.size()) {
            throw std::out_of_range("Buffer underflow during deserialization");
        }
        T data;
        std::memcpy(&data, buffer.data() + offset, sizeof(T));
        offset += sizeof(T);
        return data;
    }
};

class StringSerializer {
public:
    static void serialize(const std::string& str, std::vector<char>& buffer) {
        // Serialize string length
        size_t length = str.size();
        Serializer<size_t>::serialize(length, buffer);
        
        // Serialize string data
        buffer.insert(buffer.end(), str.begin(), str.end());
    }
    
    static std::string deserialize(const std::vector<char>& buffer, size_t& offset) {
        // Deserialize string length
        size_t length = Serializer<size_t>::deserialize(buffer, offset);
        
        // Deserialize string data
        if (offset + length > buffer.size()) {
            throw std::out_of_range("Buffer underflow during deserialization");
        }
        std::string str(buffer.begin() + offset, buffer.begin() + offset + length);
        offset += length;

        return str;
    }
};

// template<typename T>
// class Serializer {
// public:
//     static void serialize(const T& data, std::vector<char>& buffer) {
//         const char* data_ptr = reinterpret_cast<const char*>(&data);
//         buffer.insert(buffer.end(), data_ptr, data_ptr + sizeof(data));
//     }
    
//     static T deserialize(const std::vector<char>& buffer, size_t& offset) {
//         if (offset + sizeof(T) > buffer.size()) {
//             throw std::out_of_range("Buffer underflow during deserialization");
//         }
//         T data;
//         std::memcpy(&data, buffer.data() + offset, sizeof(T));
//         offset += sizeof(T);
//         return data;
//     }
// };

// class StringSerializer {
// public:
//     static void serialize(const std::string& str, std::vector<char>& buffer) {
//         // Serialize string length (as fixed-size, assuming 32-bit length)
//         size_t length = str.size();
//         Serializer<size_t>::serialize(length, buffer);
        
//         // Serialize string data
//         buffer.insert(buffer.end(), str.begin(), str.end());
//     }
    
//     static std::string deserialize(const std::vector<char>& buffer, size_t& offset, size_t length) {
//         // Deserialize string length
//         printf("offset: %d \t length: %d \t size(): %d \n", offset, length, buffer.size());
//         if (offset + length > buffer.size()) {
//             throw std::out_of_range("Buffer underflow during deserialization");
//         }
//         std::string str(buffer.begin() + offset, buffer.begin() + offset + length);
//         offset += length;

//         return str;
//     }
// };



#endif // SERIALIZATION_H