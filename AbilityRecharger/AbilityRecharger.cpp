// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUnrest/GAS/Abilities/AbilityRecharger.h"
#include "AbilitySystemComponent.h"
#include "ProjectUnrest/GAS/PUGameplayEffect.h"



void UAbilityRecharger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	RechargeHandle = Handle;
	RechargeActorInfo = *ActorInfo;
	RechargeActivationInfo = ActivationInfo;

	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (AbilitySystemComponent->HasMatchingGameplayTag(RechargeTag))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}


	bool bFoundChargesAttribute = false;
	float Charges = AbilitySystemComponent->GetGameplayAttributeValue(ChargesAttribute, bFoundChargesAttribute);
	ensure(bFoundChargesAttribute);

	bool bFoundMaxChargesAttribute = false;
	float MaxCharges = AbilitySystemComponent->GetGameplayAttributeValue(MaxChargesAttribute, bFoundMaxChargesAttribute);
	ensure(bFoundMaxChargesAttribute);


	if (Charges == MaxCharges)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}


	AbilitySystemComponent->AddLooseGameplayTag(RechargeTag);

	SetRechargeTimer(AbilitySystemComponent);
}

void UAbilityRecharger::SetRechargeTimer(UAbilitySystemComponent* AbilitySystemComponent)
{
	bool bFoundRechargeDurationAttribute = false;
	float RechargeDuration = AbilitySystemComponent->GetGameplayAttributeValue(RechargeDurationAttribute, bFoundRechargeDurationAttribute);
	ensure(bFoundRechargeDurationAttribute);

	UWorld* World = GetWorld();
	checkf(World != nullptr, TEXT("World is nullptr"));

	FTimerManager& TimerManager = World->GetTimerManager();
	FTimerDelegate TimerDelegate;

	TimerDelegate.BindUFunction(this, FName("ExecuteRecharge"), RechargeHandle, RechargeActorInfo, RechargeActivationInfo);

	TimerManager.SetTimer(RechargeTimer, TimerDelegate, 1, false, RechargeDuration);
}

void UAbilityRecharger::ExecuteRecharge()
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	ApplyEffectToIncrementCharge(AbilitySystemComponent);


	bool bFoundChargesAttribute = false;
	float Charges = AbilitySystemComponent->GetGameplayAttributeValue(ChargesAttribute, bFoundChargesAttribute);
	ensure(bFoundChargesAttribute);

	bool bFoundMaxChargesAttribute = false;
	float MaxCharges = AbilitySystemComponent->GetGameplayAttributeValue(MaxChargesAttribute, bFoundMaxChargesAttribute);
	ensure(bFoundMaxChargesAttribute);


	if (Charges == MaxCharges)
	{
		RechargeTimer.Invalidate();
		AbilitySystemComponent->RemoveLooseGameplayTag(RechargeTag);
		EndAbility(RechargeHandle, &RechargeActorInfo, RechargeActivationInfo, true, false);

		return;
	}

	SetRechargeTimer(AbilitySystemComponent);
}

void UAbilityRecharger::ApplyEffectToIncrementCharge(UAbilitySystemComponent* AbilitySystemComponent) const
{
	UPUGameplayEffect* GEIncrementCharge = NewObject<UPUGameplayEffect>(GetTransientPackage());

	GEIncrementCharge->DurationPolicy = EGameplayEffectDurationType::Instant;

	const int32 ModIndex = GEIncrementCharge->Modifiers.Num();
	GEIncrementCharge->Modifiers.SetNum(ModIndex + 1);

	FGameplayModifierInfo& StackModifier = GEIncrementCharge->Modifiers[ModIndex];
	StackModifier.Attribute = ChargesAttribute;
	StackModifier.ModifierOp = EGameplayModOp::Additive;
	StackModifier.ModifierMagnitude = FScalableFloat(1);

	AbilitySystemComponent->ApplyGameplayEffectToSelf(GEIncrementCharge, 1, AbilitySystemComponent->MakeEffectContext());
}
