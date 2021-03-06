#pragma once

#include <Windows.h>

using namespace System;
using namespace System::Runtime::InteropServices;

using OpaqueCallback = int __fastcall();
using Callback = int __fastcall(int);
using DoCallFunction = int(OpaqueCallback*);

void prepare_call(Callback*);
void clear_call();
int __fastcall perform_wrapped_call();

namespace DelphiCallerLib
{
	public ref class Caller
	{
		[UnmanagedFunctionPointer(CallingConvention::FastCall)]
		delegate int ExportFunction(int);

	public:
		static int WrapAndDoCall(Func<int, int> ^callback)
		{
			HMODULE library = LoadLibraryW(L"DelphiLib.dll");
			if (library == nullptr) throw gcnew Exception("Cannot load library");
			try
			{
				DoCallFunction *doCallFunction = reinterpret_cast<DoCallFunction*>(GetProcAddress(library, "DoCall"));
				if (doCallFunction == nullptr) throw gcnew Exception("Cannot get function address");

				auto exportFunction = gcnew ExportFunction(callback, &Func<int, int>::Invoke);
				auto ptr = Marshal::GetFunctionPointerForDelegate(safe_cast<Delegate^>(exportFunction));
				
				prepare_call(static_cast<Callback*>(ptr.ToPointer()));
				int result = doCallFunction(&perform_wrapped_call);
				GC::KeepAlive(exportFunction);

				return result;
			}
			finally
			{
				clear_call();
				FreeLibrary(library);
			}
		}
	};
}

#pragma unmanaged

Callback *CallbackInstance;

void prepare_call(Callback *arg)
{
	CallbackInstance = arg;
}

void clear_call()
{
	CallbackInstance = nullptr;
}

int __fastcall perform_wrapped_call()
{
	int real_argument;
	__asm { 
		mov real_argument, eax
	}

	int result = CallbackInstance(real_argument);
	return result;
}
