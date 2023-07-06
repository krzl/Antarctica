struct VertexIn
{
	float3 pos		: POSITION0;
    float3 normal	: NORMAL;
	float2 texcoord	: TEXCOORD0;
	uint instanceId	: SV_InstanceID;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float3 worldPos	: POSITION;
    float3 normal	: NORMAL;
	float2 texcoord	: TEXCOORD;
};

struct PerObject
{
	float4x4	world;
};

StructuredBuffer<PerObject> perObjectBuffers : register(t0);

cbuffer cbCamera : register(b1)
{
	float4x4	viewProj;
	float4x4	proj;
};

cbuffer cbCall : register(b2)
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
			"DescriptorTable( SRV(t3, numDescriptors = 1))," \
			"StaticSampler(s0)"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut vout;

	float4 worldPos = mul(float4(vin.pos.xyz, 1.0f), perObjectBuffers[vin.instanceId].world);

	vout.pos		= mul(worldPos,    viewProj);
	vout.worldPos	= worldPos.xyz;
    vout.normal		= vin.normal;
	vout.texcoord	= float2(vin.texcoord.x, 1.0f - vin.texcoord.y);
    
    return vout;
}

float4 ps(VertexOut pin) : SV_Target
{
	return tex.Sample(samp, pin.texcoord);
}
