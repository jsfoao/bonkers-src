#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BonkItemInteractableComponent.generated.h"

class UBonkItemInteractComponent;

UENUM()
enum InteractableType
{
	Static, Dynamic
};

UCLASS(ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent))
class BONK_API UBonkItemInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBonkItemInteractableComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Dynamic interactables can be picked up
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<InteractableType> Type = Dynamic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanSwing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SwingSpeedMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TorqueOnDrop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Socket to attach to"))
	FName SocketName = "RightHandSocket";

	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* PrimitiveComp;
	
	// UPROPERTY(BlueprintReadOnly)
	// int SelectedCount = 0;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsHeld;

	UPROPERTY(BlueprintReadOnly)
	UBonkItemInteractComponent* HeldByInteract;

	UPROPERTY(BlueprintReadOnly)
	UBonkItemInteractComponent* LastHeldByInteract;
	
public:
	UFUNCTION(BlueprintCallable, meta=(ToolTip="Only needed for dynamic interactables"))
	void SetPrimitiveComponent(UPrimitiveComponent* PrimitiveComponent);
	
	UFUNCTION()
	void TrySelect(UBonkItemInteractComponent* InteractComp);

	UFUNCTION()
	void TryDeselect(UBonkItemInteractComponent* InteractComp);

	// Can be called by both static and dynamic interactables
	UFUNCTION()
	void Action(UBonkItemInteractComponent* InteractComp);
	
	// Only called by dynamic interactables
	UFUNCTION()
	void Pickup(UBonkItemInteractComponent* InteractComp);

	UFUNCTION()
	void Drop(UBonkItemInteractComponent* InteractComp, FVector Force);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelect);
	UPROPERTY(BlueprintAssignable)
	FOnSelect OnSelect;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeselect);
	UPROPERTY(BlueprintAssignable)
	FOnDeselect OnDeselect;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupResponse, UBonkItemInteractComponent*, InteractComp);
	UPROPERTY(BlueprintAssignable)
	FOnPickupResponse OnPickupResponse;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDropResponse, UBonkItemInteractComponent*, InteractComp);
	UPROPERTY(BlueprintAssignable)
	FOnDropResponse OnDropResponse;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionResponse, UBonkItemInteractComponent*, InteractComp);
	UPROPERTY(BlueprintAssignable)
	FOnActionResponse OnActionResponse;
};
