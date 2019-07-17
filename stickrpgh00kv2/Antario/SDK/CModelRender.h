#pragma once

typedef unsigned short ModelInstanceHandle_t;
class IMaterial;
class IMatRenderContext;
class IClientRenderable;
struct studiohdr_t;
struct studiohwdata_t;

struct cache_user_t;
struct brushdata_t;
struct studiodata_t;
struct spritedata_t;

enum
{
	MODEL_INSTANCE_INVALID = (ModelInstanceHandle_t)~0
};

#define STUDIO_NONE						0x00000000
#define STUDIO_RENDER					0x00000001
#define STUDIO_VIEWXFORMATTACHMENTS		0x00000002
#define STUDIO_DRAWTRANSLUCENTSUBMODELS 0x00000004
#define STUDIO_TWOPASS					0x00000008
#define STUDIO_STATIC_LIGHTING			0x00000010
#define STUDIO_WIREFRAME				0x00000020
#define STUDIO_ITEM_BLINK				0x00000040
#define STUDIO_NOSHADOWS				0x00000080
#define STUDIO_WIREFRAME_VCOLLIDE		0x00000100

// Not a studio flag, but used to flag when we want studio stats
#define STUDIO_GENERATE_STATS			0x01000000

// Not a studio flag, but used to flag model as using shadow depth material override
#define STUDIO_SSAODEPTHTEXTURE			0x08000000

// Not a studio flag, but used to flag model as using shadow depth material override
#define STUDIO_SHADOWDEPTHTEXTURE		0x40000000

// Not a studio flag, but used to flag model as a non-sorting brush model
#define STUDIO_TRANSPARENCY				0x80000000


enum modtype_t
{
	mod_bad = 0,
	mod_brush,
	mod_sprite,
	mod_studio
};

struct model_t
{
	char        name[255];

	// UNDONE: Choose between these two methods
	//int            registration_sequence;
	int            needload;        // mark loaded/not loaded

	modtype_t    type;
	int            flags;

	//
	// volume occupied by the model graphics
	//        
	Vector mins, maxs;
	float        radius;

	int            extradatasize;
	cache_user_t*    cache;

	union
	{
		brushdata_t*      brush;
		studiodata_t*    studio;
		spritedata_t*   sprite;
	};
};



#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
DECLARE_POINTER_HANDLE(StudioDecalHandle_t);

enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,

	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,

	STUDIORENDER_DRAW_ACCURATETIME = 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,

	STUDIORENDER_DRAW_WIREFRAME = 0x80,

	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,

	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,

	STUDIORENDER_NO_SKIN = 0x400,

	STUDIORENDER_SKIP_DECALS = 0x800
};


enum DrawFlags_t
{
	DF_RENDER_REFRACTION = 0x1,
	DF_RENDER_REFLECTION = 0x2,

	DF_CLIP_Z = 0x4,
	DF_CLIP_BELOW = 0x8,

	DF_RENDER_UNDERWATER = 0x10,
	DF_RENDER_ABOVEWATER = 0x20,
	DF_RENDER_WATER = 0x40,

	DF_SSAO_DEPTH_PASS = 0x100,
	DF_WATERHEIGHT = 0x200,
	DF_DRAW_SSAO = 0x400,
	DF_DRAWSKYBOX = 0x800,

	DF_FUDGE_UP = 0x1000,

	DF_DRAW_ENTITITES = 0x2000,
	DF_UNUSED3 = 0x4000,

	DF_UNUSED4 = 0x8000,

	DF_UNUSED5 = 0x10000,
	DF_SAVEGAMESCREENSHOT = 0x20000,
	DF_CLIP_SKYBOX = 0x40000,

	DF_SHADOW_DEPTH_MAP = 0x100000	// Currently rendering a shadow depth map
};

enum
{
	ADDDECAL_TO_ALL_LODS = -1
};


struct DrawModelState_t
{
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pStudioHWData;
	IClientRenderable* m_pRenderable;
	const matrix3x4_t* m_pModelToWorld;
	StudioDecalHandle_t m_decals;
	int m_drawFlags;
	int m_lod;
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
};

struct ModelRenderInfo_t
{
	Vector origin;
	Vector angles;
	char pad[0x4];
	IClientRenderable* pRenderable;
	const model_t* pModel;
	const matrix3x4_t* pModelToWorld;
	const matrix3x4_t* pLightingOffset;
	const Vector* pLightingOrigin;
	int flags;
	int index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;

	ModelRenderInfo_t()
	{
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};

class CModelRender
{
public:
	void DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		typedef void(__thiscall* Fn)(void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
		Utils::GetVFunc<Fn>(this, 21)(this, ctx, state, pInfo, pCustomBoneToWorld);
	}

	void ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL)
	{
		typedef void(__thiscall* Fn)(void*, IMaterial*, OverrideType_t, int);
		Utils::GetVFunc<Fn>(this, 1)(this, newMaterial, nOverrideType, NULL);
	}
};

extern CModelRender* g_pModelRender;