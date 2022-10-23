#include "BonkAbilityComponent.h"
#include "BonkGameplayEffect.h"
#include "Bonk/Player/BonkPlayer.h"
#include "Bonk/Player/BonkPlayerPawn.h"

UBonkAbilityComponent::UBonkAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBonkAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = Cast<ABonkPlayerPawn>(GetOwner());
}

void UBonkAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PlayerData == nullptr)
		return;
	for (const auto Effect : PlayerData->Effects)
	{
		if (Effect->Type == Instant)
			continue;
		if (!Effect->bActiveTick)
			continue;
		Effect->SuperEffectTick(PlayerPawn, DeltaTime);
	}
}

void UBonkAbilityComponent::SetPlayerData(UBonkPlayer* NewPlayerData)
{
	PlayerData = NewPlayerData;
}

void UBonkAbilityComponent::OnAttack()
{
	if (PlayerData == nullptr)
		return;
	for (const auto Effect : PlayerData->Effects)
	{
		if (Effect->Type == Instant)
			continue;
		Effect->OnAttack(PlayerPawn);
	}
}

void UBonkAbilityComponent::OnHit(AActor* Actor)
{
	if (PlayerData == nullptr)
		return;
	for (const auto Effect : PlayerData->Effects)
	{
		if (Effect->Type == Instant)
			continue;
		Effect->OnHit(PlayerPawn, Actor);
	}
}

void UBonkAbilityComponent::OnPickup(AActor* Actor)
{
	if (PlayerData == nullptr)
		return;
	for (const auto Effect : PlayerData->Effects)
	{
		if (Effect->Type == Instant)
			continue;
		Effect->OnPickup(PlayerPawn, Actor);
	}
}

void UBonkAbilityComponent::OnDrop(AActor* Actor)
{
	if (PlayerData == nullptr)
		return;
	for (const auto Effect : PlayerData->Effects)
	{
		if (Effect->Type == Instant)
			continue;
		Effect->OnDrop(PlayerPawn, Actor);
	}
}

