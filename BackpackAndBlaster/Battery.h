// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ProjectUnrest/GAS/PUGameplayAbility.h"
#include "Battery.generated.h"


class UNiagaraSystem;


/**
 *	An actor that is a battery with a boolean charge. Has gameplay ability and VFX related to when the battery is discharged.
*/
UCLASS()
class PROJECTUNREST_API ABattery : public AActor
{
	GENERATED_BODY()
	
public:	
	ABattery();	

	/*
	*	The calling function for recharging battery. 
	*	Turns on the battery emissive material and sets has charge to true.
	*	Then calls Recharge_BP
	*/
	UFUNCTION(BlueprintCallable, Category = "Battery")
	void Recharge_Exec();

	/*
	 *	Only Recharge_Exec should call this function.
	 *	Uses timeline to animate recharge.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Battery")
	void Recharge_BP();


	/*
	*	The calling function for discharging battery.
	*	Turns off the battery emissive material and sets has charge to false.
	*	Then calls Discharge_BP
	*/	
	UFUNCTION(BlueprintCallable, Category = "Battery")
	void Discharge_Exec();

	/*
	 *	Only Discharge_Exec should call this function.
	 *	Uses timeline to animate discharge.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Battery")
	void Discharge_BP();



	#pragma region === Accessors ===

	UFUNCTION(BlueprintCallable, Category = "Battery")
	FGameplayTag GetBatteryTypeTag() const;

	UFUNCTION(BlueprintCallable, Category = "Battery")
	bool HasCharge() const;

	UFUNCTION(BlueprintCallable, Category = "Battery")
	TSubclassOf<UPUGameplayAbility> GetDischargeAbility() const;

	UFUNCTION(BlueprintCallable, Category = "Battery")
	UNiagaraSystem* GetDischargeBeamVFX() const;

	UFUNCTION(BlueprintCallable, Category = "Battery")
	UNiagaraSystem* GetDischargeMuzzleVFX() const;

	UFUNCTION(BlueprintCallable, Category = "Battery")
	const FLinearColor& GetVisualsColor() const;

	/* Returns the active material depending on if has charge */
	UMaterialInstance* GetActiveMaterial() const;


#pragma endregion


protected:
	/* The static mesh of the battery */
	UPROPERTY(EditDefaultsOnly, Category = "Battery")
	UStaticMeshComponent* BatteryMesh = nullptr;


private:
	/* An identifier for this battery type */
	UPROPERTY(EditDefaultsOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	FGameplayTag BatteryTypeTag = FGameplayTag::EmptyTag;

	/* The ability unique to this battery type that is activated when it is discharged from the blaster */
	UPROPERTY(EditDefaultsOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPUGameplayAbility> DischargeAbility = nullptr;

	/* The material for when the battery has charge */
	UPROPERTY(EditDefaultsOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* ChargedMaterial = nullptr;
	
	/* The material for when the battery has no charge */
	UPROPERTY(EditDefaultsOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* DischargedMaterial = nullptr;

	/* The Niagara VFX that travels along the hitscan when the battery is discharged */
	UPROPERTY(EditDefaultsOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* DischargeBeamVFX;

	/* The muzzle flash Niagara VFX that is mounted to the blaster muzzle socket */
	UPROPERTY(EditDefaultsOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* DischargeMuzzleVFX;

	/* The color used for simple parameterized visuals */
	UPROPERTY(EditDefaultsOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	FLinearColor VisualsColor;

	/* Whether the battery has charge and can be discharged */
	bool bHasCharge = true;

	/* The material slot index for the emissive material that changes when the charge state changes */
	int32 EmissiveMaterialSlotIndex = 1;
};
