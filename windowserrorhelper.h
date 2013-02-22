#pragma once

#include <qstring.h>
#include <Windows.h>

class WindowsErrorHelper
{
public:
	/**
	 * Returns the localized error string for the given error code.
	 */
	static QString errorToString(DWORD errorCode)
	{
		LPTSTR messageText = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&messageText,
			0,
			NULL);

#ifdef UNICODE
		QString message = QString::fromWCharArray(messageText);
#else
		QString message = QString::fromLocal8Bit(messageText);
#endif
		LocalFree(messageText);
		return message;
	}
};

