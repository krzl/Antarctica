struct VertexIn
{
	float3 pos			: POSITION0;
	float2 uv			: TEXCOORD0;
	float4 splatWeights	: TEXCOORD1;
	uint instanceId		: SV_InstanceID;
};

struct VertexOut
{
	float4 pos			: SV_POSITION;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD0;
	float4 splatWeights	: TEXCOORD1;
};


struct PerObject
{
	float4x4	world;
};

StructuredBuffer<PerObject> perObjectBuffers : register(t0);

cbuffer cbCamera : register(b1)
{
	float4x4	viewProj;
};

Texture2D tex1 : register(t3);
Texture2D tex2 : register(t4);
Texture2D tex3 : register(t5);
Texture2D tex4 : register(t6);
SamplerState samp : register(s0);

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"DescriptorTable(SRV(t0, numDescriptors = 1))," \
			"CBV(b1)," \
			"CBV(b2)," \
			"DescriptorTable( SRV(t3, numDescriptors = 1))," \
			"DescriptorTable( SRV(t4, numDescriptors = 1))," \
			"DescriptorTable( SRV(t5, numDescriptors = 1))," \
			"DescriptorTable( SRV(t6, numDescriptors = 1))," \
			"StaticSampler(s0)"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut vout;

	float4 worldPos = mul(float4(vin.pos.xyz, 1.0f), perObjectBuffers[vin.instanceId].world);

	vout.pos			= mul(worldPos, viewProj);
	vout.worldPos		= worldPos.xyz;
	vout.uv				= float2(vin.uv.x, 1.0f - vin.uv.y);
	vout.splatWeights	= vin.splatWeights;
    
    return vout;
}

float4 ps(VertexOut pin) : SV_Target
{
	float2 uv = frac(pin.uv);
	
	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	color.rgb += pin.splatWeights.x * tex1.Sample(samp, uv).rgb;
	color.rgb += pin.splatWeights.y * tex2.Sample(samp, uv).rgb;
	color.rgb += pin.splatWeights.z * tex3.Sample(samp, uv).rgb;
	color.rgb += pin.splatWeights.w * tex4.Sample(samp, uv).rgb;
	
	return color;
}
