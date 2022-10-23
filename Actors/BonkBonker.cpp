#include "BonkBonker.h"
#include "Bonk/Components/BonkAttribute_WeaponSize.h"
#include "Bonk/Components/BonkBonkerComponent.h"
#include "Bonk/Components/BonkBonkerSphereComponent.h"
#include "Bonk/Components/BonkItemInteractableComponent.h"
#include "Bonk/Components/BonkItemInteractComponent.h"
#include "Bonk/Player/BonkPlayer.h"
#include "Bonk/Player/BonkPlayerPawn.h"
#include "Kismet/KismetMathLibrary.h"

ABonkBonker::ABonkBonker()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshColliderComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshCollider");
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("ModelMesh");
	TraceStart = CreateDefaultSubobject<USceneComponent>("SweepStart");
	TraceEnd = CreateDefaultSubobject<USceneComponent>("SweepEnd");

	InteractableComponent = CreateDefaultSubobject<UBonkItemInteractableComponent>("InteractableComponent");
	BonkerComponent = CreateDefaultSubobject<UBonkBonkerComponent>("BonkerComponent");

	RootComponent = MeshColliderComp;
	MeshComp->SetupAttachment(MeshColliderComp);
	TraceStart->SetupAttachment(MeshComp);
	TraceEnd->SetupAttachment(MeshComp);
	Cast<UPrimitiveComponent>(MeshColliderComp)->SetSimulatePhysics(true);
}

void ABonkBonker::BeginPlay()
{
	Super::BeginPlay();
	InteractableComponent->SetPrimitiveComponent(Cast<UPrimitiveComponent>(RootComponent));

	InteractableComponent->OnPickupResponse.AddDynamic(this, &ABonkBonker::Pickup_Callback);
	InteractableComponent->OnDropResponse.AddDynamic(this, &ABonkBonker::Drop_Callback);

	BonkerComponent->SetTraceLocationFromSceneComps(TraceStart, TraceEnd);


	BaseSize = MeshComp->GetComponentScale().X;
	TargetSize = BaseSize;

	UBonkBonkerSphereComponent* BonkerSphere = Cast<UBonkBonkerSphereComponent>(BonkerComponent);
	if (BonkerSphere != nullptr)
	{
		BaseTraceRadius = BonkerSphere->TraceRadius;
	}

	const TArray<UMaterialInterface*> MeshMaterials = MeshComp->GetMaterials();
	for (int i = 0; i < MeshMaterials.Num(); i++)
	{
		UMaterialInstanceDynamic* MatInstance = MeshComp->CreateDynamicMaterialInstance(i, MeshMaterials[i]);
		MaterialInstances.Add(MatInstance);
		BaseColors.Add(MatInstance->K2_GetVectorParameterValue(MatParameter));
	}
	
}

void ABonkBonker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	BonkerComponent->TraceStart = TraceStart->GetComponentLocation();
	BonkerComponent->TraceEnd = TraceEnd->GetComponentLocation();

	if (InteractableComponent->bCanSwing)
	{
		float Size = UKismetMathLibrary::FInterpTo(CurrentSize, TargetSize, DeltaTime, SizeInterpSpeed);
		MeshComp->SetWorldScale3D(FVector(Size, Size, Size));
		CurrentSize = Size;
	}

	if (!InteractableComponent->bIsHeld)
	{
		if (MeshColliderComp->GetComponentVelocity().Length() >= SpeedTraceThreshold)
		{
			BonkerComponent->ActivateTracing(true);
		}
		else
		{
			BonkerComponent->ActivateTracing(false);
		}
	}
}

void ABonkBonker::Pickup_Callback(UBonkItemInteractComponent* InteractComp)
{
	BonkerComponent->HoldingPlayer = Cast<ABonkPlayerPawn>(InteractComp->GetOwner());
	HoldingPlayer = BonkerComponent->HoldingPlayer;

	BonkerComponent->ActorsToIgnore.Add(HoldingPlayer);

	if (InteractableComponent->bCanSwing)
	{
		const float CurrentWeaponSize = HoldingPlayer->PlayerInfo->WeaponSize->CurrentValue;
		TargetSize = CurrentWeaponSize * BaseSize;
		BaseTraceRadius = CurrentWeaponSize * BaseTraceRadius;
	}
}

void ABonkBonker::Drop_Callback(UBonkItemInteractComponent* InteractComp)
{
	BonkerComponent->HoldingPlayer = nullptr;
	HoldingPlayer = nullptr;

	TargetSize = BaseSize;
	Cast<UBonkBonkerSphereComponent>(BonkerComponent)->TraceRadius = BaseTraceRadius;
}


