struct VertexIn
{
	float3 pos		: POSITION0;
	float2 texcoord	: TEXCOORD0;
	uint instanceId	: SV_InstanceID;
	uint vertexId	: SV_VertexID;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float3 worldPos	: POSITION;
    float3 normal	: NORMAL;
	float2 texcoord	: TEXCOORD;
};

cbuffer cbObject : register(b0)
{
	struct
	{
		float4x4	world;
	} perObjectBuffer[2];
};


cbuffer cbCamera : register(b1)
{
	float4x4	viewProj;
};

cbuffer cbCall : register(b2)
{
	uint instanceCount;
	uint vertexCount;
	uint indexCount;
	uint padding;
};

StructuredBuffer<float3> positions : register(t3);

Texture2D tex : register(t4);
SamplerState samp : register(s0);

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"CBV(b0)," \
			"CBV(b1)," \
			"CBV(b2)," \
			"DescriptorTable(SRV(t3, numDescriptors = 1))," \
			"DescriptorTable(SRV(t4, numDescriptors = 1))," \
			"StaticSampler(s0)"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut vout;

	vout.worldPos	= positions[vin.instanceId * vertexCount + vin.vertexId];
	vout.pos		= mul(float4(vout.worldPos, 1.0f), viewProj);
    vout.normal		= vin.pos;
	vout.texcoord	= float2(vin.texcoord.x, 1.0f - vin.texcoord.y);
    
    return vout;
}

float4 ps(VertexOut pin) : SV_Target
{
	return tex.Sample(samp, pin.texcoord);
    return float4(1,0.2,0.0,1);
}
