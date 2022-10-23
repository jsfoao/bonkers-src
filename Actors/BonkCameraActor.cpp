// Fill out your copyright notice in the Description page of Project Settings.

#include "BonkCameraActor.h"
#include "../Libraries/BonkMacros.h"
#include "Kismet/KismetMathLibrary.h"

float ABonkCameraActor::GetFarthestDistanceToLocation(const TArray<AActor*> Actors, const FVector Location) const
{
	float FarthestDistance = 0;

	for (const AActor* Actor : Actors)
	{
		// Edit by Carl, 09-16, 15:30. Fixed the warning.
		if (IsValid(Actor))
		{
			float Distance = FVector::Distance(Actor->GetActorLocation(), Location);
			if (Distance > FarthestDistance)
				FarthestDistance = Distance;
		}
	}
	return FarthestDistance;
}

FVector ABonkCameraActor::GetCenterLocation(const TArray<AActor*> Actors) const
{
	FVector CenterLocation = FVector::ZeroVector;
	int NumValidActors = 0;
	for (const AActor* Actor : ActorsToFollow)
	{
		// Edit by Carl, 09-16, 15:30. Fixed the warning.
		if (IsValid(Actor))
		{
			CenterLocation += Actor->GetActorLocation();
			NumValidActors++;
		}
	}
	if (NumValidActors == 0)
		return FVector::ZeroVector;

	return CenterLocation / NumValidActors;
}

void ABonkCameraActor::UpdateCameraZoomLocations()
{
	FVector CenterLocation, MinLocation, MaxLocation, ZoomedInOffset, ZoomedOutOffset;

	CenterLocation = GetCenterLocation(ActorsToFollow);
	MinLocation = UKismetMathLibrary::Vector_ComponentMax(CenterLocation, GetLocationMinBounds(FirstCenterLocation));
	MaxLocation = UKismetMathLibrary::Vector_ComponentMin(CenterLocation, GetLocationMaxBounds(FirstCenterLocation));
	CenterLocation.X = FMath::Clamp(CenterLocation.X, MinLocation.X, MaxLocation.X);
	CenterLocation.Y = FMath::Clamp(CenterLocation.Y, MinLocation.Y, MaxLocation.Y);
	CenterLocation.Z = FMath::Clamp(CenterLocation.Z, MinLocation.Z, MaxLocation.Z);
	ZoomedInOffset = ZoomInDistance * GetActorForwardVector();
	ZoomedOutOffset = ZoomOutDistance * GetActorForwardVector();
	ZoomedOutLocation = CenterLocation - ZoomedOutOffset;
	ZoomedInLocation = CenterLocation - ZoomedInOffset;
}

FVector ABonkCameraActor::GetTargetLocation()
{
	FVector NewLocation = FVector::ZeroVector;
	if (ActorsToFollow.Num() == 0)
	{
		return InitialLocation;
	}
	FVector CenterLocation = GetCenterLocation(ActorsToFollow);
	float FarthestDistance = GetFarthestDistanceToLocation(ActorsToFollow, CenterLocation);
	UpdateCameraZoomLocations();
	
	if (bCanZoomIn)
	{
		NewLocation = FMath::Lerp(ZoomedInLocation, ZoomedOutLocation, FMath::Clamp(FarthestDistance / TrackingDistance, 0, 1));
	}
	else
	{
		NewLocation = ZoomedOutLocation;
	}
	return NewLocation;
}

void ABonkCameraActor::TrackTargets(float DeltaTime)
{
	FVector TargetLocation = GetTargetLocation();

	SetActorLocation(FMath::Lerp(GetActorLocation(), TargetLocation, SmoothSpeed * DeltaTime));
}

void ABonkCameraActor::ChangeState(CameraState State)
{
	switch (State)
	{
	case ZoomingOut:
		StateDuration = ZoomInOutDuration;
		StateStartLocation = GetActorLocation();
		StateEndLocation = InitialLocation;
		break;
	case Stopped:
		StateDuration = StopDuration;
		break;
	case ZoomingIn:
		StateDuration = ZoomInOutDuration;
		StateStartLocation = InitialLocation;
		StateEndLocation = GetTargetLocation();
		break;
	default:
		break;
	}
	StateTimer = 0;
	CurrentState = State;
}

void ABonkCameraActor::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	CurrentState = CameraState::Tracking;
}

ABonkCameraActor::ABonkCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABonkCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentState == CameraState::Tracking)
	{
		TrackTargets(DeltaTime);
	}
	else
	{
		StateTimer += DeltaTime;
		if (StateTimer > StateDuration)
		{
			StateTimer = 0;
			ChangeState(StateStack.Pop());
			return;
		}

		if (CurrentState == CameraState::ZoomingIn)
			StateEndLocation = GetTargetLocation();

		if (CurrentState != CameraState::Stopped)
		{
			SetActorLocation(FMath::Lerp(StateStartLocation, StateEndLocation, FMath::SmoothStep(0.f, StateDuration, StateTimer)));
		}
			
	}
}

void ABonkCameraActor::SetFollowTargets(const TArray<AActor*> Actors)
{
	ActorsToFollow = Actors;
	FirstCenterLocation = GetCenterLocation(Actors) - (ZoomInDistance * GetActorForwardVector());
}

FVector ABonkCameraActor::GetLocationMinBounds(const FVector& Location) const
{
	return Location + CameraMinBounds;
}

FVector ABonkCameraActor::GetLocationMaxBounds(const FVector& Location) const
{
	return Location + CameraMaxBounds;
}

void ABonkCameraActor::ClearFollowTargets()
{
	ActorsToFollow.Empty();
}

void ABonkCameraActor::StartZoomoutSequence()
{
	ZoomStartLocation = GetActorLocation();
	StateStack.Empty();
	StateStack.Push(CameraState::Tracking);
	StateStack.Push(CameraState::ZoomingIn);
	StateStack.Push(CameraState::Stopped);
	ChangeState(CameraState::ZoomingOut);
}
