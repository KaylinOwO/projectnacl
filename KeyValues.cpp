#include "KeyValues.h"
#include "Offsets.h"

bool KeyValues::LoadFromBuffer(KeyValues* kv, char const *resourceName, const char *pBuffer, IBaseFileSystem* pFileSystem, const char *pPathID)
{
	typedef int(__thiscall *LoadFromBufferFn)(KeyValues* kv, char const*, const char*, IBaseFileSystem*, const char*);
	static LoadFromBufferFn Load = (LoadFromBufferFn)gOffsets.LoadFromBuffer;

	return Load(kv, resourceName, pBuffer, pFileSystem, pPathID);
}

KeyValues* KeyValues::Initialize(KeyValues* kv, char* name)
{
	typedef KeyValues*(__thiscall *InitializeFn)(KeyValues*, char*);
	static InitializeFn Init = (InitializeFn)gOffsets.KeyValues;

	return Init(kv, name);
}