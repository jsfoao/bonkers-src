#include "BonkBonkerComponent.h"
#include "BonkBonkableComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UBonkBonkerComponent::UBonkBonkerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBonkBonkerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBonkBonkerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bCanBonk)
		return;
	
	if (!bTracingIsActive)
		return;

	TickExecuteTracing(DeltaTime);
}

TArray<FHitResult> UBonkBonkerComponent::ExecuteTracing()
{
	TArray<FHitResult> CurrentHitResults;
	return CurrentHitResults;
}

void UBonkBonkerComponent::ActivateTracing(bool bActivate)
{
	bTracingIsActive = bActivate;
}

// To use on BeginPlay
void UBonkBonkerComponent::SetTraceLocationFromSceneComps(USceneComponent* TraceStartScene,
	USceneComponent* TraceEndScene)
{
	TraceStartComp = TraceStartScene;
	TraceEndComp = TraceEndScene;
}

// To use on tick
void UBonkBonkerComponent::SetTraceLocations(FVector NewTraceStart, FVector NewTraceEnd)
{
	TraceStart = NewTraceStart;
	TraceEnd = NewTraceEnd;
}

void UBonkBonkerComponent::HitInternal(UBonkBonkableComponent* BonkableComponent, FHitResult HitResult)
{
	OnHit.Broadcast(BonkableComponent, HitResult);
}

void UBonkBonkerComponent::TickExecuteTracing(float DeltaTime)
{
	TraceStart = TraceStartComp->GetComponentLocation();
	TraceEnd = TraceEndComp->GetComponentLocation();
	
	TArray<FHitResult> CurrentHitResults = ExecuteTracing();
	if (CurrentHitResults.Num() == 0)
	{
		if (HitBonkables.Num() >= 1)
		{
			HitBonkables.Empty();
		}
		return;
	}

	for (auto const CurrentHitResult : CurrentHitResults)
	{
		// Current Hit bonkable
		UBonkBonkableComponent* CurrentBonkable = CurrentHitResult.GetActor()->FindComponentByClass<UBonkBonkableComponent>();
		if (CurrentBonkable == nullptr)
			return;

		// If hit bonkable has no element then CurrentHitBonkable is unique and added
		if (HitBonkables.Num() == 0)
		{
			FHitBonkable* TempBonkable = new FHitBonkable(CurrentBonkable, CurrentHitResult, ResetTime);
			HitBonkables.Add(TempBonkable);
			
			// // Call event
			// HitInternal(CurrentBonkable, CurrentHitResult);
			// CurrentBonkable->HitResponseInternal(this, CurrentHitResult);
			// continue;
		}
		
		// Doesn't need to ensure uniqueness if there's only one hit result
		if (HitBonkables.Num() == 1)
			continue;
		
		// Ensure unique hit bonkables
		for (auto const HitBonkable: HitBonkables)
		{
			if (CurrentBonkable != HitBonkable->Comp)
			{
				FHitBonkable* TempBonkable = new FHitBonkable(CurrentBonkable, CurrentHitResult, ResetTime);
				HitBonkables.Add(TempBonkable);		
				// // Call event
				// HitInternal(CurrentBonkable, CurrentHitResult);
				// CurrentBonkable->HitResponseInternal(this, CurrentHitResult);
			}
		}
	}

	for (const auto HitBonkable : HitBonkables)
	{
		if (HitBonkable->Timer <= 0)
		{
			HitBonkable->Timer = ResetTime;
		}

		if (HitBonkable->Timer == ResetTime)
		{
			HitInternal(HitBonkable->Comp, HitBonkable->HitResult);
			HitBonkable->Comp->HitResponseInternal(this, HitBonkable->HitResult);
		}
		
		HitBonkable->Timer -= DeltaTime;
	}
}

