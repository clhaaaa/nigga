#include "Menu.h"
#include "TGFCfg.h"
#include "..\SDK\Vector.h"
#include "..\Utils\Color.h"
#include "..\Utils\GlobalVars.h"
#include "..\SDK\ISurface.h"
#include "..\Utils\XorStr.h"

Menu g_Menu;

std::string epicspookyhacker1337encryptstring(int gotem) // am i a genius???? (no)
{
	switch (gotem)
	{
	case 1: return "a"; break;
	case 2: return "b"; break;
	case 3: return "c"; break;
	case 4: return "d"; break;
	case 5: return "e"; break;
	case 6: return "f"; break;
	case 7: return "g"; break;
	case 8: return "h"; break;
	case 9: return "i"; break;
	case 10: return "j"; break;
	case 11: return "k"; break;
	case 12: return "l"; break;
	case 13: return "m"; break;
	case 14: return "n"; break;
	case 15: return "o"; break;
	case 16: return "p"; break;
	case 17: return "q"; break;
	case 18: return "r"; break;
	case 19: return "s"; break;
	case 20: return "t"; break;
	case 21: return "u"; break;
	case 22: return "v"; break;
	case 23: return "w"; break;
	case 24: return "x"; break;
	case 25: return "y"; break;
	case 26: return "z"; break;
	case 27: return " "; break;
	case 28: return "."; break;
	case 29: return "July 13 2019"; break;
	case 30: return "2"; break;
	}
}

void Menu::Render()
{
	static bool Pressed = false;

	if (!Pressed && GetAsyncKeyState(VK_INSERT))
		Pressed = true;
	else if (Pressed && !GetAsyncKeyState(VK_INSERT))
	{
		Pressed = false;
		menuOpened = !menuOpened;
	}

	static int dragX = 0;
	static int dragY = 0;

	static std::string Title = epicspookyhacker1337encryptstring(19) + epicspookyhacker1337encryptstring(20) + epicspookyhacker1337encryptstring(9) + epicspookyhacker1337encryptstring(3) + epicspookyhacker1337encryptstring(11) + epicspookyhacker1337encryptstring(18) + epicspookyhacker1337encryptstring(16) + epicspookyhacker1337encryptstring(7) + epicspookyhacker1337encryptstring(8)+ epicspookyhacker1337encryptstring(15)+ epicspookyhacker1337encryptstring(15)+ epicspookyhacker1337encryptstring(11) + epicspookyhacker1337encryptstring(27) + epicspookyhacker1337encryptstring(22) + epicspookyhacker1337encryptstring(28) + epicspookyhacker1337encryptstring(30) + epicspookyhacker1337encryptstring(27) + epicspookyhacker1337encryptstring(29); // insert p2c name gotem 9999999999999999 iq

	static bool Dragging = false;
	bool click = false;

	if (menuOpened)
	{
		if (GetAsyncKeyState(VK_LBUTTON))
			click = true;

		Vector2D MousePos = g_pRender->GetMousePosition();

		if (Dragging && !click)
		{
			Dragging = false;
		}

		if (Dragging && click)
		{
			Pos.x = MousePos.x - dragX;
			Pos.y = MousePos.y - dragY;
		}

		if (g_pRender->MouseInRegion(Pos.x, Pos.y, Width, 20))
		{
			Dragging = true;
			dragX = MousePos.x - Pos.x;
			dragY = MousePos.y - Pos.y;
		}

		if (Pos.x < 0)
			Pos.x = 0;
		if (Pos.y < 0)
			Pos.y = 0;
		if ((Pos.x + Width) > iScreenWidth)
			Pos.x = iScreenWidth - Width;
		if ((Pos.y + Height) > iScreenHeight)
			Pos.y = iScreenHeight - Height;

		g_pRender->FilledRect(Pos.x, Pos.y, Width, Height, Color(25, 25, 25, 225));
		g_pRender->FilledRect(Pos.x, Pos.y + 20, Width, Height - 40, Color(33, 33, 33, 255));

		int nen = Pos.x - 10;
		int nen2 = Pos.y - 10;

		for (int grad = 1; grad <= 9; grad++)
			g_pRender->OutlinedRect(nen + grad, nen2 + grad, Width + 20 - (grad * 2), Height + 20 - (grad * 2), Color(25, 25, 25, (grad * 25)));

		GroupTabPos[0] = Pos.x + 2;
		GroupTabPos[1] = Pos.y + 43;
		GroupTabPos[2] = Width - 4;
		GroupTabPos[3] = Height - 63;

		ControlsX = GroupTabPos[0];
		GroupTabBottom = GroupTabPos[1] + GroupTabPos[3];

		g_pRender->DrawT(Pos.x + 6, Pos.y + 2, Color(255, 255, 255, 255), g::CourierNew, false, Title.c_str()); // title

		OffsetY = GroupTabPos[1] + 7;

		static bool CfgInitLoad = true;
		static bool CfgInitSave = true;
		static bool CfgInitComb = true;
		static bool CfgCombOpen = false;

		if (g_pRender->MouseInRegion(Pos.x + 106, Pos.y + (Height - 18), 50, 16) && click)
		{
			if (CfgInitComb)
			{
				CfgCombOpen = !CfgCombOpen;
				CfgInitComb = false;
			}
		}
		else
			CfgInitComb = true;

		if (CfgCombOpen)
		{
			for (int i = 1; i <= 5; i++)
			{
				g_pRender->AutoGradientRect(Pos.x + 106, Pos.y + (Height - 18) + (16 * i), 50, 16, Color(119, 119, 119, 255));
				g_pRender->DrawT(Pos.x + 117, Pos.y + (Height - 18) + (16 * i), Color(255, 255, 255, 255), g::CourierNew, false, ("cfg" + std::to_string(i)).c_str());
				if (g_pRender->MouseInRegion(Pos.x + 106, Pos.y + (Height - 18) + (16 * i), 50, 16) && click)
					SaveTab = i - 1;
			}
		}

		g_pRender->AutoGradientRect(Pos.x + 106, Pos.y + (Height - 18), 50, 16, Color(119, 119, 119, 255));
		g_pRender->DrawT(Pos.x + 117, Pos.y + (Height - 18), Color(255, 255, 255, 255), g::CourierNew, false, ("cfg" + std::to_string(SaveTab + 1)).c_str());

		g_pRender->AutoGradientRect(Pos.x + 2, Pos.y + (Height - 18), 50, 16, Color(119, 119, 119, 255));
		g_pRender->AutoGradientRect(Pos.x + 54, Pos.y + (Height - 18), 50, 16, Color(119, 119, 119, 255));

		if (g_pRender->MouseInRegion(Pos.x + 54, Pos.y + (Height - 18), 50, 16))
		{
			if (click)
				g_pRender->AutoGradientRect(Pos.x + 54, Pos.y + (Height - 18), 50, 16, Color(53, 53, 53, 255));
			if (CfgInitLoad && click)
			{
				g_Config->Load();
				g_Menu.PopUpAdd(100, 100, "Test", "Loaded Config #" + std::to_string(SaveTab + 1));
				CfgInitLoad = false;
			}
		}
		else
			CfgInitLoad = true;

		if (g_pRender->MouseInRegion(Pos.x + 2, Pos.y + (Height - 18), 50, 16))
		{
			if (click)
				g_pRender->AutoGradientRect(Pos.x + 2, Pos.y + (Height - 18), 50, 16, Color(53, 53, 53, 255));
			if (CfgInitSave && click)
			{
				g_Config->Save();
				g_Menu.PopUpAdd(100, 100, "Test", "Saved Config #" + std::to_string(SaveTab + 1));
				CfgInitSave = false;
			}
		}
		else
			CfgInitSave = true;

		g_pRender->DrawT(Pos.x + 13, Pos.y + (Height - 18), Color(255, 255, 255, 255), g::CourierNew, false, "save");
		g_pRender->DrawT(Pos.x + 65, Pos.y + (Height - 18), Color(255, 255, 255, 255), g::CourierNew, false, "load");

		TabOffset = 0;
		SubTabOffset = 0;
		PreviousControl = -1;
		OldOffsetY = 0;
		ControlsReturn = false;

		if (g::CheatType == 2)
		{
			Tab("legit");
			{
				CheckBox("backtrack", &Config.LegitBacktrack);
			}
		}

		Tab("visual");
		{
			SubTab("esp");
			{
				CheckBox("active", &Config.Esp);
				ComboBox("font", { "courier", "tahoma" }, &Config.Font);
				ColorPicker("font color", Config.FontColor);
				ComboBox("draw name", { "off", "top", "right" }, &Config.Name);
				ComboBox("draw health", { "off", "right" }, &Config.HealthVal);
				ComboBox("draw weapon", { "off", "bottom", "right" }, &Config.Weapon);
				CheckBox("2d box", &Config.Box);
				ColorPicker("box color", Config.BoxColor);
				CheckBox("health bar", &Config.HealthBar);
				CheckBox("backtrack", &Config.VisualBacktrack);
				MultiComboBox("draw skeleton", { "normal", "backtrack" }, Config.Skeleton);
				ColorPicker("skele color", Config.SkeletonColor);	
				CheckBox("offscreen", &Config.OffScreen);
				ColorPicker("offscreen color", Config.OffScreenColor);
				if (g::CheatType == 1)
					CheckBox("hitpoints", &Config.HitPoints);
			}

			SubTab("render");
			{
				if (g::CheatType == 2)
				{
					CheckBox("chams", &Config.Chams);
					ColorPicker("chams color", Config.ChamsColor);
				}
				else if (g::CheatType == 1)
				{
					CheckBox("viewmodel color", &Config.ViewModelChange);
					ColorPicker("viewmodel colord", Config.ViewModelColor);
					CheckBox("weapon color", &Config.WeaponChange);
					ColorPicker("weapon colord", Config.WeaponColor);
					CheckBox("no zoom", &Config.NoZoom);
					CheckBox("no scope", &Config.NoScope);
					CheckBox("no recoil", &Config.NoRecoil);
					Slider(150, "fov", &Config.Fov);
					CheckBox("crosshair", &Config.Crosshair);
					CheckBox("night mode", &Config.NightMode);
					CheckBox("disable postprocess", &Config.PostProcess);
				}
			}
		}

		if (g::CheatType == 1)
		{
			Tab("rage");
			{
				SubTab("main");
				{
					std::string aimbotname = "active";
					if (Config.LegitBacktrack)
						aimbotname += " (turn off legitbacktrack)";
					CheckBox(aimbotname, &Config.Aimbot);
					Slider(100, "mindamage", &Config.Mindmg);
					ComboBox("silent aim", { "off", "normal", "static" }, &Config.SilentAim);
					CheckBox("autostop", &Config.Autostop);
					CheckBox("autoscope", &Config.AutoScope);
					Slider(100, "hitchance", &Config.HitchanceValue);
					MultiComboBox("resolver", { "yaw", "pitch" }, Config.Resolver);
				}

				SubTab("hitscan");
				{
					MultiComboBox("hitgroups", { "head", "chest", "pelvis", "upper arm", "toes", "thighs" }, Config.Hitscan);
					CheckBox("multipoint", &Config.MultiPoint);
					if (Config.MultiPoint)
					{
						Slider(100, "head scale", &Config.HeadScale);
						Slider(100, "body scale", &Config.BodyScale);
					}
					CheckBox("delay shot", &Config.DelayShot);
					CheckBox("ignore limbs on-move", &Config.IgnoreLimbs);
				}

				SubTab("accuracy");
				{
					CheckBox("shot backtrack", &Config.ShotBacktrack);
					CheckBox("position backtrack", &Config.PosBacktrack);
					if (Config.PosBacktrack)
						MultiComboBox("backtrack hitboxes", { "head", "pelvis" }, Config.BacktrackHitbox);
					CheckBox("baim if lethal", &Config.BaimLethal);
					CheckBox("baim bad pitch", &Config.BaimPitch);
					CheckBox("baim in air", &Config.BaimInAir);
				}
			}
		}

		if (g::CheatType == 1)
		{
			Tab("antiaim");
			{
				SubTab("angle");
				{
					std::string antiaimname = "active";
					if (Config.LegitBacktrack)
						antiaimname += " (turn off legitbacktrack)";
					CheckBox(antiaimname, &Config.Antiaim);
					CheckBox("desync angle", &Config.DesyncAngle);
					ComboBox("pitch", { "emotion", "fake jitter", "fake zero", "spin", "up", "fake up" }, &Config.Pitch);
					ComboBox("yaw", { "backward", "forward", "t-jitter", "spin" }, &Config.Yaw);
					CheckBox("freestanding", &Config.Freestanding);
					CheckBox("at-target", &Config.AtTarget);
					CheckBox("random jitter in-range", &Config.RandJitterInRange);
					Slider(360, "jitter range", &Config.JitterRange);
				}

				SubTab("fakelag");
				{
					Slider(14, "standing lag", &Config.StandingLag);
					Slider(14, "moving lag", &Config.MovingLag);
					Slider(14, "inair lag", &Config.InAirLag);
					CheckBox("flag on peek", &Config.FakeLagOnPeek);
					ComboBox("shooting lag", { "sendpacket", "double sendpacket", "dont sendpacket" }, &Config.ShotLag);
				}
			}
		}

		Tab("misc");
		{
			CheckBox("bhop", &Config.Bhop);
			CheckBox("autostrafe", &Config.AutoStrafe);
			CheckBox("anti ut", &Config.AntiUntrusted);
			if (g::CheatType == 1)
			{
				KeyBind("circlestrafe", &Config.CircleStrafeKey);
				CheckBox("lag model", &Config.FakeLagModel);
				KeyBind("thirdperson", &Config.ThirdPersonKey);
				Slider(150, "cam distance", &Config.ThirdPersonDistance);
				KeyBind("fake duck", &Config.FakeDuckKey);
				CheckBox("walkbot (W.I.P)", &Config.WalkBot);
				CheckBox("steal heads", &Config.HeadStealer);
			}
		}
		
		TabSize = TabOffset;
		SubTabSize = SubTabOffset;
	}
}

void Menu::Tab(std::string name)
{
	int TabLength = ((Width - 4) / TabSize);

	int TabArea[4] = { Pos.x + 2 + (TabOffset * TabLength), Pos.y + 22, TabLength, 21 };

	if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(TabArea[0], TabArea[1], TabArea[2], TabArea[3]))
	{
		ControlsReturn = true;
		TabNum = TabOffset;
	}

	if (TabOffset == 0)
		g_pRender->AutoGradientRect(TabArea[0], TabArea[1], (Width - 4), 21 , Color(119, 119, 119, 255));

	int Offset = ((TabSize - 1) == TabOffset) ? 0 : 1;

	if (((TabSize - 1) == TabOffset) && (((TabLength * TabSize) > (Width - 4)) || ((TabLength * TabSize) < (Width - 4))))
		Offset = ((Width - 4) - (TabLength * TabSize));

	if (TabOffset == TabNum)
		g_pRender->AutoGradientRect(TabArea[0], TabArea[1], TabLength + Offset, TabArea[3], Color(81, 115, 244, 255));

	RECT TextSize = g_pRender->GetTextSizeRect(g::CourierNew, name.c_str());

	int TextPosition[2] = { TabArea[0] + (TabLength / 2) - (TextSize.right / 2), (TabArea[1] + 10) - (TextSize.bottom / 2) };

	g_pRender->DrawT(TextPosition[0], TextPosition[1], Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());

	TabOffset += 1;
	PreviousControl = -1;
}

void Menu::SubTab(std::string name)
{
	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	RECT TextSize = g_pRender->GetTextSizeRect(g::CourierNew, name.c_str());

	static int TabSkip = 0;

	if (SubTabOffset == 0)
		g_pRender->AutoGradientRect(GroupTabPos[0], GroupTabPos[1], GroupTabPos[2], 21, Color(119, 119, 119, 255));

	if (SubTabSize != 0 && TabSkip == TabNum)
	{
		if (SubTabNum >= SubTabSize)
			SubTabNum = 0;

		int TabLength = (GroupTabPos[2] / SubTabSize);

		int GroupTabArea[4] = { (GroupTabPos[0]) + (TabLength * SubTabOffset), GroupTabPos[1], TabLength, 21 };

		if ((GroupTabArea[0] + GroupTabArea[3]) <= (GroupTabPos[0] + GroupTabPos[2]))
		{
			int TextPosition[2] = { GroupTabArea[0] + (TabLength / 2) - (TextSize.right / 2), (GroupTabArea[1] + 10) - (TextSize.bottom / 2) };

			if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(GroupTabArea[0], GroupTabArea[1], GroupTabArea[2], GroupTabArea[3]))
			{
				ControlsReturn = true;
				SubTabNum = SubTabOffset;
			}

			int Offset = ((SubTabSize - 1) == SubTabOffset) ? 0 : 1;

			if (((SubTabSize - 1) == SubTabOffset) && (((TabLength * SubTabSize) > GroupTabPos[2]) || ((TabLength * SubTabSize) < GroupTabPos[2])))
				Offset = (GroupTabPos[2] - (TabLength * SubTabSize));

			if (SubTabNum == SubTabOffset)
				g_pRender->AutoGradientRect(GroupTabArea[0], GroupTabArea[1], GroupTabArea[2] + Offset, GroupTabArea[3], Color(73, 73, 73, 255));

			g_pRender->DrawT(TextPosition[0], TextPosition[1], Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());
		}
	}

	if (TabSkip != TabNum) // frame skip for drawing
		TabSkip = TabNum;

	if (SubTabOffset == SubTabNum)
		OffsetY += 20;

	SubTabOffset += 1;
	PreviousControl = -1;
}

void Menu::CheckBox(std::string name, bool* item)
{
	if (ControlsReturn)
		return;

	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	static bool pressed = false;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 277, OffsetY, 16, 16))
	{
		if (pressed)
		{
			pressed = false;
			*item = !*item;
		}
	}
	else if (g_pRender->MouseInRegion(ControlsX + 277, OffsetY, 16, 16))
		pressed = true;

	if (*item == true)
		g_pRender->AutoGradientRect(ControlsX + 277, OffsetY, 16, 16, Color(120, 209, 109, 255));
	else
		g_pRender->AutoGradientRect(ControlsX + 277, OffsetY, 16, 16, Color(192, 97, 108, 255));

	g_pRender->DrawT(ControlsX + 6, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());

	OldOffsetY = OffsetY;
	OffsetY += 18;
	PreviousControl = check_box;
}

void Menu::Slider(int max, std::string name, int* item)
{
	if (ControlsReturn)
		return;

	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	float pixelValue = max / 134.f;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY, 140, 16))
		*item = (g_pRender->GetMousePosition().x - (ControlsX + 155)) * pixelValue;

	if (*item > max)
		*item = max;
	if (*item < 0)
		*item = 0;

	std::string nAme = name + " - " + std::to_string(*item);

	g_pRender->DrawT(ControlsX + 6, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, nAme.c_str());
	g_pRender->AutoGradientRect(ControlsX + 153, OffsetY , 140, 16, Color(62, 62, 62, 255));
	g_pRender->AutoGradientRect(ControlsX + 153 + (*item / pixelValue), OffsetY, 6, 16, Color(119, 119, 119, 255));

//	g_pRender->DrawT(ControlsX + 288, OffsetY + 1, Color(255, 255, 255, 255), g::CourierNew, true, std::to_string(*item).c_str());

	OldOffsetY = OffsetY;
	OffsetY += 18;
	PreviousControl = slider;
}

void Menu::ComboBox(std::string name, std::vector< std::string > itemname, int* item)
{
	if (ControlsReturn)
		return;

	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	bool pressed = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY, 140, 16) && !clickRest)
	{
		nameSelected = name;
		pressed = true;
		clickRest = true;
	}
	else if (!GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY, 140, 16))
		clickRest = false;

	if (pressed)
	{
		if (!rest)
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if (nameSelected == name)
		open = selectedOpened;

	g_pRender->DrawT(ControlsX + 6, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());
	g_pRender->AutoGradientRect(ControlsX + 153, OffsetY, 140, 16, Color(119, 119, 119, 255));

	if (open)
	{
		g_pRender->AutoGradientRect(ControlsX + 153, OffsetY, 140, 17 + (itemname.size() * 16), Color(119, 119, 119, 255));

		for (int i = 0; i < itemname.size(); i++)
		{
			if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY + 16 + i * 16, 140, 16))
				*item = i;

			if (*item == i)
				g_pRender->AutoGradientRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16, Color(53, 53, 53, 255));

			g_pRender->DrawT(ControlsX + 159, OffsetY + 16 + (i * 16), Color(255, 255, 255, 255), g::CourierNew, false, itemname.at(i).c_str());
		}
	}

	g_pRender->DrawT(ControlsX + 159, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, itemname.at(*item).c_str());

	OldOffsetY = OffsetY;

	if (open)
		OffsetY += 19 + (itemname.size() * 16);
	else
		OffsetY += 18;

	PreviousControl = combo_box;
}

void Menu::MultiComboBox(std::string name, std::vector< std::string > itemname, bool* item)
{
	if (ControlsReturn)
		return;

	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	static bool multiPressed = false;
	bool pressed = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;
	std::string itemsSelected = "";
	int lastItem = 0;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY, 140, 16) && !clickRest)
	{
		nameSelected = name;
		pressed = true;
		clickRest = true;
	}
	else if (!GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY, 140, 16))
		clickRest = false;

	if (pressed)
	{
		if (!rest)
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if (nameSelected == name)
		open = selectedOpened;

	g_pRender->DrawT(ControlsX + 6, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());
	g_pRender->AutoGradientRect(ControlsX + 153, OffsetY, 140, 16, Color(119, 119, 119, 255));

	if (open)
	{
		g_pRender->AutoGradientRect(ControlsX + 153, OffsetY, 140, 17 + (itemname.size() * 16), Color(119, 119, 119, 255));

		for (int i = 0; i < itemname.size(); i++)
		{
			if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY + 16 + (i * 16), 140, 16))
			{
				if (multiPressed)
				{
					multiPressed = false;
					item[i] = !item[i];
				}
			}
			else if (g_pRender->MouseInRegion(ControlsX + 153, OffsetY + 16 + (i * 16), 140, 16))
				multiPressed = true;

			if (item[i])
				g_pRender->AutoGradientRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16, Color(120, 209, 109, 255));
			else
				g_pRender->AutoGradientRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16, Color(192, 97, 108, 255));

			g_pRender->DrawT(ControlsX + 159, OffsetY + 16 + (i * 16), Color(255, 255, 255, 255), g::CourierNew, false, itemname.at(i).c_str());
		}

	}

	bool items = false;

	// man look at all these for loops i sure am retarded

	for (int i = 0; i < itemname.size(); i++)
	{
		if (item[i])
		{
			if (lastItem < i)
				lastItem = i;
		}
	}

	for (int i = 0; i < itemname.size(); i++)
	{ 
		if (item[i]) 
		{
			items = true;
			RECT TextSize = g_pRender->GetTextSizeRect(g::CourierNew, itemsSelected.c_str());
			RECT TextSizeGonaAdd = g_pRender->GetTextSizeRect(g::CourierNew, itemname.at(i).c_str());
			if (TextSize.right + TextSizeGonaAdd.right < 130)
				itemsSelected += itemname.at(i) + ((lastItem == i) ? "" : ", ");
		}
	}

	if (!items)
		itemsSelected = "off";

	g_pRender->DrawT(ControlsX + 159, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, itemsSelected.c_str());

	OldOffsetY = OffsetY;

	if (open)
		OffsetY += 19 + (itemname.size() * 16);
	else
		OffsetY += 18;

	PreviousControl = multi_box;
}

void Menu::ColorPicker(std::string name, ColorV2& item) // best coder in the universe
{
	if (ControlsReturn)
		return;

	if (GroupTabBottom <= OldOffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	if (PreviousControl == slider || PreviousControl == -1)
		return;

	int CtrXoffset = 0;

	if (PreviousControl != check_box)
		CtrXoffset = 135;
	else
		CtrXoffset = 259;

	int yoffset = OldOffsetY + 10;
	int xoffset = ControlsX + 330;

	int xoffsetoverscreen = 430;

	if (xoffset + 100 > iScreenWidth)
	{
		xoffset -= 460;
		xoffsetoverscreen -= 460;
	}

	if (yoffset + 110 > iScreenHeight)
		yoffset -= ((yoffset + 110) - iScreenHeight);

	Color rainbow;

	bool pressed = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + CtrXoffset, OldOffsetY, 16, 16) && !clickRest)
	{
		nameSelected = name;
		pressed = true;
		clickRest = true;
	}
	else if (!GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + CtrXoffset, OldOffsetY, 16, 16))
		clickRest = false;

	if (pressed)
	{
		if (!rest)
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if (nameSelected == name)
		open = selectedOpened;
	
	if (open)
	{
		g_pRender->FilledRect(xoffset, yoffset - 10, 100, 20, Color(0, 0, 0, 255));
		g_pRender->FilledRect(xoffset, yoffset + 90, 100, 20, Color(255, 255, 255, 255));

		if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(xoffset, OldOffsetY, 100, 10))
		{
			item.red = 0;
			item.green = 0;
			item.blue = 0;
			item.alpha = 255;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(xoffset, OldOffsetY + 110, 100, 10))
		{
			item.red = 255;
			item.green = 255;
			item.blue = 255;
			item.alpha = 255;
		}

		int nen = xoffset - 10;
		int nen2 = yoffset - 20;

		for (int grad = 1; grad <= 9; grad++)
			g_pRender->OutlinedRect(nen + grad, nen2 + grad, 100 + 20 - (grad * 2), 120 + 20 - (grad * 2), Color(25, 25, 25, (grad * 25)));
	
		for (int i = 0; i < 100; i++)
		{
			if (xoffset >= ControlsX + xoffsetoverscreen)
			{
				xoffset -= 100;
				yoffset += 10;
			}

			float hue = (i * .01f);

			rainbow.FromHSV(hue, 1.f, 1.f);

			g_pRender->FilledRect(xoffset , yoffset, 10, 10, rainbow);

			if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(xoffset, yoffset, 10, 10))
			{
				item.red = rainbow.red;
				item.green = rainbow.green;
				item.blue = rainbow.blue;
				item.alpha = 255.f;
			}			

			xoffset += 10;
		}
	}

	rainbow.red = item.red;
	rainbow.green = item.green;
	rainbow.blue = item.blue;
	rainbow.alpha = 255;

	g_pRender->FilledRect(ControlsX + CtrXoffset, OldOffsetY, 16, 16, rainbow);
}

void Menu::KeyBind(std::string name, int* item)
{
	if (ControlsReturn)
		return;

	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	static bool gotKey = false;
	bool pressed = false;
	if (gotKey)
		pressed = true;
	gotKey = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY, 140, 16) && !clickRest)
	{
		nameSelected = name;
		pressed = true;
		clickRest = true;
	}
	else if (!GetAsyncKeyState(VK_LBUTTON) && g_pRender->MouseInRegion(ControlsX + 153, OffsetY, 140, 16))
		clickRest = false;

	if (pressed)
	{
		if (!rest)
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if (nameSelected == name)
		open = selectedOpened;

	g_pRender->DrawT(ControlsX + 6, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());
	g_pRender->AutoGradientRect(ControlsX + 153, OffsetY, 140, 16, Color(119, 119, 119, 255));

	if (open)
	{
		for (int i = 0; i < 255; i++)
		{
			if ((GetAsyncKeyState(i) & 0x8000) && i != 1 && i != 45)
			{
				*item = i;
				gotKey = true;
			}
		}
		g_pRender->AutoGradientRect(ControlsX + 153, OffsetY, 140, 16, Color(53, 53, 53, 255));
	}

	g_pRender->DrawT(ControlsX + 159, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, std::string(1, char(*item)).c_str()); // K LOL

	OldOffsetY = OffsetY;
	OffsetY += 18;

	PreviousControl = key_bind;
}

void Menu::PopUpAdd(int x, int y, std::string name, std::string item, int code)
{
	PopBuff.push_back({ x, y, name, item, code});
}

bool contains(RECT R1, RECT R2) // https://stackoverflow.com/questions/27768039/find-out-if-a-rectangle-is-inside-another-rectangle-c
{
	if ((R2.left + R2.right) < (R1.left + R1.right)
		&& (R2.left) > (R1.left)
		&& (R2.top) < (R1.top)
		&& (R2.top + R2.bottom) > (R1.top + R1.bottom)
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Menu::PopUpRender() // just recycled codee from the start of menu xd
{
	if (PopBuff.size() == 0)
		return;

	for (int i = 0; i < PopBuff.size(); i++)
	{
		PopUpStc* PopEnt = &PopBuff.at(i);
		
		int PopWidth, PopHeight;
		PopHeight = 65;
		PopWidth = g_pRender->GetTextSizeRect(g::CourierNew, PopEnt->item.c_str()).right + 12;

		bool Dragging = false;
		bool click = false;

		if (GetAsyncKeyState(VK_LBUTTON))
			click = true;

		Vector2D MousePos = g_pRender->GetMousePosition();

		if (g_pRender->MouseInRegion(PopEnt->x, PopEnt->y, PopWidth, 20) && click)
		{
			PopEnt->x = MousePos.x - int(PopWidth / 2);
			PopEnt->y = MousePos.y - 10;
		}

		if (PopEnt->x < 0)
			PopEnt->x = 0;
		if (PopEnt->y < 0)
			PopEnt->y = 0;
		if ((PopEnt->x + PopWidth) > iScreenWidth)
			PopEnt->x = iScreenWidth - PopWidth;
		if ((PopEnt->y + PopHeight) > iScreenHeight)
			PopEnt->y = iScreenHeight - PopHeight;

	/*	if (PopBuff.size() > 1) tbh im kinda retarded
		{
			for (int ti = 0; ti < PopBuff.size(); ti++)
			{
				if (ti == i)
					continue;

				PopUpStc PopTemp = PopBuff.at(ti);

				int PopWidthTemp, PopHeightTemp;
				PopHeightTemp = 65;
				PopWidthTemp = g_pRender->GetTextSizeRect(g::CourierNew, PopTemp.item.c_str()).right + 12;

				RECT PopRec = RECT{ PopEnt->x, PopEnt->y, PopWidth, PopHeight };
				RECT PopRecTemp = RECT{ PopTemp.x, PopTemp.y, PopWidthTemp, PopHeightTemp };

				if (contains(PopRec, PopRecTemp))
					PopEnt->x = PopEnt->x + PopWidth; // rly frickn shhid and retarded
			}
		}*/

		g_pRender->FilledRect(PopEnt->x, PopEnt->y, PopWidth, PopHeight, Color(25, 25, 25, 225));
		g_pRender->FilledRect(PopEnt->x, PopEnt->y + 20, PopWidth, PopHeight - 40, Color(33, 33, 33, 255));

		g_pRender->DrawT(PopEnt->x + 6, PopEnt->y + 2, Color(255, 255, 255, 255), g::CourierNew, false, PopEnt->name.c_str()); // title

		g_pRender->DrawT(PopEnt->x + 6, PopEnt->y + 22, Color(255, 255, 255, 255), g::CourierNew, false, PopEnt->item.c_str()); // item

		g_pRender->AutoGradientRect(PopEnt->x + 2, PopEnt->y + (PopHeight - 18), 50, 16, Color(119, 119, 119, 255));

		if (g_pRender->MouseInRegion(PopEnt->x + 2, PopEnt->y + (PopHeight - 18), 50, 16))
		{
			if (click)
			{
				g_pRender->AutoGradientRect(PopEnt->x + 2, PopEnt->y + (PopHeight - 18), 50, 16, Color(53, 53, 53, 255));
				PopBuff.erase(PopBuff.begin() + i);
				continue;
			}
		}

		g_pRender->DrawT(PopEnt->x + 13, PopEnt->y + (PopHeight - 18), Color(255, 255, 255, 255), g::CourierNew, false, "okay");
	}
}