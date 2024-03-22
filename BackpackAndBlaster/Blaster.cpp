// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUnrest/Actors/PUBlaster.h"
#include "ProjectUnrest/Actors/Battery.h"
#include "ProjectUnrest/Actors/Backpack.h"
#include "ProjectUnrest/GAS/PUAbilitySystemComponent.h"



APUBlaster::APUBlaster()
{
	PrimaryActorTick.bCanEverTick = false;

	BlasterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlasterMesh"));
	SetRootComponent(BlasterMesh);
}


void APUBlaster::Init(UPUAbilitySystemComponent* _OwnerASC, ABackpack* _Backpack)
{
	check(_OwnerASC);
	check(_Backpack);

	OwnerASC = _OwnerASC;
	Backpack = _Backpack;

	GiveDefaultAbilities();

	Backpack->CurrentBatteryChangedEvent.AddDynamic(this, &APUBlaster::UpdateEmissiveMaterial);

	UpdateEmissiveMaterial();
}

void APUBlaster::Discharge(FHitResult HitScanResult)
{
	FGameplayEventData EventData;

	FGameplayEffectContextHandle GameplayContextHandle = OwnerASC->MakeEffectContext();
	GameplayContextHandle.AddHitResult(HitScanResult);
	GameplayContextHandle.AddSourceObject(this);
	GameplayContextHandle.AddOrigin(GetActorLocation());

	EventData.ContextHandle = GameplayContextHandle;
	EventData.Instigator = Owner;


	const ABattery* CurrentBattery = Backpack->GetCurrentBattery();
	check(CurrentBattery);

	FGameplayAbilitySpec AbilitySpec =
		FGameplayAbilitySpec(CurrentBattery->GetDischargeAbility(), Backpack->GetCurrentBatteryCount());


	OwnerASC->GiveAbilityAndActivateOnce(AbilitySpec, &EventData);

	OwnerASC->HandleGameplayEvent(DischargeEventTag, &EventData);


	Backpack->DischargeCurrentBattery();
}

const ABackpack* APUBlaster::GetBackpack() const
{
	return Backpack;
}



void APUBlaster::GiveDefaultAbilities()
{
	for (TSubclassOf<UPUGameplayAbility> DefaultAbility : DefaultAbilities)
	{
		FGameplayAbilitySpecHandle AbilitySpec = OwnerASC->GiveAbility(FGameplayAbilitySpec(DefaultAbility, 1, 0, this));
		
		ensureMsgf(AbilitySpec.IsValid(), TEXT("Failed to give ability: %s"));
	}
}

void APUBlaster::UpdateEmissiveMaterial()
{
	const ABattery* CurrentBattery = Backpack->GetCurrentBattery();

	check(CurrentBattery);

	if (CurrentBattery)
	{
		BlasterMesh->SetMaterial(EmissiveMaterialSlot, CurrentBattery->GetActiveMaterial());
	}
}
