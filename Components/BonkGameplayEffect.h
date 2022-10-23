#pragma once
#include "UObject/Object.h"
#include "BonkGameplayEffect.generated.h"

UENUM()
enum EModifierOperation
{
	Add,
	Subtract,
	Multiply,
	Divide,
	Override,
	OverrideBase
};

UENUM(BlueprintType)
enum EEffectTier
{
	Low,
	Mid,
	High
};

// Instant will effect modifiers on bind
UENUM(BlueprintType)
enum EEffectType
{
	Instant,
	Gameplay,
	Both
};

class UBonkPlayer;
class UBonkAbilityComponent;
class UBonkAttribute;
class UBonkAttributeSet;
class ABonkPlayerPawn;

USTRUCT(BlueprintType)
struct FAttributeModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBonkAttribute> AttributeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TEnumAsByte<EModifierOperation> Operation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Value;
};

UCLASS(BlueprintType, Blueprintable)
class BONK_API UBonkGameplayEffect : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EEffectTier> Tier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(ToolTip="Instant: Called when binded; Gameplay: No data modifiers;"))
	TEnumAsByte<EEffectType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttributeModifier> Modifiers;

	UPROPERTY()
	float DeltaSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActiveTick = true;

	UPROPERTY(BlueprintReadOnly)
	UBonkPlayer* PlayerData;

private:
	UPROPERTY()
	UWorld* World;

public:
	UFUNCTION(BlueprintCallable)
	void Bind(UBonkPlayer* NewPlayerData);

	UFUNCTION(BlueprintCallable)
	void Unbind();

	UFUNCTION(BlueprintCallable)
	void ModifierEffect();

	UFUNCTION(BlueprintCallable, meta=(ToolTip="Only works with Add and Subtract modifiers"))
	void ReverseModifierEffect();

	UFUNCTION(BlueprintCallable)
	void StartTick()
	{
		bActiveTick = true;
	}

	UFUNCTION(BlueprintCallable)
	void StopTick()
	{
		bActiveTick = false;
	}

	// To call on player events
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttack(ABonkPlayerPawn* Pawn);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHit(ABonkPlayerPawn* Pawn, AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPickup(ABonkPlayerPawn* Pawn, AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDrop(ABonkPlayerPawn* Pawn, AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent)
	void EffectTick(ABonkPlayerPawn* Pawn, float DeltaTime);

	UFUNCTION()
	void SuperEffectTick(ABonkPlayerPawn* Pawn, float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UWorld* GetWorldContext() { return World; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDeltaSeconds() { return DeltaSeconds; };
};
