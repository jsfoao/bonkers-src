#include "BonkPlayer.h"
#include "Bonk/Components/BonkAttribute_Health.h"
#include "Bonk/Components/BonkAttribute_AttackSpeed.h"
#include "Bonk/Components/BonkAttribute_Damage.h"
#include "Bonk/Components/BonkAttribute_MoveSpeed.h"
#include "Bonk/Components/BonkAttribute_Resilience.h"
#include "Bonk/Components/BonkAttribute_WeaponSize.h"
#include "Bonk/Components/BonkGameplayEffect.h"

UBonkPlayer::UBonkPlayer()
{
	TeamIndex = 0;
	PlayerIndex = -1;
	bLockedIn = false;
	bAlive = true;
	PlayerMoney = 0;
	KillCount = 0;
	BountyCount = 0;

	Health = NewObject<UBonkAttribute_Health>();
	AttackSpeed = NewObject<UBonkAttribute_AttackSpeed>();
	Damage  = NewObject<UBonkAttribute_Damage>();
	MoveSpeed = NewObject<UBonkAttribute_MoveSpeed>();
	Resilience = NewObject<UBonkAttribute_Resilience>();
	WeaponSize = NewObject<UBonkAttribute_WeaponSize>();

	Health->SetBaseValue(0.f);
	AttackSpeed->SetBaseValue(1.f);
	Damage->SetBaseValue(1.f);
	Health->SetBaseValue(1.f);
	MoveSpeed->SetBaseValue(1.f);
	Resilience->SetBaseValue(0.f);
	WeaponSize->SetBaseValue(1.f);
}

void UBonkPlayer::AddEffect(TSubclassOf<UBonkGameplayEffect> EffectClass)
{
	UBonkGameplayEffect* Effect = NewObject<UBonkGameplayEffect>(this, EffectClass);
	Effect->Bind(this);
}

UBonkAttribute* UBonkPlayer::GetAttribute(TSubclassOf<UBonkAttribute> AttributeClass)
{
	if (Health->GetClass() == AttributeClass)
		return Health;
	if (AttackSpeed->GetClass() == AttributeClass)
		return AttackSpeed;
	if (Damage->GetClass() == AttributeClass)
		return Damage;
	if (MoveSpeed->GetClass() == AttributeClass)
		return MoveSpeed;
	if (Resilience->GetClass() == AttributeClass)
		return Resilience;
	if (WeaponSize->GetClass() == AttributeClass)
		return WeaponSize;

	return nullptr;
}

float UBonkPlayer::GetAttributeBase(TSubclassOf<UBonkAttribute> AttributeClass)
{
	return GetAttribute(AttributeClass)->BaseValue;
}

bool UBonkPlayer::CanAfford(int32 Cost)
{
	return Cost <= PlayerMoney;
}

void UBonkPlayer::RemoveMoney(int32 Amount)
{
	PlayerMoney -= Amount;
	OnMoneyUpdated.Broadcast(PlayerMoney);
}

void UBonkPlayer::AddMoney(int32 Amount)
{
	PlayerMoney += Amount;
	OnMoneyUpdated.Broadcast(PlayerMoney);
}

void UBonkPlayer::AddKill(UBonkPlayer* KilledPlayer)
{
	KillCount++;
	KilledPlayers.Add(KilledPlayer);
	BountyCount += KilledPlayer->KillCount;
	OnAddedKill.Broadcast(KillCount);
}

float UBonkPlayer::GetAttributeCurrent(TSubclassOf<UBonkAttribute> AttributeClass)
{
	return GetAttribute(AttributeClass)->CurrentValue;
}

