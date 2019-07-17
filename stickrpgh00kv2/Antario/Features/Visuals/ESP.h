#pragma once

struct TestPos
{
	Vector Pos;
	float SimulationTime;
};

class ESP : public singleton<ESP> // single ton for ok
{
public:
    void Render();
private:
	struct
	{
		int left, top, right, bottom;
	}Box;

	Color boxcolor;
	Color textcolor;
	Color skelecolor;
	Color offscrncolor;
	DWORD font;
	int offsetY;

	void BoundBox(C_BaseEntity* pEnt);
    void RenderBox();
	void RenderWeaponName(C_BaseEntity* pEnt);
    void RenderName(C_BaseEntity* pEnt, int iterator);
	void RenderHealth(C_BaseEntity* pEnt);
	void RenderBacktrack(C_BaseEntity* pEnt, C_BaseEntity* pLocalEnt);
	void RenderSkeleton(C_BaseEntity* pEnt);
	void RenderOffScreen(C_BaseEntity* pEnt, C_BaseEntity* pLocalEnt);
	void RenderHitpoints(int index);
};
