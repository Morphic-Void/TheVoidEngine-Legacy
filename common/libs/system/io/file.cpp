
//  File:   file.cpp
//  Author: Ritchie Brannan
//  Date:   11 Nov 10
//
//  Modernised 18 Dec 2025
//
//  Basic load and save utility functions (file <=> memory blob)
//
//  Multi-threaded usage assumes that multiple threads will not be saving
//  files with the same name.

#include "file.h"

#include <cstring>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cctype>
#include <new>

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <Windows.h>
    #include <wchar.h>
#else
//  default, assumes _POSIX_VERSION or __APPLE__ or__linux__ or similar
    #include <sys/types.h>
#endif

namespace file
{

#if defined(_WIN32) || defined(_WIN64)
    using path_t = wchar_t;
#else
    using path_t = char;
#endif

namespace internal
{

path_t* stdPath(const char* const utf8_path) noexcept
{
    path_t* std_path = nullptr;
    if ((utf8_path != nullptr) && (utf8_path[0] != 0))
    {   //  input path is not nullptr or an empty string
        std::size_t path_length = strlen(utf8_path);
        std::size_t name_index = 0;
        bool has_wildcard = false;
        char* candidate_utf8_path = new(std::nothrow) char[path_length + 1];
        if (candidate_utf8_path != nullptr)
        {
            for (std::size_t byte_index = 0; byte_index <= path_length; ++byte_index)
            {
                char byte = utf8_path[byte_index];
                if (byte == '\\')
                {
                    byte = '/';
                }
                if (byte == '/')
                {
                    name_index = byte_index + 1;
                }
                else if ((byte == '?') || (byte == '*'))
                {
                    has_wildcard = true;
                }
                candidate_utf8_path[byte_index] = byte;
            }
            std::size_t name_length = path_length - name_index;
            bool path_is_valid = !(has_wildcard || (name_length == 0) || (candidate_utf8_path[path_length - 1] == '/'));
            if (path_is_valid)
            {
                const char name_byte0 = candidate_utf8_path[name_index];
                const char name_byte1 = candidate_utf8_path[name_index + 1];
                if ((name_byte0 == '.') && ((name_length == 1) || ((name_length == 2) && (name_byte1 == '.'))))
                {   //  directory navigation only
                    path_is_valid = false;
                }
#if defined(_WIN32) || defined(_WIN64)
                else if (path_length >= 2)
                {
                    const char path_byte0 = candidate_utf8_path[0];
                    const char path_byte1 = candidate_utf8_path[1];
                    if ((path_byte0 == '/') && (path_byte1 == '/'))
                    {   //  reject Windows UNC and extended-length
                        path_is_valid = false;
                    }
                    else if ((path_length == 2) && (path_byte1 == ':') && isalpha(static_cast<unsigned char>(path_byte0)))
                    {   //  reject drive only
                        path_is_valid = false;
                    }
                }
#endif
            }
#if defined(_WIN32) || defined(_WIN64)
            if (path_is_valid)
            {
                const int wlen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, candidate_utf8_path, -1, nullptr, 0);
                if (wlen > 0)
                {   //  success, the returned wlen is the wchar count INCLUDING the null terminator
                    wchar_t* wpath = new(std::nothrow) wchar_t[static_cast<std::size_t>(wlen)];
                    if (wpath != nullptr)
                    {
                        const int wchk = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, candidate_utf8_path, -1, wpath, wlen);
                        if (wchk == wlen)
                        {   //  success, the returned size is the wchar count INCLUDING the null terminator
                            std_path = wpath;
                        }
                        else
                        {
                            delete[] wpath;
                        }
                    }
                }
            }
            delete[] candidate_utf8_path;
#else
            if (path_is_valid)
            {
                std_path = candidate_utf8_path;
            }
            else
            {
                delete[] candidate_utf8_path;
            }
#endif
        }
    }
    return std_path;
}

path_t* tmpPath(const path_t* const std_path) noexcept
{
    path_t* tmp_path = nullptr;
    if (std_path != nullptr)
    {
#if defined(_WIN32) || defined(_WIN64)
        std::size_t tmp_length = wcslen(std_path) + 5;
#else
        std::size_t tmp_length = strlen(std_path) + 5;
#endif
        tmp_path = new(std::nothrow) path_t[tmp_length];
        if (tmp_path != nullptr)
        {
#if defined(_WIN32) || defined(_WIN64)
            if (_snwprintf_s(tmp_path, tmp_length, (tmp_length - 1), L"%s.tmp", std_path) < 0)
#else
            if (std::snprintf(tmp_path, tmp_length, "%s.tmp", std_path) < 0)
#endif
            {
                delete[] tmp_path;
                tmp_path = nullptr;
            }
        }
    }
    return tmp_path;
}

FILE* openFile(const path_t* const file_path, const bool write = false) noexcept
{
    FILE* handle = nullptr;
    if (file_path != nullptr)
    {
#if defined(_WIN32) || defined(_WIN64)
        if (_wfopen_s(&handle, file_path, (write ? L"wb" : L"rb")) != 0)
        {   //  file open failed, ensure that the handle is nullptr
            handle = nullptr;
        }
#else
        handle = std::fopen(file_path, (write ? "wb" : "rb"));
#endif
    }
    return handle;
}

void removeFile(const path_t* const file_path) noexcept
{
    if (file_path != nullptr)
    {
#if defined(_WIN32) || defined(_WIN64)
        _wremove(file_path);
#else
        std::remove(file_path);
#endif
    }
}

bool renameFile(const path_t* const src_path, const path_t* const dst_path) noexcept
{
#if defined(_WIN32) || defined(_WIN64)
    bool success = MoveFileExW(src_path, dst_path, (MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) != 0;
#else
    bool success = std::rename(src_path, dst_path) == 0;
#endif
    return success;
}

std::size_t getFileSize(FILE* const handle, const std::size_t pad = 0) noexcept
{
    std::size_t size = 0;
    if (handle != nullptr)
    {
#if defined(_WIN32) || defined(_WIN64)
        if (_fseeki64(handle, 0, SEEK_END) == 0)
#elif defined(_POSIX_VERSION) || defined(__APPLE__) || defined(__linux__)
        if (fseeko(handle, 0, SEEK_END) == 0)
#else
        if (fseek(handle, 0, SEEK_END) == 0)
#endif
        {
#if defined(_WIN32) || defined(_WIN64)
            const __int64 tell = _ftelli64(handle);
            if (_fseeki64(handle, 0, SEEK_SET) == 0)
#elif defined(_POSIX_VERSION) || defined(__APPLE__) || defined(__linux__)
            const off_t tell = ftello(handle);
            if (fseeko(handle, 0, SEEK_SET) == 0)
#else
            const long tell = ftell(handle);
            if (fseek(handle, 0, SEEK_SET) == 0)
#endif
            {
                if (tell > 0)
                {
                    static const std::uint64_t max64 = static_cast<std::uint64_t>(~static_cast<std::size_t>(0));
                    std::uint64_t len64 = static_cast<std::uint64_t>(tell);
                    if (len64 <= max64)
                    {
                        std::uint64_t pad64 = static_cast<std::uint64_t>(pad);
                        if (pad64 <= (max64 - len64))
                        {
                            size = static_cast<std::size_t>(len64 + pad64);
                        }
                    }
                }
            }
        }
    }
    return size;
}

}   //  namespace internal

void* loadFile(const char* const utf8_path, std::size_t& size, const std::size_t pad) noexcept
{
    size = 0;
    void* data = nullptr;
    path_t* std_path = internal::stdPath(utf8_path);
    if (std_path != nullptr)
    {
        FILE* handle = internal::openFile(std_path, false);
        if (handle != nullptr)
        {
            bool success = false;
            size = internal::getFileSize(handle, pad);
            if (size > 0)
            {
                data = new(std::nothrow) char[size];
                if (data != nullptr)
                {
                    std::size_t file_size = size - pad;
                    if (std::fread(data, 1, file_size, handle) == file_size)
                    {
                        if (pad != 0)
                        {
                            std::memset(reinterpret_cast<char*>(data) + file_size, 0, pad);
                        }
                        success = true;
                    }
                }
            }
            if (std::fclose(handle) != 0)
            {   //  close failed
                success = false;
            }
            if (!success)
            {
                if (data != nullptr)
                {
                    delete[] data;
                    data = nullptr;
                }
                size = 0;
            }
        }
        delete[] std_path;
    }
    return data;
}

bool saveFile(const char* const utf8_path, const std::size_t size, const void* const data) noexcept
{
    bool success = false;
    if ((data != nullptr) && (size > 0))
    {
        path_t* std_path = internal::stdPath(utf8_path);
        if (std_path != nullptr)
        {
            path_t* tmp_path = internal::tmpPath(std_path);
            if (tmp_path != nullptr)
            {
                FILE* handle = internal::openFile(tmp_path, true);
                if (handle != nullptr)
                {
                    success = std::fwrite(data, 1, size, handle) == size;
                    if (std::fflush(handle) != 0)
                    {   //  flush failed
                        success = false;
                    }
                    if (std::fclose(handle) != 0)
                    {   //  close failed
                        success = false;
                    }
                    if (!success || !internal::renameFile(tmp_path, std_path))
                    {   //  rename failed or prior failure
                        success = false;
                        internal::removeFile(tmp_path);
                    }
                }
                delete[] tmp_path;
            }
            delete[] std_path;
        }
    }
    return success;
}

}	//	namespace file
