#pragma once

#include "defines.hpp"
#include <string>
#include <vector>
#include <unordered_map>


using StringTabular = std::vector<std::vector<std::string>>;

class CSVFile {
public:
    explicit CSVFile( const std::string &filename );

    bool read( char separator = ',' );

    const std::vector<std::string> *getColumn( const std::string &name ) const noexcept;

    u32 getColCount() const noexcept;

    constexpr u32 getRowCount() const noexcept { return m_rows; }

protected:
    std::string m_filename;
    std::unordered_map<std::string, u32> m_header;
    StringTabular m_values;

    u32 m_rows;

    bool m_isOk;
    bool m_hasData;
};
