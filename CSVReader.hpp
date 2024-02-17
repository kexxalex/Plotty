#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cinttypes>
#include <iostream>


using StringTabular = std::vector< std::vector<std::string> >;

class CSVFile
{
public:
    CSVFile(const std::string &filename);

    bool read(const char separator = ',');

    const std::vector<std::string> * getColumn(const std::string &name) const noexcept;
    uint32_t getColCount() const noexcept;
    constexpr uint32_t getRowCount() const noexcept { return m_rows; }

protected:
    std::string m_filename;
    std::unordered_map<std::string, uint32_t> m_header;
    StringTabular m_values;

    uint32_t m_rows;

    bool m_isOk;
    bool m_hasData;
};
