#include "BonkPlayerPawn.h"
#include "BonkPlayer.h"
#include "Bonk/Components/BonkAbilityComponent.h"
#include "Bonk/Components/BonkAttribute_Health.h"
#include "Bonk/Components/BonkAttribute_MoveSpeed.h"
#include "Bonk/Components/BonkAttribute_Resilience.h"
#include "Bonk/Components/BonkBonkableComponent.h"
#include "Bonk/Components/BonkItemInteractableComponent.h"
#include "Bonk/Components/BonkItemInteractComponent.h"
#include "Bonk/Libraries/BonkMacros.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Vector.h"

ABonkPlayerPawn::ABonkPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComp;
	SkeletalComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalComponent"));
	SkeletalComp->SetupAttachment(RootComponent);

	AbilityComp = CreateDefaultSubobject<UBonkAbilityComponent>(TEXT("AbilityComponent"));
	BonkableComp = CreateDefaultSubobject<UBonkBonkableComponent>(TEXT("BonkableComponent"));
	InteractComp = CreateDefaultSubobject<UBonkItemInteractComponent>(TEXT("InteractComponent"));
}

// Called when the game starts or when spawned
void ABonkPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	InteractComp->SetSkeletalComponent(SkeletalComp);
	BonkableComp->SetPrimitiveComponent(CapsuleComp);
	AbilityComp->SetPlayerData(PlayerInfo);
	ActorScale = GetActorScale().X;
}

// Called every frame
void ABonkPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckIfGrounded();
	ApplyPlayerMovement(DeltaTime);
	ApplyPlayerRotation(DeltaTime);

	// Extra gravity
	if (bIsGrounded)
	{
		CapsuleComp->AddForce(FVector::UpVector * GravityGroundAccel * DeltaTime);
	}
	else
	{
		CapsuleComp->AddForce(FVector::UpVector * GravityAirAccel * DeltaTime);
	}
}

// Called to bind functionality to input
void ABonkPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ABonkPlayerPawn::Jump_Callback);
	InputComponent->BindAction("Attack", IE_Pressed, this, &ABonkPlayerPawn::InputAttack_Callback);
	InputComponent->BindAction("Throw", IE_Pressed, this, &ABonkPlayerPawn::InputThrowDown_Callback);
	InputComponent->BindAction("Throw", IE_Released, this, &ABonkPlayerPawn::InputThrowUp_Callback);
	InputComponent->BindAction("Interact", IE_Pressed, this, &ABonkPlayerPawn::InputInteract_Callback);
	InputComponent->BindAxis("Move Forward/Backward", this, &ABonkPlayerPawn::InputMoveForward_Callback);
	InputComponent->BindAxis("Move Right/Left", this, &ABonkPlayerPawn::InputMoveRight_Callback);
}

void ABonkPlayerPawn::CheckIfGrounded()
{
	// Sphere trace to check if grounded
	// Allows for grounding on edges
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	const FVector StartLocation = GetActorLocation() - FVector::UpVector * GroundStartOffset * ActorScale;
	const FVector EndLocation = StartLocation - FVector::UpVector * GroundTraceLength * ActorScale;
	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		GroundTraceRadius * ActorScale,
		GroundTraceChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true
		);

	if (HitResult.bBlockingHit)
	{
		//Add Force to keep the Player floating above ground.
		FVector vel = CapsuleComp->GetPhysicsLinearVelocity();
		FVector RayDir = FVector::DownVector;

		float RayDirVel = FVector::DotProduct(RayDir, vel);


		float X = HitResult.Distance - RideHeight * ActorScale;
		float SpringForce = (X * RideSpringStrength) - (RayDirVel * RideSpringDamper);

		CapsuleComp->AddForce(RayDir * SpringForce);

		bIsGrounded = true;
		GroundNormal = HitResult.Normal;
	}
	else
	{
		bIsGrounded = false;
		GroundNormal = FVector::UpVector;
	}
}

void ABonkPlayerPawn::ApplyPlayerMovement(float DeltaTime)
{
	CurrentVelocity = CapsuleComp->GetPhysicsLinearVelocity();
	CurrentVelocity.Z = 0;

	InputMoveVector = InputMoveRightVector + InputMoveForwardVector;

	// Allows for analogue values
	if (InputMoveVector.Length() > 1)
	{
		InputMoveVector.Normalize();
	}

	if (bIsGrounded)
	{
		float Accel = Acceleration;

		FVector NewGoalVelocity = InputMoveVector * MaxSpeed * PlayerInfo->MoveSpeed->CurrentValue;

		GoalVelocity = FMath::Lerp(GoalVelocity, NewGoalVelocity, Accel * DeltaTime);

		FVector NeededAcceleration = (GoalVelocity - CurrentVelocity / DeltaTime);

		float MaxAcceleration = MaxAccelerationForce;

		NeededAcceleration.GetClampedToMaxSize(MaxAcceleration);

		CapsuleComp->AddForce(NeededAcceleration);
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, TEXT("Air"));
		FVector DragForce = -CurrentVelocity * DragAirAccel;

		CapsuleComp->AddForce(InputMoveVector * AirMoveForce);

		CapsuleComp->AddForce(DragForce);
	}
}

void ABonkPlayerPawn::ApplyPlayerRotation(float DeltaTime)
{
	/* Rotation in the movement direction*/
	if (InputMoveVector.Length() < 0.5)
		return;

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, InputMoveVector);
	CurrentRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
	SetActorRotation(CurrentRotation);
}

void ABonkPlayerPawn::InputMoveForward_Callback(float AxisValue)
{
	InputMoveForwardVector = CameraActor->GetActorForwardVector() * AxisValue;
	InputMoveForwardVector.Z = 0.f;
}

void ABonkPlayerPawn::InputMoveRight_Callback(float AxisValue)
{
	InputMoveRightVector = CameraActor->GetActorRightVector() * AxisValue;
	InputMoveRightVector.Z = 0.f;
}

void ABonkPlayerPawn::Jump_Callback()
{
	if (!bCanJump)
		return;
	
	if (bIsGrounded) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Jump"));
		CapsuleComp->AddImpulse(FVector(0.f, 0.f, JumpForce));
		bIsGrounded = false;
		JustJumped = true;
		OnJump();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				UE_LOG(LogTemp, Warning, TEXT("This text will appear in the console 3 seconds after execution"));
				JustJumped = false;
			}, 0.5f, false);
		
	}
}

void ABonkPlayerPawn::InputAttack_Callback()
{
	if (!InteractComp->bIsHolding || !bCanAttack)
		return;

	if (!InteractComp->CurrentInteractable->bCanSwing && !InteractComp->bIsThrowing)
		return;

	bCanAttack = false;
	AbilityComp->OnAttack();
	OnAttack();
}

void ABonkPlayerPawn::InputThrowDown_Callback()
{
	InteractComp->StartThrow();
}

void ABonkPlayerPawn::InputThrowUp_Callback()
{
	InteractComp->Throw(GetActorForwardVector());
}

void ABonkPlayerPawn::InputInteract_Callback()
{
	InteractComp->TryAction();
}

void ABonkPlayerPawn::ApplyDamage(float Damage, UBonkBonkerComponent* FromBonkerComp, ABonkPlayerPawn* FromPlayer)
{
	if (Damage <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("Couldn't damage, value less than 0"));
		return;
	}
	
	float ResilienceMulti = 1 - PlayerInfo->Resilience->CurrentValue;
	if (ResilienceMulti < 0.2f)
		ResilienceMulti = 0.2f;
	
	const float FinalDamage =  Damage * ResilienceMulti;
	float FinalHealth = PlayerInfo->Health->CurrentValue + FinalDamage;
	
	PlayerInfo->Health->SetCurrentValue(FinalHealth);

	LastDamagedBy = FromBonkerComp;
	OnDamaged.Broadcast(PlayerInfo->Health->CurrentValue, Damage, FromBonkerComp, FromPlayer);
}

void ABonkPlayerPawn::ApplyHeal(float Heal)
{
	if (Heal <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("Couldn't heal, value less than 0"));
		return;
	}

	float FinalHealth = PlayerInfo->Health->CurrentValue - Heal;
	if (FinalHealth <= 0.f)
	{
		FinalHealth = 0.f;
	}
	PlayerInfo->Health->SetCurrentValue(FinalHealth);
	OnHealed.Broadcast(PlayerInfo->Health->CurrentValue, Heal);
}
