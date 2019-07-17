#pragma once

#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / g_pGlobalVars->intervalPerTick ) )
#define TICKS_TO_TIME( t )		( g_pGlobalVars->intervalPerTick *( t ) )

struct TickInfo
{
	float SimTime = -1.f;
	Vector Head;
	bool Shot;
	float Velocity;
	matrix3x4_t Matrix[128];
	float Pitch;
};

class LagComp : public singleton<LagComp> // singleton ftw
{
public:
	float ShotTime[65];
	int ShotTick[65];
	int Sequence[65];
	matrix3x4_t pMatrix[65][128];
	std::deque<TickInfo> PlayerRecord[65] = {};
	float LerpTime();
	bool ValidTick(float simtime);
	void CreateRecord(C_BaseEntity* Entity);
	void ClearRecords(int i);
	void FrameStage(ClientFrameStage_t stage);

	template<class T, class U>
	T clamp(T in, U low, U high);
private:

};