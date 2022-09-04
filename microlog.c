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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#define ANSI_CLEAR_EOL   "\x1B[K"
#define ANSI_COLOR_RED   "\x1b[31m"
#define ANSI_COLOR_BLUE  "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

enum MessageType {
	MessageTypeNormal,
	MessageTypeDebug,
	MessageTypeError,
};

struct Config {
	bool is_initialized;
	bool has_smart_output;
	bool show_debug_output;
};

static struct Config g_config = {
	.is_initialized = false,
	.has_smart_output = false,
	.show_debug_output = false
};

void log_init(enum LogOutputLevel output_level)
{
	if (g_config.is_initialized)
		return;

	if (output_level == LogOutputLevelDebug)
		g_config.show_debug_output = true;

	const char* terminal = getenv("TERM");
	if (!terminal) {
		g_config.has_smart_output = false;
		g_config.is_initialized = true;

		return;
	}

	g_config.has_smart_output = isatty(1) && strcmp(terminal, "dumb") != 0;
	g_config.is_initialized = true;
}

static void set_color(FILE* stream, enum MessageType message_type)
{
	if (!g_config.has_smart_output)
		return;

	switch (message_type) {
	case MessageTypeDebug:
		fprintf(stream, ANSI_COLOR_BLUE);
		break;
	case MessageTypeError:
		fprintf(stream, ANSI_COLOR_RED);
		break;
	default:
		break;
	}
}

static void reset_color(FILE* stream)
{
	if (!g_config.has_smart_output)
		return;

	fprintf(stream, ANSI_COLOR_RESET);
}

static void log_internal(enum MessageType message_type, const char* format, va_list args)
{
	FILE* stream = message_type == MessageTypeError ? stderr : stdout;

	set_color(stream, message_type);
	vfprintf(stream, format, args);
	reset_color(stream);

	fprintf(stream, "\n");
}

void log_info(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	log_internal(MessageTypeNormal, format, args);

	va_end(args);
}

void log_debug(const char* format, ...)
{
	if (!g_config.show_debug_output)
		return;

	va_list args;
	va_start(args, format);

	log_internal(MessageTypeDebug, format, args);

	va_end(args);
}

void log_error(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	log_internal(MessageTypeError, format, args);

	va_end(args);
}
