#pragma once

#include <string>
#include <filesystem>
#include <deque>

struct ColorV2
{
	float red, green, blue, alpha;
};

struct PopUpStc
{
	int x;
	int y;
	std::string name;
	std::string item;
	int code = 0;
};

class Menu // wtf no SIngLETOne :(
{
public:
	int iScreenWidth, iScreenHeight;
	int SaveTab = 0;
	bool menuOpened;
	void Render();

	void PopUpRender();
	void PopUpAdd(int x, int y, std::string name, std::string item, int code = 0);

	struct
	{
		bool LegitBacktrack;

		bool Aimbot;
		int HitchanceValue;
		int Mindmg;
		int SilentAim;
		bool AutoScope;
		bool Resolver[2] = { false,false };
		bool Hitscan[6] = { false,false,false,false,false,false };
		int BodyScale;
		int HeadScale;
		bool MultiPoint;
		bool DelayShot;
		bool IgnoreLimbs;
		bool Autostop;
		bool PosBacktrack;
		bool BacktrackHitbox[2] = { false,false };
		bool ShotBacktrack;
		bool BaimLethal;
		bool BaimPitch;
		bool BaimInAir;


		bool Antiaim;
		bool DesyncAngle;
		bool RandJitterInRange;
		int	Pitch;
		int	Yaw;
		int	JitterRange;
		bool Freestanding;
		bool AtTarget;
		int	StandingLag;
		int	MovingLag;
		int	InAirLag;
		bool FakeLagOnPeek;
		int	ShotLag;

		bool Esp;
		int Font;
		ColorV2 FontColor = { 255.f,255.f,255.f,255.f };
		int Name;
		int HealthVal;
		int Weapon;
		bool Box;
		ColorV2 BoxColor = { 255.f,255.f,255.f,255.f };
		bool HealthBar;
		bool VisualBacktrack;
		bool Skeleton[2] = { false,false };
		ColorV2 SkeletonColor = { 255.f,255.f,255.f,255.f };
		bool OffScreen;
		ColorV2 OffScreenColor = { 255.f,255.f,255.f,255.f };
		bool HitPoints;
		bool Chams;
		ColorV2 ChamsColor = { 255.f,255.f,255.f,255.f };
		bool ViewModelChange;
		ColorV2 ViewModelColor = { 255.f,255.f,255.f,255.f };
		bool WeaponChange;
		ColorV2 WeaponColor = { 255.f,255.f,255.f,255.f };
		bool NoZoom;
		bool NoScope;
		bool NoRecoil;
		int Fov;
		bool Crosshair;
		bool NightMode;
		bool PostProcess;

		bool Bhop;
		bool AutoStrafe;
		int CircleStrafeKey;
		bool AntiUntrusted = false;
		bool FakeLagModel;
		int ThirdPersonKey;
		int ThirdPersonDistance;
		int FakeDuckKey;
		bool WalkBot;
		bool HeadStealer;
	}Config;

private:
	struct
	{
		float x = 0.f, y = 0.f;
	}Pos; // lol

	enum
	{
		check_box,
		slider,
		combo_box,
		multi_box,
		key_bind
	};

	std::deque<PopUpStc> PopBuff;

	int Width = 303; // 303
	int Height = 340; // 340

	int ControlsX;
	int GroupTabBottom;
	int OffsetY;
	int OldOffsetY;
	int TabOffset;
	int SubTabOffset;
	float SubTabSize; // cpp fuckin sux had to make this a float or the whole thing crashes *jk i fukin sux
	float TabSize;
	int GroupTabPos[4];

	int TabNum = 0;
	int SubTabNum = 0;
	int PreviousControl = -1;
	bool ControlsReturn;

	void Tab(std::string name);
	void SubTab(std::string name);
	void CheckBox(std::string name, bool* item);
	void Slider(int max, std::string name, int* item);
	void ComboBox(std::string name, std::vector< std::string > itemname, int* item);
	void MultiComboBox(std::string name, std::vector< std::string > itemname, bool* item);
	void ColorPicker(std::string name, ColorV2& item);
	void KeyBind(std::string name, int* item);
};

extern Menu g_Menu;