#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "dt_recv2.h"
#include <Windows.h>

//Credits to Altimor + F1SSI0N

class CNetVars {
	struct node;
	using map_type = std::unordered_map<std::string, std::shared_ptr<node>>;

	struct node {
		node(DWORD offset) : offset(offset) {}
		map_type nodes;
		DWORD offset;
	};

	map_type nodes;

public:
	void Initialize();

private:
	void populate_nodes(class RecvTable *recv_table, map_type *map);

	/**
	* get_offset_recursive - Return the offset of the final node
	* @map:	Node map to scan
	* @acc:	Offset accumulator
	* @name:	Netvar name to search for
	*
	* Get the offset of the last netvar from map and return the sum of it and accum
	*/
	DWORD get_offset_recursive(map_type &map, int acc, const char *name)
	{
		return acc + map[name]->offset;
	}

	/**
	* get_offset_recursive - Recursively grab an offset from the tree
	* @map:	Node map to scan
	* @acc:	Offset accumulator
	* @name:	Netvar name to search for
	* @args:	Remaining netvar names
	*
	* Perform tail recursion with the nodes of the specified branch of the tree passed for map
	* and the offset of that branch added to acc
	*/
	template<typename ...args_t>
	DWORD get_offset_recursive(map_type &map, int acc, const char *name, args_t ...args)
	{
		const auto &node = map[name];
		return get_offset_recursive(node->nodes, acc + node->offset, args...);
	}

public:
	/**
	* get_offset - Get the offset of a netvar given a list of branch names
	* @name:	Top level datatable name
	* @args:	Remaining netvar names
	*
	* Initiate a recursive search down the branch corresponding to the specified datable name
	*/
	template<typename ...args_t>
	DWORD get_offset(const char *name, args_t ...args)
	{
		const auto &node = nodes[name];
		return get_offset_recursive(node->nodes, node->offset, args...);
	}
};

extern CNetVars gNetVars;

template<typename T>
class CDynamicNetvar
{
	DWORD off;

public:
	template<typename... args_t>
	CDynamicNetvar(args_t... a)
	{
		// just assume these wont be accessed until after netvars has been inited

		off = gNetVars.get_offset(a...);
	}
	template<typename... args_t>
	CDynamicNetvar(int offset, args_t... a)
	{
		off = gNetVars.get_offset(a...) + offset;
	}

	T GetValue(PVOID base)
	{
		return *reinterpret_cast< T * >((DWORD)base + (DWORD)off);
	}
	void SetValue(PVOID base, T val)
	{
		*reinterpret_cast<T * >((DWORD)(base)+((DWORD)(off))) = val;
	}
};

#define DYNVAR(name, type, ...) static CDynamicNetvar<type> ##name( __VA_ARGS__ )
#define DYNVAR_RETURN(type, base, ...) DYNVAR(n, type, __VA_ARGS__); return n.GetValue(base)

#define DYNVAR_OFF(name, type, offset, ...) static CDynamicNetvar<type> ##name(offset, __VA_ARGS__)
#define DYNVAR_OFF_RETURN(type, base, offset, ...) DYNVAR_OFF(n, type, offset, __VA_ARGS__); return n.GetValue(base)