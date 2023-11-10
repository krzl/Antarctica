struct VertexIn
{
	float3 pos		: POSITION0;
	float4 color	: COLOR;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR0;
};

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

Texture2D tex : register(t3);
SamplerState samp : register(s0);

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"DescriptorTable( SRV(t0, numDescriptors = 1))," \
			"CBV(b1)," \
			"CBV(b2)," \
			"StaticSampler(s0)"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut vout;

	vout.pos	= mul(float4(vin.pos, 1.0f), viewProj);
	vout.color = vin.color;
	
    return vout;
}

float4 ps(VertexOut pin) : SV_Target
{
	return pin.color;
}
