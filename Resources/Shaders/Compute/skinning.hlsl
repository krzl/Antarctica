struct BoneWeight
{
	unsigned int boneId;
	float weight;
};

struct VertexWeights
{
	BoneWeight boneWeights[4];
};

StructuredBuffer<VertexWeights> vertexWeights : register(t0);
StructuredBuffer<float4x4> boneTransforms : register(t1);
ByteAddressBuffer srcBuffer[4] : register(t2);
RWStructuredBuffer<float3> dstBuffer : register(u0);


#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
			"DescriptorTable(SRV(t0, numDescriptors = 1))," \
			"DescriptorTable(SRV(t1, numDescriptors = 1))," \
			"DescriptorTable(SRV(t2, numDescriptors = 4, flags=DESCRIPTORS_VOLATILE))," \
			"DescriptorTable(UAV(u0, numDescriptors = 1))"

[RootSignature(RS)]
[numthreads(64, 1, 1)]
void cs(uint3 groupID : SV_GroupID, uint3 tid : SV_DispatchThreadID, uint3 localTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	uint w, h;
	vertexWeights.GetDimensions(w, h);
	
	uint w2, h2;
	dstBuffer.GetDimensions(w2, h2);
	
	uint w3, h3;
	boneTransforms.GetDimensions(w3, h3);
	
	uint meshCount = w2 / w;
	uint id = groupID.x * 64 + groupIndex;

	uint vertexId = id % w;
	uint meshId = id / (w);
	
	uint boneCount = w3 / meshCount;
	
	VertexWeights vw = vertexWeights[vertexId];
	
	float3 finalPos = float3(0.0f, 0.0f, 0.0f);
	
	float4 inVertex = float4(0,0,0,0);

	inVertex = float4(asfloat(srcBuffer[0].Load3(vertexId * 12)), 1.0f);
	
	finalPos += mul(inVertex, boneTransforms[meshId * boneCount + vw.boneWeights[0].boneId]).xyz * vw.boneWeights[0].weight;
	finalPos += mul(inVertex, boneTransforms[meshId * boneCount + vw.boneWeights[1].boneId]).xyz * vw.boneWeights[1].weight;
	finalPos += mul(inVertex, boneTransforms[meshId * boneCount + vw.boneWeights[2].boneId]).xyz * vw.boneWeights[2].weight;
	finalPos += mul(inVertex, boneTransforms[meshId * boneCount + vw.boneWeights[3].boneId]).xyz * vw.boneWeights[3].weight;
	
	dstBuffer[id] = finalPos;
}