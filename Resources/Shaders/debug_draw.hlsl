struct VertexIn
{
	float3 pos		: POSITION0;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
};

struct PerObject
{
	float4x4	world;
};

StructuredBuffer<PerObject> perObjectBuffers : register(t0);

cbuffer _cbCamera : register(b1)
{
	float4x4	viewProj;
	float4x4	proj;
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
	float4 color;
}

Texture2D tex : register(t3);
SamplerState samp : register(s0);

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"DescriptorTable( SRV(t0, numDescriptors = 1))," \
			"CBV(b1)," \
			"CBV(b2)," \
			"DescriptorTable(CBV(b3))," \
			"StaticSampler(s0)"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut vout;

	vout.pos	= mul(float4(vin.pos, 1.0f), viewProj);
	
    return vout;
}

float4 ps(VertexOut pin) : SV_Target
{
	return color;
}
