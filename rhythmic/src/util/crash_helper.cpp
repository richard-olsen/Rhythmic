#include "crash_helper.hpp"

#include "misc.hpp"
#include <wge/io/filesystem.hpp>

#include "../io/io_window.hpp"

#include <string>

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
#include <tchar.h>

typedef bool(*miniDump)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);

void create_minidump(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
	HMODULE mhLib = ::LoadLibrary("dbghelp.dll");
	miniDump pDump = (miniDump)::GetProcAddress(mhLib, "MiniDumpWriteDump");

	HANDLE  hFile = ::CreateFile(WGE::FileSystem::GetFileInDirectory("crash.dmp").c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
	ExInfo.ThreadId = ::GetCurrentThreadId();
	ExInfo.ExceptionPointers = apExceptionInfo;
	ExInfo.ClientPointers = FALSE;

	pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, _MINIDUMP_TYPE::MiniDumpNormal, &ExInfo, NULL, NULL);
	::CloseHandle(hFile);
}

const char *get_error(DWORD exception)
{
	switch (exception)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return "Access Violation";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return "Array Out Of Bounds";
	case EXCEPTION_BREAKPOINT:
		return "Breakpoint";
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return "Data Misaligned";
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return "Denormal Float";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return "Float Divided By 0";
	case EXCEPTION_FLT_INEXACT_RESULT:
		return "Float Inexact Result";
	case EXCEPTION_FLT_INVALID_OPERATION:
		return "Float Invalid Operation";
	case EXCEPTION_FLT_OVERFLOW:
		return "Float Overflow";
	case EXCEPTION_FLT_STACK_CHECK:
		return "Float Stack Error";
	case EXCEPTION_FLT_UNDERFLOW:
		return "Float Underflow";
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return "Illegal Instruction";
	case EXCEPTION_IN_PAGE_ERROR:
		return "In Page Error";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return "Integer Divided By 0";
	case EXCEPTION_INT_OVERFLOW:
		return "Integer Overflow";
	case EXCEPTION_INVALID_DISPOSITION:
		return "Invalid Disposition";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return "Noncontinuable Exception";
	case EXCEPTION_PRIV_INSTRUCTION:
		return "Instruction Not Allowed";
	case EXCEPTION_SINGLE_STEP:
		return "Single Step";
	case EXCEPTION_STACK_OVERFLOW:
		return "Stack Overflow";
	}
	return "Unknown...";
}

LONG WINAPI unhandled_exception(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
	Rhythmic::IO_Window_Destroy();
	std::string str = "Rhythmic ran into a problem!\n{";
	str += get_error(apExceptionInfo->ExceptionRecord->ExceptionCode);
	str += "}";
	MessageBox(NULL, str.c_str(), "Crashed", MB_ICONERROR);
	create_minidump(apExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}


#endif


namespace Rhythmic
{
	void SetUpCrashHandler()
	{
#ifdef WIN32
		SetUnhandledExceptionFilter(unhandled_exception);
#endif

	}
}