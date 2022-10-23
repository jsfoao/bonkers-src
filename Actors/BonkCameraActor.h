// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "UObject/Class.h"
#include "BonkCameraActor.generated.h"

/**
 * 
 */
UENUM()
enum CameraState
{
	Tracking    UMETA(DisplayName = "Tracking"),
	Stopped     UMETA(DisplayName = "Stopped"),
	ZoomingOut  UMETA(DisplayName = "ZoomingOut"),
	ZoomingIn	UMETA(DisplayName = "ZoomingIn"),
};

UCLASS()
class BONK_API ABonkCameraActor : public ACameraActor
{
	GENERATED_BODY()

	float GetFarthestDistanceToLocation(const TArray<AActor*> Actors, const FVector Location) const;

	FVector GetCenterLocation(const TArray<AActor*> Actors) const;

	void UpdateCameraZoomLocations();

	FVector GetTargetLocation();

	void TrackTargets(float DeltaTime);

	void ChangeState(CameraState State);

	UPROPERTY()
	TArray<AActor*> ActorsToFollow;

	CameraState CurrentState = CameraState::Tracking;
	
	FVector ZoomedInLocation = FVector::ZeroVector;
	FVector ZoomedOutLocation = FVector::ZeroVector;
	FVector ZoomStartLocation = FVector::ZeroVector;

	FVector FirstCenterLocation = FVector::ZeroVector;


	TArray<CameraState> StateStack;

	float StateTimer = 0;
	float StateDuration = 0;
	FVector StateStartLocation = FVector::ZeroVector;
	FVector StateEndLocation = FVector::ZeroVector;

protected:
	virtual void BeginPlay() override;
public:
	ABonkCameraActor();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetFollowTargets(const TArray<AActor*> Actors);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetLocationMinBounds(const FVector& Location) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetLocationMaxBounds(const FVector& Location) const;

	UFUNCTION(BlueprintCallable)
	void ClearFollowTargets();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Bonk Camera Options")
	void StartZoomoutSequence();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonk Camera|Tracking Options")
	FVector CameraMinBounds = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonk Camera|Tracking Options")
	FVector CameraMaxBounds = FVector::ZeroVector;

	/*The distance away (from average player location) when zoomed in*/
	UPROPERTY(EditAnywhere, Category = "Bonk Camera|Tracking Options")
	float ZoomInDistance = 0;

	/*The distance away (from average player location) when zoomed out*/
	UPROPERTY(EditAnywhere, Category = "Bonk Camera|Tracking Options")
	float ZoomOutDistance = 0;

	/*The distance over which the zooming happens (based on player position away from average)*/
	UPROPERTY(EditAnywhere, Category = "Bonk Camera|Tracking Options")
	float TrackingDistance = 0;

	UPROPERTY(EditAnywhere, Category = "Bonk Camera|Tracking Options")
	bool bCanZoomIn = true;

	UPROPERTY(EditAnywhere, Category = "Bonk Camera|Tracking Options")
	float SmoothSpeed = 1;

	UPROPERTY(EditAnywhere, Category = "Bonk Camera|Weapon Event Options")
	float ZoomInOutDuration = 1.f;

	UPROPERTY(EditAnywhere, Category = "Bonk Camera|Weapon Event Options")
	float StopDuration = 3.f;

	

	UPROPERTY(BlueprintReadOnly)
	FVector InitialLocation = FVector::ZeroVector;
};
