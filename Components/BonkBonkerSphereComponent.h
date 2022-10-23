#pragma once
#include "CoreMinimal.h"
#include "BonkBonkerComponent.h"
#include "BonkBonkerSphereComponent.generated.h"


UCLASS(ClassGroup=(Bonking), meta=(BlueprintSpawnableComponent))
class BONK_API UBonkBonkerSphereComponent : public UBonkBonkerComponent
{
	GENERATED_BODY()

private:
	virtual TArray<FHitResult> ExecuteTracing() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceRadius = 5.f;
};
