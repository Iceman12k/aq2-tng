
//
// Reki
// HD Mode
// Segmented md3 player models
//

#include "g_local.h"
#include "md3.h"

#ifdef AQTION_EXTENSION
//// LEGS ////
static void HDMode_PlayerL_Stand(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	ent->s.frame = max(ent->s.frame + 1, 165); if (ent->s.frame >= 174) ent->s.frame = 165;
}

static void HDMode_PlayerL_Walk(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;
	
	ent->s.frame = max(ent->s.frame + 1, 99); if (ent->s.frame >= 110) ent->s.frame = 99;
}

static void HDMode_PlayerL_Run(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	ent->s.frame = max(ent->s.frame + 1, 111); if (ent->s.frame >= 119) ent->s.frame = 111;
}

static void HDMode_PlayerL_Backrun(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	ent->s.frame = max(ent->s.frame + 1, 121); if (ent->s.frame >= 129) ent->s.frame = 121;
}

static void HDMode_PlayerL_Jump(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	if (ent->s.frame > 144 || ent->s.frame < 140)
		ent->s.frame = 140;
	else
		ent->s.frame = min(144, ent->s.frame + 1);
}

static void HDMode_PlayerL_Land(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	if (ent->s.frame > 150 || ent->s.frame < 145)
		ent->s.frame = 145;
	else
		ent->s.frame = min(150, ent->s.frame + 1);
}

static void HDMode_PlayerL_Crouch(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	if (ent->s.frame > 185 || ent->s.frame < 173)
	{
		if (ent->s.frame >= 91 && ent->s.frame <= 98) // if we're transferring from the crouch walk anim
			ent->s.frame = 175;
		else
			ent->s.frame = 173;
	}
	else
	{
		ent->s.frame++;
		if (ent->s.frame == 184)
			ent->s.frame = 175;
	}
}

static void HDMode_PlayerL_CrouchWalk(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	ent->s.frame = max(ent->s.frame + 1, 91); if (ent->s.frame >= 98) ent->s.frame = 91;
}


// idle / normal
void HDMode_PlayerL_State(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	ent->think = HDMode_PlayerL_State;
	ent->nextthink = level.framenum + FRAMEDIV;

	vec3_t vel_to_use; VectorCopy(player->velocity, vel_to_use); vel_to_use[2] = 0;
	float spd = VectorLength(vel_to_use);

	if (!(player->client->ps.pmove.pm_flags & PMF_ON_GROUND))
	{
		if (player->velocity[2] < -110)
			HDMode_PlayerL_Land(ent);
		else
			HDMode_PlayerL_Jump(ent);
	}
	else if (player->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		if (spd > 30)
		{
			HDMode_PlayerL_CrouchWalk(ent);
		}
		else
		{
			HDMode_PlayerL_Crouch(ent);
		}
	}
	else
	{
		if (spd > 160)
		{
			vec3_t forward, right;
			AngleVectors(ent->s.angles, forward, right, NULL);

			if (DotProduct(player->velocity, forward) < -30)
				HDMode_PlayerL_Backrun(ent);
			else
				HDMode_PlayerL_Run(ent);
		}
		else if (spd > 30)
		{
			HDMode_PlayerL_Walk(ent);
		}
		else
		{
			HDMode_PlayerL_Stand(ent);
		}
	}

}

// death anim
void HDMode_PlayerL_Death(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	if (player->deadflag == DEAD_NO)
	{
		HDMode_PlayerL_State(ent);
		return;
	}

	if (ent->s.frame < 31 || ent->s.frame > 59)
		ent->s.frame = 31;
	else
		ent->s.frame = min(59, ent->s.frame + 2);

	ent->think = HDMode_PlayerL_Death;
	ent->nextthink = level.framenum + FRAMEDIV;
}

//// TORSO ////
// idle / normal
void HDMode_PlayerT_State(edict_t *ent)
{
	ent->s.frame += 2;
	if (ent->s.frame < 130 || ent->s.frame >= 136)
		ent->s.frame = 130;

	ent->think = HDMode_PlayerT_State;
	ent->nextthink = level.framenum + FRAMEDIV;
}

// death anim
void HDMode_PlayerT_Death(edict_t *ent)
{
	edict_t *player = ent->owner;
	if (!player)
		return;

	if (player->deadflag == DEAD_NO)
	{
		HDMode_PlayerT_State(ent);
		return;
	}

	if (ent->s.frame < 31 || ent->s.frame > 59)
		ent->s.frame = 31;
	else
		ent->s.frame = min(59, ent->s.frame + 1);

	ent->think = HDMode_PlayerT_Death;
	ent->nextthink = level.framenum + FRAMEDIV;
}

static void HDMode_HandlePlayer(edict_t *ent)
{
	if (!ent->hdm_head)
	{
		ent->hdm_head = G_Spawn();
		ent->hdm_head->hdm_flags = HDMODE_ASSET;
		ent->hdm_head->s.modelindex = gi.modelindex("models/grunt/head.md2");
		ent->hdm_head->s.frame = 0;
		ent->hdm_head->dimension_visible = DIMENSION_HDMODE;
		ent->hdm_head->owner = ent;
	}

	if (!ent->hdm_torso)
	{
		ent->hdm_torso = G_Spawn();
		ent->hdm_torso->hdm_flags = HDMODE_ASSET;
		ent->hdm_torso->s.modelindex = gi.modelindex("models/grunt/upper.md2");
		ent->hdm_torso->s.frame = 170;
		ent->hdm_torso->dimension_visible = DIMENSION_HDMODE;
		ent->hdm_torso->owner = ent;
		HDMode_PlayerT_State(ent->hdm_torso);
	}

	if (!ent->hdm_legs)
	{
		ent->hdm_legs = G_Spawn();
		ent->hdm_legs->hdm_flags = HDMODE_ASSET;
		ent->hdm_legs->s.modelindex = gi.modelindex("models/grunt/lower.md2");
		ent->hdm_legs->s.frame = 116;
		ent->hdm_legs->dimension_visible = DIMENSION_HDMODE;
		ent->hdm_legs->owner = ent;
		HDMode_PlayerL_State(ent->hdm_legs);
	}

	float lean_scale;
	vec3_t forward, right;
	AngleVectors(ent->s.angles, forward, right, NULL);

	if (ent->client->ps.pmove.pm_flags & PMF_ON_GROUND)
		lean_scale = 1;
	else
		lean_scale = 2;

	VectorCopy(ent->s.origin, ent->hdm_legs->s.origin);
	VectorCopy(ent->s.old_origin, ent->hdm_legs->s.old_origin);

	//if (VectorLength(ent->velocity) > 120)
	//	ent->hdm_legs->s.angles[1] = vectoyaw(ent->velocity);

	vec3_t vel_to_use; VectorCopy(ent->velocity, vel_to_use); vel_to_use[2] = 0;

	// determine leg dir
	if (VectorLength(vel_to_use) > 30)
	{
		ent->hdm_legs->angle = vectoyaw(vel_to_use);
	}

	float angdiff = anglediff(ent->s.angles[1], ent->hdm_legs->angle);
	if (angdiff > 120 || angdiff < -120)
	{
		ent->hdm_legs->angle = vectoyaw(vel_to_use) + 180;
	}
	else if (angdiff > 110)
	{
		ent->hdm_legs->angle = ent->s.angles[1] + 45;
	}
	else if (angdiff > 45)
	{
		if (VectorLength(vel_to_use) > 30)
			ent->hdm_legs->angle = ent->s.angles[1] - 45;
		else
			ent->hdm_legs->angle = ent->s.angles[1] - 15;
	}
	else if (angdiff < -110)
	{
		ent->hdm_legs->angle = ent->s.angles[1] - 45;
	}
	else if (angdiff < -45)
	{
		if (VectorLength(vel_to_use) > 30)
			ent->hdm_legs->angle = ent->s.angles[1] + 45;
		else
			ent->hdm_legs->angle = ent->s.angles[1] - 15;
	}
	
	angdiff = anglediff(ent->hdm_legs->s.angles[1], ent->hdm_legs->angle);
	float absdiff = fabs(angdiff);
	float spd = 50;
	if (absdiff > 40)
		spd = 600;
	else if (absdiff > 20)
		spd = 400;
	else if (absdiff > 5)
		spd = 200;
	spd = min(spd * FRAMETIME, fabs(angdiff));
	if (angdiff < 0)
		ent->hdm_legs->s.angles[1] = ent->hdm_legs->s.angles[1] + spd;
	else if (angdiff > 0)
		ent->hdm_legs->s.angles[1] = ent->hdm_legs->s.angles[1] - spd;

	angdiff = anglediff(ent->s.angles[1], ent->hdm_legs->s.angles[1]);
	if (angdiff > 60)
		ent->hdm_legs->s.angles[1] = ent->s.angles[1] - 60;
	else if (angdiff < -60)
		ent->hdm_legs->s.angles[1] = ent->s.angles[1] + 60;
	//



	ent->hdm_legs->s.angles[0] = DotProduct(vel_to_use, forward) * 0.025 * lean_scale;
	ent->hdm_legs->s.angles[2] = DotProduct(vel_to_use, right) * 0.03 * lean_scale;

	MD3_AttachEdict(MD3_LoadModel("models/grunt/lower.md3"), "tag_torso", ent->hdm_legs, ent->hdm_torso);

	if (ent->deadflag == DEAD_NO)
	{
		ent->hdm_torso->s.angles[0] = (ent->hdm_legs->s.angles[0] * 0.1) + (ent->s.angles[0] * 1.1);
		ent->hdm_torso->s.angles[1] = ent->s.angles[1];
		ent->hdm_torso->s.angles[2] = ent->hdm_legs->s.angles[2] * 0.3;
		ent->hdm_torso->s.angles[0] += ent->client->kick_angles[0] * 1.5;
		ent->hdm_torso->s.angles[0] += ent->client->v_dmg_pitch * 0.4;
		ent->hdm_torso->s.angles[2] += ent->client->v_dmg_roll * 0.2;
	}

	MD3_AttachEdict(MD3_LoadModel("models/grunt/upper.md3"), "tag_head", ent->hdm_torso, ent->hdm_head);

	if (ent->deadflag == DEAD_NO)
	{
		ent->hdm_head->s.angles[0] += (ent->s.angles[0] * 0.8);
		ent->hdm_head->s.angles[0] -= ent->client->kick_angles[0] * 1.5;
		ent->hdm_head->s.angles[0] += ent->client->v_dmg_pitch;
		ent->hdm_head->s.angles[2] += ent->client->v_dmg_roll;
	}

	gi.linkentity(ent->hdm_head);
	gi.linkentity(ent->hdm_torso);
	gi.linkentity(ent->hdm_legs);
}

void HDMode_HandleReplacements(void)
{
	edict_t *ent;
	int i;

	ent = &g_edicts[0];
	for (i = 0; i < globals.num_edicts; i++, ent++)
	{
		if (!ent->inuse)
			continue;

		if (ent->hdm_flags & HDMODE_ASSET)
			continue;

		if (ent->hdm_flags & HDMODE_PLAYER)
			HDMode_HandlePlayer(ent);
	}
}







#endif