#include "BonkGameplayEffect.h"
#include "BonkAttribute.h"
#include "Bonk/Player/BonkPlayer.h"

void UBonkGameplayEffect::Bind(UBonkPlayer* NewPlayerData)
{
	PlayerData = NewPlayerData;
	World = PlayerData->World;
	PlayerData->Effects.Add(this);
	
	if (Type == Instant || Type == Both)
	{
		ModifierEffect();
	}
}

void UBonkGameplayEffect::Unbind()
{
	PlayerData = nullptr;
}

void UBonkGameplayEffect::ModifierEffect()
{
	if (PlayerData == nullptr)
		return;
	
	for (auto const Modifier : Modifiers)
	{
		UBonkAttribute* Attribute = PlayerData->GetAttribute(Modifier.AttributeClass);
		if (Attribute == nullptr)
			continue;

		if (Modifier.Operation == Add)
			Attribute->CurrentValue += Modifier.Value;

		if (Modifier.Operation == Subtract)
			Attribute->CurrentValue -= Modifier.Value;

		if (Modifier.Operation == Multiply)
			Attribute->CurrentValue *= Modifier.Value;

		if (Modifier.Operation == Divide)
			Attribute->CurrentValue /= Modifier.Value;

		if (Modifier.Operation == Override)
			Attribute->CurrentValue = Modifier.Value;
		
		if (Modifier.Operation == OverrideBase)
			Attribute->SetBaseValue(Modifier.Value);
	}
}

void UBonkGameplayEffect::SuperEffectTick(ABonkPlayerPawn* Pawn, float DeltaTime)
{
	DeltaSeconds = DeltaTime;
	EffectTick(Pawn, DeltaTime);
}

// Only works of addition and subtraction
void UBonkGameplayEffect::ReverseModifierEffect()
{
	if (PlayerData == nullptr)
		return;
	
	for (auto const Modifier : Modifiers)
	{
		UBonkAttribute* Attribute = PlayerData->GetAttribute(Modifier.AttributeClass);
		if (Attribute == nullptr)
			continue;

		if (Modifier.Operation == Add)
			Attribute->CurrentValue -= Modifier.Value;

		if (Modifier.Operation == Subtract)
			Attribute->CurrentValue += Modifier.Value;

		if (Modifier.Operation == Multiply)
			Attribute->CurrentValue /= Modifier.Value;
		
		if (Modifier.Operation == Divide)
			Attribute->CurrentValue *= Modifier.Value;
	}
}