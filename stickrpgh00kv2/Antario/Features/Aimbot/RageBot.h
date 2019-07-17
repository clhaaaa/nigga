#pragma once

struct HitboxInfo
{
public:
	Vector Pos;
	bool Body;
};

class RageBot : public singleton<RageBot> // singleton ftw
{
public:
	int Sequence;
	void OnCreateMove();
	int bestEntDmg;
	std::vector<HitboxInfo> Hitboxes[65];
private:
	bool Backtrack[65];
	bool ShotBacktrack[65];
	Vector Hitscan(C_BaseEntity* pEnt);
	bool HitChance(C_BaseEntity* pEnt, C_BaseCombatWeapon* pWeapon, Vector Angle, Vector Point, int chance);
	void Autostop();
};
