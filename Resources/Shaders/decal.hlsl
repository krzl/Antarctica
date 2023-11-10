struct PerObject
{
	float4x4	world;
	float4x4	worldInv;
};

StructuredBuffer<PerObject> perObjectBuffers : register(t0);

Texture2D<float> DepthTexture : register(t3);
SamplerState samp : register(s0);

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
	uint screenWidth;
	uint screenHeight;
};


struct VertexIn
{
	float3 pos		: POSITION;
	float4 col		: COLOR0;
	uint instanceId	: SV_InstanceID;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float4 col		: COLOR0;
	uint instanceId	: SV_InstanceID;
	
};

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"DescriptorTable(SRV(t0, numDescriptors = 1))," \
			"CBV(b1)," \
			"CBV(b2)," \
			"DescriptorTable( SRV(t3, numDescriptors = 1))," \
			"StaticSampler(s0)"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut output;
	
	float4 worldPos = mul(float4(vin.pos.xyz, 1.0f), perObjectBuffers[vin.instanceId].world);

	output.pos			= mul(worldPos, viewProj);
	output.col			= vin.col;
	output.instanceId	= vin.instanceId;
	
	return output;
}

float4 ps(VertexOut pin) : SV_Target
{
	float2 uv = float2(pin.pos.x / screenWidth, pin.pos.y / screenHeight);
	float depth = DepthTexture.Sample(samp, uv);
	
	float4 ndc = float4(uv * 2.0f - 1.0f, depth, 1.0f);
	ndc.y *= -1.0f;
	float4 worldPosition = mul(ndc, viewProjInv);
	worldPosition = worldPosition / worldPosition.w;
	
	float4 objectPosition = mul(float4(worldPosition.xyz, 1.0f), perObjectBuffers[pin.instanceId].worldInv);
	
	float2 extendedCoords = objectPosition.xz * 2.0f;
	float dotC = dot(extendedCoords, extendedCoords);
	
	float alpha = dotC > 1.0f || dotC < 0.6f ? 0.0f : 1.0f;
	
	return float4(1.0f, 0.86f, 0.17f, alpha);
}