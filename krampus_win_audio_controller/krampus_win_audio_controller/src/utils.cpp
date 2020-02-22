#include "utils.h"

#include <handleapi.h>
#include <Psapi.h>

BEGIN_KPAC_NAMESPACE

std::wstring to_wstring(const wchar_t* wstr)
{
	return std::wstring(wstr);
}

std::wstring get_process_name(DWORD pid)
{
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!handle)
		return L"";

	TCHAR buffer[MAX_PATH];
	if (GetModuleFileNameEx(handle, 0, buffer, MAX_PATH))
	{
		CloseHandle(handle);
		return std::wstring{ buffer };
	}
	else
	{
		CloseHandle(handle);
		return L"";
	}
}


END_KPAC_NAMESPACE


bool operator! (const Percentage& p) { return !p._per; }

std::ostream& operator<< (std::ostream& os, const Percentage& p) { return os << std::defaultfloat << (p._per * 100.f) << "%"; }
std::wostream& operator<< (std::wostream& os, const Percentage& p) { return os << std::defaultfloat << (p._per * 100.f) << L"%"; }
