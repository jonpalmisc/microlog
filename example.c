#include <microlog.h>

#include <assert.h>

int main()
{
	assert(ulog_output_level() == MicrologOutputLevelInfo);

	ulog_set_output_level(MicrologOutputLevelTrace);
	ulog_enable_feature(MicrologFeatureTime);

	// Four messages in different colors should be printed.
	ulog_info("This is an info-level message, used for general communication");
	ulog_debug("This is a debug message, used to add detail");
	ulog_trace("This is a trace message, used for providing granular context");
	ulog_error("This is an error message, for when things go wrong");

	ulog_disable_feature(MicrologFeatureColor);

	// Should print with no color now that the feature is disabled.
	ulog_trace("This is a also trace message, but should have no color");

	ulog_set_output_level(MicrologOutputLevelDebug);

	// Should not print now that trace messages are disabled.
	ulog_trace("This trace message should be hidden");

	return 0;
}
