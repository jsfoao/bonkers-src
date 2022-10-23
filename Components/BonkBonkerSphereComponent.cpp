#include "BonkBonkerSphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FHitResult> UBonkBonkerSphereComponent::ExecuteTracing()
{
	TArray<FHitResult> CurrentHitResults;
	UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		TraceStart,
		TraceEnd,
		TraceRadius,
		TraceChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		CurrentHitResults,
		true);
	return CurrentHitResults;
}
