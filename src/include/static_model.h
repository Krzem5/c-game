#ifndef _STATIC_MODEL_H__
#define _STATIC_MODEL_H__
#include <common.h>



typedef struct _RAW_ASSET_MODEL RawAssetModel;
typedef struct _ASSET_MODEL* AssetModel;



struct _RAW_ASSET_MODEL{
	uint16_t f;
	uint32_t vll;
	uint32_t ill;
	const float* vl;
	union _RAW_ASSET_MODEL_INDECIES{
		const uint16_t* _16;
		const uint32_t* _32;
	} il;
};



struct _ASSET_MODEL{
	bool u32;
	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	uint32_t ill;
};



AssetModel load_model(RawAssetModel m);



void draw_model(AssetModel m);



#endif
