#ifndef RANDAR_UTILITY_FILE_HPP
#define RANDAR_UTILITY_FILE_HPP

#include <fstream>
#include <stdexcept>

namespace randar
{
    /**
     * Retrieves a file's extension.
     *
     * Always returned in lowercase.
     */
    std::string getFileExtension(const std::string& file);

    /**
     * Retrieves a file's base name.
     */
    std::string getFileBaseName(const std::string& file);

    /**
     * Loads an ASCII file into memory.
     */
    std::string readAsciiFile(std::string filename);

    /**
     * Writes an ASCII file to disk.
     */
    bool writeAsciiFile(std::string filename, std::string contents);

    /**
     * Retrieves a list of files in a directory.
     */
    std::vector<std::string> getDirectoryFiles(const std::string& directory);
}

#endif
