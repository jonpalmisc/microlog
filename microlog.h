//===-- microlog.h - Small logging library for C --------------------------===//
//
// Copyright (c) 2022 Jon Palmisciano
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//===----------------------------------------------------------------------===//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ULOG_FORMAT __attribute__((format(printf, 1, 2)))

/// Log output level options.
enum MicrologOutputLevel {
	/// Do not show any log messages.
	MicrologOutputLevelNone = 0,

	/// Only show error messages.
	MicrologOutputLevelError = 1,

	/// Show info messages and error messages.
	MicrologOutputLevelInfo = 2,

	/// Show debug, info, and error messages.
	MicrologOutputLevelDebug = 3,

	/// Show trace, debug, info, and error messages.
	MicrologOutputLevelTrace = 4
};

/// Output options/feature flags.
enum MicrologFeature : unsigned char {
	/// Enable color output.
	MicrologFeatureColor = 1 << 0,

	/// Enable timestamped output.
	MicrologFeatureTime = 1 << 1,
};

/// Set the log output level.
void ulog_set_output_level(enum MicrologOutputLevel);

/// Get the log output level.
enum MicrologOutputLevel ulog_output_level(void);

/// Enable an output feature.
void ulog_enable_feature(enum MicrologFeature);

/// Disable an output feature.
void ulog_disable_feature(enum MicrologFeature);

/// Check if an output feature is enabled.
int ulog_has_feature(enum MicrologFeature);

//===----------------------------------------------------------------------===//

/// Log a formatted info message to the standard output stream.
ULOG_FORMAT void ulog_info(const char* format, ...);

/// Log a formatted debug message to the standard output stream. Does nothing if
/// debug output is not enabled.
ULOG_FORMAT void ulog_debug(const char* format, ...);

/// Log a formatted trace message to the standard output stream. Does nothing
/// if trace output is not enabled.
ULOG_FORMAT void ulog_trace(const char* format, ...);

/// Log a formatted message to the standard error stream.
ULOG_FORMAT void ulog_error(const char* format, ...);

#ifdef __cplusplus
}
#endif
