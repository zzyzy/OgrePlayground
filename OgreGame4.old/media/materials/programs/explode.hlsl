/*
  Our explosion vertex program that just passes the alpha value of the vertex
  on to the fragment program
*/
void main_vp(float4 position : POSITION,
			float2 uv		  : TEXCOORD0,
			float4 color	: COLOR0,
 
			out float4 oPosition : POSITION,
			out float2 oUv	   : TEXCOORD0,
			out float4 oColor   : COLOR0,
 
			uniform float4x4 worldViewProj)
{
	oPosition = mul(worldViewProj, position);
	oUv = uv;
	oColor = color;
}
 
void main_fp(
	float4 position	: POSITION,
	float2 uv		: TEXCOORD0,
	float4 color	: COLOR0,
 
	uniform sampler3D tex:register(s0),
 
	out float4 oColour	: COLOR)
{
	oColour = tex3D( tex, float3(uv.x, uv.y, 1.0 - color.a));
}