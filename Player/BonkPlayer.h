// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bonk/Components/BonkGameplayEffect.h"
#include "UObject/Object.h"
#include "BonkPlayer.generated.h"

class UBonkAttribute_WeaponSize;
class UBonkAttribute_Resilience;
class UBonkAttribute_MoveSpeed;
class UBonkAttribute_Damage;
class UBonkAttribute_AttackSpeed;
class UBonkAttribute_Health;

USTRUCT(BlueprintType)
struct FMoneySource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SourceText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;

	FMoneySource()
	{
		Amount = 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyUpdated, int32, NewAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdated, int32, NewHealth);

/**
 * The object that holds all the data for each player such as Player Index, Team, bought upgrades e.t.
 */
UCLASS(BlueprintType, Blueprintable)
class BONK_API UBonkPlayer : public UObject
{
	GENERATED_BODY()

public:

	UBonkPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TeamIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PlayerIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLockedIn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAlive;

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerMoney;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int KillCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UBonkPlayer*> KilledPlayers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BountyCount;

	// Used by the shop widget to do a count up animation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InitialRoundMoney;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMoneySource> MoneySources;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UBonkGameplayEffect*> Effects;
	UPROPERTY(BlueprintReadWrite)
	UWorld* World;
	UPROPERTY()
	class ABonkPlayerPawn* PlayerPawn;

	// ATTRIBUTES
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBonkAttribute_Health* Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBonkAttribute_AttackSpeed* AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBonkAttribute_Damage* Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBonkAttribute_MoveSpeed* MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBonkAttribute_Resilience* Resilience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBonkAttribute_WeaponSize* WeaponSize;
	

public:
	UFUNCTION(BlueprintCallable)
	void AddEffect(TSubclassOf<UBonkGameplayEffect> EffectClass);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UBonkAttribute* GetAttribute(TSubclassOf<UBonkAttribute> AttributeClass);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAttributeCurrent(TSubclassOf<UBonkAttribute> AttributeClass);

	UFUNCTION(BlueprintCallable)
	float GetAttributeBase(TSubclassOf<UBonkAttribute> AttributeClass);

	UFUNCTION(BlueprintCallable)
	inline ABonkPlayerPawn* GetPlayerPawn()
	{
		return PlayerPawn;
	};

	UFUNCTION(BlueprintCallable)
	bool CanAfford(int32 Cost);
	
	UFUNCTION(BlueprintCallable)
	void RemoveMoney(int32 Amount);
	
	UFUNCTION(BlueprintCallable)
	void AddMoney(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void AddKill(UBonkPlayer* KilledPlayer);

	UPROPERTY(BlueprintAssignable)
	FOnMoneyUpdated OnMoneyUpdated;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnHealthUpdated OnHealthUpdated;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddedKill, int, NewKillCount);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAddedKill OnAddedKill;
	
};
