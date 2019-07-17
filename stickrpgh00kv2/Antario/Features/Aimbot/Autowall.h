#pragma once

struct FireBulletData
{
	FireBulletData(const Vector &eyePos, C_BaseEntity* entity) : src(eyePos) , filter(entity)
	{
	}

	Vector          src;
	C_Trace         enter_trace;
	Vector          direction;
	C_TraceFilter   filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
};

class Autowall : public singleton<Autowall> // singleton ftw
{
public:
	bool VectortoVectorVisible(Vector src, Vector point, C_BaseEntity* pEnt = nullptr);
	bool CanHitFloatingPoint(const Vector &point, const Vector &source, C_BaseEntity* pEnt = nullptr);
	float Damage(const Vector &point);
private:
};
