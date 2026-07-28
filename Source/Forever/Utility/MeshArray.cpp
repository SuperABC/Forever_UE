#include "MeshArray.h"

#include <cmath>


static int Binomial(int n, int k) {
	if (k < 0 || k > n) return 0;
	if (k == 0 || k == n) return 1;
	long long result = 1;
	for (int i = 1; i <= k; i++) {
		result = result * (n - i + 1) / i;
	}
	return static_cast<int>(result);
}

static double Bernstein(int n, int i, double t) {
	return Binomial(n, i) * pow(t, i) * pow(1.0 - t, n - i);
}

static double BernsteinDerivative(int n, int i, double t) {
	double left = (i > 0) ? Bernstein(n - 1, i - 1, t) : 0.0;
	double right = (i < n) ? Bernstein(n - 1, i, t) : 0.0;
	return n * (left - right);
}

// 计算有理Bezier曲线在参数t处的位置
static FVector EvalCurvePoint(const TArray<FVector>& points, const TArray<float>& weights, double t) {
	int m = points.Num() - 1;
	double sumWeight = 0.0;
	FVector sum = FVector::ZeroVector;
	for (int i = 0; i <= m; i++) {
		double b = Bernstein(m, i, t) * weights[i];
		sumWeight += b;
		sum += points[i] * static_cast<float>(b);
	}
	return (sumWeight != 0.0) ? sum / static_cast<float>(sumWeight) : sum;
}

// 计算有理Bezier曲线在参数t处的切线方向（非单位向量，商法则求导）
static FVector EvalCurveTangent(const TArray<FVector>& points, const TArray<float>& weights, double t) {
	int m = points.Num() - 1;
	double sumWeight = 0.0, sumWeightDeriv = 0.0;
	FVector sum = FVector::ZeroVector;
	FVector sumDeriv = FVector::ZeroVector;
	for (int i = 0; i <= m; i++) {
		double b = Bernstein(m, i, t) * weights[i];
		double bd = BernsteinDerivative(m, i, t) * weights[i];
		sumWeight += b;
		sumWeightDeriv += bd;
		sum += points[i] * static_cast<float>(b);
		sumDeriv += points[i] * static_cast<float>(bd);
	}
	if (sumWeight == 0.0) return FVector::ZeroVector;
	return (sumDeriv * static_cast<float>(sumWeight) - sum * static_cast<float>(sumWeightDeriv)) /
		static_cast<float>(sumWeight * sumWeight);
}

// 在弧长查找表中二分查找目标弧长对应的曲线参数t
static float FindParamAtLength(const TArray<double>& cumulativeLength, double targetLen) {
	targetLen = FMath::Clamp(targetLen, 0.0, cumulativeLength.Last());

	int lo = 0;
	int hi = cumulativeLength.Num() - 1;
	while (lo < hi - 1) {
		int mid = (lo + hi) / 2;
		if (cumulativeLength[mid] < targetLen) lo = mid;
		else hi = mid;
	}

	double lenLo = cumulativeLength[lo];
	double lenHi = cumulativeLength[hi];
	double ratio = (lenHi > lenLo) ? (targetLen - lenLo) / (lenHi - lenLo) : 0.0;
	int resolution = cumulativeLength.Num() - 1;
	return static_cast<float>((lo + ratio) / resolution);
}

bool UMeshArray::RepeatMesh1D(FVector center, FVector size, FVector primitive, float rotate, int axis,
	TArray<FVector>& centers, TArray<FVector>& sizes) {
	centers.Empty();
	sizes.Empty();

	if (axis < 1 || axis > 3) return false;

	double spaceLen = (axis == 1) ? size.X : (axis == 2) ? size.Y : size.Z;
	double primLen = (axis == 1) ? primitive.X : (axis == 2) ? primitive.Y : primitive.Z;
	if (spaceLen <= 0.0 || primLen <= 0.0) return false;

	// n * scaledPrim = spaceLen, scaledPrim ∈ [0.8*primLen, 1.2*primLen]
	// => n ∈ [spaceLen / (1.2*primLen), spaceLen / (0.8*primLen)]
	int nLow = FMath::CeilToInt(spaceLen / (1.2 * primLen));
	int nHigh = FMath::FloorToInt(spaceLen / (0.8 * primLen));
	if (nLow > nHigh || nLow <= 0) return false;

	// Pick n closest to the unscaled ideal to minimize stretch
	int n = FMath::Clamp(FMath::RoundToInt(spaceLen / primLen), nLow, nHigh);

	double actualLen = spaceLen / n;

	FVector meshSize = FVector(1.0, 1.0, 1.0);
	double scale = actualLen / primLen;
	if (axis == 1) meshSize.X = scale;
	else if (axis == 2) meshSize.Y = scale;
	else meshSize.Z = scale;

	double axisCenter = (axis == 1) ? center.X : (axis == 2) ? center.Y : center.Z;
	double firstCenter = axisCenter - spaceLen * 0.5 + actualLen * 0.5;

	for (int i = 0; i < n; i++) {
		FVector meshCenter = center;
		double c = firstCenter + i * actualLen;
		if (axis == 1) meshCenter.X = c;
		else if (axis == 2) meshCenter.Y = c;
		else meshCenter.Z = c;

		centers.Add(meshCenter);
		sizes.Add(meshSize);
	}

	if (rotate != 0.f) {
		double rad = FMath::DegreesToRadians((double)rotate);
		double cosR = FMath::Cos(rad);
		double sinR = FMath::Sin(rad);
		for (FVector& mc : centers) {
			double dx = mc.X - center.X;
			double dy = mc.Y - center.Y;
			mc.X = center.X + dx * cosR - dy * sinR;
			mc.Y = center.Y + dx * sinR + dy * cosR;
		}
	}

	return true;
}

bool UMeshArray::RepeatMeshAlongCurve(FVector v1, FVector v2, const TArray<FVector>& controls, const TArray<float>& weights,
	FVector primitive, int axis,
	TArray<FVector>& starts, TArray<FVector>& startTangents,
	TArray<FVector>& ends, TArray<FVector>& endTangents, TArray<FVector>& sizes, int& segmentCount) {
	starts.Empty();
	startTangents.Empty();
	ends.Empty();
	endTangents.Empty();
	sizes.Empty();
	segmentCount = 0;

	if (axis < 1 || axis > 3) return false;
	if (controls.Num() != weights.Num()) return false;

	double primLen = (axis == 1) ? primitive.X : (axis == 2) ? primitive.Y : primitive.Z;
	if (primLen <= 0.0) return false;

	TArray<FVector> points;
	points.Reserve(controls.Num() + 2);
	points.Add(v1);
	points.Append(controls);
	points.Add(v2);

	TArray<float> pointWeights;
	pointWeights.Reserve(weights.Num() + 2);
	pointWeights.Add(1.f);
	pointWeights.Append(weights);
	pointWeights.Add(1.f);

	// 沿曲线细分采样，建立弧长-参数对照表，用于按弧长均匀分段
	constexpr int RESOLUTION = 256;
	TArray<FVector> samples;
	TArray<double> cumulativeLength;
	samples.Reserve(RESOLUTION + 1);
	cumulativeLength.Reserve(RESOLUTION + 1);
	for (int i = 0; i <= RESOLUTION; i++) {
		double t = static_cast<double>(i) / RESOLUTION;
		FVector p = EvalCurvePoint(points, pointWeights, t);
		samples.Add(p);
		cumulativeLength.Add((i == 0) ? 0.0 : cumulativeLength[i - 1] + FVector::Dist(samples[i - 1], p));
	}

	double totalLen = cumulativeLength.Last();
	if (totalLen <= 0.0) return false;

	int nLow = FMath::CeilToInt(totalLen / (1.2 * primLen));
	int nHigh = FMath::FloorToInt(totalLen / (0.8 * primLen));
	if (nLow > nHigh || nLow <= 0) return false;

	int n = FMath::Clamp(FMath::RoundToInt(totalLen / primLen), nLow, nHigh);
	double actualSegLen = totalLen / n;
	segmentCount = n;

	FVector meshSize = FVector(1.0, 1.0, 1.0);
	double scale = actualSegLen / primLen;
	if (axis == 1) meshSize.X = scale;
	else if (axis == 2) meshSize.Y = scale;
	else meshSize.Z = scale;

	TArray<float> params;
	params.Reserve(n + 1);
	for (int k = 0; k <= n; k++) {
		params.Add(FindParamAtLength(cumulativeLength, k * actualSegLen));
	}

	for (int k = 0; k < n; k++) {
		float t0 = params[k];
		float t1 = params[k + 1];

		starts.Add(EvalCurvePoint(points, pointWeights, t0));
		ends.Add(EvalCurvePoint(points, pointWeights, t1));
		startTangents.Add(EvalCurveTangent(points, pointWeights, t0).GetSafeNormal() * actualSegLen);
		endTangents.Add(EvalCurveTangent(points, pointWeights, t1).GetSafeNormal() * actualSegLen);
		sizes.Add(meshSize);
	}

	return true;
}