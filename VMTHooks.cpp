
#include <cstdlib>
#include "vmthooks.h"
#include "memory.h"

// This code is public domain but credits (to Casual_Hacker on www.unknowncheats.me) are always appreciated.

namespace toolkit
{

	unsigned int CountFuncs(void** vmt)
	{
		unsigned int i = -1;
		do ++i; while (vmt[i]);
		return i;
	}
	int FindFunc(void** vmt, void* func, unsigned int vfuncs)
	{
		if (!vfuncs) vfuncs = CountFuncs(vmt);
		for (unsigned int i = 0; i<vfuncs; i++)
		{
			if (vmt[i] == func) return i;
		}
		return -1;
	}

	// VMTBaseManager
	VMTBaseManager& VMTBaseManager::Init(void* inst, unsigned int offset, unsigned int vfuncs)
	{
		_vftable = &_getvtbl(inst, offset);
		_oldvmt = *_vftable;
		// Count vfuncs ourself if needed
		if (!vfuncs)
		{
			vfuncs = CountFuncs(_oldvmt);
			assert(vfuncs >= 1);
		}
		// Allocate room for the new vtable + 3 places before + 1 place after for a null ptr
		void** arr = _array = (void**) ::malloc((vfuncs + 4)*sizeof(void*));
		arr[0] = this; // Ptr back to the hook object
		arr[1] = (void*)GUARD; // Guard tells us if the vtable is hooked
		(arr + 3)[vfuncs] = nullptr; // Marks the end of the vtable
		// Copy over the other vfuncs + RTTI ptr
		{
			unsigned int i = -1;
			do arr[i + 3] = _oldvmt[i];
			while (++i<vfuncs);
		}
		return *this;
	}
	void VMTBaseManager::Kill()
	{
		if (IsInitialized())
		{
			Unhook();
			_vftable = nullptr;
		}
		::free(_array);
		_array = nullptr;
	}
	void VMTBaseManager::EraseHooks()
	{
		unsigned int i = 0;
		void** vmt = _array + 3;
		do vmt[i] = _oldvmt[i];
		while (vmt[++i]);
	}


	// VMTBasePointer
	VMTBasePointer& VMTBasePointer::Init(void* inst, CallGateFn gate, unsigned int vfuncs)
	{
		_inst = inst;
		_gate = gate;
		void** vmt = *(void***)inst;
		// Count vfuncs ourself if needed
		if (!vfuncs)
		{
			vfuncs = CountFuncs(vmt);
			assert(vfuncs >= 1);
		}
		// Allocate memory for hooked vtable
		void** arr = (void**) ::malloc((vfuncs + 4)*sizeof(void*));
		_dummy.vtable = arr + 3;
		arr[0] = this;
		arr[1] = (void*)GUARD;
		arr[2] = vmt[-1];
		(arr + 3)[vfuncs] = nullptr;
		// Init the vfuncs to our call gate
		for (unsigned int i = 0; i<vfuncs; ++i)
			(arr + 3)[i] = gate;
		return *this;
	}
	void VMTBasePointer::Kill()
	{
		if (_dummy.vtable)
		{
			::free(_dummy.vtable - 3);
		}
	}
	void* __fastcall VMTBasePointer::FindCallOffset(VMTBasePointer* _this, unsigned char* addr)
	{
		unsigned int vfn;

#if defined(_M_IX86)
		// X86 specific stuff!

		// So yeah this doesn't work with tail recursion...

		// Quick primer on MOD/RM
		// The MOD/RM byte is split in 3 parts xxyyyzzz
		// xx is the operating mode, 11 = registers, 01 = memory + 1 byte offset, 10 = memory + 4 byte offset, 00 = memory no offset
		// yyy is always a register used directly
		// zzz is a register with various addressing modes
		// Note: special cases not explained because irrelevant

		// mov eax, [ecx]	; Get vtable
		// mov edx, [eax+8]	; Get vfunc
		// call edx			; Call vfunc
		if (addr[-2] == 0xFF && (addr[-1] & 0xC0) == 0xC0)
		{
			// Get the register for matching
			unsigned char rm = (addr[-1] & 0x07) << 3;

			// Start searching for a mov reg, [r+x] instruction
			typedef unsigned char* iterator;
			for (iterator it = addr - 4, end = it - 64; it != end; --it)
			{
				// mov instruction
				if (it[0] == 0x8B)
				{
					unsigned char modrm = (it[1] & 0xF8);
					// no displacement
					if (modrm == (0x00 | rm))
					{
						vfn = 0;
						break;
					}
					// 1 byte displacement
					else if (modrm == (0x40 | rm))
					{
						vfn = it[2];
						break;
					}
					// 4 byte displacement
					else if (modrm == (0x80 | rm))
					{
						vfn = *(unsigned int*)(it + 2);
						break;
					}
				}
			}
		}
		// mov eax, [ecx]	; Get vtable
		// call [eax+8]		; Get and call vfunc
		else if (addr[-2] == 0xFF && (addr[-1] & 0xC0) == 0x00)
		{
			// no displacement
			vfn = 0;
		}
		else if (addr[-3] == 0xFF && (addr[-2] & 0xC0) == 0x40)
		{
			// 1 byte displacement
			vfn = addr[-1];
		}
		else if (addr[-6] == 0xFF && (addr[-5] & 0xC0) == 0x80)
		{
			// 4 byte displacement
			vfn = *(unsigned int*)(addr - 4);
		}
		// Not found, break
		else
		{
			__debugbreak();
			vfn = -1;
		}
#else
		// Not implemented for this architecture...
		return nullptr;
#endif

		char* vmt = (char*)(*(void**)_this->_inst);
		return *(void**)(vmt + vfn);
	}
#if defined(_M_IX86)
	__declspec(naked) void __fastcall VMTBasePointer::CallGate_thiscall()
	{
		__asm
		{
			// Get original this for later
			mov eax, [ecx + 4]
				push eax
				// Get original vfunc index
				mov edx, [esp + 4]
				call FindCallOffset
				// Jump to original func
				pop ecx
				jmp eax
		}
	}
	__declspec(naked) void __fastcall VMTBasePointer::CallGate_stdcall()
	{
		__asm
		{
			// Correct this on the stack
			mov ecx, [esp + 4]
				mov eax, [ecx + 4]
				mov[esp + 4], eax
				// Get original vfunc
				mov edx, [esp]
				call FindCallOffset
				// Jump to original func
				jmp eax
		}
	}
#else
#error Implement me for this architecture!
#endif

	// VMTBaseHook
	VMTBaseHook& VMTBaseHook::Init(void** vmt, unsigned int vfuncs)
	{
		_vftable = vmt;
		if (!vfuncs)
		{
			vfuncs = CountFuncs(vmt);
			assert(vfuncs >= 1);
		}
		_vcount = vfuncs;
		_backup = (void**) ::malloc(vfuncs*sizeof(void*));
		// Copy the original vtable
		for (unsigned int i = 0; i<vfuncs; ++i)
			_backup[i] = _vftable[i];
		return *this;
	}
	void VMTBaseHook::Kill()
	{
		if (_backup)
		{
			EraseHooks();
			::free(_backup);
			_backup = nullptr;
		}
	}
	bool VMTBaseHook::WriteHook(void* dest, const void* src, unsigned int bytes)
	{
		//If you don't like memcpy, use your own here.
		return memcpy(dest, src, bytes);
	}
}