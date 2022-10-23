// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BonkLobbyPawn.generated.h"

DECLARE_DELEGATE_OneParam(FOnPlayerRegistered, int32)

UCLASS()
class BONK_API ABonkLobbyPawn : public APawn
{
	GENERATED_BODY()

public:
	
	ABonkLobbyPawn();

	UPROPERTY(VisibleAnywhere)
	int32 PlayerIndex;
	
	FOnPlayerRegistered OnPlayerRegistered;

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	virtual void UnPossessed() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartPressed();
	
};
