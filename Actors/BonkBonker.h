#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BonkBonker.generated.h"

class ABonkPlayerPawn;
class UBonkItemInteractComponent;
class UBonkItemInteractableComponent;
class UBonkBonkerComponent;

/**
 * Base actor for all dynamic weapons in the game i.e can be picked up and used to attack
 */

UCLASS(Blueprintable)
class BONK_API ABonkBonker : public AActor
{
	GENERATED_BODY()

public:
	ABonkBonker();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	//
	// COMPONENTS
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBonkBonkerComponent* BonkerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBonkItemInteractableComponent* InteractableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshColliderComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* TraceStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* TraceEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KnockbackMultiplier;
	//

	//
	// Tracing
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Speed to start tracing at"))
	float SpeedTraceThreshold = 200.f;

	//
	// Weapon size
	//
	UPROPERTY(BlueprintReadWrite)
	float BaseSize;

	UPROPERTY(BlueprintReadWrite)
	float TargetSize;

	UPROPERTY(BlueprintReadWrite)
	float CurrentSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SizeInterpSpeed = 5.f;

	UPROPERTY(BlueprintReadWrite)
	float BaseTraceRadius;
	//
	
	//
	// Materials
	//
	UPROPERTY(BlueprintReadOnly)
	TArray<UMaterialInstanceDynamic*> MaterialInstances;
	UPROPERTY(BlueprintReadOnly)
	TArray<FLinearColor> BaseColors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MatParameter = "Param";
	//
	
private:
	UPROPERTY()
	ABonkPlayerPawn* HoldingPlayer;

public:
	// Callbacks
	UFUNCTION()
	void Pickup_Callback(UBonkItemInteractComponent* InteractComp);

	UFUNCTION()
	void Drop_Callback(UBonkItemInteractComponent* InteractComp);

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStaticMeshComponent* GetColliderComp() const
	{
		return MeshColliderComp;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure) const
	UStaticMeshComponent* GetMeshComp()
	{
		return MeshComp;
	}
};
