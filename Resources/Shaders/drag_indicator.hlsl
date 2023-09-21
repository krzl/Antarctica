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


struct VertexIn
{
	float3 pos	: POSITION;
	float4 col	: COLOR0;
};

struct VertexOut
{
	float4 pos	: SV_POSITION;
	float4 col	: COLOR0;
};

#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"DescriptorTable(SRV(t0, numDescriptors = 1))," \
			"CBV(b1)," \
			"CBV(b2),"
			
[RootSignature(RS)]
VertexOut vs(VertexIn vin)
{
	VertexOut output;
	
	output.pos	= float4(vin.pos, 1.0f);
	output.col	= vin.col;
	
	return output;
}

float4 ps(VertexOut pin) : SV_Target
{
	return pin.col;
}