/***********************************************************************************************************************
 * Copyright (C) 2016 Andrew Zonenberg and contributors                                                                *
 *                                                                                                                     *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General   *
 * Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) *
 * any later version.                                                                                                  *
 *                                                                                                                     *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  *
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for     *
 * more details.                                                                                                       *
 *                                                                                                                     *
 * You should have received a copy of the GNU Lesser General Public License along with this program; if not, you may   *
 * find one here:                                                                                                      *
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt                                                              *
 * or you may search the http://www.gnu.org website for the version 2.1 license, or you may write to the Free Software *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA                                      *
 **********************************************************************************************************************/

#ifndef log_h
#define log_h

#include <memory>
#include <vector>

/**
	@brief The message severity
 */
enum class Severity
{
	FATAL	= 1,	//State is totally unusable, must exit right now
	ERROR	= 2,	//Design is unroutable, cannot continue
	WARNING	= 3,	//Design may have an error, but we'll attempt to proceed at your own risk
	NOTICE	= 4,	//Useful information about progress
	VERBOSE	= 5,	//Detailed information end users may sometimes need, but not often
	DEBUG = 6		//Extremely detailed information only useful to people working on the toolchain internals
};

/**
	@brief The log sink
 */
class LogSink
{
public:
	virtual ~LogSink() {}

	virtual void Log(Severity severity, const std::string &msg) = 0;
	virtual void Log(Severity severity, const char *format, va_list va) = 0;
};

/**
	@brief The log sink writing to stdout/stderr
 */
class STDLogSink : public LogSink
{
public:
	STDLogSink(Severity min_severity = Severity::VERBOSE);
	~STDLogSink() override;

	void Log(Severity severity, const std::string &msg) override;
	void Log(Severity severity, const char *format, va_list va) override;

protected:
	Severity	m_min_severity;

};
/**
	@brief The log sink writing to a FILE* file handle
 */
class FILELogSink : public LogSink
{
public:
	FILELogSink(FILE *f, bool line_buffered = false, Severity min_severity = Severity::VERBOSE);
	~FILELogSink() override;

	void Log(Severity severity, const std::string &msg) override;
	void Log(Severity severity, const char *format, va_list va) override;

protected:
	FILE		*m_file;
	Severity	m_min_severity;

};

extern std::vector<std::unique_ptr<LogSink>> g_log_sinks;

/**
	@brief Helper function for parsing arguments that use common syntax
 */
bool ParseLoggerArguments(
	int& i,
	int argc,
	char* argv[],
	Severity& console_verbosity);
	

#ifdef __GNUC__
#define ATTR_FORMAT(n, m) __attribute__((__format__ (__printf__, n, m)))
#define ATTR_NORETURN     __attribute__((noreturn))
#else
#define ATTR_FORMAT(n, m)
#define ATTR_NORETURN
#endif

ATTR_FORMAT(1, 2) void LogVerbose(const char *format, ...);
ATTR_FORMAT(1, 2) void LogNotice(const char *format, ...);
ATTR_FORMAT(1, 2) void LogWarning(const char *format, ...);
ATTR_FORMAT(1, 2) void LogError(const char *format, ...);
ATTR_FORMAT(1, 2) void LogDebug(const char *format, ...);
ATTR_FORMAT(1, 2) ATTR_NORETURN void LogFatal(const char *format, ...);

///Just print the message at given log level, don't do anything special for warnings or errors
ATTR_FORMAT(2, 3) void Log(Severity severity, const char *format, ...);

#undef ATTR_FORMAT
#undef ATTR_NORETURN

#endif

