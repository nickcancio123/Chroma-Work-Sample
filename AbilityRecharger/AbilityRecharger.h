// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilityRecharger.generated.h"

/**
 *	A base ability that recharges another ability that can stack by incrementing its charges attribute after recharge duration. 
 *	Should be inherited from only to change the attribute fields.
 */
UCLASS()
class PROJECTUNREST_API UAbilityRecharger : public UGameplayAbility
{
	GENERATED_BODY()


protected:
	/** The meta attribute holding the current number of ability uses left */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Recharger")
		FGameplayAttribute ChargesAttribute;

	/** The attribute specifying the maximum charges in a stack */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Recharger")
		FGameplayAttribute MaxChargesAttribute;

	/** The attribute specifying the duration to recharge a single charge */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Recharger")
		FGameplayAttribute RechargeDurationAttribute;

	/** The tag to place on owner actor while it is recharging */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Recharger")
		FGameplayTag RechargeTag;


public:
	/** GameplayAbility callback */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);


private:
	/** These fields are cached because they can't be exposed in a UFUNCTION which is necessary for a timer */
	FTimerHandle RechargeTimer;
	FGameplayAbilitySpecHandle RechargeHandle;
	FGameplayAbilityActorInfo RechargeActorInfo;
	FGameplayAbilityActivationInfo RechargeActivationInfo;

	/** Sets the timer to call ExecuteRecharge after the recharge duration */
	void SetRechargeTimer(UAbilitySystemComponent* AbilitySystemComponent);

	/** Applies effect to increment charges attribute and sets a timer to repeat this if still not at max charges */
	UFUNCTION()
		void ExecuteRecharge();

	/** Creates and applies a GE to increment the charge attribute */
	void ApplyEffectToIncrementCharge(UAbilitySystemComponent* AbilitySystemComponent) const;
};
