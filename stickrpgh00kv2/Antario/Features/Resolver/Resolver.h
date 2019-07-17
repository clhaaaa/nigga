#pragma once
class Resolver : public singleton<Resolver> // singleton for ur mom
{
public:
	bool UseFreestandAngle[65];
	float FreestandAngle[65];
	Vector absOriginBackup;
	float pitchHit[65];

	void OnCreateMove();
	void AnimationFix(C_BaseEntity* pEnt, C_BaseEntity* pLocalEnt);
	void HandleHits(C_BaseEntity* pEnt, C_BaseEntity* pLocalEnt);
private:
};
