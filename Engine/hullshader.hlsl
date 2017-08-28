////////////////////////////////////////////////////////////////////////////////
// Filename: hullshader.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer TessellationBuffer
{
    float tessellationAmount;
    float3 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct HullInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct HullOutputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 16> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;


	// Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = tessellationAmount;
    output.edges[1] = tessellationAmount;
    output.edges[2] = tessellationAmount;
	output.edges[3] = tessellationAmount;

	// Set the tessellation factor for tessallating inside the triangle.
    output.inside[0] = tessellationAmount;
	output.inside[1] = tessellationAmount;

    return output;
}


////////////////////////////////////////////////////////////////////////////////
// Hull Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 16> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HullOutputType output;


	// Set the position for this control point as the output position.
    output.position = patch[pointId].position;

	// Set the input color as the output color.
    output.color = patch[pointId].color;

    return output;
}
