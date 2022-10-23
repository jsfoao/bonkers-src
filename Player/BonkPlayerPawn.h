#pragma once
#include "CoreMinimal.h"
#include "Bonk/Components/BonkBonkerComponent.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraActor.h"
#include "BonkPlayerPawn.generated.h"

UCLASS()
class BONK_API ABonkPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ABonkPlayerPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//Functions that are called on Tick
	void CheckIfGrounded();
	void ApplyPlayerMovement(float DeltaTime);
	void ApplyPlayerRotation(float DeltaTime);

	//Input Callbacks
	void InputMoveForward_Callback(float AxisValue);
	void InputMoveRight_Callback(float AxisValue);
	void Jump_Callback();
	void InputAttack_Callback();
	void InputThrowDown_Callback();
	void InputThrowUp_Callback();
	void InputInteract_Callback();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttack();

	UFUNCTION(BlueprintImplementableEvent)
	void OnJump();
	
public:
	UPROPERTY(BlueprintReadWrite)
	class UBonkPlayer* PlayerInfo;
	
	//
	// COMPONENTS
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* SkeletalComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBonkAbilityComponent* AbilityComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBonkBonkableComponent* BonkableComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBonkItemInteractComponent* InteractComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	AActor* CameraActor;

	//
	// BASE ATTRIBUTES
	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseAttributes")
	float BaseAttackSpeed = 1.f;
	
	//
	// MOVEMENT
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MaxSpeed = 100000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Acceleration = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MaxAccelerationForce = 2000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float AirMoveForce = 2000;

	FVector GoalVelocity;
	FVector CurrentVelocity;

	FVector InputMoveRightVector;
	FVector InputMoveForwardVector;
	FVector InputMoveVector;

	//Rotation Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float RotationSpeed = 8.f;
	UPROPERTY(BlueprintReadOnly, Category=Movement)
	FRotator CurrentRotation;

	//JumpVariables 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float JumpForce = 1500.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool JustJumped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanJump = false;

	//Forces
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float GravityGroundAccel = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
	float GravityAirAccel = -100000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
	float DragAirAccel = 0.f;

	// Grounding
	float ActorScale;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsGrounded = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	float GroundStartOffset = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
	float GroundTraceLength = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	float GroundTraceRadius = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	TEnumAsByte<ETraceTypeQuery> GroundTraceChannel;
	UPROPERTY(BlueprintReadOnly)
	FVector GroundNormal;

	//floating
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float RideHeight = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float RideSpringStrength = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float RideSpringDamper = 10.f;

	//
	// COMBAT
	//
	UPROPERTY(BlueprintReadOnly)
	UBonkBonkerComponent* LastDamagedBy;
	
	UPROPERTY(BlueprintReadWrite)
	bool bCanAttack;

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float Damage, UBonkBonkerComponent* FromBonkerComp = nullptr, ABonkPlayerPawn* FromPlayer = nullptr);

	UFUNCTION(BlueprintCallable)
	void ApplyHeal(float Heal);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDamaged, float, NewHealth, float, DamagedApplied, UBonkBonkerComponent*, Bonker, ABonkPlayerPawn*, Player);
	UPROPERTY(BlueprintAssignable)
	FOnDamaged OnDamaged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealed, float, NewHealth, float, HealthApplied);
	UPROPERTY(BlueprintAssignable)
	FOnHealed OnHealed;
};
