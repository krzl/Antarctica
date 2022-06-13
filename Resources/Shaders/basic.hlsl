
struct VertexIn
{
	float3 pos		: POSITION;
    float3 normal	: NORMAL;
	float2 texcoord	: TEXCOORD;
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

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"CBV(b0)," \
			"CBV(b1)" \
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut vout;

	float4 worldPos = mul(float4(vin.pos.xyz, 1.0f), world);

	vout.pos		= mul(worldPos, viewProj);
	vout.worldPos	= worldPos.xyz;
    vout.normal		= vin.normal;
	vout.texcoord	= vin.texcoord;
    
    return vout;
}

float4 ps(VertexOut pin) : SV_Target
{
    return float4(1,1,0,1);
}
