#pragma once
#include <vector>

class CBaseEntity;

enum class EntTag : int
{
	NO_TAGS = 0,
	PLAYER = (1 << 0),
	BUILDING = (1 << 1),
	SENTRY = (1 << 2),
	DISPENSER = (1 << 3),
	TELEPORTER = (1 << 4),
	WEAPON = (1 << 5),
	PRIMARY = (1 << 6),
	SECONDARY = (1 << 7),
	MELEE = (1 << 8),
	PDA = (1 << 9),
	PROJECTILE = (1 << 10),
};

class CEntTag
{
	EntTag tags;

public:
	CEntTag(CBaseEntity *ent);
	CEntTag() : tags(EntTag::NO_TAGS) {};

	// get the current tags for manual looking
	EntTag getTags();

	// helper functions
	bool isPlayer();
	bool isBuilding();
	bool isSentry();
	bool isDispenser();
	bool isTele();
	bool isWeap();
	bool isPrimary();
	bool isSecondary();
	bool isMelee();
	bool isPda();
	bool isProjectile();
};
