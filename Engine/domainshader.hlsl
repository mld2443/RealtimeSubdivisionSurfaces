////////////////////////////////////////////////////////////////////////////////
// Filename: domainshader.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
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

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////////////
float4 Spline(float t)
{
	float T0 = 2.0f + t;
	float T1 = 1.0f + t;
	float T2 = 0.0f + t;
	float T3 = 1.0f - t;
	float T4 = 2.0f - t;
	float T5 = 3.0f - t;

	return float4(	(T3 * T3 * T3) / 6.0f,
					((T0 * T3 * T3) + (T1 * T3 * T4) + (T2 * T4 * T4)) / 6.0f,
					((T1 * T1 * T3) + (T1 * T2 * T4) + (T2 * T2 * T5)) / 6.0f,
					(T2 * T2 * T2) / 6.0f);
}

float4 dSpline(float t)
{
	float T1 = 1.0f + t;
	float T2 = 0.0f + t;
	float T3 = 1.0f - t;
	float T4 = 2.0f - t;

	return float4(	(T3 * T3) / -6.0f,
					((T3 * T3) - (T1 * T3) - (T2 * T4)) / 6.0f,
					((T1 * T3) + (T2 * T4) - (T2 * T2)) / 6.0f,
					(T2 * T2) / 6.0f);
}

float4 CatmullRom(float t)
{
	float T0_ = t + 1.0f;
	float T1_ = t + 0.0f;
	float T2_ = t - 1.0f;
	float _1T = 0.0f - t;
	float _2T = 1.0f - t;
	float _3T = 2.0f - t;

	return float4(	(_1T * _2T * _2T) / 2.0f,
					((T0_ * _2T * _2T) + (_2T * T0_ * _2T) + (_2T * _3T * T1_)) / 2.0f,
					((T1_ * T0_ * _2T) + (T1_ * _3T * T1_) + (_3T * T1_ * T1_)) / 2.0f,
					(T2_ * T1_ * T1_) / 2.0f);
}

float4 dCatmullRom(float t)
{
	float T0_ = t + 1.0f;
	float T1_ = t + 0.0f;
	float T2_ = t - 1.0f;
	float _1T = 0.0f - t;
	float _2T = 1.0f - t;
	float _3T = 2.0f - t;

	return float4(	(_2T * _2T) / -2.0f,
					((_2T * _2T) - (T0_ * _2T) - (_3T * T1_)) / 2.0f,
					((T0_ * _2T) + (_3T * T1_) - (T1_ * T1_)) / 2.0f,
					(T1_ * T1_) / 2.0f);
}

float3 EvaluatePos(const OutputPatch<HullOutputType, 16> patch, float4 BasisU, float4 BasisV)
{
	float3 Value = float3(0, 0, 0);
	Value = BasisV.x * (patch[0].position * BasisU.x + patch[1].position * BasisU.y + patch[2].position * BasisU.z + patch[3].position * BasisU.w);
	Value += BasisV.y * (patch[4].position * BasisU.x + patch[5].position * BasisU.y + patch[6].position * BasisU.z + patch[7].position * BasisU.w);
	Value += BasisV.z * (patch[8].position * BasisU.x + patch[9].position * BasisU.y + patch[10].position * BasisU.z + patch[11].position * BasisU.w);
	Value += BasisV.w * (patch[12].position * BasisU.x + patch[13].position * BasisU.y + patch[14].position * BasisU.z + patch[15].position * BasisU.w);
	return Value;
}

float4 EvaluateColor(const OutputPatch<HullOutputType, 16> patch, float4 BasisU, float4 BasisV)
{
	float4 Value = float4(0, 0, 0, 0);
	Value = BasisV.x * (patch[0].color * BasisU.x + patch[1].color * BasisU.y + patch[2].color * BasisU.z + patch[3].color * BasisU.w);
	Value += BasisV.y * (patch[4].color * BasisU.x + patch[5].color * BasisU.y + patch[6].color * BasisU.z + patch[7].color * BasisU.w);
	Value += BasisV.z * (patch[8].color * BasisU.x + patch[9].color * BasisU.y + patch[10].color * BasisU.z + patch[11].color * BasisU.w);
	Value += BasisV.w * (patch[12].color * BasisU.x + patch[13].color * BasisU.y + patch[14].color * BasisU.z + patch[15].color * BasisU.w);
	return Value;
}


////////////////////////////////////////////////////////////////////////////////
// Domain Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]

PixelInputType ColorDomainShader(ConstantOutputType input, float2 UV : SV_DomainLocation, const OutputPatch<HullOutputType, 16> patch)
{
	float3 vertexPosition;
	PixelInputType output;
	
	// Evaluate the basis functions at (u, v)
	//float4 BasisU = CatmullRom(UV.x);
	//float4 BasisV = CatmullRom(UV.y);
	//float4 dBasisU = dCatmullRom(UV.x);
	//float4 dBasisV = dCatmullRom(UV.y);
	float4 BasisU = Spline(UV.x);
	float4 BasisV = Spline(UV.y);
	float4 dBasisU = dSpline(UV.x);
	float4 dBasisV = dSpline(UV.y);
	// Evaluate the surface position for this vertex
	vertexPosition = EvaluatePos(patch, BasisU, BasisV);

	// Evaluate the tangent space for this vertex (using derivatives)
	float3 Tangent = EvaluatePos(patch, dBasisU, BasisV);
	float3 BiTangent = EvaluatePos(patch, BasisU, dBasisV);

	// Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Send the input color into the pixel shader.
	output.color = EvaluateColor(patch, BasisU, BasisV);

	output.normal = normalize(cross(Tangent, BiTangent));

    return output;
}