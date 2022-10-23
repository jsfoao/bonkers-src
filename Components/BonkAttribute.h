#pragma once
#include "UObject/Object.h"
#include "BonkAttribute.generated.h"

UCLASS(BlueprintType, Blueprintable)
class BONK_API UBonkAttribute : public UObject
{
	GENERATED_BODY()

public:
	UBonkAttribute();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentValue;

	UFUNCTION(BlueprintCallable)
	void Init()
	{
		CurrentValue = BaseValue;
	}

	UFUNCTION(BlueprintCallable)
	void SetBaseValue(float Value)
	{
		BaseValue = Value;
		CurrentValue = BaseValue;
	}

	UFUNCTION(BlueprintCallable)
	void SetCurrentValue(float Value)
	{
		CurrentValue = Value;
	}
};

