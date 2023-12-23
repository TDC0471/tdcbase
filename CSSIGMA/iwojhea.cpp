
void RunAimbot(c_usercmd* cmd) {
	Vector bestAngle{};

	int idx = engine->GetLocalPlayer();
	if (idx == 0) idx++;
	localPlayer = reinterpret_cast<CPlayer*>(ent_list->GetClientEntity(idx));

	if (!localPlayer)
		return;

	if (localPlayer->GetHealth() <= 0)
		return;





	Vector localPos = localPlayer->GetAbsOrigin();
	Vector localHeadPos = localPlayer->CameraPos();
	auto viewModel = localPlayer->m_hViewModel();
	auto viewidx = viewModel & 0xFFFF;

	auto viewModelEnt = reinterpret_cast<CViewModel*>(ent_list->GetClientEntity(viewidx));

	if (!viewModelEnt) return;

	auto weap = viewModelEnt->GetWeapon();
	auto weap_index = weap & 0xFFFF;

	if (weap_index < interfaces::ent_list->GetHighestEntityIndex()) {

		auto weap_ent = reinterpret_cast<CWeaponX*>(ent_list->GetClientEntity(weap_index));

		if (weap_ent) {
			*(float*)(weap_ent + 0x12F0) = 0;
		}

	}


	float best_dist = FLT_MAX;
	for (int i = 1; i < ent_list->GetHighestEntityIndex(); i++) {
		auto entity = reinterpret_cast<CPlayer*>(ent_list->GetClientEntity(i));

		if (!entity)
			continue;


		if (!entity->GetName()) continue;
		if (strcmp(entity->GetName(), "") == 0) continue;
		// if name is weaponx then continue
		if (strcmp(entity->GetName(), "weaponx") == 0) continue;
		if (entity == localPlayer) continue;
		if (aimbot::g_Aimbot_AI && entity->IsAI()) continue;
		if (!aimbot::g_AimbotIncludeTeam) {
			if (entity->GetTeam() == localPlayer->GetTeam()) continue;
		}

		if (entity->GetHealth() <= 0) continue;

		Vector entityPos = entity->GetVecOrigin();

		Vector headPos;
		CMatrix3x4 bones[256];

		if (!entity->SetupBones(bones, 256, 0x7FF00, interfaces::globals->curtime)) continue;
		//auto bone = GetHeadBone(entity);
		auto bone = 11;
		if (bone == -1) continue;

		if (!bones->get_bone(headPos, bone));

		if (entity->IsTitan()) {
			if (!bones->get_bone(headPos, 14)) continue;
		}


		auto lastVisTime = entity->LastVisibleTime();
		auto curTime = interfaces::globals->frametime;
		if (lastVisTime < 0) continue;
		auto lastVisDelta = curTime - lastVisTime;

		if (lastVisDelta > 0.1f) continue;

		// Convert the head position to view angle 
		Vector aimPunch = localPlayer->AimPunch();
		Vector delta = { headPos.x - localHeadPos.x, headPos.y - localHeadPos.y, headPos.z - localHeadPos.z };

		Vector ang;
		if (aimbot::g_Silent) {
			ang = cmd->attackangles;
		}
		else {
			ang = cmd->viewangles;
		}
		float dist = localHeadPos.distance(headPos);
		std::cout << "Dist: " << dist << std::endl;
		Vector enemyAngle{
			delta.ToAngle() - (ang)
		};

		auto out = world2screen(viewMatrix2, enemyAngle);

		auto drawlist = ImGui::GetBackgroundDrawList();

		if (out != Vector()) {
			drawlist->AddCircleFilled(ImVec2(out.x, out.y), 5, ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), 12);
		}


		auto expPos = headPos + entity->GetVelocity();

		const auto lerp = [&](const Vector& a, const Vector& b, const float t) {
			return a + (b - a) * t;
		};


		// make sure the target is in the fov range set by the user
		float fov = std::hypotf(enemyAngle.x, enemyAngle.y);
		auto weapon_idx = localPlayer->GetWeapon() & 0xFFFF;
		if (weapon_idx < interfaces::ent_list->GetHighestEntityIndex()) {
			auto weapon = reinterpret_cast<CWeaponX*>(ent_list->GetClientEntity(weapon_idx));

			// find the bullet speed of the weapon
			auto bulletSpeed = weapon->bulletSpeed();
			// find the bullet gravity of the weapon
			auto bulletGravity = weapon->bulletGravity();

		}




		const auto fov_scale = [&]() {
			auto weapon_idx = localPlayer->GetWeapon() & 0xFFFF;

			if (localPlayer->IsZoom()) {
				if (weapon_idx < interfaces::ent_list->GetHighestEntityIndex()) {
					auto weapon = reinterpret_cast<CWeaponX*>(ent_list->GetClientEntity(weapon_idx));
					if (weapon->zoomFov() != 0.0f && weapon->zoomFov() != 1.0f)
						return weapon->zoomFov() / 90.0f;
				}
			}
			return 1.0f;
		};
		fov *= fov_scale();
		float fov_s = aimbot::g_Aimbot_Fov * fov_scale();

		if (dist < best_dist && fov <= fov_s) {
			best_dist = dist;
			bestAngle = enemyAngle;
		}

	}
	if (aimbot::g_Silent) {
		cmd->attackangles += bestAngle;
		cmd->attackangles.Clamp();

	}

	if ((cmd->buttons & in_zoom))
	{
		if (!aimbot::g_Silent) {
			//bestAngle.Scale(aimbot::g_Aimbot_Smooth);
			cmd->viewangles += bestAngle;

			cmd->viewangles.Clamp();

		}

		if (aimbot::g_Aimbot_AutoShoot) {
			cmd->buttons |= IN_ATTACK;
		}
	}
	return;

}