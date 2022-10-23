#include "BonkBonkableComponent.h"

#include "BonkAttribute_Damage.h"
#include "BonkAttribute_Health.h"
#include "BonkBonkerComponent.h"
#include "Bonk/Player/BonkPlayer.h"
#include "Bonk/Player/BonkPlayerPawn.h"

UBonkBonkableComponent::UBonkBonkableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsEnabled = true;
}

void UBonkBonkableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBonkBonkableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBonkBonkableComponent::KnockbackFromHit(FHitResult Hit, float Multiplier)
{
	const FVector NormalizedHitDirection = FVector::VectorPlaneProject((GetOwner()->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal(), FVector(0,0,1));
	const FVector NormalizedDirection = NormalizedHitDirection.GetSafeNormal();
	PrimitiveComp->AddImpulse(NormalizedDirection * Multiplier + FVector::UpVector * KnockbackHeight);
	UE_LOG(LogTemp,Log, TEXT("Knocked back with %f"),Multiplier);
}

void UBonkBonkableComponent::KnockbackFromDirectionAndHit(FVector HorizontalForce, FHitResult Hit)
{
	const FVector NormalizedHitDirection = FVector::VectorPlaneProject((GetOwner()->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal(), FVector(0,0,1));
	const FVector Force = (NormalizedHitDirection * (1-PlayerForwardWeight)) + (HorizontalForce * PlayerForwardWeight) + FVector::UpVector * KnockbackHeight;
	PrimitiveComp->AddImpulse(Force);
}

void UBonkBonkableComponent::KnockbackFromDirection(FVector HorizontalForce, float Multiplier)
{
	const FVector NormalizedDirection = HorizontalForce.GetSafeNormal();
	const FVector Force = NormalizedDirection.GetSafeNormal();
	PrimitiveComp->AddImpulse(Force * Multiplier + FVector::UpVector * KnockbackHeight);
	UE_LOG(LogTemp,Log, TEXT("Knocked back with %f"), Multiplier);
}

void UBonkBonkableComponent::SetPrimitiveComponent(UPrimitiveComponent* PrimitiveComponent)
{
	PrimitiveComp = PrimitiveComponent;
}

void UBonkBonkableComponent::HitResponseInternal(UBonkBonkerComponent* BonkerComponent, FHitResult Hit)
{
	if (!bIsEnabled)
		return;
	
	ABonkPlayerPawn* BonkerPlayer = BonkerComponent->GetHoldingPlayer();
	ABonkPlayerPawn* HitPlayer = Cast<ABonkPlayerPawn>(GetOwner());
	
	// Object hit Object 
	if (BonkerPlayer == nullptr && HitPlayer == nullptr)
	{
		OnHitResponse.Broadcast(BonkerComponent, Hit);
		return;
	}

	// Player hit object
	if (BonkerPlayer != nullptr && HitPlayer == nullptr)
	{
		KnockbackFromDirection(BonkerPlayer->GetActorForwardVector(), BonkerComponent->Damage * KnockbackMultiplier);
		OnHitResponse.Broadcast(BonkerComponent, Hit);
		return;
	}

	// Object hit player
	if (BonkerPlayer == nullptr && HitPlayer != nullptr)
	{
		const float TotalDamage = BonkerComponent->Damage;
		HitPlayer->ApplyDamage(KnockbackMultiplier * TotalDamage, BonkerComponent);
		
		const float ForceToApply = KnockbackMultiplier * TotalDamage * (BaseHealthKnockback + HitPlayer->PlayerInfo->Health->CurrentValue);
		KnockbackFromHit(Hit, ForceToApply);

		LastBonkerHit = BonkerComponent;
		
		OnHitResponse.Broadcast(BonkerComponent, Hit);

		if (TotalDamage > 0)
		{
			OnDamagedHitResponse.Broadcast(BonkerComponent, Hit);
		}
		return;
	}

	// Player hit Player
	if (BonkerPlayer != nullptr && HitPlayer != nullptr)
	{
		if (BonkerPlayer->PlayerInfo == nullptr)
		{
			return;
		}
		// bonker damage * player damage multiplier
		const float TotalDamage = BonkerComponent->Damage * BonkerPlayer->PlayerInfo->Damage->CurrentValue;
		HitPlayer->ApplyDamage(TotalDamage, BonkerComponent, BonkerPlayer);

		if (TotalDamage <= 0)
		{
			const float ForceToApply = KnockbackMultiplier * 5.f * (BaseHealthKnockback + HitPlayer->PlayerInfo->Health->CurrentValue);
			KnockbackFromDirection(BonkerPlayer->GetActorForwardVector(), ForceToApply);
		}
		else
		{
			const float ForceToApply = KnockbackMultiplier * TotalDamage * (BaseHealthKnockback + HitPlayer->PlayerInfo->Health->CurrentValue);
			KnockbackFromDirection(BonkerPlayer->GetActorForwardVector(), ForceToApply);
		}
		
		LastBonkerHit = BonkerComponent;
		LastPlayerHit = BonkerPlayer;
		
		OnHitResponse.Broadcast(BonkerComponent, Hit);
		OnHitResponsePlayer.Broadcast(BonkerComponent, BonkerPlayer, BonkerPlayer->PlayerInfo, Hit);

		if (TotalDamage > 0)
		{
			OnDamagedHitResponse.Broadcast(BonkerComponent, Hit);
		}
	}
}

void UBonkBonkableComponent::EnableBonkable(bool bActive)
{
	bIsEnabled = bActive;
}

