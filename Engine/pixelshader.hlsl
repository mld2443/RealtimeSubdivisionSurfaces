////////////////////////////////////////////////////////////////////////////////
// Filename: pixelshader.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	float3 lightDir;
	float lightIntensity;
	float4 color;
	
	// Invert the light direction for calculations.
	lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, lightDir));
	
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = saturate(diffuseColor * lightIntensity);

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color * input.color;

	return color;
}