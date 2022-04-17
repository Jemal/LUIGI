#pragma once

namespace uieditor
{
	enum log_message_type
	{
		log_normal,
		log_error,
		log_warning
	};

	struct log_message
	{
		int type;
		char* message;
	};

	class log
	{
	public:
		static void draw();

		static void print(int type, const char* fmt, ...);
		static void print_message(int type, char* message);

	private:
		static ImVector<log_message> output_;
	};
}