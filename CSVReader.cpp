#include "CSVReader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>


static std::vector<std::string> readCSVLine(const std::string &line, const char sep)
{
    std::vector<std::string> splitLineData;

    std::stringstream line_ss(line);
    std::string data;

    while (std::getline(line_ss, data, sep))
    {
        splitLineData.push_back(data);
    }

    return splitLineData;
}

static std::vector<std::string> readCSVLine(const std::string &line, const char sep, const uint32_t columns)
{
    std::vector<std::string> splitLineData(columns);

    std::stringstream line_ss(line);
    std::string data;

    for(uint32_t i=0; i < columns; i++)
    {
        if (std::getline(line_ss, data, sep))
            splitLineData[i] = data;
    }

    return splitLineData;
}


CSVFile::CSVFile(const std::string &filename)
    : m_filename(filename)
    , m_hasData(false)
{
    m_isOk = std::filesystem::is_regular_file(filename);
    if (!m_isOk)
        std::cerr << "File \"" << m_filename << "\" does not exist." << std::endl;
}

void CSVFile::read(const char separator)
{
    if (!m_isOk)
        return;

    std::ifstream f(m_filename);
    if (!f)
    {
        std::cerr << "Cannot read file \"" << m_filename << "\"" << std::endl;
        return;
    }

    std::string headerLine;
    std::getline(f, headerLine);
    std::vector<std::string> headers = readCSVLine(headerLine, separator);

    const uint32_t columns = uint32_t(headers.size());
    m_values.reserve(columns);
    for (uint32_t i=0; i < columns; i++)
    {
        m_header[headers[i]] = i;
        m_values.emplace_back(0);
    }

    std::string line;
    m_rows = 0;
    while (std::getline(f, line))
    {
        uint32_t i=0;
        for (const std::string &entry : readCSVLine(line, separator, columns))
        {
            m_values[i].push_back(entry);
            i++;
        }
        m_rows++;
    }

    std::cout << "Read " << m_rows << " data lines" << std::endl;

    m_hasData = true;
}


const std::vector<std::string> * CSVFile::getColumn(const std::string &name) const noexcept
{
    if (m_header.find(name) == m_header.cend())
    {
        std::cerr << "No column <" << name << "> in file \"" << m_filename << "\"" << std::endl;
        return nullptr;
    }
    
    return &m_values[m_header.at(name)];
}
