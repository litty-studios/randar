#include <algorithm>
#include <randar/Filesystem/Path.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

// Default constructor.
randar::Path::Path()
: randar::Path(".")
{

}

// Constructor, given a UNIX-style string path.
randar::Path::Path(const std::string& path)
{
    *this = path;
}

// Copy constructor.
randar::Path::Path(const randar::Path& other)
: parts(other.parts)
{

}

// Converts to a platform-appropriate string.
std::string randar::Path::toString() const
{
    return randar::join(this->parts, '/');
}

randar::Path::operator std::string() const
{
    return this->toString();
}

// Assignment operator, to a UNIX-style string path.
randar::Path& randar::Path::operator =(const std::string& path)
{
    if (path == "") {
        this->parts.clear();
        this->parts.push_back(".");
    }

    else {
        randar::split(path, "/\\", this->parts);
    }

    return *this;
}

#if defined (__linux__)
#error "Unimplemented randar::Path::getCwd()"
#elif defined (_WIN32)
std::string randar::Path::getCwd()
{
	char buffer[MAX_PATH];
	::GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string path(buffer);
	auto pos = path.find_last_of("\\/");
	
	return path.substr(0, pos + 1);
}
#else
#error "Unimplemented randar::Path::getCwd()"
#endif