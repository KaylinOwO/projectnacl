#pragma once
#include "SDK.h"

class IMaterial;
typedef IMaterial* Matptr;

class CMat
{
public:
	Matptr shaded, shiny, glow, wireframe;
	Matptr shadedother, shinyother, glowother, wireframeother;
	void Initialize();
	void InititalizeOther();

	Matptr CreateMaterial(bool IgnoreZ, bool Flat, bool Wireframe = false, bool Shiny = false);
	Matptr CreateMaterialOther(bool IgnoreZ, bool Flat, bool Wireframe = false, bool Shiny = false);
	void ForceMaterial(SColor Color, Matptr Material, bool ForceColor = true, bool ForceMat = true);
	void ResetMaterial();
};
extern CMat gMat;