// Fill out your copyright notice in the Description page of Project Settings.


#include "BonkLobbyPawn.h"


ABonkLobbyPawn::ABonkLobbyPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ABonkLobbyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABonkLobbyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABonkLobbyPawn::UnPossessed()
{
	Super::UnPossessed();
	Destroy();
}

void ABonkLobbyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Start", IE_Pressed, this, &ABonkLobbyPawn::StartPressed);
}

void ABonkLobbyPawn::StartPressed()
{
	OnPlayerRegistered.ExecuteIfBound(PlayerIndex);
}

