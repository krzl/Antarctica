struct PerObject
{
	float4x4	world;
	float4x4	worldInv;
};

StructuredBuffer<PerObject> perObjectBuffers : register(t0);

cbuffer _cbCamera : register(b1)
{
	float4x4	viewProj;
	float4x4	proj;
	float4x4 	viewProjInv;
};

cbuffer _cbCall : register(b2)
{
	uint instanceCount;
	uint vertexCount;
	uint indexCount;
	uint padding;
};

cbuffer MaterialData : register(b3)
{
	float2 scale;
}

Texture2D tex : register(t4);
SamplerState samp : register(s0);

struct VertexIn
{
	float3 pos	: POSITION;
	float2 uv	: TEXCOORD0;
	float4 col	: COLOR0;
};

struct VertexOut
{
	float4 pos	: SV_POSITION;
	float4 col	: COLOR0;
	float2 uv	: TEXCOORD0;
};

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"DescriptorTable(SRV(t0, numDescriptors = 1))," \
			"CBV(b1)," \
			"CBV(b2)," \
			"DescriptorTable(CBV(b3))," \
			"DescriptorTable(SRV(t4, numDescriptors = 1))," \
			"StaticSampler(s0)"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut output;
	
	output.pos	= mul(proj, float4(vin.pos, 1.0f));
	output.col	= vin.col;
	output.uv	= vin.uv;
	
	float2 translate = float2(-1.0f, 1.0f);
	output.pos = float4(vin.pos.xy * scale + translate, 0.0f, 1.0f);
	
	return output;
}

float4 ps(VertexOut pin) : SV_Target
{
	return pin.col * tex.Sample(samp, pin.uv);
}