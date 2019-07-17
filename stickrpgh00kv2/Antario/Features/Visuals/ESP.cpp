 #include "..\Features.h"

void ESP::RenderBox()
{
	g_pRender->OutlinedRect(Box.left, Box.top, Box.right, Box.bottom, boxcolor);
	g_pRender->OutlinedRect(Box.left + 1, Box.top + 1, Box.right - 2, Box.bottom - 2, Color(0, 0, 0, 240));
	g_pRender->OutlinedRect(Box.left - 1, Box.top - 1, Box.right + 2, Box.bottom + 2, Color(0, 0, 0, 240));
}

void ESP::RenderName(C_BaseEntity* pEnt, int iterator)
{
    PlayerInfo_t pInfo;
    g_pEngine->GetPlayerInfo(iterator, &pInfo);

	if (g_Menu.Config.Name == 1)
		g_pRender->DrawT(Box.left + (Box.right / 2), Box.top - 16, textcolor, font, true, pInfo.szName);
	else if (g_Menu.Config.Name == 2)
	{
		g_pRender->DrawT(Box.left + Box.right + 5, Box.top, textcolor, font, false, pInfo.szName);
		offsetY += 1;
	}
}

void ESP::RenderWeaponName(C_BaseEntity* pEnt)
{
    auto weapon = pEnt->GetActiveWeapon();

    if (!weapon)
        return;

    auto strWeaponName = weapon->GetName();

    strWeaponName.erase(0, 7);

	if (g_Menu.Config.Weapon == 1)
		g_pRender->DrawT(Box.left + (Box.right / 2), Box.top + Box.bottom, textcolor, font, true, strWeaponName.c_str());
	else if (g_Menu.Config.Weapon == 2)
	{
		g_pRender->DrawT(Box.left + Box.right + 5, Box.top + (offsetY * 11), textcolor, font, false, strWeaponName.c_str());
		offsetY += 1;
	}
}

void ESP::RenderHealth(C_BaseEntity* pEnt)
{
	if (g_Menu.Config.HealthBar)
	{
		g_pRender->FilledRect(Box.left - 6, Box.top - 1, 4, Box.bottom + 2, Color(0, 0, 0, 240));
		int pixelValue = pEnt->GetHealth() * Box.bottom / 100;
		g_pRender->FilledRect(Box.left - 5, Box.top + Box.bottom - pixelValue, 2, pixelValue, Color(0, 255, 0, 250));
	}

	if (g_Menu.Config.HealthVal == 1)
	{
		std::string Health = "HP " + std::to_string(pEnt->GetHealth());
		g_pRender->DrawT(Box.left + Box.right + 5, Box.top + (offsetY * 11), textcolor, font, false, Health.c_str());
		offsetY += 1;
	}
}

void ESP::RenderBacktrack(C_BaseEntity* pEnt, C_BaseEntity* pLocalEnt)
{
	if (LagComp::get().PlayerRecord[pEnt->EntIndex()].size() == 0)
		return;

	for (int tick = 0; tick < LagComp::get().PlayerRecord[pEnt->EntIndex()].size(); tick++)
	{
		Vector2D HeadW2S;
		if (!Utils::WorldToScreen(LagComp::get().PlayerRecord[pEnt->EntIndex()].at(tick).Head, HeadW2S))
			HeadW2S = Vector2D(99999.f, 99999.f);

		if (pLocalEnt->IsAlive() && !Autowall::get().VectortoVectorVisible(pLocalEnt->GetEyePosition(), LagComp::get().PlayerRecord[pEnt->EntIndex()].at(tick).Head, pEnt))
			g_pRender->FilledRect(HeadW2S.x - 1, HeadW2S.y + 1, 2, 2, Color(255, 0, 0, 255));
		else
			g_pRender->FilledRect(HeadW2S.x - 1, HeadW2S.y + 1, 2, 2, Color(0, 255, 0, 255));
	}
}

void ESP::RenderSkeleton(C_BaseEntity* pEnt) // the best
{
	if (LagComp::get().PlayerRecord[pEnt->EntIndex()].size() == 0)
		return;
	int size = 0;

	if (g_Menu.Config.Skeleton[0])
		size++;
	if (g_Menu.Config.Skeleton[1])
		size++;

	for (int mode = 0; mode < size; mode++)
	{
		Vector Hitbox[19];
		Vector2D Hitboxw2s[19];

		int Record = 0;

		if (mode == 0 && g_Menu.Config.Skeleton[0])
		{
			Record = LagComp::get().PlayerRecord[pEnt->EntIndex()].size() - 1;

			if (LagComp::get().ShotTick[pEnt->EntIndex()] != -1)
				Record = LagComp::get().ShotTick[pEnt->EntIndex()];
		}

		for (int hitbox = 0; hitbox < 19; hitbox++)
			Utils::WorldToScreen(pEnt->GetHitboxPosition(hitbox, LagComp::get().PlayerRecord[pEnt->EntIndex()].at(Record).Matrix), Hitboxw2s[hitbox]);

		//spine
		g_pRender->Line(Hitboxw2s[HITBOX_HEAD].x, Hitboxw2s[HITBOX_HEAD].y, Hitboxw2s[HITBOX_NECK].x, Hitboxw2s[HITBOX_NECK].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_NECK].x, Hitboxw2s[HITBOX_NECK].y, Hitboxw2s[HITBOX_UPPER_CHEST].x, Hitboxw2s[HITBOX_UPPER_CHEST].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_UPPER_CHEST].x, Hitboxw2s[HITBOX_UPPER_CHEST].y, Hitboxw2s[HITBOX_LOWER_CHEST].x, Hitboxw2s[HITBOX_LOWER_CHEST].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_LOWER_CHEST].x, Hitboxw2s[HITBOX_LOWER_CHEST].y, Hitboxw2s[HITBOX_THORAX].x, Hitboxw2s[HITBOX_THORAX].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_THORAX].x, Hitboxw2s[HITBOX_THORAX].y, Hitboxw2s[HITBOX_BELLY].x, Hitboxw2s[HITBOX_BELLY].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_BELLY].x, Hitboxw2s[HITBOX_BELLY].y, Hitboxw2s[HITBOX_PELVIS].x, Hitboxw2s[HITBOX_PELVIS].y, skelecolor);

		//right leg
		g_pRender->Line(Hitboxw2s[HITBOX_PELVIS].x, Hitboxw2s[HITBOX_PELVIS].y, Hitboxw2s[HITBOX_RIGHT_THIGH].x, Hitboxw2s[HITBOX_RIGHT_THIGH].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_RIGHT_THIGH].x, Hitboxw2s[HITBOX_RIGHT_THIGH].y, Hitboxw2s[HITBOX_RIGHT_CALF].x, Hitboxw2s[HITBOX_RIGHT_CALF].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_RIGHT_CALF].x, Hitboxw2s[HITBOX_RIGHT_CALF].y, Hitboxw2s[HITBOX_RIGHT_FOOT].x, Hitboxw2s[HITBOX_RIGHT_FOOT].y, skelecolor);

		//right arm
		g_pRender->Line(Hitboxw2s[HITBOX_NECK].x, Hitboxw2s[HITBOX_NECK].y, Hitboxw2s[HITBOX_RIGHT_UPPER_ARM].x, Hitboxw2s[HITBOX_RIGHT_UPPER_ARM].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_RIGHT_UPPER_ARM].x, Hitboxw2s[HITBOX_RIGHT_UPPER_ARM].y, Hitboxw2s[HITBOX_RIGHT_FOREARM].x, Hitboxw2s[HITBOX_RIGHT_FOREARM].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_RIGHT_FOREARM].x, Hitboxw2s[HITBOX_RIGHT_FOREARM].y, Hitboxw2s[HITBOX_RIGHT_HAND].x, Hitboxw2s[HITBOX_RIGHT_HAND].y, skelecolor);

		//left leg
		g_pRender->Line(Hitboxw2s[HITBOX_PELVIS].x, Hitboxw2s[HITBOX_PELVIS].y, Hitboxw2s[HITBOX_LEFT_THIGH].x, Hitboxw2s[HITBOX_LEFT_THIGH].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_LEFT_THIGH].x, Hitboxw2s[HITBOX_LEFT_THIGH].y, Hitboxw2s[HITBOX_LEFT_CALF].x, Hitboxw2s[HITBOX_LEFT_CALF].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_LEFT_CALF].x, Hitboxw2s[HITBOX_LEFT_CALF].y, Hitboxw2s[HITBOX_LEFT_FOOT].x, Hitboxw2s[HITBOX_LEFT_FOOT].y, skelecolor);

		//left arm
		g_pRender->Line(Hitboxw2s[HITBOX_NECK].x, Hitboxw2s[HITBOX_NECK].y, Hitboxw2s[HITBOX_LEFT_UPPER_ARM].x, Hitboxw2s[HITBOX_LEFT_UPPER_ARM].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_LEFT_UPPER_ARM].x, Hitboxw2s[HITBOX_LEFT_UPPER_ARM].y, Hitboxw2s[HITBOX_LEFT_FOREARM].x, Hitboxw2s[HITBOX_LEFT_FOREARM].y, skelecolor);
		g_pRender->Line(Hitboxw2s[HITBOX_LEFT_FOREARM].x, Hitboxw2s[HITBOX_LEFT_FOREARM].y, Hitboxw2s[HITBOX_LEFT_HAND].x, Hitboxw2s[HITBOX_LEFT_HAND].y, skelecolor);


	}
}

void ESP::RenderOffScreen(C_BaseEntity* pEnt, C_BaseEntity* pLocalEnt)
{
	if (!pLocalEnt->IsAlive())
		return;

	if (LagComp::get().PlayerRecord[pLocalEnt->EntIndex()].size() == 0)
		return;

	int Height, Width;
	g_pEngine->GetScreenSize(Width, Height);

	Vector2D OriginW2S, HeadW2S;
	Utils::WorldToScreen(pEnt->GetOrigin(), OriginW2S);
	Utils::WorldToScreen(pEnt->GetOrigin() + Vector(0, 0, pEnt->GetHitboxPosition(HITBOX_HEAD, LagComp::get().pMatrix[pEnt->EntIndex()]).z + 10), HeadW2S);

	if (g_Math.InScreen(OriginW2S.x, OriginW2S.y, Width, Height) || g_Math.InScreen(HeadW2S.x, HeadW2S.y, Width, Height))
		return;

	Vector ViewAngles, ViewForward;

	if (GetKeyState(g_Menu.Config.ThirdPersonKey))
	{
		ViewForward = pLocalEnt->GetOrigin();
		ViewForward.z += abs(pLocalEnt->GetOrigin().z - pLocalEnt->GetHitboxPosition(HITBOX_HEAD, LagComp::get().pMatrix[pLocalEnt->EntIndex()]).z);
		ViewForward.z -= 20;
	}
	else
	{
		g_pEngine->GetViewAngles(ViewAngles);

		g_Math.AngleVectors(ViewAngles, &ViewForward);
		g_Math.NormalizeAngles(ViewForward);

		ViewForward = pLocalEnt->GetEyePosition() + (ViewForward * 50);
		ViewForward.z -= 10;
	}

	Vector Angle = g_Math.CalcAngle(ViewForward, pEnt->GetOrigin());
	Angle.y += 90;
	Vector MidTri = g_Math.ProjectPoint(ViewForward, Angle.y, (GetKeyState(g_Menu.Config.ThirdPersonKey) ? 30 : 25)), SidTri1 = g_Math.ProjectPoint(ViewForward, Angle.y + (GetKeyState(g_Menu.Config.ThirdPersonKey) ? 6 : 5), (GetKeyState(g_Menu.Config.ThirdPersonKey) ? 26 : 24)), SidTri2 = g_Math.ProjectPoint(ViewForward, Angle.y - (GetKeyState(g_Menu.Config.ThirdPersonKey) ? 6 : 5), (GetKeyState(g_Menu.Config.ThirdPersonKey) ? 26 : 24));
	Vector2D MidTriW2S, SidTri1W2S, SidTri2W2S;

	Utils::WorldToScreen(MidTri, MidTriW2S);
	Utils::WorldToScreen(SidTri1, SidTri1W2S);
	Utils::WorldToScreen(SidTri2, SidTri2W2S);

	static Vertex_t TriAng[3];

	TriAng[0].mPosition = MidTriW2S;
	TriAng[1].mPosition = SidTri1W2S;
	TriAng[2].mPosition = SidTri2W2S;

	g_pRender->Polygon(3, TriAng, offscrncolor);
}

void ESP::RenderHitpoints(int index)
{
	if (RageBot::get().Hitboxes[index].size() == 0)
		return;

	for (int i = 0; i < RageBot::get().Hitboxes[index].size(); i++)
	{
		Vector2D Hitboxw2s;
		Utils::WorldToScreen(RageBot::get().Hitboxes[index].at(i).Pos, Hitboxw2s);
		g_pRender->FilledRect(Hitboxw2s.x - 1, Hitboxw2s.y + 1, 2, 2, Color(255, 255, 0, 255));
	}
}

void ESP::BoundBox(C_BaseEntity* pEnt)
{
	Box.bottom = 0;
	Box.top = 0;
	Box.left = 0;
	Box.right = 0;

	if (!LagComp::get().pMatrix[pEnt->EntIndex()])
		return;

	Vector2D w2sBottom, w2sTop;

	Utils::WorldToScreen(pEnt->GetOrigin() - Vector(0, 0, 8), w2sBottom); 
	Utils::WorldToScreen(pEnt->GetHitboxPosition(0, LagComp::get().pMatrix[pEnt->EntIndex()]) + Vector(0, 0, 10), w2sTop);
	
	int Middle = w2sBottom.y - w2sTop.y;
	int Width = Middle / 3.f;

	Box.bottom = Middle;
	Box.top = w2sTop.y;
	Box.left = w2sBottom.x - Width;
	Box.right = Width * 2;
}

void ESP::Render()
{
    if (!g_pEngine->IsInGame()|| !g_pEngine->IsConnected() || !g_Menu.Config.Esp)
        return;

	C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (!pLocalEntity)
		return;

	boxcolor = Color(g_Menu.Config.BoxColor.red, g_Menu.Config.BoxColor.green, g_Menu.Config.BoxColor.blue, 240);
	textcolor = Color(g_Menu.Config.FontColor.red, g_Menu.Config.FontColor.green, g_Menu.Config.FontColor.blue, 240);
	skelecolor = Color(g_Menu.Config.SkeletonColor.red, g_Menu.Config.SkeletonColor.green, g_Menu.Config.SkeletonColor.blue, 240);
	offscrncolor = Color(g_Menu.Config.OffScreenColor.red, g_Menu.Config.OffScreenColor.green, g_Menu.Config.OffScreenColor.blue, 230);

	(g_Menu.Config.Font == 0) ? font = g::CourierNew : font = g::Tahoma;

	for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
    {
        C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

        if (!pPlayerEntity
            || !pPlayerEntity->IsAlive()
            || pPlayerEntity->IsDormant()
            || pPlayerEntity == pLocalEntity
			|| pPlayerEntity->GetTeam() == pLocalEntity->GetTeam())
            continue;

		offsetY = 0;

		BoundBox(pPlayerEntity);

		if (Box.bottom == 0)
			continue;

		if (g_Menu.Config.Skeleton[0] || g_Menu.Config.Skeleton[1])
			RenderSkeleton(pPlayerEntity);

        if (g_Menu.Config.Box)
            RenderBox();

        if (g_Menu.Config.Name > 0)
            RenderName(pPlayerEntity, i);

        if (g_Menu.Config.Weapon)
            RenderWeaponName(pPlayerEntity);

		if (g_Menu.Config.HealthBar || g_Menu.Config.HealthVal > 0)
			RenderHealth(pPlayerEntity);

		if (g_Menu.Config.VisualBacktrack)
			RenderBacktrack(pPlayerEntity, pLocalEntity);

		if (g_Menu.Config.HitPoints)
			RenderHitpoints(i);

		if (g_Menu.Config.OffScreen)
			RenderOffScreen(pPlayerEntity, pLocalEntity);
    }
}