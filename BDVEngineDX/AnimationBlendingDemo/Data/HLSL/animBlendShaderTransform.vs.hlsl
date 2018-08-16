#pragma pack_matrix(row_major)
#define MAX_NUM_JOINTS 100
#define EPSILON 0.000001f

struct JointAnim
{
	float4 t;
	float4 s;
	float4 q;
};

cbuffer cbInterpolationInfo : register(b0)
{
	float gLerpPercent;
	int gInterpolate;
	int gIsBlending;
	float gBlendTime;
}

cbuffer cbBeginAnim : register(b1)
{
	JointAnim gBeginAnims[MAX_NUM_JOINTS];
}

cbuffer cbEndAnim : register(b2)
{
	JointAnim gEndAnims[MAX_NUM_JOINTS];
}

cbuffer cbBlendAnim : register(b3)
{
	JointAnim gBlendAnims[MAX_NUM_JOINTS];
}

struct Joint
{
	float4x4 invBindPose;
	int parent;
};

cbuffer cbJoint : register(b4)
{
	Joint gJoints[MAX_NUM_JOINTS];
}

float4 Slerp(float4 q1, float4 q2, float lerpPercent)
{
	float cosOmega = dot(q1, q2);
	if (cosOmega < 0.0f)
	{
		cosOmega = -cosOmega;
		q2 = -q2;
	}

	float omega = acos(cosOmega);

	float mult1 = 1.0f - lerpPercent;
	float mult2 = lerpPercent;
	if (omega > EPSILON)
	{
		float oneOverSinOmega = 1.0f / sin(omega);
		mult1 = sin(mult1 * omega) * oneOverSinOmega;
		mult2 = sin(mult2 * omega) * oneOverSinOmega;
	}

	float4 result = q1 * mult1 + q2 * mult2;

	return normalize(result);
}

float4x4 QuatToMatrix(float4 q)
{
	float xx = 2.0f * q.x * q.x;
	float yy = 2.0f * q.y * q.y;
	float zz = 2.0f * q.z * q.z;

	float xy = 2.0f * q.x * q.y;
	float xz = 2.0f * q.x * q.z;
	float xw = 2.0f * q.x * q.w;

	float yz = 2.0f * q.y * q.z;
	float yw = 2.0f * q.y * q.w;

	float zw = 2.0f * q.z * q.w;

	float4x4 r;
	r[0] = float4(1.0f - yy - zz, xy + zw, xz - yw, 0.0f);
	r[1] = float4(xy - zw, 1.0f - xx - zz, yz + xw, 0.0f);
	r[2] = float4(xz + yw, yz - xw, 1.0f - xx - yy, 0.0f);
	r[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return r;
}

float4x4 ProcessTransforms(int jointIndex, int numJoints)
{
	float4x4 prevRSM = float4x4(
		float4(1.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 1.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 1.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
		);

	float4x4 sM = (float4x4)0;
	float4x4 rM = (float4x4)0;
	float4x4 currRSM = (float4x4)0;
	float3 s = float3(0.0f, 0.0f, 0.0f);
	float3 t = float3(0.0f, 0.0f, 0.0f);
	float4 q = float4(0.0f, 0.0f, 0.0f, 0.0f);

	int currJoint = jointIndex;
	bool keepGoing = true;
	while (keepGoing)
	{
		if (gInterpolate == 1)
		{
			// Interpolate between animations
			s = lerp(gBeginAnims[currJoint].s.xyz, gEndAnims[currJoint].s.xyz, gLerpPercent);
			t = lerp(gBeginAnims[currJoint].t.xyz, gEndAnims[currJoint].t.xyz, gLerpPercent);
			q = Slerp(gBeginAnims[currJoint].q, gEndAnims[currJoint].q, gLerpPercent);
		}
		else
		{
			s = gBeginAnims[currJoint].s.xyz;
			t = gBeginAnims[currJoint].t.xyz;
			q = gBeginAnims[currJoint].q;
		}

		// Perform blending
		if (gIsBlending == 1)
		{
			s = lerp(s, gBlendAnims[currJoint].s.xyz, gBlendTime);
			t = lerp(t, gBlendAnims[currJoint].t.xyz, gBlendTime);
			q = Slerp(q, gBlendAnims[currJoint].q, gBlendTime);
		}

		// Generate matrix
		sM[0] = float4(s.x, 0.0f, 0.0f, 0.0f);
		sM[1] = float4(0.0f, s.y, 0.0f, 0.0f);
		sM[2] = float4(0.0f, 0.0f, s.z, 0.0f);
		sM[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

		rM = QuatToMatrix(q);

		currRSM = mul(sM, rM);
		currRSM[3] += float4(t, 0.0f);

		currRSM = mul(prevRSM, currRSM);
		prevRSM = currRSM;

		currJoint = gJoints[currJoint].parent;
		if (currJoint == -1)
		{
			keepGoing = false;
		}
	}

	currRSM = mul(gJoints[jointIndex].invBindPose, currRSM);

	return currRSM;
}

struct JointIn
{
	int jointIndex : JOINTINDEX;
	int numJoints : NUMJOINTS;
};

struct MatrixOut
{
	float4 row0 : ROWA;
	float4 row1 : ROWB;
	float4 row2 : ROWC;
	float4 row3 : ROWD;
};

MatrixOut VS(JointIn jin)
{
	float4x4 finalMatrix = ProcessTransforms(jin.jointIndex, jin.numJoints);
		MatrixOut mOut;
	mOut.row0 = finalMatrix[0];
	mOut.row1 = finalMatrix[1];
	mOut.row2 = finalMatrix[2];
	mOut.row3 = finalMatrix[3];

	return mOut;
}