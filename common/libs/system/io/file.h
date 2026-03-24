
//  File:   file.h
//  Author: Ritchie Brannan
//  Date:   11 Nov 10
//
//  Modernised 18 Dec 2025
//
//  Basic load and save utility functions (file <=> memory blob)
//
//  Multi-threaded usage assumes that multiple threads will not be saving
//  files with the same name.

#pragma once

#ifndef __FILE_INCLUDED__
#define __FILE_INCLUDED__

#include <stddef.h>

namespace file
{
// utf8_path is expected to be UTF-8 encoded.
// Empty files are treated as failure.
// On success, size includes pad bytes, which are zero-filled.
// Returned buffer is allocated with new[]; caller must delete[].

[[nodiscard]] void* loadFile(const char* const utf8_path, size_t& size, const size_t pad = 0) noexcept;
bool  saveFile(const char* const utf8_path, const size_t size, const void* const data) noexcept;
}

#endif // __FILE_INCLUDED__
