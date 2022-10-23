#include "BonkItemInteractComponent.h"
#include "BonkItemInteractableComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

class UBonkItemInteractableComponent;

UBonkItemInteractComponent::UBonkItemInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	SelectedInteractable = nullptr;
	ThrowMultiplier = 0.f;
}

void UBonkItemInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBonkItemInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ThrowTimer(DeltaTime);

	if (bIsHolding)
		return;

	TwoHandedGrabAttachLocation = GetOwner()->GetActorLocation() +
		(GetOwner()->GetActorForwardVector() + TwoHandedGrabOffset.X) +
			(GetOwner()->GetActorRightVector() + TwoHandedGrabOffset.Y) +
				(GetOwner()->GetActorUpVector() + TwoHandedGrabOffset.Z);
	
	if (bDebug)
	{
		DRAW_SPHERE(GetOwner()->GetActorLocation(), ClosenessThreshold, FColor::Blue);
		DRAW_SPHERE(TwoHandedGrabAttachLocation, 10.f, FColor::Red);
	}
	
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	bool BlockingHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation() - FVector(0.f, 0.1f, 0.f),
		Range,
		InteractableChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResults,
		true);
	
	if (BlockingHit)
	{
		// Should always return valid interactable
		UBonkItemInteractableComponent* MaxInteractable = MaxScoreValidInteractable(HitResults);


		// No valid selectable interactable but is not selecting
		if (MaxInteractable == nullptr && SelectedInteractable == nullptr)
		{
			return;
		}
		
		// No valid selectable interactable but is selecting
		else if (MaxInteractable == nullptr && SelectedInteractable != nullptr)
		{
			SelectedInteractable->TryDeselect(this);
		}

		// New max interactable when already selecting (Deselect old, select new)
		else if (MaxInteractable != SelectedInteractable && SelectedInteractable != nullptr)
		{
			SelectedInteractable->TryDeselect(this);
			MaxInteractable->TrySelect(this);
		}
		// New max interactable when not selecting (select new)
		else if (SelectedInteractable == nullptr)
		{
			MaxInteractable->TrySelect(this);
		}
	}
	else
	{
		if (SelectedInteractable != nullptr)
		{
			SelectedInteractable->TryDeselect(this);
		}
	}

	if (SelectedInteractable != nullptr)
	{
		if (bDebug)
		{
			DRAW_SPHERE(SelectedInteractable->GetOwner()->GetActorLocation(), 70.f, FColor::Yellow);
		}
	}
}

void UBonkItemInteractComponent::SetSkeletalComponent(USceneComponent* SkeletalMesh)
{
	SkeletalComponentAttach = SkeletalMesh;
}

UBonkItemInteractableComponent* UBonkItemInteractComponent::MaxScoreValidInteractable(TArray<FHitResult> HitResults)
{
	float MaxScore = -9999;
	int MaxIndex = -1;

	const FVector Start = GetOwner()->GetActorLocation();

	for (int i = 0; i < HitResults.Num(); ++i)
	{
		UBonkItemInteractableComponent* Interactable = HitResults[i].GetActor()->FindComponentByClass<UBonkItemInteractableComponent>();
		if (Interactable == nullptr)
			continue;
		
		FVector End = HitResults[i].ImpactPoint;

		FVector Direction = End - Start;
		FVector ProjectedDirection = FVector::VectorPlaneProject(Direction, FVector(0.f, 0.f, 1.f));

		const float DotScore = FVector::DotProduct(GetOwner()->GetActorForwardVector(), ProjectedDirection.GetSafeNormal());
		const float DistanceScore = 1 - (Direction.Length() / Range);

		float TotalScore;

		if (bDebug)
		{
			DRAW_LINE(Start, End, FColor::Red);
			DRAW_SPHERE(Interactable->GetOwner()->GetActorLocation(), 50.f, FColor::Red);
		}
		
		if (Direction.Length() < ClosenessThreshold)
		{
			TotalScore = DistanceScore;

			if (TotalScore > MaxScore)
			{
				MaxScore = TotalScore;
				MaxIndex = i;
			}
			
			continue;
		}
		
		TotalScore = (DistanceScore * (1 - DotWeight)) + (DotScore * DotWeight);
		
		if (Interactable->bIsHeld)
			continue;

		if (DotScore < DotProductLimits)
			continue;

		if (IsBlocked(End))
			continue;

		if (Interactable->Type == Dynamic)
		{
			if (Interactable->PrimitiveComp->GetComponentVelocity().Length() > MaxInteractSpeed)
			{
				continue;
			}
		}
		
		if (TotalScore > MaxScore)
		{
			MaxScore = TotalScore;
			MaxIndex = i;
		}
	}
	
	if (MaxIndex == -1)
		return nullptr;

	UBonkItemInteractableComponent* MaxScoreInteractable = HitResults[MaxIndex].GetActor()->FindComponentByClass<UBonkItemInteractableComponent>();

	if (bDebug)
	{
		DRAW_SPHERE(MaxScoreInteractable->GetOwner()->GetActorLocation(), 50.f, FColor::Green);
		DRAW_LINE(Start, MaxScoreInteractable->GetOwner()->GetActorLocation(), FColor::Green);
	}
	return MaxScoreInteractable;
}

bool UBonkItemInteractComponent::IsBlocked(const FVector Location) const
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		Location,
		BlockObjects,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true);

	return HitResult.bBlockingHit;
}

bool UBonkItemInteractComponent::TryAction()
{
	if (SelectedInteractable == nullptr)
		return false;
	SelectedInteractable->Action(this);
	OnAction.Broadcast(SelectedInteractable);
	return TryPickup();
}

bool UBonkItemInteractComponent::TryPickup()
{
	if (SelectedInteractable == nullptr)
		return false;

	if (SelectedInteractable->bIsHeld || SelectedInteractable->Type != Dynamic)
		return false;
	
	if (CurrentInteractable != nullptr)
	{
		TryDrop(FVector::ZeroVector);
	}
	
	// Successful pickup
	bIsHolding = true;
	CurrentInteractable = SelectedInteractable;
	OnPickup.Broadcast(CurrentInteractable);
	CurrentInteractable->Pickup(this);
	return true;
}

bool UBonkItemInteractComponent::TryDrop(FVector Force)
{
	if (!bIsHolding)
		return false;

	// Successful drop
	bIsHolding = false;
	
	CurrentInteractable->Drop(this, Force);
	OnDrop.Broadcast(CurrentInteractable);
	
	CurrentInteractable->TrySelect(this);
	
	CurrentInteractable = nullptr;
	return true;
}

void UBonkItemInteractComponent::ThrowTimer(float DeltaTime)
{
	if (bIsHolding && bIsThrowing)
	{
		if (ThrowMultiplier < 1)
		{
			ThrowMultiplier += ThrowMultiplierIncrementSpeed * DeltaTime;
			if (ThrowMultiplier > 1)
			{
				ThrowMultiplier = 1;
			}	
		}
	}
}

void UBonkItemInteractComponent::StartThrow()
{
	if (CurrentInteractable == nullptr)
		return;
	bIsThrowing = true;
	OnStartThrow.Broadcast();
}

void UBonkItemInteractComponent::Throw(FVector Direction)
{
	if (!bIsThrowing)
		return;
	const FVector FinalDirection = (Direction + FVector::UpVector * ThrowHeight).GetSafeNormal();
	TryDrop(FinalDirection * ThrowMultiplier * ThrowBaseStrength);
	ResetThrow();
}

void UBonkItemInteractComponent::ResetThrow()
{
	bIsThrowing = false;
	ThrowMultiplier = 0.f;
}

