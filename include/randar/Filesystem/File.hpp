#ifndef RANDAR_FILESYSTEM_FILE_HPP
#define RANDAR_FILESYSTEM_FILE_HPP

#include <randar/Filesystem/Path.hpp>
#include <randar/Filesystem/BinaryFileStream.hpp>

namespace randar
{
    class File : public Path
    {
    public:
        /**
         * Inherit constructors.
         */
        using Path::Path;

        /**
         * Opens a new binary writing stream.
         */
        BinaryFileWriter openBinaryWriter();
    };
}

#endif
