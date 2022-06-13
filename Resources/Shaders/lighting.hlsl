#define NUM_DIR_LIGHTS 4
#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

struct DirLight
{
	float3		color;
	float		padding;
	float3		direction;
	float		padding2;
};

struct PointLight
{
	float3		color;
	float		falloffStart;
	float3		position;
	float		falloffEnd;
};

struct SpotLight
{
	float3		color;
	float		falloffStart;
	float3		position;
	float		falloffEnd;
	float3		direction;
	float		spotPower;
};

struct LightSetup
{
	DirLight	dirLights[NUM_DIR_LIGHTS];
	PointLight	pointLights[NUM_POINT_LIGHTS];
	SpotLight	spotLights[NUM_SPOT_LIGHTS];
	float3		ambientColor;
	uint		dirLightCount;
	uint		pointLightCount;
	uint		spotLightCount;
};

struct SurfaceInfo
{
	float3 diffuseColor;
	float3 specularColor;
	float roughness;
};

float CalcAttenuation(float distance, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - distance) / (falloffEnd - falloffStart));
}

float3 SchlickFresnel(float3 specularColor, float3 halfVec, float3 lightVec)
{
	float incidentAngle = saturate(dot(halfVec, lightVec));
	float f0 = 1.0f - incidentAngle;
	
	return specularColor + (1.0f - specularColor) * pow(f0, 5.0f);
}

float3 BlinnPhong(SurfaceInfo surface, float3 lightColor, float3 lightVec, float3 normal, float3 eyeDirection)
{
	const float m = (1.0f - surface.roughness) * 256.0f;
	float3 halfVec = normalize(eyeDirection + lightVec);
	
	float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(surface.specularColor, halfVec, lightVec);
	
	float3 specularColor = fresnelFactor * roughnessFactor;
	specularColor = specularColor / (specularColor + 1.0f);
	
	return (surface.diffuseColor * specularColor) * lightColor;
}

float3 CalcDirLight(DirLight light, SurfaceInfo surface, float3 normal, float3 eyeDirection)
{
	float3 lightVec = -light.direction;
	
	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 color = light.color * ndotl;
	
	return BlinnPhong(surface, color, lightVec, normal, eyeDirection);
}

float3 CalcPointLight(PointLight light, SurfaceInfo surface, float3 position, float3 normal, float3 eyeDirection)
{
	float3 lightVec = light.position - position;
	
	float distance = length(lightVec);
	
	float3 finalColor;
	if (distance > light.falloffEnd)
	{
		finalColor = float3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		lightVec /= distance;
		
		float ndotl = max(dot(lightVec, normal), 0.0f);
		float3 color = light.color * ndotl;
		
		float attenuation = CalcAttenuation(distance, light.falloffStart, light.falloffEnd);
		color *= attenuation;
		
		finalColor = BlinnPhong(surface, color, lightVec, normal, eyeDirection);
	}
	
	return finalColor;
}

float3 CalcSpotLight(SpotLight light, SurfaceInfo surface, float3 position, float3 normal, float3 eyeDirection)
{
	float3 lightVec = light.position - position;
	
	float distance = length(lightVec);
	
	float3 finalColor;
	if (distance > light.falloffEnd)
	{
		finalColor = float3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		lightVec /= distance;
		
		float ndotl = max(dot(lightVec, normal), 0.0f);
		float3 color = light.color * ndotl;
		
		float attenuation = CalcAttenuation(distance, light.falloffStart, light.falloffEnd);
		color *= attenuation;
		
		float spotFactor = pow(max(dot(-lightVec, light.direction), 0.0f), light.spotPower);
		color *= spotFactor;
	
		finalColor = BlinnPhong(surface, color, lightVec, normal, eyeDirection);
	}
	
	return finalColor;
}

float3 CalcLighting(LightSetup lights, SurfaceInfo surface, float3 position, float3 normal, float3 eyeDirection)
{
	float3 color = saturate(lights.ambientColor * surface.diffuseColor);
	
	uint i = 0;
	for (i = 0; i < lights.dirLightCount; ++i)
	{
		color += saturate(CalcDirLight(lights.dirLights[i], surface, normal, eyeDirection));
	}
	
	for (i = 0; i < lights.pointLightCount; ++i)
	{
		color += saturate(CalcPointLight(lights.pointLights[i], surface, position, normal, eyeDirection));
	}
	
	for (i = 0; i < lights.spotLightCount; ++i)
	{
		color += saturate(CalcSpotLight(lights.spotLights[i], surface, position, normal, eyeDirection));
	}
	
	return saturate(color);
}