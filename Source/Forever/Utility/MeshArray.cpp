#include "MeshArray.h"


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