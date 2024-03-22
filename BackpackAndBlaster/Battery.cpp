// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUnrest/Actors/Battery.h"



ABattery::ABattery()
{
	PrimaryActorTick.bCanEverTick = false;

	if (!BatteryMesh)
	{
		BatteryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Battery Mesh"));
		SetRootComponent(BatteryMesh);
	}
}


void ABattery::Recharge_Exec()
{
	if (bHasCharge)
	{
		return;
	}

	bHasCharge = true;

	check(BatteryMesh);

	BatteryMesh->SetMaterial(EmissiveMaterialSlotIndex, ChargedMaterial);

	Recharge_BP();
}


void ABattery::Discharge_Exec()
{
	if (!bHasCharge)
	{
		return;
	}

	bHasCharge = false;

	check(BatteryMesh);

	BatteryMesh->SetMaterial(EmissiveMaterialSlotIndex, DischargedMaterial);

	Discharge_BP();
}


#pragma region === Accessors ===

FGameplayTag ABattery::GetBatteryTypeTag() const
{
	return BatteryTypeTag;
}

bool ABattery::HasCharge() const
{
	return bHasCharge;
}

TSubclassOf<UPUGameplayAbility> ABattery::GetDischargeAbility() const
{
	return DischargeAbility;
}

UNiagaraSystem* ABattery::GetDischargeBeamVFX() const
{
	return DischargeBeamVFX;
}

UNiagaraSystem* ABattery::GetDischargeMuzzleVFX() const
{
	return DischargeMuzzleVFX;
}

UMaterialInstance* ABattery::GetActiveMaterial() const
{
	return bHasCharge ? ChargedMaterial : DischargedMaterial;
}

const FLinearColor& ABattery::GetVisualsColor() const
{
	return VisualsColor;
}

#pragma endregion