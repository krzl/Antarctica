struct VertexIn
{
	float3 pos		: POSITION0;
    float3 normal	: NORMAL;
	float2 texcoord	: TEXCOORD0;
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
	float4x4	world;
};

cbuffer cbCamera : register(b1)
{
	float4x4	viewProj;
};

Texture2D tex : register(t2);
SamplerState samp : register(s0);

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"CBV(b0)," \
			"CBV(b1)," \
			"DescriptorTable( SRV(t2, numDescriptors = 1))," \
			"StaticSampler(s0)"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut vout;

	float4 worldPos = mul(float4(vin.pos.xyz, 1.0f), world);

	vout.pos		= mul(worldPos,    viewProj);
	vout.worldPos	= worldPos.xyz;
    vout.normal		= vin.normal;
	vout.texcoord	= float2(vin.texcoord.x, 1.0f - vin.texcoord.y);
    
    return vout;
}

float4 ps(VertexOut pin) : SV_Target
{
	return tex.Sample(samp, pin.texcoord);
    return float4(1,0.2,0.0,1);
}
