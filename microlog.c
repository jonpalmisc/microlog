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

#include <unistd.h>

#define ANSI_COLOR_ERROR "\x1b[31m" /* Red */
#define ANSI_COLOR_DEBUG "\x1b[33m" /* Yellow */
#define ANSI_COLOR_TRACE "\x1b[34m" /* Blue */
#define ANSI_COLOR_RESET "\x1b[0m"

enum MessageType {
	MessageTypeNormal,
	MessageTypeDebug,
	MessageTypeTrace,
	MessageTypeError,
};

struct Config {
	bool is_initialized;
	bool has_smart_output;
	enum MicrologOutputLevel output_level;
};

static struct Config g_config = {
	.is_initialized = false,
	.has_smart_output = false,
	.output_level = MicrologOutputLevelNormal,
};

static void ulog_init()
{
	if (g_config.is_initialized)
		return;

	const char* terminal = getenv("TERM");
	if (!terminal) {
		g_config.has_smart_output = false;
		g_config.is_initialized = true;

		return;
	}

	g_config.has_smart_output = isatty(1) && strcmp(terminal, "dumb") != 0;
	g_config.is_initialized = true;
}

void ulog_set_output_level(enum MicrologOutputLevel output_level)
{
	g_config.output_level = output_level;
}

static void set_color(FILE* stream, enum MessageType message_type)
{
	if (!g_config.has_smart_output)
		return;

	switch (message_type) {
	case MessageTypeDebug:
		fprintf(stream, ANSI_COLOR_DEBUG);
		break;
	case MessageTypeTrace:
		fprintf(stream, ANSI_COLOR_TRACE);
		break;
	case MessageTypeError:
		fprintf(stream, ANSI_COLOR_ERROR);
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
	if (!g_config.is_initialized)
		ulog_init();

	FILE* stream = message_type == MessageTypeError ? stderr : stdout;

	set_color(stream, message_type);
	vfprintf(stream, format, args);
	reset_color(stream);

	fprintf(stream, "\n");
}

void ulog_info(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	log_internal(MessageTypeNormal, format, args);

	va_end(args);
}

void ulog_debug(const char* format, ...)
{
	if (g_config.output_level < MicrologOutputLevelDebug)
		return;

	va_list args;
	va_start(args, format);

	log_internal(MessageTypeDebug, format, args);

	va_end(args);
}

void ulog_trace(const char* format, ...)
{
	if (g_config.output_level < MicrologOutputLevelTrace)
		return;

	va_list args;
	va_start(args, format);

	log_internal(MessageTypeTrace, format, args);

	va_end(args);
}

void ulog_error(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	log_internal(MessageTypeError, format, args);

	va_end(args);
}
