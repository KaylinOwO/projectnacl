#pragma once
#include <Windows.h>
#include <functional>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <memoryapi.h>
#include <iostream>

class BytePatch
{
	void* addr{ 0 };
	size_t size;
	std::vector<unsigned char> patch_bytes;
	std::vector<unsigned char> original;
	bool patched{ false };

public:
	~BytePatch()
	{
		Shutdown();
	}
	BytePatch(std::function<uintptr_t(const char*)> SigScanFunc, const char* pattern, size_t offset, std::vector<unsigned char> patch) : patch_bytes{ patch }
	{
		addr = (void*)SigScanFunc(pattern);
		if (!addr)
		{
			//Log::Error("BytePatch error: address not found.");
			std::cout << "Address not found." << std::endl;
		}
		addr = static_cast<void*>(static_cast<char*>(addr) + offset);
		size = patch.size();
		original.resize(size);
		memcpy(&original[0], addr, size);
	}
	BytePatch(uintptr_t addr, std::vector<unsigned char> patch) : addr{ reinterpret_cast<void*>(addr) }, patch_bytes{ patch }
	{
		size = patch.size();
		original.resize(size);
		memcpy(&original[0], reinterpret_cast<void*>(addr), size);
	}
	BytePatch(void* addr, std::vector<unsigned char> patch) : addr{ addr }, patch_bytes{ patch }
	{
		size = patch.size();
		original.resize(size);
		memcpy(&original[0], addr, size);
	}

	void Patch()
	{
		if (!patched)
		{
			void* page = (void*)((uint64_t)addr & ~0xFFF);
			void* end_page = (void*)(((uint64_t)(addr)+size) & ~0xFFF);
			uintptr_t mprot_len = (uint64_t)end_page - (uint64_t)page + 0xFFF;
			DWORD dummy;

			VirtualProtect(page, mprot_len, PAGE_EXECUTE_READWRITE, &dummy);
			memcpy(addr, &patch_bytes[0], size);
			VirtualProtect(page, mprot_len, PAGE_EXECUTE, &dummy);
			patched = true;
		}
	}
	void Shutdown()
	{
		if (patched)
		{
			void* page = (void*)((uint64_t)addr & ~0xFFF);
			void* end_page = (void*)(((uint64_t)(addr)+size) & ~0xFFF);
			uintptr_t mprot_len = (uint64_t)end_page - (uint64_t)page + 0xFFF;
			DWORD dummy;

			VirtualProtect(page, mprot_len, PAGE_EXECUTE_READWRITE, &dummy);
			memcpy(addr, &original[0], size);
			VirtualProtect(page, mprot_len, PAGE_EXECUTE, &dummy);
			patched = false;
		}
	}
};