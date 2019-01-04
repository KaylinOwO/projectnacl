#pragma once
#include "SDK.h"

class IBaseFileSystem;
class KeyValues
{
public:
	static KeyValues* Initialize(KeyValues* kv, char* name);
	static bool LoadFromBuffer(KeyValues* kv, char const *resourceName, const char *pBuffer, IBaseFileSystem* pFileSystem = NULL, const char *pPathID = NULL);

	int m_iKeyName;	// keyname is a symbol defined in KeyValuesSystem

	// These are needed out of the union because the API returns string pointers

	char *m_sValue;
	wchar_t *m_wsValue;

	// we don't delete these
	union
	{
		int m_iValue;
		float m_flValue;
		void *m_pValue;
		unsigned char m_Color[4];
	};

	char	m_iDataType;
	char	m_bHasEscapeSequences;	// true, if while parsing this KeyValue, Escape Sequences are used (default false)
	char	m_bEvaluateConditionals; // true, if while parsing this KeyValue, conditionals blocks are evaluated (default true)
	char	unused[1];

	KeyValues *m_pPeer;	// pointer to next key in list
	KeyValues *m_pSub;	// pointer to Start of a new sub key list
	KeyValues *m_pChain;// Search here if it's not in our list
};