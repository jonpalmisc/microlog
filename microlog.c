//===-- microlog.c - Small logging library for C --------------------------===//
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

#include "microlog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>

#define ANSI_COLOR_ERROR "\x1b[31m" /* Red */
#define ANSI_COLOR_DEBUG "\x1b[33m" /* Yellow */
#define ANSI_COLOR_TRACE "\x1b[34m" /* Blue */
#define ANSI_COLOR_RESET "\x1b[0m"

struct Config {
	enum MicrologOutputLevel output_level;
	unsigned char features;
};

static struct Config g_config = {
	.features = MicrologFeatureColor,
	.output_level = MicrologOutputLevelInfo,
};

bool g_epoch_initialized = false;
struct timespec g_epoch;

//===----------------------------------------------------------------------===//

void ulog_set_output_level(enum MicrologOutputLevel output_level)
{
	g_config.output_level = output_level;
}

enum MicrologOutputLevel ulog_output_level()
{
	return g_config.output_level;
}

void ulog_enable_feature(enum MicrologFeature feature)
{
	g_config.features |= feature;
}

void ulog_disable_feature(enum MicrologFeature feature)
{
	g_config.features &= ~feature;
}

int ulog_has_feature(enum MicrologFeature feature)
{
	return g_config.features & feature;
}

//===----------------------------------------------------------------------===//

static void set_color(FILE* stream, enum MicrologOutputLevel level)
{
	if (!ulog_has_feature(MicrologFeatureColor))
		return;

	switch (level) {
	case MicrologOutputLevelError:
		fprintf(stream, ANSI_COLOR_ERROR);
		break;
	case MicrologOutputLevelDebug:
		fprintf(stream, ANSI_COLOR_DEBUG);
		break;
	case MicrologOutputLevelTrace:
		fprintf(stream, ANSI_COLOR_TRACE);
		break;
	default:
		break;
	}
}

static void reset_color(FILE* stream)
{
	if (!ulog_has_feature(MicrologFeatureColor))
		return;

	fprintf(stream, ANSI_COLOR_RESET);
}

//===----------------------------------------------------------------------===//

static inline void time_subtract(struct timespec* lhs, struct timespec* rhs, struct timespec* out)
{
	out->tv_sec = lhs->tv_sec - rhs->tv_sec;
	out->tv_nsec = lhs->tv_nsec - rhs->tv_nsec;

	if (out->tv_nsec < 0) {
		--out->tv_sec;
		out->tv_nsec += 1000000000L;
	}
}

static void log_internal_time(FILE* stream)
{
	struct timespec now, elapsed;

	if (!g_epoch_initialized) {
		clock_gettime(CLOCK_MONOTONIC, &g_epoch);
		g_epoch_initialized = true;
	}

	clock_gettime(CLOCK_MONOTONIC, &now);
	time_subtract(&now, &g_epoch, &elapsed);

	fprintf(stream, "%03lu.%06lu | ", elapsed.tv_sec, elapsed.tv_nsec / 1000);
}

static void log_internal(enum MicrologOutputLevel level, const char* format, va_list args)
{
	FILE* stream = level == MicrologOutputLevelError ? stderr : stdout;

	set_color(stream, level);
	if (ulog_has_feature(MicrologFeatureTime))
		log_internal_time(stream);
	vfprintf(stream, format, args);
	reset_color(stream);

	fprintf(stream, "\n");
}

#define ULOG_LOG_FUNCTION_BODY(level)      \
	va_list args;                      \
	if (level > g_config.output_level) \
		return;                    \
	va_start(args, format);            \
	log_internal(level, format, args); \
	va_end(args);

void ulog_info(const char* format, ...)
{
	ULOG_LOG_FUNCTION_BODY(MicrologOutputLevelInfo);
}

void ulog_debug(const char* format, ...)
{
	ULOG_LOG_FUNCTION_BODY(MicrologOutputLevelDebug);
}

void ulog_trace(const char* format, ...)
{
	ULOG_LOG_FUNCTION_BODY(MicrologOutputLevelTrace);
}

void ulog_error(const char* format, ...)
{
	ULOG_LOG_FUNCTION_BODY(MicrologOutputLevelError);
}
