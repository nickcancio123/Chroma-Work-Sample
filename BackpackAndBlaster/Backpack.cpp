// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUnrest/Actors/Backpack.h"
#include "ProjectUnrest/Actors/Battery.h"
#include "ProjectUnrest/GAS/PUAbilitySystemComponent.h"


ABackpack::ABackpack()
{
	PrimaryActorTick.bCanEverTick = false;

	BackpackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackpackMesh"));
}


void ABackpack::Init(ACharacter* _OwningCharacter, UPUAbilitySystemComponent* _ASC)
{
	check(_OwningCharacter);
	check(_ASC);

	OwnerCharacter = _OwningCharacter;
	OwnerASC = _ASC;

	CurrentBatteryChangedEvent.AddDynamic(this, &ABackpack::UpdateEmissiveMaterial);

	CreateInitialBatteries();
	CountBatteryTypes();
}


void ABackpack::Rechamber_Exec()
{
	if (CurrentBatteryIndex == OwnedBatteriesCount - 1)
	{
		ActivateReloadAbility();
		return;
	}

	Rechamber_BP();
}

void ABackpack::Rechamber_CPP()
{
	++CurrentBatteryIndex;

	if (CurrentBatteryChangedEvent.IsBound())
	{
		CurrentBatteryChangedEvent.Broadcast();
	}
}


void ABackpack::Reload_Exec()
{
	Reload_BP();
}

void ABackpack::Reload_CPP()
{
	CurrentBatteryIndex = 0;

	for (auto Battery : OwnedBatteries)
	{
		Battery->Recharge_Exec();
	}

	if (CurrentBatteryChangedEvent.IsBound())
	{
		CurrentBatteryChangedEvent.Broadcast();
	}
}


void ABackpack::InsertNewBattery(TSubclassOf<ABattery> NewBatteryClass, int32 ChamberIndex)
{
	check(NewBatteryClass);
	check(ChamberIndex >= 0 && ChamberIndex < OwnedBatteriesCount);
	
	// Clamp for release build
	ChamberIndex = FMath::Clamp(ChamberIndex, 0, OwnedBatteriesCount - 1);

	UWorld* World = GetWorld();
	check(World);


	// Uncount old battery and destroy
	FGameplayTag OldType = OwnedBatteries[ChamberIndex]->GetBatteryTypeTag();
	int32 OldTypeCount = *BatteryTypeCounts.Find(OldType) - 1;
	BatteryTypeCounts.Emplace(OldType, OldTypeCount);

	OwnedBatteries[ChamberIndex]->Destroy();


	// Create new battery and add to count
	ABattery* NewBattery = Cast<ABattery>(World->SpawnActor(NewBatteryClass));
	OwnedBatteries[ChamberIndex] = NewBattery;

	FGameplayTag NewType = OwnedBatteries[ChamberIndex]->GetBatteryTypeTag();
	int32* NewTypeCount = BatteryTypeCounts.Find(NewType);

	if (NewTypeCount == nullptr)
	{
		BatteryTypeCounts.Add(NewType, 1);
	}
	else
	{
		BatteryTypeCounts.Emplace(NewType, *NewTypeCount + 1);
	}

	AttachBatteryToSocket(NewBattery, ChamberIndex);


	if (CurrentBatteryIndex == ChamberIndex && CurrentBatteryChangedEvent.IsBound())
	{
		CurrentBatteryChangedEvent.Broadcast();
	}
}

void ABackpack::SwapOwnedBatteries(int32 FirstBatteryIndex, int32 SecondBatteryIndex)
{
	check(FirstBatteryIndex >= 0 && FirstBatteryIndex < OwnedBatteriesCount);
	check(SecondBatteryIndex >= 0 && SecondBatteryIndex < OwnedBatteriesCount);

	// Clamp for release build
	FirstBatteryIndex = FMath::Clamp(FirstBatteryIndex, 0, OwnedBatteriesCount - 1);
	SecondBatteryIndex = FMath::Clamp(SecondBatteryIndex, 0, OwnedBatteriesCount - 1);

	ABattery* TempBatteryPtr = OwnedBatteries[FirstBatteryIndex];
	OwnedBatteries[FirstBatteryIndex] = OwnedBatteries[SecondBatteryIndex];
	OwnedBatteries[SecondBatteryIndex] = TempBatteryPtr;

	AttachBatteryToSocket(OwnedBatteries[FirstBatteryIndex], FirstBatteryIndex);
	AttachBatteryToSocket(OwnedBatteries[SecondBatteryIndex], SecondBatteryIndex);


	bool bSwappedCurrentBattery = CurrentBatteryIndex == FirstBatteryIndex || CurrentBatteryIndex == SecondBatteryIndex;
	if (bSwappedCurrentBattery && CurrentBatteryChangedEvent.IsBound())
	{
		CurrentBatteryChangedEvent.Broadcast();
	}
}


void ABackpack::DischargeCurrentBattery()
{
	ABattery* CurrentBattery = OwnedBatteries[CurrentBatteryIndex];
	check(CurrentBattery);

	CurrentBattery->Discharge_Exec();

	if (CurrentBatteryChangedEvent.IsBound())
	{
		CurrentBatteryChangedEvent.Broadcast();
	}
}


#pragma region === Accessors ===

const ABattery* ABackpack::GetCurrentBattery() const
{
	return OwnedBatteries[CurrentBatteryIndex];
}

const ABattery* ABackpack::GetBatteryAtIndex(int32 Index) const
{
	if (Index < 0 || Index >= OwnedBatteries.Num())
	{
		return nullptr;
	}

	return OwnedBatteries[Index];
}

int32 ABackpack::GetCurrentBatteryIndex() const
{
	return CurrentBatteryIndex;
}

int32 ABackpack::GetNextBatteryIndex() const
{
	int32 NextBatteryIndex = CurrentBatteryIndex + 1;
	
	if (NextBatteryIndex >= OwnedBatteriesCount)
	{
		NextBatteryIndex = 0;
	}
	
	return NextBatteryIndex;
}

const int32 ABackpack::GetCurrentBatteryCount() const
{
	const ABattery* CurrentBattery = GetCurrentBattery();
	check(CurrentBattery);

	const int* CountPtr = BatteryTypeCounts.Find(CurrentBattery->GetBatteryTypeTag());

	return CountPtr ? *CountPtr : 0;
}

#pragma endregion


void ABackpack::CreateInitialBatteries()
{
	check(OwnedBatteriesCount > 0);
	check(InitialOwnedBatteryTypes.Num() == OwnedBatteriesCount);

	OwnedBatteries.SetNum(OwnedBatteriesCount);

	UWorld* World = GetWorld();
	check(World);

	for (int32 i = 0; i < OwnedBatteriesCount; i++)
	{
		check(InitialOwnedBatteryTypes[i]);

		if (OwnedBatteries[i] != nullptr)
		{
			OwnedBatteries[i]->Destroy();
		}

		OwnedBatteries[i] = Cast<ABattery>(World->SpawnActor(InitialOwnedBatteryTypes[i]));

		AttachBatteryToSocket(OwnedBatteries[i], i);
	}

	if (CurrentBatteryChangedEvent.IsBound())
	{
		CurrentBatteryChangedEvent.Broadcast();
	}
}

void ABackpack::CountBatteryTypes()
{
	BatteryTypeCounts.Empty();

	for (ABattery* Battery : OwnedBatteries)
	{
		if (!ensure(Battery))
		{
			continue;
		}

		FGameplayTag BatteryTypeTag = Battery->GetBatteryTypeTag();

		int32* TypeCount = BatteryTypeCounts.Find(BatteryTypeTag);

		if (TypeCount == nullptr)
		{
			BatteryTypeCounts.Add(BatteryTypeTag, 1);
		}
		else
		{
			BatteryTypeCounts.Emplace(BatteryTypeTag, *TypeCount + 1);
		}
	}
}

void ABackpack::ActivateReloadAbility()
{
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(ReloadAbility);

	OwnerASC->GiveAbilityAndActivateOnce(AbilitySpec);
}

void ABackpack::UpdateEmissiveMaterial()
{
	const ABattery* CurrentBattery = GetCurrentBattery();
	check(CurrentBattery);

	BackpackMesh->SetMaterial(EmissiveMaterialSlot, CurrentBattery->GetActiveMaterial());
}
