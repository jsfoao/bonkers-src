// Fill out your copyright notice in the Description page of Project Settings.


#include "BonkAudience.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "../Libraries/BonkMacros.h"


// Sets default values
ABonkAudience::ABonkAudience()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool ABonkAudience::GetRandomPointInShape(UShapeComponent* Shape, FVector& OutPoint) const
{
	UBoxComponent* Box = Cast<UBoxComponent>(Shape);
	if (Box != nullptr)
	{
		FVector Center = Box->GetComponentLocation();
		FVector Extents = Box->GetScaledBoxExtent();
		OutPoint = UKismetMathLibrary::RandomPointInBoundingBox(Center, Extents);
		return true;
	}
	USphereComponent* Sphere = Cast<USphereComponent>(Shape);
	if (Sphere != nullptr)
	{
		FVector Center = Sphere->GetComponentLocation();
		float Radius = Sphere->GetScaledSphereRadius();
		float X = FMath::RandRange(Center.X - Radius, Center.X + Radius);
		float Y = FMath::RandRange(Center.Y - Radius, Center.Y + Radius);
		float Z = FMath::RandRange(Center.Z - Radius, Center.Z + Radius);
		OutPoint = FVector(X, Y, Z);
		return true;
	}
	return false;
}

bool ABonkAudience::GetShapeActors(TArray<AActor*>& Sources, TArray<AActor*>& Targets)
{
	if (SourcePointsClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AudienceCPP: SourcePointsClass not assigned in audience bp."));
		return false;
	}
	if (TargetsClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AudienceCPP: TargetsClass not assigned in audience bp."));
		return false;
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetsClass, Targets);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SourcePointsClass, Sources);

	if (SpawnableActorClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AudienceCPP: The SpawnableActorClasses of audiencec is empty."), *SourcePointsClass.Get()->GetFName().ToString());
		return false;
	}

	if (Targets.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AudienceCPP: No Actors of class: %s found."), *TargetsClass.Get()->GetFName().ToString());
		return false;
	}

	if (Sources.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AudienceCPP: No Actors of class: %s found."), *SourcePointsClass.Get()->GetFName().ToString());
		return false;
	}
		
	return true;
}

UINT ABonkAudience::GetSumOfSpawnWeights()
{
	SumOfWeights = 0;
	for (auto& item : SpawnableActorClasses)
	{
		SumOfWeights += item.Weight;
	}
	return SumOfWeights;
}

void ABonkAudience::ThrowItemToPointInBox()
{
	if (WasSuccessfullySetup == false)
		return;

	int StartIndex = FMath::RandRange(0, AudienceShapes.Num()-1);
	int TargetIndex = FMath::RandRange(0, TargetShapes.Num()-1);

	TSubclassOf<AActor> ActorToSpawn = nullptr;

	float TargetWeight = FMath::RandRange(0.f, SumOfWeights);

	float CurrentWeight = 0;

	for (auto& item : SpawnableActorClasses)
	{
		CurrentWeight += item.Weight;
		if (CurrentWeight >= TargetWeight)
		{
			ActorToSpawn = item.ActorSubClass;
			break;
		}
	}

	UShapeComponent* Shape1 = AudienceShapes[StartIndex];
	UShapeComponent* Shape2 = TargetShapes[TargetIndex];
	FVector StartRandomPoint;
	FVector EndRandomPoint;
	GetRandomPointInShape(Shape1, StartRandomPoint);
	GetRandomPointInShape(Shape2, EndRandomPoint);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	float X = 0, Y = 0, Z = 0;
	X = FMath::RandRange(75.f, 105.f);
	Y = FMath::RandRange(0.f, 360.f);
	Z = FMath::RandRange(0.f, 360.f);
	X = FMath::RandBool() ? X : -X;
	FRotator Rot = FRotator(X, Y, Z);

	auto Projectile = GetWorld()->SpawnActor<AActor>(ActorToSpawn, StartRandomPoint, Rot, SpawnInfo);
	
	FVector StartLoc = StartRandomPoint;
	FVector EndLoc = EndRandomPoint;

	float HeightDiff = FMath::Abs(EndLoc.Z - StartLoc.Z);
	StartLoc.Z = 0;
	EndLoc.Z = 0;

	FVector Direction = (EndLoc - StartLoc).GetSafeNormal();

	float XYDist = FVector::Dist(StartLoc, EndLoc);

	float Gravity = 981.f;
	float Angle = 0;
	//float TimeOfFlight = 2 * InitialVelocity * FMath::Sin(Angle) / Gravity;
	float TimeOfFlight = FMath::Sqrt(HeightDiff * 2 / Gravity);

	float InitialVelocity = XYDist/TimeOfFlight;
	AudienceData* Data = new AudienceData();
	AudienceDataArr.Add(Data);
	Data->StartPosition = StartRandomPoint;

	FVector Center, Extents;
	Projectile->GetActorBounds(false, Center, Extents);

	for (int i = 1; i <= CurvePoints; i++)
	{
		float CurrentTime = (float)i / CurvePoints * TimeOfFlight;
		float XZOffset = InitialVelocity * CurrentTime;
		float x = StartRandomPoint.X + Direction.X * XZOffset;
		float y = StartRandomPoint.Y + Direction.Y * XZOffset;
		float z = StartRandomPoint.Z - (0.5f * Gravity * CurrentTime * CurrentTime) + Extents.Z;
		Data->Points.Enqueue(FVector(x, y, z));
	}
	Data->Points.Dequeue(Data->EndPosition);
	Data->Actor = Projectile;
	Data->DurationPerPoint = TimeOfFlight / CurvePoints;
	if (ShowDebugOutlines)
	{
		DrawDebugSphere(GetWorld(), EndRandomPoint, 25.f, 16, FColor::Yellow, false, 10.f);
		DrawDebugSphere(GetWorld(), StartRandomPoint, 25.f, 16, FColor::Blue, false, 10.f);
	}
	OnWeaponSpawned.Broadcast();
}

void ABonkAudience::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> Targets;
	TArray<AActor*> SourcePoints;
	SumOfWeights = GetSumOfSpawnWeights();
	WasSuccessfullySetup = GetShapeActors(SourcePoints, Targets);
	if (WasSuccessfullySetup)
	{
		for (AActor* actor : SourcePoints)
		{
			UShapeComponent* Shape = Cast<UShapeComponent>(actor->GetComponentByClass(UShapeComponent::StaticClass()));
			if (Shape)
			{
				AudienceShapes.Add(Shape);
			}
		}
		for (AActor* actor : Targets)
		{
			UShapeComponent* Shape = Cast<UShapeComponent>(actor->GetComponentByClass(UShapeComponent::StaticClass()));
			if (Shape)
			{
				TargetShapes.Add(Shape);
			}
		}
	}
	else
	{
		PrimaryActorTick.bCanEverTick = false;
	}
}

//// Called every frame
void ABonkAudience::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = AudienceDataArr.Num() - 1; i >= 0; i--)
	{
		AudienceData* Data = AudienceDataArr[i];
		if (IsValid(Data->Actor))
		{
			Data->CurrentAlpha += DeltaTime;
			if (Data->CurrentAlpha >= Data->DurationPerPoint)
			{
				FVector Prev = Data->EndPosition;
				FVector Target;
				if (Data->Points.Dequeue(Target))
				{
					Data->StartPosition = Prev;
					Data->EndPosition = Target;
					Data->CurrentAlpha -= Data->DurationPerPoint;
				}
				else
				{
					Data->Actor->SetActorLocation(Data->EndPosition);

					AudienceDataArr.RemoveAt(i);
					continue;
				}
			}
			Data->Actor->SetActorLocation(FMath::Lerp(Data->StartPosition, Data->EndPosition, Data->CurrentAlpha / Data->DurationPerPoint));
		}
	}

}

