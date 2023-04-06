cbuffer param
{
	float4x4 matWorldViewProj;		// la matrice totale 
	float4x4 matWorld;				// matrice de transformation dans le monde 
	float4 vDirLum; 				// la direction de la lumi�re
	float4 vCamera; 				// la position de la cam�ra
	float4 vAEcl; 					// la valeur ambiante de l'�clairage
	float4 vAMat; 					// la valeur ambiante du mat�riau
	float4 vDEcl; 					// la valeur diffuse de l'�clairage 
	float4 vDMat; 					// la valeur diffuse du mat�riau 
}

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
	float2 coordTex : TEXCOORD3;
	float3 WorldPos : TEXCOORD4;
	float3 WorldNorm : TEXCOORD5;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL,
	float2 coordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0), matWorld).xyz;

	sortie.WorldPos = mul(Pos, matWorld).xyz;
	sortie.WorldNorm = Normale;

	sortie.vDirLum = vDirLum.xyz;
	//sortie.vDirLum = float3(0, 1, 0);
	sortie.vDirCam = vCamera.xyz - sortie.WorldPos;

	// Coordonn�es d�application de texture
	sortie.coordTex = coordTex;

	return sortie;
}

// Les textures
Texture2D textureEntree;

// L'état de sampling
SamplerState SampleState; 

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
	float3 couleur;
	float coeffAmbientLight = 4.0f;

	// Normaliser les paramètres
	float3 N = normalize(vs.Norm);
	float3 L = normalize(vs.vDirLum);
	float3 V = normalize(vs.vDirCam);

	// Valeur de la composante diffuse
	float3 diff = saturate(dot(N, L));

	// R = 2 * (N.L) * N � L
	float3 R = normalize(2 * diff * N - L);

	// Puissance de 4 - pour l'exemple
	float S = pow(saturate(dot(R, V)), 4.0);
	
	// �chantillonner la couleur du pixel � partir de la texture
	float3 couleurTexture = textureEntree.Sample(SampleState, vs.coordTex).rgb;

	// I = A + D * N.L + (R.V)n
	couleur = couleurTexture * vAEcl.rgb * coeffAmbientLight * vAMat.rgb +
		      couleurTexture * vDEcl.rgb * vDMat.rgb * diff;

	/*if (alpha >= 1.0f)
	{
		couleur += S;
	}*/

	return float4(couleur, 1.0);
}

technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_5_0, MiniPhongPS()));
		SetGeometryShader(NULL);
	}
}
