#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BonkAbilityComponent.generated.h"

class UBonkGameplayEffect;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class BONK_API UBonkAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBonkAbilityComponent();

protected:
	virtual void BeginPlay() override;

private:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintReadOnly)
	class UBonkPlayer* PlayerData;

	UPROPERTY(BlueprintReadOnly)
	class ABonkPlayerPawn* PlayerPawn;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerData(UBonkPlayer* NewPlayerData);
	
	UFUNCTION(BlueprintCallable)
	void OnAttack();

	UFUNCTION(BlueprintCallable)
	void OnHit(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnPickup(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnDrop(AActor* Actor);
};
