
#include "q_shared.h"
#include "g_local.h"
#include "md3.h"


static void LittleBlock(void *out, const void *in, size_t size)
{
	memcpy(out, in, size);
#if USE_BIG_ENDIAN
	for (int i = 0; i < size / 4; i++)
		((uint32_t *)out)[i] = LittleLong(((uint32_t *)out)[i]);
#endif
}

int			num_md3models;
md3model_t	*md3models[128];

void MD3_GetTagLocation(md3model_t *model, int frame, char *tagname, vec3_t o_out, vec3_t axis[3])
{
	md3tagframe_t *fr = &model->tagframes[bound(0, frame, model->num_frames)];
	for (int i = 0; i < model->num_tags; i++)
	{
		dmd3tag_t tag = fr->tags[i];
		if (Q_strnicmp(tagname, tag.name, MD3_MAX_PATH))
			continue;
		//o_out[0] = (float)tag.origin[0];
		//o_out[1] = (float)tag.origin[1];
		//o_out[2] = (float)tag.origin[2];

		VectorMA(o_out, tag.origin[0], axis[0], o_out);
		VectorMA(o_out, -tag.origin[1], axis[1], o_out);
		VectorMA(o_out, tag.origin[2], axis[2], o_out);
		//for (int i = 0; i < 3; i++) {
		//	VectorMA(o_out, tag.origin[i], axis[i], o_out);
		//}
		
		vec3_t e_axis[3];
		VectorCopy(axis[0], e_axis[0]);
		VectorCopy(axis[1], e_axis[1]);
		VectorCopy(axis[2], e_axis[2]);
		//VectorRotate(tag.axis[0], a_out, forward); // this is probably wildly inefficient, idk
		//VectorRotate(tag.axis[2], a_out, up);
		//vectoangles2(axis, tag.axis[0], tag.axis[2], false);
		MatrixMultiply(tag.axis, e_axis, axis);

		//for (int i = 0; i < 3; i++) {
			//VectorMA(o_out, tag.origin[i], axis[i], o_out);
		//}
		return;
	}
}

void MD3_AttachEdict(int md3, const char *name, edict_t *to, edict_t *ent)
{
	vec3_t tag_o;
	vec3_t tag_axis[3];
	VectorClear(tag_o);

	AngleVectors(to->s.angles, tag_axis[0], tag_axis[1], tag_axis[2]);
	MD3_GetTagLocation(md3models[md3], to->s.frame, name, tag_o, tag_axis);
	
	vec3_t t_axis[3];
	vec3_t e_axis[3];
	vec3_t o_axis[3];
	//AngleVectors(tag_a, t_axis[0], t_axis[1], t_axis[2]);
	//AngleVectors(to->s.angles, e_axis[0], e_axis[1], e_axis[2]);
	//MatrixMultiply(t_axis, e_axis, o_axis);
	vectoangles2(ent->s.angles, tag_axis[0], tag_axis[2], false);

	//VectorRotate(tag_o, to->s.angles, tag_o);
	VectorAdd(to->s.origin, tag_o, ent->s.origin);
	//VectorClear(ent->s.angles);
	//VectorAdd(to->s.angles, tag_a, ent->s.angles);
}

int MD3_LoadModel(char *filename)
{
	char	name[MAX_OSPATH];
	cvar_t *game;

	game = gi.cvar("game", "", 0);
	sprintf(name, "%s/%s", !*game->string ? GAMEVERSION : game->string, filename);

	for (int i = 0; i < num_md3models; i++)
	{
		md3model_t *model = md3models[i];
		if (Q_strnicmp(filename, model->name, MAX_QPATH))
			continue;
		return i;
	}
	
	FILE *fp = fopen(name, "rb");
	if (!fp)
	{
		Com_Printf("%s: model not found\n", __func__);
		return 0;
	}

	dmd3header_t    header;
	fread(&header, sizeof(header), 1, fp);
	
	md3model_t *model = gi.TagMalloc(sizeof(md3model_t), TAG_GAME);
	md3models[num_md3models] = model;
	num_md3models++;

	Q_strncpyz(model->name, filename, MAX_QPATH);
	model->num_frames = header.num_frames;
	model->num_tags = header.num_tags;
	model->tagframes = gi.TagMalloc(sizeof(md3tagframe_t) * header.num_frames, TAG_GAME);

	fseek(fp, header.ofs_tags, SEEK_SET);
	for (int i = 0; i < header.num_frames; i++)
	{
		for (int j = 0; j < header.num_tags; j++)
		{
			md3tagframe_t *fr = &model->tagframes[i];
			dmd3tag_t *tag = &fr->tags[j];
			fread(tag, sizeof(dmd3tag_t), 1, fp);
		}
	}

	fseek(fp, header.ofs_meshes, SEEK_SET);
	dmd3mesh_t mesh;
	fread(&mesh, sizeof(dmd3mesh_t), 1, fp);

	Com_Printf("%s: SHADER NUM = %i\n", __func__, mesh.num_skins);

	fseek(fp, header.ofs_meshes + mesh.ofs_skins, SEEK_SET);
	dmd3skin_t shader;
	fread(&shader, sizeof(dmd3skin_t), 1, fp);

	Com_Printf("%s: SHADER NAME = %s\n", __func__, shader.name);

	return num_md3models - 1;
}