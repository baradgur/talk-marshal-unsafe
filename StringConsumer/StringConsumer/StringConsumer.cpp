// StringConsumer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

extern "C" __declspec(dllexport) void PassAnsiString(char *) {
}

extern "C" __declspec(dllexport) void PassUnicodeString(wchar_t *) {
}