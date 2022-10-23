#pragma once
#include "CoreMinimal.h"
#include "Bonk/Player/BonkPlayerPawn.h"
#include "Components/ActorComponent.h"
#include "BonkBonkableComponent.generated.h"

class UBonkBonkerComponent;
class ABonkPlayerPawn;
class UBonkPlayer;

/**
 * Component response triggered by bonkers
 */
UCLASS(ClassGroup=(Bonking), meta=(BlueprintSpawnableComponent))
class BONK_API UBonkBonkableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBonkBonkableComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* PrimitiveComp;

	UPROPERTY(EditAnywhere, meta=(ToolTip="Similar to damage values, only affects player"))
	float BaseHealthKnockback = 25.f;
	
	UPROPERTY(EditAnywhere)
	float KnockbackHeight = 1.f;
	
	UPROPERTY(EditAnywhere)
	float KnockbackMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.f, ClampMax=1.f))
	float PlayerForwardWeight = 0.5f;

	UPROPERTY(BlueprintReadOnly)
	UBonkBonkerComponent* LastBonkerHit;

	UPROPERTY(BlueprintReadOnly)
	ABonkPlayerPawn* LastPlayerHit;

	UPROPERTY(BlueprintReadWrite)
	bool bIsEnabled = true;
	
public:
	UFUNCTION(BlueprintCallable)
    void SetPrimitiveComponent(UPrimitiveComponent* PrimitiveComponent);
	
	UFUNCTION(BlueprintCallable)
	void KnockbackFromHit(FHitResult Hit, float Multiplier);

	UFUNCTION(BlueprintCallable)
	void KnockbackFromDirectionAndHit(FVector HorizontalForce, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void KnockbackFromDirection(FVector HorizontalForce, float Multiplier);
	
	UFUNCTION()
	void HitResponseInternal(UBonkBonkerComponent* BonkerComponent, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void EnableBonkable(bool bActive);

	// Called on any hit (player and objects)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitResponse, UBonkBonkerComponent*, BonkerComponent, FHitResult, Hit);
	UPROPERTY(BlueprintAssignable)
	FOnHitResponse OnHitResponse;

	// Called on player hits
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHitResponsePlayer, UBonkBonkerComponent*, BonkerComponent, ABonkPlayerPawn*, Pawn, UBonkPlayer*, PlayerInfo, FHitResult, Hit);
	UPROPERTY(BlueprintAssignable)
	FOnHitResponsePlayer OnHitResponsePlayer;

	// Called when hit and damaged
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamagedHitResponse, UBonkBonkerComponent*, BonkerComponent, FHitResult, Hit);
	UPROPERTY(BlueprintAssignable)
	FOnDamagedHitResponse OnDamagedHitResponse;
};
