#include <common.h>
#include <static_model.h>
#include <renderer.h>
#include <color_shading_3d_vertex.h>
#include <color_shading_3d_pixel.h>
#include <stdio.h>



ID3D11VertexShader* sm_vs=NULL;
ID3D11PixelShader* sm_ps=NULL;
ID3D11InputLayout* sm_vl=NULL;



AssetModel load_model(RawAssetModel m){
	AssetModel o=malloc(sizeof(struct _ASSET_MODEL));
	o->u32=((m.f>>15)==1?true:false);
	D3D11_BUFFER_DESC bd={
		(uint32_t)(m.ill*(o->u32==true?sizeof(uint32_t):sizeof(uint16_t))),
		D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_INDEX_BUFFER,
		0,
		0,
		0
	};
	D3D11_SUBRESOURCE_DATA dt={
		(o->u32==true?(void*)m.il._32:(void*)m.il._16),
		0,
		0
	};
	HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&(o->ib));
	bd.ByteWidth=(uint32_t)(m.vll*sizeof(float));
	bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	dt.pSysMem=m.vl;
	hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&(o->vb));
	o->ill=m.ill;
	return o;
}



void draw_model(AssetModel m){
	if (sm_vs==NULL||sm_vl==NULL||sm_ps==NULL){
		assert(sm_vs==NULL);
		assert(sm_vl==NULL);
		assert(sm_ps==NULL);
		D3D11_INPUT_ELEMENT_DESC sm_il[]={
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			}
		};
		sm_vs=load_vertex_shader(g_color_shading_3d_vs,sizeof(g_color_shading_3d_vs),sm_il,sizeof(sm_il)/sizeof(D3D11_INPUT_ELEMENT_DESC),&sm_vl);
		sm_ps=load_pixel_shader(g_color_shading_3d_ps,sizeof(g_color_shading_3d_ps));
	}
	ID3D11DeviceContext_IASetInputLayout(renderer_d3_dc,sm_vl);
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,sm_vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,sm_ps,NULL,0);
	unsigned int st=9*sizeof(float);
	unsigned int off=0;
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&(m->vb),&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,m->ib,(m->u32==true?DXGI_FORMAT_R32_UINT:DXGI_FORMAT_R16_UINT),0);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,m->ill,0,0);
}
