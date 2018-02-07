// Copyright (c) 2017, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: David Seifert

// Reduce the number of exposed symbols in order to speed up
// DSO load times
// https://gcc.gnu.org/wiki/Visibility

#pragma once

#if defined _WIN32 || defined __CYGWIN__
#ifdef UNANIMITY_BUILDING_LIBRARY
#ifdef __GNUC__
#define UNANIMITY_PUBLIC_API __attribute__((dllexport))
#else
#define UNANIMITY_PUBLIC_API __declspec(dllexport)  // Note: gcc seems to also supports this syntax
#endif
#else
#ifdef __GNUC__
#define UNANIMITY_PUBLIC_API __attribute__((dllimport))
#else
#define UNANIMITY_PUBLIC_API __declspec(dllimport)  // Note: gcc seems to also supports this syntax
#endif
#endif
#define UNANIMITY_PRIVATE_API
#else
#if __GNUC__ >= 4
#define UNANIMITY_PUBLIC_API __attribute__((visibility("default")))
#define UNANIMITY_PRIVATE_API __attribute__((visibility("hidden")))
#else
#define UNANIMITY_PUBLIC_API
#define UNANIMITY_PRIVATE_API
#endif
#endif

// Compatibility guard to prevent old compilers
// choking on relaxed C++14 constexpr handling
// https://github.com/PacificBiosciences/pitchfork/issues/417

#if (__GNUC__ >= 6) || defined(__clang__)
#define UNANIMITY_CONSTEXPR constexpr
#else
#define UNANIMITY_CONSTEXPR
#endif