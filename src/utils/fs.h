
#ifndef _FS_H_
#define _FS_H_

//#include <netinet/in.h>
#include <stdlib.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>

namespace fs {

inline uint32_t read_uint32_be(std::ifstream &ifs) {
    uint32_t v;
    ifs.read(reinterpret_cast<char *>(&v), sizeof(v));
    if (ifs.fail() == true) {
        std::cout << "file read error" << std::endl;
        exit(EXIT_FAILURE);
    }
    // return ntohl(v);
    return (((v & 0x000000ff) << 24) | ((v & 0x0000ff00) << 8) |
            ((v & 0x00ff0000) >> 8) | ((v & 0xff000000) >> 24));
}

inline std::vector<int8_t> read_all_int8(std::ifstream &ifs) {
    int64_t pos = ifs.tellg();
    ifs.seekg(0, ifs.end);
    int64_t length = ifs.tellg();
    ifs.seekg(pos);

    std::vector<int8_t> ret;
    ret.reserve(length);
    ret.insert(ret.begin(), std::istreambuf_iterator(ifs), {});
    return ret;
}

inline std::vector<uint8_t> read_all_uint8(std::ifstream &ifs) {
    int64_t pos = ifs.tellg();
    ifs.seekg(0, ifs.end);
    int64_t length = ifs.tellg();
    ifs.seekg(pos);

    std::vector<uint8_t> ret;
    ret.reserve(length);
    ret.insert(ret.begin(), std::istreambuf_iterator(ifs), {});
    return ret;
}

}; // namespace fs

#endif /* _FS_H_ */
