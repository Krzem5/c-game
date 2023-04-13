#include <common.h>
#include <engine.h>
#include <renderer.h>
#include <static_model.h>
#include <models.h>
#include <stdio.h>



/********************/
typedef struct _CAMERA* Camera;



struct _CAMERA{
	float x;
	float y;
	float z;
	float rx;
	float ry;
	float rz;
	float ms;
	float rs;
	bool lock;
	bool enabled;
	bool _fs;
	float _dx;
	float _dy;
	float _dz;
	float _drx;
	float _dry;
	float _drz;
};



Camera create_camera(float ms,float rs,float x,float y,float z,float rx,float ry,float rz,float zm){
	Camera o=malloc(sizeof(struct _CAMERA));
	o->x=x;
	o->y=y;
	o->z=z;
	o->rx=(rx-90)*PI_DIV_180;
	o->ry=ry*PI_DIV_180;
	o->rz=rz*PI_DIV_180;
	o->ms=ms;
	o->rs=rs;
	o->lock=false;
	o->enabled=false;
	o->_fs=false;
	o->_dx=x;
	o->_dy=y;
	o->_dz=z;
	o->_drx=rx;
	o->_dry=ry;
	o->_drz=rz;
	return o;
}



Matrix update_camera(Camera c,float dt){
	if (c->_fs==false&&c->lock==true){
		SetCursorPos(renderer_wx+renderer_ww/2,renderer_wy+renderer_wh/2);
		c->_fs=true;
		return NULL;
	}
	POINT mp;
	GetCursorPos(&mp);
	if (c->enabled==true){
		c->_drx+=((int32_t)(renderer_wy+renderer_wh/2-mp.y))*c->rs*dt;
		c->_dry+=((int32_t)(renderer_wx+renderer_ww/2-mp.x))*c->rs*dt;
		if (c->_drx>179.999f){
			c->_drx=179.999f;
		}
		if (c->_drx<0){
			c->_drx=0;
		}
		float drxr=(c->_drx-90)*PI_DIV_180;
		float dryr=c->_dry*PI_DIV_180;
		float drzr=c->_drz*PI_DIV_180;
		float dx=cosf(dryr)*c->ms*dt;
		float dz=sinf(dryr)*c->ms*dt;
		if (is_pressed(0x57)){
			c->_dx-=dx;
			c->_dz-=dz;
		}
		if (is_pressed(0x53)){
			c->_dx+=dx;
			c->_dz+=dz;
		}
		if (is_pressed(0x41)){
			c->_dx+=dz;
			c->_dz-=dx;
		}
		if (is_pressed(0x44)){
			c->_dx-=dz;
			c->_dz+=dx;
		}
		if (is_pressed(VK_SPACE)){
			c->_dy+=c->ms*dt;
		}
		if (is_pressed(VK_LSHIFT)){
			c->_dy-=c->ms*dt;
		}
		c->x=c->_dx;
		c->y=c->_dy;
		c->z=c->_dz;
		c->rx=drxr;
		c->ry=dryr;
		c->rz=drzr;
	}
	if (c->lock==true){
		SetCursorPos(renderer_wx+renderer_ww/2,renderer_wy+renderer_wh/2);
	}
	return look_at_matrix(c->x,c->y,c->z,sinf(c->rx-PI_DIV_TWO)*cosf(c->ry),cosf(c->rx-PI_DIV_TWO),sinf(c->rx-PI_DIV_TWO)*sinf(c->ry),0,1,0);
}
/********************/



/********************/
int64_t g_sz[]={-10,-10,10,10};
ID3D11Buffer* g_ib=NULL;
size_t g_ib_l=0;
ID3D11Buffer* g_vb=NULL;
/********************/
ID3D11Buffer* cb=NULL;
AssetModel m=NULL;
Camera c;



void init_engine(void){
	cb=create_constant_buffer(sizeof(CBufferLayout));
	m=load_model(MODEL_ARTIFACT);
	c=create_camera(10,500,0,0,0,90,0,0,1);
	c->lock=true;
	c->enabled=true;
	ShowCursor(false);
	SetCapture(renderer_w);
}



void update_engine(double dt){
	static float t=0;
	t+=(float)(dt*1e-6);
	Matrix cm=update_camera(c,(float)(dt*1e-6));
	if (cm==NULL){
		return;
	}
	#define _near (0.1f)
	#define _far (1000.0f)
	#define _fov (90.0f)
	float _ctan_fov=cosf(_fov/2)/sinf(_fov/2);
	CBufferLayout cb_dt={
		raw_matrix(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1),
		as_raw_matrix(cm),
		raw_matrix(_ctan_fov/renderer_aspect_ratio,0,0,0,0,_ctan_fov,0,0,0,0,_far/(_far+_near),1,0,0,-_near*_far/(_far+_near),0)
		// raw_matrix(1/renderer_aspect_ratio,0,0,0,0,1,0,0,0,0,-2/(_far-_near),-(_far+_near)/(_far-_near),0,0,0,1)
	};
	update_constant_buffer(cb,(void*)&cb_dt);
	ID3D11DeviceContext_VSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	ID3D11DeviceContext_PSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	draw_model(m);
	/********************/
	g_ib_l=(*(g_sz+2)-(*g_sz)+(*(g_sz+3))-(*(g_sz+1))+2)*2;
	size_t vlsz=(*(g_sz+2)-(*g_sz)+(*(g_sz+3))-(*(g_sz+1))+2)*2*9;
	uint16_t* il=malloc(g_ib_l*sizeof(uint16_t));
	float* vl=malloc(vlsz*sizeof(float));
	size_t j=0;
	uint16_t k=0;
	for (int64_t i=*g_sz;i<=*(g_sz+2);i++){
		*(vl+j)=(float)i;
		*(vl+j+1)=0;
		*(vl+j+2)=(float)(*(g_sz+1));
		*(vl+j+3)=0;
		*(vl+j+4)=0;
		*(vl+j+5)=0;
		*(vl+j+6)=0.1f;
		*(vl+j+7)=0.1f;
		*(vl+j+8)=0.1f;
		*(vl+j+9)=(float)i;
		*(vl+j+10)=0;
		*(vl+j+11)=(float)*(g_sz+3);
		*(vl+j+12)=0;
		*(vl+j+13)=0;
		*(vl+j+14)=0;
		*(vl+j+15)=0.1f;
		*(vl+j+16)=0.1f;
		*(vl+j+17)=0.1f;
		*(il+k)=k;
		*(il+k+1)=k+1;
		j+=18;
		k+=2;
	}
	for (int64_t i=*(g_sz+1);i<=*(g_sz+3);i++){
		*(vl+j)=(float)(*g_sz);
		*(vl+j+1)=0;
		*(vl+j+2)=(float)i;
		*(vl+j+3)=0;
		*(vl+j+4)=0;
		*(vl+j+5)=0;
		*(vl+j+6)=0.1f;
		*(vl+j+7)=0.1f;
		*(vl+j+8)=0.1f;
		*(vl+j+9)=(float)(*(g_sz+2));
		*(vl+j+10)=0;
		*(vl+j+11)=(float)i;
		*(vl+j+12)=0;
		*(vl+j+13)=0;
		*(vl+j+14)=0;
		*(vl+j+15)=0.1f;
		*(vl+j+16)=0.1f;
		*(vl+j+17)=0.1f;
		*(il+k)=k;
		*(il+k+1)=k+1;
		j+=18;
		k+=2;
	}
	if (g_vb==NULL||g_ib==NULL){
		assert(g_vb==NULL);
		assert(g_ib==NULL);
		D3D11_BUFFER_DESC bd={
			(uint32_t)(g_ib_l*sizeof(uint16_t)),
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_INDEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0,
			0
		};
		D3D11_SUBRESOURCE_DATA dt={
			il,
			0,
			0
		};
		HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&g_ib);
		bd.ByteWidth=(uint32_t)(vlsz*sizeof(float));
		bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
		dt.pSysMem=vl;
		hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&g_vb);
	}
	else{
		ID3D11Resource* vbr;
		ID3D11Buffer_QueryInterface(g_vb,&IID_ID3D11Resource,(void**)&vbr);
		D3D11_MAPPED_SUBRESOURCE mr;
		HRESULT hr=ID3D11DeviceContext_Map(renderer_d3_dc,vbr,0,D3D11_MAP_WRITE_DISCARD,0,&mr);
		float* nvl=(float*)(mr.pData);
		for (size_t i=0;i<vlsz;i++){
			*(nvl+i)=*(vl+i);
		}
		ID3D11DeviceContext_Unmap(renderer_d3_dc,vbr,0);
		ID3D11Resource_Release(vbr);
		ID3D11Resource* ibr;
		ID3D11Buffer_QueryInterface(g_ib,&IID_ID3D11Resource,(void**)&ibr);
		hr=ID3D11DeviceContext_Map(renderer_d3_dc,ibr,0,D3D11_MAP_WRITE_DISCARD,0,&mr);
		uint16_t* nil=(uint16_t*)(mr.pData);
		for (size_t i=0;i<g_ib_l;i++){
			*(nil+i)=*(il+i);
		}
		ID3D11DeviceContext_Unmap(renderer_d3_dc,ibr,0);
		ID3D11Resource_Release(ibr);
	}
	free(il);
	free(vl);
	unsigned int off=0;
	unsigned int st=9*sizeof(float);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&g_vb,&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,g_ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,(uint32_t)g_ib_l,0,0);
	/********************/
	free(cm);
}
