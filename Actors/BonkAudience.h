// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BonkAudience.generated.h"

class ATriggerBox;
class UShapeComponent;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAudienceThrowSignature);

USTRUCT()
struct FAudienceSpawnPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Weight;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorSubClass;
};

struct AudienceData
{
	AActor* Actor = nullptr;

	float CurrentAlpha = 0;

	float DurationPerPoint = 0;

	FVector StartPosition = FVector::ZeroVector;
	FVector EndPosition = FVector::ZeroVector;

	TQueue<FVector> Points;
};

UCLASS()
class BONK_API ABonkAudience : public AActor
{
	GENERATED_BODY()

	bool GetRandomPointInShape(UShapeComponent* Shape, FVector& OutPoint) const;

	bool GetShapeActors(TArray<AActor*>& Sources, TArray<AActor*>& Targets);

	UINT GetSumOfSpawnWeights();

protected:
	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	ABonkAudience();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ThrowItemToPointInBox();

	UPROPERTY(EditInstanceOnly)
	bool ShowDebugOutlines;

	UPROPERTY(EditDefaultsOnly)
	TArray<FAudienceSpawnPair> SpawnableActorClasses;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> TargetsClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SourcePointsClass;

	UPROPERTY(BlueprintAssignable)
	FAudienceThrowSignature OnWeaponSpawned;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "2", ClampMax = "20", UIMin = "2", UIMax = "20"))
	int CurvePoints = 6;

	TArray<UShapeComponent*> AudienceShapes;
	TArray<UShapeComponent*> TargetShapes;

	TArray<AudienceData*> AudienceDataArr;

	float SumOfWeights = 0;
	bool WasSuccessfullySetup = false;
};
