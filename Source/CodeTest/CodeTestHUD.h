// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CodeTestHUD.generated.h"

UCLASS()
class ACodeTestHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACodeTestHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;



private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

