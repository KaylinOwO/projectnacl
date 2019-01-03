#pragma once

//----------------------------------------------------------------
// Toolkit: Virtual method table hooks
//----------------------------------------------------------------
// These can be hooked in two distinct manners.
//
// The VMTManager class hooks by replacing the vmt pointer with a custom crafted one.
// Only calls through this instance will be hooked.
//
// The VMTPointer class hooks a pointer to an interface.
// Only calls through this specific variable will be hooked.
//
// The VMTHook class hooks by editing the vtable in the dll's .rdata.
// Every instance using that vtable will be hooked.
//
// A general hint:
// Abuse the __fastcall convention ( look it up on the msdn ) to catch the this pointer.
//
// More specific information follows the class declaration.
//
// Each class has a basic version (no constructors/destructors) and a RAII version.
//
// This code is public domain but credits (to Casual_Hacker on www.unknowncheats.me) are always appreciated.

#include <cassert>

namespace toolkit
{

	// Find the number of vfuncs in a vtable
	unsigned int CountFuncs(void** pVMT);

	// Find the index for a vfunc, result is negative if not found
	int FindFunc(void** pVMT, void* pFunc, unsigned int vfuncs = 0);




	//----------------------------------------------------------------
	// Class: VMTManager
	//----------------------------------------------------------------
	// Hooks virtual functions by replacing the vtable pointer from an instance.
	// VMTManager is the RAII version of VMTBaseManager
	class VMTBaseManager
	{
	public:
		// TODO: Abstractions can never hurt...
		//typedef const void* pfn_t;
		//struct vtbl_t
		//{
		//	pfn_t data[1];
		//	inline const pfn_t& operator[] ( unsigned int i ) const { return data[i]; }
		//	inline pfn_t& operator[] ( unsigned int i ) { return data[i]; }
		//};

		// Guard identifies if an instance is hooked.
		enum { GUARD = 0xFAC0D775 }; //4206942069 huehuehuehuehue

		// Init the hook, will not hook the instance yet.
		VMTBaseManager& Init(void* inst, unsigned int offset = 0, unsigned int vfuncs = 0);
		// Unhook and forget about this instance.
		void Kill();
		// Are we initialized.
		bool IsInitialized() const;

		// Hooks a function by index.
		VMTBaseManager& HookMethod(void* newfunc, unsigned int index);
		// Unhooks a function by index.
		VMTBaseManager& UnhookMethod(unsigned int index);
		// Unhooks all functions.
		void EraseHooks();

		// Return the original vtable. Instance will appear completely unhooked.
		VMTBaseManager& Unhook();
		// Put the vtable with the hooks back.
		VMTBaseManager& Rehook();
		// Test if the hooks are active.
		bool Hooked() const;

		// Get the original function.
		// Use a function prototype for the template argument to make it very easy to call this function.
		// Example syntax: hook.GetMethod<bool (__thiscall*)( void*, int )>( 12 )( inst, arg );
		template< typename Fn >
		const Fn& GetMethod(unsigned int index) const;

		// Get/Set the userdata.
		void* UserData() const;
		void UserData(void* data);

		// Test if the instance is hooked.
		static bool HookPresent(void* inst, unsigned int offset = 0);
		// Get the hook object. Does not check if the instance is actually hooked!
		static VMTBaseManager& GetHook(void* inst, unsigned int offset = 0);

	protected:
		static void**& _getvtbl(void* inst, unsigned int offset);

	protected:
		void*** _vftable;
		void** _oldvmt;
		void** _array;
		void* _userdata;
	};
	class VMTManager : public VMTBaseManager
	{
		// Forbid copy constructing and assignment.
		VMTManager(const VMTManager&);
		VMTManager& operator= (const VMTManager&);

	public:
		// Initialize & hook the instance.
		VMTManager(void* inst, unsigned int offset = 0, unsigned int vfuncs = 0);
		~VMTManager();

		// If the instance is somehow destroyed before you get a chance to unhook it or destruct this hook object, call this.
		// It'll prevent the destructor from crashing.
		void Poof();

		// Get the hook object. Does not check if the instance is actually hooked!
		static VMTManager& GetHook(void* inst, unsigned int offset = 0);
	};
	// VMTBaseManager inlines
	inline bool VMTBaseManager::IsInitialized() const
	{
		return _vftable != nullptr;
	}
	inline VMTBaseManager& VMTBaseManager::HookMethod(void* newfunc, unsigned int index)
	{
		assert(index<CountFuncs(_array + 3) && IsInitialized());
		_array[index + 3] = newfunc;
		return *this;
	}
	inline VMTBaseManager& VMTBaseManager::UnhookMethod(unsigned int index)
	{
		assert(index<CountFuncs(_array + 3) && IsInitialized());
		_array[index + 3] = _oldvmt[index];
		return *this;
	}
	inline VMTBaseManager& VMTBaseManager::Unhook()
	{
		*_vftable = _oldvmt;
		return *this;
	}
	inline VMTBaseManager& VMTBaseManager::Rehook()
	{
		*_vftable = _array + 3;
		return *this;
	}
	inline bool VMTBaseManager::Hooked() const
	{
		return *_vftable != _oldvmt;
	}
	template< typename Fn >
	inline const Fn& VMTBaseManager::GetMethod(unsigned int index) const
	{
		assert(index<CountFuncs(_array + 3));
		return *(const Fn*)&_oldvmt[index];
	}
	inline void* VMTBaseManager::UserData() const
	{
		return _userdata;
	}
	inline void VMTBaseManager::UserData(void* data)
	{
		_userdata = data;
	}
	inline bool VMTBaseManager::HookPresent(void* inst, unsigned int offset)
	{
		void** vmt = _getvtbl(inst, offset);
		return vmt[-2] == (void*)GUARD;
	}
	inline VMTBaseManager& VMTBaseManager::GetHook(void* inst, unsigned int offset)
	{
		void** vmt = _getvtbl(inst, offset);
		return *reinterpret_cast<VMTBaseManager*>(vmt[-3]);
	}
	inline void**& VMTBaseManager::_getvtbl(void* inst, unsigned int offset)
	{
		return *reinterpret_cast<void***>((char*)inst + offset);
	}
	// VMTManager inlines
	inline VMTManager::VMTManager(void* inst, unsigned int offset, unsigned int vfuncs)
	{
		Init(inst, offset, vfuncs).Rehook();
	}
	inline VMTManager::~VMTManager()
	{
		Kill();
	}
	inline void VMTManager::Poof()
	{
		_vftable = nullptr;
	}
	inline VMTManager& VMTManager::GetHook(void* inst, unsigned int offset)
	{
		return static_cast<VMTManager&>(VMTBaseManager::GetHook(inst, offset));
	}



	//----------------------------------------------------------------
	// Class: VMTPointer
	//----------------------------------------------------------------
	// Hooks virtual functions by replacing the whole pointer to the class with a dummy interface.
	// Assumes only 1 vtable (no MI) placed at the start of the class.
	class VMTBasePointer
	{
	public:
		enum { GUARD = 0xDE4DB3EF };

		// Init the hook, will not hook the instance yet. (the vfuncs use __thiscall convention)
		VMTBasePointer& Init_thiscall(void* inst, unsigned int vfuncs = 0);
		// Init the hook, will not hook the instance yet. (the vfuncs use __stdcall convention)
		VMTBasePointer& Init_stdcall(void* inst, unsigned int vfuncs = 0);
		// Unhook and forget about this instance.
		void Kill();

		// Hooks a function by index.
		// WARNING! The this ptr in your hook points to the hook object!
		VMTBasePointer& HookMethod(void* newfunc, unsigned int index);
		// Unhooks a function by index.
		VMTBasePointer& UnhookMethod(unsigned int index);

		// Hook for this variable.
		VMTBasePointer& Rehook(void* var);
		// Unhook for this variable.
		VMTBasePointer& Unhook(void* var);

		// Get the original function.
		// Use a function prototype for the template argument to make it very easy to call this function.
		// Example syntax: hook.GetMethod<bool (__thiscall*)( void*, int )>( 12 )( inst, arg );
		template< typename Fn >
		const Fn& GetMethod(unsigned int index) const;

		// Get/Set the userdata.
		void* UserData() const;
		void UserData(void* data);

		// Get the original instance.
		void* Instance() const;
		// Get the dummy instance.
		void* Dummy() const;

		// Test if the instance is hooked.
		static bool HookPresent(void* inst);
		// Get the hook object. Does not check if the instance is actually hooked!
		static VMTBasePointer& GetHook(void* inst);

	protected:
		// Internal constructor
		typedef void(__fastcall* CallGateFn)();
		VMTBasePointer& Init(void* inst, CallGateFn gate, unsigned int vfuncs = 0);

		static void* __fastcall FindCallOffset(VMTBasePointer* _this, unsigned char* addr);
		static void __fastcall CallGate_thiscall(); // This passed in ecx
		static void __fastcall CallGate_stdcall(); // This passed on stack
		static void** _getvtbl(void* inst, unsigned int offset = 0);

	protected:
		// Note! Do not move these members around!
		// Dummy object has to come first, original instance ptr has to be at this+4!

		// Fake 'this' points to our hook object
		struct dummy_t
		{
			void** vtable;
		} _dummy;
		// Original instance
		void* _inst;
		// Call gate (__thiscall/__stdcall version, needed for UnhookMethod)
		CallGateFn _gate;
		// User data context
		void* _userdata;
	};
	class VMTPointer : public VMTBasePointer
	{
		// Forbid copy constructing and assignment
		VMTPointer(const VMTPointer&);
		VMTPointer& operator= (const VMTPointer&);
	public:
		VMTPointer(void** var, bool thiscall = true, unsigned int vfuncs = 0);
		~VMTPointer();

		// Return the original vtable. Instance will appear completely unhooked.
		VMTPointer& Unhook();
		// Put the vtable with the hooks back.
		VMTPointer& Rehook();
		// Test if the hooks are active.
		bool Hooked() const;

		static VMTPointer& GetHook(void* inst);
	private:
		void** _var;
	};
	// Inlines for VMTBasePointer
	inline VMTBasePointer& VMTBasePointer::Init_thiscall(void* inst, unsigned int vfuncs)
	{
		return Init(inst, &CallGate_thiscall, vfuncs);
	}
	inline VMTBasePointer& VMTBasePointer::Init_stdcall(void* inst, unsigned int vfuncs)
	{
		return Init(inst, &CallGate_stdcall, vfuncs);
	}
	inline VMTBasePointer& VMTBasePointer::HookMethod(void* newfunc, unsigned int index)
	{
		assert(index<CountFuncs(_dummy.vtable) && newfunc);
		_dummy.vtable[index] = newfunc;
		return *this;
	}
	inline VMTBasePointer& VMTBasePointer::UnhookMethod(unsigned int index)
	{
		assert(index<CountFuncs(_dummy.vtable));
		_dummy.vtable[index] = _gate;
		return *this;
	}
	inline VMTBasePointer& VMTBasePointer::Rehook(void* var)
	{
		void*& x = *(void**)var;
		assert(!x || x == _inst || x == &_dummy);
		x = &_dummy;
		return *this;
	}
	inline VMTBasePointer& VMTBasePointer::Unhook(void* var)
	{
		void*& x = *(void**)var;
		assert(!x || x == _inst || x == &_dummy);
		x = _inst;
		return *this;
	}
	template< typename Fn >
	inline const Fn& VMTBasePointer::GetMethod(unsigned int index) const
	{
		assert(index<CountFuncs(_dummy.vtable));
		return *(const Fn*)(&_getvtbl(_inst)[index]);
	}
	inline void* VMTBasePointer::UserData() const
	{
		return _userdata;
	}
	inline void VMTBasePointer::UserData(void* data)
	{
		_userdata = data;
	}
	inline void* VMTBasePointer::Instance() const
	{
		return _inst;
	}
	inline void* VMTBasePointer::Dummy() const
	{
		return const_cast<dummy_t*>(&_dummy);
	}
	inline bool VMTBasePointer::HookPresent(void* inst)
	{
		void** vmt = *(void***)inst;
		return vmt[-2] == (void*)GUARD;
	}
	inline VMTBasePointer& VMTBasePointer::GetHook(void* inst)
	{
		void** vmt = *(void***)inst;
		return *reinterpret_cast<VMTBasePointer*>(vmt[-3]);
	}
	inline void** VMTBasePointer::_getvtbl(void* inst, unsigned int offset)
	{
		return *reinterpret_cast<void***>((char*)inst + offset);
	}
	// Inlines for VMTPointer
	inline VMTPointer::VMTPointer(void** var, bool thiscall, unsigned int vfuncs) : _var(var)
	{
		if (thiscall)
			Init_thiscall(*var, vfuncs);
		else
			Init_stdcall(*var, vfuncs);
	}
	inline VMTPointer::~VMTPointer()
	{
		*_var = _inst;
		Kill();
	}
	inline VMTPointer& VMTPointer::Unhook()
	{
		*_var = _inst;
		return *this;
	}
	inline VMTPointer& VMTPointer::Rehook()
	{
		*_var = &_dummy;
		return *this;
	}
	inline bool VMTPointer::Hooked() const
	{
		return *_var == &_dummy;
	}
	inline VMTPointer& VMTPointer::GetHook(void* inst)
	{
		return static_cast<VMTPointer&>(VMTBasePointer::GetHook(inst));
	}







	//----------------------------------------------------------------
	// Class: VMTHook
	//----------------------------------------------------------------
	// Hooks virtual functions by overwriting the function pointers in .rdata.
	// Currently no support for unhooking...
	class VMTBaseHook
	{
	public:
		// Init the hook.
		VMTBaseHook& Init(void* inst, unsigned int offset = 0, unsigned int vfuncs = 0);
		VMTBaseHook& Init(void** vmt, unsigned int vfuncs = 0);
		// Unhook and forget about this instance.
		void Kill();

		// Hooks a function by index.
		VMTBaseHook& HookMethod(void* new_func, unsigned int index);
		// Unhooks a function by index.
		VMTBaseHook& UnhookMethod(unsigned int index);
		// Erase the hooks.
		void EraseHooks();

		// Get the original function.
		// Use a function prototype for the template argument to make it very easy to call this function.
		// Example syntax: hook.GetMethod<bool (__thiscall*)( void*, int )>( 0x5 )( inst, arg );
		template< typename Fn >
		const Fn& GetMethod(unsigned int index) const;

	protected:
		static bool WriteHook(void* dest, const void* src, unsigned int bytes);

	private:
		void**	_vftable;
		void**	_backup;
		unsigned int _vcount;
	};
	class VMTHook : public VMTBaseHook
	{
		// Forbid copy constructing and assignment
		VMTHook(const VMTHook&);
		VMTHook& operator= (const VMTHook&);

	public:
		VMTHook(void* inst, unsigned int offset = 0, unsigned int vfuncs = 0);
		VMTHook(void** vmt, unsigned int vfuncs = 0);
		~VMTHook();
	};
	// Inlines for VMTBaseHook
	inline VMTBaseHook& VMTBaseHook::Init(void* inst, unsigned int offset, unsigned int vfuncs)
	{
		return Init(*reinterpret_cast<void***>((char*)inst + offset), vfuncs);
	}
	template< typename Fn >
	inline const Fn& VMTBaseHook::GetMethod(unsigned int index) const
	{
		return *(Fn*)&_backup[index];
	}
	inline VMTBaseHook& VMTBaseHook::HookMethod(void* func, unsigned int index)
	{
		WriteHook(&_vftable[index], &func, sizeof(void*));
		return *this;
	}
	inline VMTBaseHook& VMTBaseHook::UnhookMethod(unsigned int index)
	{
		WriteHook(&_vftable[index], &_backup[index], sizeof(void*));
		return *this;
	}
	inline void VMTBaseHook::EraseHooks()
	{
		WriteHook(_vftable, _backup, _vcount*sizeof(void*));
	}
	// Inlines for VMTHook
	inline VMTHook::VMTHook(void* inst, unsigned int offset, unsigned int vfuncs)
	{
		Init(inst, offset, vfuncs);
	}
	inline VMTHook::VMTHook(void** vmt, unsigned int vfuncs)
	{
		Init(vmt, vfuncs);
	}
	inline VMTHook::~VMTHook()
	{
		Kill();
	}


}