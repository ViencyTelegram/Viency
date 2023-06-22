#include"memory.h"
#include<TlHelp32.h>

Memory::Memory(const char* processName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	//gives us a snapShot of all the current processes running.
	const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//loops through the process using this function, if szExeFile is equal to our desired process
	//Then processID will be set to that desired process and open that process.
	while (Process32Next(snapShot, &entry))
	{
		if (!strcmp(processName, entry.szExeFile)) {
			this->ID = entry.th32ProcessID;
			this->process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->ID);
			break;
		}
	}

	//After the process has been caught, we will close the handle
	if (snapShot)
	{
		CloseHandle(snapShot);
	}
}

Memory::~Memory()
{
	if (this->process)
	{
		CloseHandle(this->process);
	}
}

DWORD Memory::GetProcessID()
{
	return this->ID;
}

HANDLE Memory::GetProcessHandle()
{
	return this->process;
}

uintptr_t Memory::GetModuleAddress(const char* moduleName)
{
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);

	//gets all the modules from our desired process
	const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->ID);

	uintptr_t result = 0;

	while (Module32Next(snapShot, &entry))
	{
		if (!strcmp(moduleName, entry.szModule))
		{
			//if the desired module is found, result is set to the base address of that module.
			result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
			break;
		}
	}

	if (snapShot) {
		CloseHandle(snapShot);
	}

	return result;
}