#include "CEtags.h"

#include "SDK.h"

DEFINE_ENUM_FLAG_OPERATORS(EntTag);

CEntTag::CEntTag(CBaseEntity *ent) : tags(EntTag::NO_TAGS)
{
	if (ent == nullptr)
		return;

	if (ent->IsDormant())
		return;

	classId c = (classId)ent->GetClientClass()->iClassID;

	if (c == classId::CTFPlayer)
	{
		tags |= EntTag::PLAYER;
		return;
	}
	else if (c == classId::CObjectSentrygun)
	{
		tags |= EntTag::BUILDING;
		tags |= EntTag::SENTRY;

		return;
	}
	else if (c == classId::CObjectDispenser)
	{
		tags |= EntTag::BUILDING;
		tags |= EntTag::DISPENSER;

		return;
	}
	else if (c == classId::CObjectTeleporter)
	{
		tags |= EntTag::BUILDING;
		tags |= EntTag::TELEPORTER;

		return;
	}
	else if (c == classId::CTFGrenadePipebombProjectile
		|| c == classId::CTFProjectile_Arrow || c == classId::CTFProjectile_Flare
		|| c == classId::CTFProjectile_Rocket || c == classId::CTFProjectile_SentryRocket
		|| c == classId::CTFProjectile_Jar || c == classId::CTFProjectile_JarMilk)
	{
		tags |= EntTag::PROJECTILE;
		return;
	}
	else
	{
		// check for weapon ids	
		if (ent->IsBaseCombatWeapon())
		{
			int slot = ((CBaseCombatWeapon *)ent)->GetSlot();

			tags |= EntTag::WEAPON;

			switch (slot)
			{
			case 0:
				tags |= EntTag::PRIMARY;
				//Log::Console("Primary");
				break;
			case 1:
				tags |= EntTag::SECONDARY;
				//Log::Console("Secondary");
				break;
			case 2:
				tags |= EntTag::MELEE;
				//Log::Console("Melee");
				break;
			case 3:
			case 4:
				tags |= EntTag::PDA;
				//Log::Console("Pda");
				break;
			}
		}
	}
}

EntTag CEntTag::getTags()
{
	return tags;
}

bool CEntTag::isPlayer()
{
	return (bool)(tags & EntTag::PLAYER);
}

bool CEntTag::isBuilding()
{
	return (bool)(tags & EntTag::BUILDING);
}

bool CEntTag::isSentry()
{
	return (bool)(tags & EntTag::SENTRY);
}

bool CEntTag::isDispenser()
{
	return (bool)(tags & EntTag::DISPENSER);
}

bool CEntTag::isTele()
{
	return (bool)(tags & EntTag::TELEPORTER);
}

bool CEntTag::isWeap()
{
	return (bool)(tags & EntTag::WEAPON);
}

bool CEntTag::isPrimary()
{
	return (bool)(tags & EntTag::PRIMARY);
}

bool CEntTag::isSecondary()
{
	return (bool)(tags & EntTag::SECONDARY);
}

bool CEntTag::isMelee()
{
	return (bool)(tags & EntTag::MELEE);
}

bool CEntTag::isPda()
{
	return (bool)(tags & EntTag::PDA);
}

bool CEntTag::isProjectile()
{
	return (bool)(tags & EntTag::PROJECTILE);
}