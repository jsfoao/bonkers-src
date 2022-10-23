#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BonkBonkerComponent.generated.h"



class UBonkBonkableComponent;
class UBonkPlayer;

USTRUCT()
struct FHitBonkable
{
	GENERATED_BODY()

	UPROPERTY()
	UBonkBonkableComponent* Comp;
	UPROPERTY()
	FHitResult HitResult;
	UPROPERTY()
	float Timer;
	
	FHitBonkable()
	{
		Comp = nullptr;
		HitResult = FHitResult();
		Timer = 0.f;
	};
	
	FHitBonkable(UBonkBonkableComponent* NewComp, FHitResult NewHitResult, float NewTimer)
	{
		Comp = NewComp;
		HitResult = NewHitResult;
		Timer = NewTimer;
	}
};

/**
 * Component agent mostly in weapons that triggers bonkables
 */
UCLASS(ClassGroup=(Bonking), meta=(BlueprintSpawnableComponent))
class BONK_API UBonkBonkerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UBonkBonkerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:

	//
	// Stats
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Damage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(ToolTip="1 is default for sword"))
	float AttackSpeed = 1.f;
	
	//
	// Collisions
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
	
	UPROPERTY(BlueprintReadWrite)
	FVector TraceStart;
	
	UPROPERTY(BlueprintReadWrite)
	FVector TraceEnd;
	
	UPROPERTY(BlueprintReadWrite)
	USceneComponent* TraceStartComp;
	
	UPROPERTY(BlueprintReadWrite)
	USceneComponent* TraceEndComp;
	
	UPROPERTY(BlueprintReadOnly)
	bool bTracingIsActive = false;

	UPROPERTY(BlueprintReadWrite)
	bool bCanBonk = true;
	
	TArray<FHitBonkable*> HitBonkables;
	
	UPROPERTY(EditAnywhere)
	float ResetTime = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsToIgnore;
	
	UPROPERTY(BlueprintReadOnly)
	class ABonkPlayerPawn* HoldingPlayer;
	
public:
	UFUNCTION(BlueprintCallable)
	void ActivateTracing(bool bActivate);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABonkPlayerPawn* GetHoldingPlayer() { return HoldingPlayer; }

	// To use on BeginPlay
	UFUNCTION(BlueprintCallable, meta=(ToolTip="Can be called in begin play"))
	void SetTraceLocationFromSceneComps(USceneComponent* TraceStartScene, USceneComponent* TraceEndScene);

	// To use on Tick
	UFUNCTION(BlueprintCallable, meta=(ToolTip="Has to be called on tick"))
	void SetTraceLocations(FVector NewTraceStart, FVector NewTraceEnd);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHit, UBonkBonkableComponent*, BonkableComponent, FHitResult, FHitResult);
	UPROPERTY(BlueprintAssignable)
	FOnHit OnHit;

private:
	virtual TArray<FHitResult> ExecuteTracing();
	
	UFUNCTION()
	void HitInternal(UBonkBonkableComponent* BonkableComponent, FHitResult HitResult);

	UFUNCTION(BlueprintCallable)
	void TickExecuteTracing(float DeltaTime);
};
