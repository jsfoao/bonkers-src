#pragma once
#include "Bonk/Libraries/BonkMacros.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BonkItemInteractComponent.generated.h"

class UBonkItemInteractableComponent;

UCLASS(ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent))
class BONK_API UBonkItemInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBonkItemInteractComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere)
	bool bDebug = false;
	
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* SkeletalComponentAttach;

	UPROPERTY(BlueprintReadOnly)
	UBonkItemInteractableComponent* SelectedInteractable;

	UPROPERTY(BlueprintReadOnly)
	UBonkItemInteractableComponent* CurrentInteractable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.f, ClampMax = 1.f))
	float DotWeight = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.f, ClampMax = 1.f))
	float DotProductLimits = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ClosenessThreshold = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxInteractSpeed = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Location offset from player when grabbing non swingable object"))
	FVector TwoHandedGrabOffset = FVector(50.f, 0.f, 20.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector TwoHandedGrabAttachLocation;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> InteractableChannel;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> BlockObjects;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Range = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrowBaseStrength = 7500.f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsThrowing = false;
	
	UPROPERTY(BlueprintReadOnly)
	float ThrowMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrowMultiplierIncrementSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrowHeight;
	
	UPROPERTY(BlueprintReadOnly)
	class USphereComponent* SphereCollider;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHolding;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetSkeletalComponent(USceneComponent* SkeletalMesh);

	UFUNCTION()
	UBonkItemInteractableComponent* MaxScoreValidInteractable(TArray<FHitResult> HitResults);
	
	UFUNCTION()
	bool IsBlocked(const FVector Location) const;

	UFUNCTION(BlueprintCallable, meta=(ToolTip="TryPickup will also be called if interactable is dynamic"))
	bool TryAction();
	
	UFUNCTION(BlueprintCallable)
	bool TryPickup();

	UFUNCTION(BlueprintCallable)
	bool TryDrop(FVector Force);

	UFUNCTION(BlueprintCallable)
	void StartThrow();

	UFUNCTION(BlueprintCallable)
	void Throw(FVector Direction);

	UFUNCTION(BlueprintCallable)
	void ResetThrow();

	UFUNCTION()
	void ThrowTimer(float DeltaTime);
	
	// EVENTS

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAction, UBonkItemInteractableComponent*, Interactable);
	UPROPERTY(BlueprintAssignable)
	FOnAction OnAction;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickup, UBonkItemInteractableComponent*, Interactable);
	UPROPERTY(BlueprintAssignable)
	FOnPickup OnPickup;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrop, UBonkItemInteractableComponent*, Interactable);
	UPROPERTY(BlueprintAssignable)
	FOnPickup OnDrop;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FonStartThrow);
	UPROPERTY(BlueprintAssignable)
	FonStartThrow OnStartThrow;
};
