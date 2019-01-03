#pragma once

#include "windows.h"

class ConVar;
class IConVar;
class CCommand;
class ConCommand;
class ConCommandBase;

class IConVar
{
public:
	// Value set
	virtual void SetValue(const char *pValue) = 0;
	virtual void SetValue(float flValue) = 0;
	virtual void SetValue(int nValue) = 0;

	// Return name of command
	virtual const char *GetName(void)const = 0;

	// Accessors.. not as efficient as using GetState()/GetInfo()
	// if you call these methods multiple times on the same IConVar
	virtual bool IsFlagSet(int nFlag) const = 0;
};

// Called when a ConVar changes value
// NOTE: For FCVAR_NEVER_AS_STRING ConVars, pOldValue == NULL
typedef void(*FnChangeCallback_t)(IConVar *var, const char *pOldValue, float flOldValue);

class IConCommandBaseAccessor
{
public:
	virtual bool RegisterConCommandBase(ConCommandBase *pVar) = 0;
};

void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor = NULL);

class ConCommandBase
{
public:
	ConCommandBase(void);
	ConCommandBase(const char *pName, const char *pHelpString = 0, int flags = 0);

	virtual						~ConCommandBase(void);
	virtual	bool				IsCommand(void) const;
	virtual bool					IsFlagSet(int flag) const;
	virtual void					AddFlags(int flags);
	virtual void					RemoveFlags(int flags);
	virtual int					GetFlags() const;
	virtual const char*			GetName(void) const;
	virtual const char*			GetHelpText(void) const;
	virtual bool					IsRegistered(void) const;
	virtual int					GetDLLIdentifier() const;
	virtual void					Create(const char *pName, const char *pHelpString = 0, int flags = 0);
	virtual void					Init();

public:
	ConCommandBase*					m_pNext;
	bool							m_bRegistered;
	const char*						m_pszName;
	const char*						m_pszHelpString;
	int								m_nFlags;

public:
	static ConCommandBase*			s_pConCommandBases;
	static IConCommandBaseAccessor*	s_pAccessor;
};

class ConVar : public ConCommandBase, public IConVar
{
public:

	typedef ConCommandBase BaseClass;

	ConVar(const char *pName, const char *pDefaultValue, int flags = 0);

	ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString);
	ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax);
	ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, FnChangeCallback_t callback);
	ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback);

	virtual						~ConVar(void);

	virtual bool					IsFlagSet(int flag) const;
	virtual const char*				GetHelpText(void) const;
	virtual bool					IsRegistered(void) const;
	virtual const char*				GetName(void) const;
	virtual const char*				GetBaseName(void) const;
	virtual int						GetSplitScreenPlayerSlot() const;
	DWORD GetColor(void) const;
	virtual void					AddFlags(int flags);
	virtual int						GetFlags() const;
	virtual	bool					IsCommand(void) const;
	virtual void					SetValue(const char *value);
	virtual void					SetValue(float value);
	virtual void					SetValue(int value);
	virtual void					SetValue(DWORD value);
	virtual void					InternalSetValue(const char *value);
	virtual void					InternalSetFloatValue(float fNewValue);
	virtual void					InternalSetIntValue(int nValue);
	virtual void					InternalSetColorValue(DWORD value);
	virtual bool					ClampValue(float& value);
	virtual void					ChangeStringValue(const char *tempVal, float flOldValue);
	virtual void					Create(const char *pName, const char *pDefaultValue, int flags = 0,
		const char *pHelpString = 0, bool bMin = false, float fMin = 0.0,
		bool bMax = false, float fMax = false, FnChangeCallback_t callback = 0);


	//----------------------------
	// Non-virtual helper methods
	//----------------------------
	float					GetFloat(void) const;
	int						GetInt(void) const;
	//DWORD					GetColor(void) const;
	const char*				GetString(void) const;
	const char*				GetDefault(void) const;

	void InstallChangeCallback(FnChangeCallback_t callback);

	// Value
	struct CVValue_t
	{
		char*	m_pszString;
		int		m_StringLength;
		float	m_fValue;
		int		m_nValue;
	};

	ConVar*					m_pParent;
	const char	*			m_pszDefaultValue;
	CVValue_t				m_Value;
	bool						m_bHasMin;
	float					m_fMinVal;
	bool						m_bHasMax;
	float					m_fMaxVal;
	FnChangeCallback_t		m_fnChangeCallback;

	char pad_0x0000[0x4]; //0x0000
	ConVar* pNext; //0x0004 
	__int32 bRegistered; //0x0008 
	char* pszName; //0x000C 
	char* pszHelpString; //0x0010 
	__int32 nFlags; //0x0014 
	char pad_0x0018[0x4]; //0x0018
	ConVar* pParent; //0x001C 
	char* pszDefaultValue; //0x0020 
	char* strString; //0x0024 
	__int32 StringLength; //0x0028 
	float fValue; //0x002C 
	__int32 nValue; //0x0030 
	__int32 bHasMin; //0x0034 
	float fMinVal; //0x0038 
	__int32 bHasMax; //0x003C 
	float fMaxVal; //0x0040 
	void* fnChangeCallback; //0x0044 
};

// moved from macros to class to not pollute the global namespace
enum class ConvarFlags
{
	// The default, no flags at all
	FCVAR_NONE = 0,

	// Command to ConVars and ConCommands
	// ConVar Systems
	FCVAR_UNREGISTERED = (1 << 0), // If this is set, don't add to linked list, etc.
	FCVAR_DEVELOPMENTONLY = (1 << 1), // Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
	FCVAR_GAMEDLL = (1 << 2), // defined by the game DLL
	FCVAR_CLIENTDLL = (1 << 3), // defined by the client DLL
	FCVAR_HIDDEN = (1 << 4), // Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

							 // ConVar only
	FCVAR_PROTECTED = (1 << 5),  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
	FCVAR_SPONLY = (1 << 6),  // This cvar cannot be changed by clients connected to a multiplayer server.
	FCVAR_ARCHIVE = (1 << 7),  // set to cause it to be saved to vars.rc
	FCVAR_NOTIFY = (1 << 8),  // notifies players when changed
	FCVAR_USERINFO = (1 << 9),  // changes the client's info string
	FCVAR_CHEAT = (1 << 14), // Only useable in singleplayer / debug / multiplayer & sv_cheats

	FCVAR_PRINTABLEONLY = (1 << 10), // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
	FCVAR_UNLOGGED = (1 << 11), // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
	FCVAR_NEVER_AS_STRING = (1 << 12), // never try to print that cvar

									   // It's a ConVar that's shared between the client and the server.
									   // At signon, the values of all such ConVars are sent from the server to the client (skipped for local
									   //  client, of course )
									   // If a change is requested it must come from the console (i.e., no remote client changes)
									   // If a value is changed while a server is active, it's replicated to all connected clients
	FCVAR_REPLICATED = (1 << 13), // server setting enforced on clients, TODO rename to FCAR_SERVER at some time
	FCVAR_DEMO = (1 << 16), // record this cvar when starting a demo file
	FCVAR_DONTRECORD = (1 << 17), // don't record these command in demofiles
	FCVAR_RELOAD_MATERIALS = (1 << 20), // If this cvar changes, it forces a material reload
	FCVAR_RELOAD_TEXTURES = (1 << 21), // If this cvar changes, if forces a texture reload

	FCVAR_NOT_CONNECTED = (1 << 22), // cvar cannot be changed by a client that is connected to a server
	FCVAR_MATERIAL_SYSTEM_THREAD = (1 << 23), // Indicates this cvar is read from the material system thread
	FCVAR_ARCHIVE_XBOX = (1 << 24), // cvar written to config.cfg on the Xbox

	FCVAR_ACCESSIBLE_FROM_THREADS = (1 << 25), // used as a debugging tool necessary to check material system thread convars

	FCVAR_SERVER_CAN_EXECUTE = (1 << 28), // the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
	FCVAR_SERVER_CANNOT_QUERY = (1 << 29), // If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
	FCVAR_CLIENTCMD_CAN_EXECUTE = (1 << 30), // IVEngineClient::ClientCmd is allowed to execute this command.

	FCVAR_MATERIAL_THREAD_MASK = (FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD)
};