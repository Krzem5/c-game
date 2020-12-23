#pragma pack_matrix(row_major)



static float3 LIGHT_DIR=float3(-0.4767312946227962,0.6674238124719146,-0.5720775535473555);



cbuffer core:register(b0){
	matrix wm;
	matrix cm;
	matrix pm;
};



struct VS_OUT{
	float4 p:SV_POSITION;
	float3 n:NORMAL;
	float3 c:COLOR;
};



VS_OUT color_shading_3d_vs(float3 p:POSITION,float3 n:NORMAL,float3 c:COLOR){
	VS_OUT o={
		mul(mul(mul(float4(p.x,p.y,p.z,1),wm),cm),pm),
		n,
		c
	};
	return o;
}



float map(float v,float aa,float ab,float ba,float bb){
	return (v-aa)/(ab-aa)*(bb-ba)+ba;
}



float4 color_shading_3d_ps(VS_OUT vo):SV_TARGET{
	return float4(vo.c.x,vo.c.y,vo.c.z,1)*(length(vo.n)!=0?map(dot(vo.n,LIGHT_DIR),-1,1,0.15,1.1):1);
}
