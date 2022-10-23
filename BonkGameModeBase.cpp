// Copyright Epic Games, Inc. All Rights Reserved.


#include "BonkGameModeBase.h"
#include "Actors/BonkCameraActor.h"
#include "Camera/CameraActor.h"
#include "Player/BonkPlayer.h"
#include "Player/BonkPlayerPawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BonkLobbyPawn.h"
#include "Widgets/BonkHUD.h"

ABonkGameModeBase::ABonkGameModeBase()
{
	bIsRoundLive = false;
	VolumeIndex = 10;
}

void ABonkGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CreateHUD();
	GatherPlayerStarts();
	GatherCameras();
	CreateLobbyPawns();

	// We need to delay this to let the HUD BPs do their stuff, otherwise the player 0 lobby card won't show up.
	GetWorldTimerManager().SetTimerForNextTick(this, &ABonkGameModeBase::AddPlayerZeroToLobby);
}

void ABonkGameModeBase::RegisterPlayer(int32 Index)
{
	for (const UBonkPlayer* Player : Players)
	{
		if (Player->PlayerIndex == Index)
			return;
	}
	UBonkPlayer* NewPlayer = NewObject<UBonkPlayer>(this);
	NewPlayer->PlayerIndex = Index;
	Players.Add(NewPlayer);
	OnPlayerJoined.Broadcast(Index);
}

void ABonkGameModeBase::LockInPlayer(int32 Index)
{
	for (UBonkPlayer* Player : Players)
	{
		if (Player->PlayerIndex == Index)
		{
			Player->bLockedIn = !Player->bLockedIn;
			
			if (Player->bLockedIn)
			{
				OnPlayerReady.Broadcast(Player->PlayerIndex);
			}
		}
	}
}

void ABonkGameModeBase::SwitchTeamForPlayer(int32 PlayerIndex, int32 NewTeamIndex)
{
	for (UBonkPlayer* Player : Players)
	{
		if (Player->PlayerIndex == PlayerIndex && !Player->bLockedIn)
		{
			Player->TeamIndex = NewTeamIndex;
			OnPlayerSwitchedTeam.Broadcast(Player->PlayerIndex, Player->TeamIndex);
		}
	}
}

void ABonkGameModeBase::CreateLobbyPawns()
{
	for (int i = 0; i < 4; i++)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), i);
		if (!PC)
		{
			PC = UGameplayStatics::CreatePlayer(GetWorld());
		}
		ABonkLobbyPawn* Pawn = GetWorld()->SpawnActorDeferred<ABonkLobbyPawn>(ABonkLobbyPawn::StaticClass(), FTransform());
		Pawn->PlayerIndex = i;
		Pawn->OnPlayerRegistered.BindUObject(this, &ABonkGameModeBase::RegisterPlayer);
		Pawn->FinishSpawning(FTransform());
		PC->Possess(Pawn);
	}
}

void ABonkGameModeBase::AddPlayerZeroToLobby()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABonkLobbyPawn* Pawn = PC->GetPawn<ABonkLobbyPawn>();
	Pawn->StartPressed();
}

void ABonkGameModeBase::CreateHUD()
{
	if (HUDWidgetClass)
	{
		HUD = CreateWidget<UBonkHUD>(GetWorld(), HUDWidgetClass);
		HUD->AddToViewport();
	}
}

void ABonkGameModeBase::DestroyLobbyPawns()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABonkLobbyPawn::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		Actor->Destroy();
	}
}

void ABonkGameModeBase::DestroyCameras()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (Actor->Tags.Num() <= 0)
		{
			Actor->Destroy();
		}
	}
}

void ABonkGameModeBase::AddPlayersToTeams()
{
	for (UBonkPlayer* Player : Players)
	{
		Teams[Player->TeamIndex].TeamPlayers.Add(Player);
	}
}

void ABonkGameModeBase::GatherPlayerStarts()
{
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);
	
	for (AActor* Actor : PlayerStartActors)
	{
		APlayerStart* Start = Cast<APlayerStart>(Actor);
		PlayerStarts.Add(Start);
		UE_LOG(LogTemp, Warning, TEXT("Player start: %s"), *Start->GetName());
	}
}

void ABonkGameModeBase::GatherCameras()
{
	TArray<AActor*> CameraActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), CameraActors);
	GameCamera = Cast<ABonkCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ABonkCameraActor::StaticClass()));
	
	for (AActor* Actor : CameraActors)
	{
		ACameraActor* Camera = Cast<ACameraActor>(Actor);
		if (Camera->ActorHasTag(FName("lobby")) || Camera->ActorHasTag(FName("arena")) || Camera->ActorHasTag(FName("shop")))
		{
			Cameras.Add(Camera);
		}
		else
		{
			if (Camera->GetClass() != GameCamera->GetClass())
			{
				Camera->Destroy();
			}
		}
	}
}

void ABonkGameModeBase::TransitionToCamera(const FName& Tag)
{
	// This is temporary, just so that we don't require this camera yet while testing.
	if (Tag == "arena")
	{
		CurrentCamera = Tag;
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PC->SetViewTarget(GameCamera);
		return;
	}
	
	for (ACameraActor* Camera : Cameras)
	{
		if (Camera->ActorHasTag(Tag))
		{
			CurrentCamera = Tag;
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			PC->SetViewTarget(Camera);
		}
	}
}

ABonkGameModeBase* ABonkGameModeBase::Get(UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetAuthGameMode<ABonkGameModeBase>();
}

void ABonkGameModeBase::StartMatch()
{
	DestroyLobbyPawns();
	DestroyCameras();
	AddPlayersToTeams();
	StartPreRound();
}

void ABonkGameModeBase::StartPreRound()
{
	CurrentRound++;
	SpawnPlayers();
	TransitionToCamera(FName("arena"));
	OnPreRoundStart.Broadcast(CurrentRound);
}

void ABonkGameModeBase::StartRound()
{
	bIsRoundLive = true;
	OnRoundStart.Broadcast(CurrentRound);
}

void ABonkGameModeBase::DestroyPlayerPawns()
{
	TArray<AActor*> PlayerPawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerPawnClass, PlayerPawns);
	for (AActor* Actor : PlayerPawns)
	{
		Actor->Destroy();
	}
}

void ABonkGameModeBase::EndRound()
{
	bIsRoundLive = false;
	OnRoundEnd.Broadcast(CurrentRound);
}

void ABonkGameModeBase::GoToShop()
{
	DestroyPlayerPawns();
	TransitionToCamera(FName("shop"));
}

void ABonkGameModeBase::EndMatch(const FTeamStructure& WinningTeam)
{
	OnTeamMatchWin.Broadcast(WinningTeam.TeamName);
}

void ABonkGameModeBase::SpawnPlayers()
{
	for (UBonkPlayer* Player : Players)
	{
		for (const APlayerStart* Start : PlayerStarts)
		{
			if (Player->TeamIndex == FCString::Atoi(*Start->PlayerStartTag.ToString()))
			{
				Player->bAlive = true;
				
				ABonkPlayerPawn* NewPawn = GetWorld()->SpawnActorDeferred<ABonkPlayerPawn>(
					PlayerPawnClass,
					Start->GetTransform(),
					nullptr,
					nullptr,
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
				NewPawn->PlayerInfo = Player;
				NewPawn->FinishSpawning(Start->GetTransform());
				
				// Bind the rescue camera workaround to pawn OnDestroyed.
				NewPawn->OnDestroyed.AddDynamic(this, &ABonkGameModeBase::RescueCamera);
				UGameplayStatics::GetPlayerController(GetWorld(), Player->PlayerIndex)->Possess(NewPawn);
			}
		}
	}
}

void ABonkGameModeBase::EnablePlayerInputs()
{
	for (int i = 0; i < 4; i++)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), i))
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				Pawn->EnableInput(PC);
			}
		}
	}
}

void ABonkGameModeBase::DisablePlayerInputs()
{
	for (int i = 0; i < 4; i++)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), i))
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				Pawn->DisableInput(PC);
			}
		}
	}
}

void ABonkGameModeBase::RescueCamera(AActor* Actor)
{
	TransitionToCamera(CurrentCamera);
}

bool ABonkGameModeBase::CheckForRoundWin(FTeamStructure& OutWinningTeam)
{
	int32 ActiveTeams = 0;

	FTeamStructure* WinningTeam = nullptr;
	for (FTeamStructure& Team : Teams)
	{
		for (const UBonkPlayer* Player : Team.TeamPlayers)
		{
			if (Player->bAlive)
			{
				ActiveTeams++;
				WinningTeam = &Team;
				OutWinningTeam = Team;
				break;
			}
		}
	}
	
	if (ActiveTeams > 1)
		return false;

	if (WinningTeam)
	{
		WinningTeam->WinCount++;
	}
	return true;
}

bool ABonkGameModeBase::CheckForMatchWin(FTeamStructure& OutWinningTeam)
{
	for (const FTeamStructure& Team : Teams)
	{
		if (Team.WinCount >= RoundsToWin)
		{
			OutWinningTeam = Team;
			return true;
		}
	}
	return false;
}

bool ABonkGameModeBase::CheckAllPlayersLockedIn()
{
	for (const UBonkPlayer* Player : Players)
	{
		if (!Player->bLockedIn)
		{
			return false;
		}
	}
	return true;
}

bool ABonkGameModeBase::CanStartMatch()
{
	if (Players.Num() < MinimumPlayers && !bIsDebugging)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough players in lobby to start game."));
		return false;
	}
	if (!CheckAllPlayersLockedIn() && !bIsDebugging)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not all players are locked in."));
		return false;
	}
	return true;
}

void ABonkGameModeBase::PlayerDied(int32 PlayerIndex, int32 PlayerKillerIndex)
{
	if (!bIsRoundLive)
		return;
 
	UBonkPlayer* DeadPlayer = nullptr;
	UBonkPlayer* KillPlayer = nullptr;
    
	for (UBonkPlayer* Player : Players)
	{
		if (Player->PlayerIndex == PlayerKillerIndex)
		{
			KillPlayer = Player;
		}
        
		if (Player->PlayerIndex == PlayerIndex)
		{
			DeadPlayer = Player;
		}
	}
	
 
	if (KillPlayer != nullptr)
	{
		KillPlayer->AddKill(DeadPlayer);
	}

	DeadPlayer->bAlive = false;
	OnPlayerDied.Broadcast(PlayerIndex);
    
	if (FTeamStructure Team; CheckForRoundWin(Team))
	{
		EndRound();
		if (CheckForMatchWin(Team))
		{
			EndMatch(Team);
		}
		else
		{
			OnTeamRoundWin.Broadcast(Team.TeamName);
		}
	}
}


const FTeamStructure& ABonkGameModeBase::GetTeam(int32 TeamIndex)
{
	return Teams[TeamIndex];
}

const TArray<FTeamStructure>& ABonkGameModeBase::GetTeams()
{
	return Teams;
}

UBonkPlayer* ABonkGameModeBase::GetPlayer(int32 PlayerIndex)
{
	for (UBonkPlayer* Player : Players)
	{
		if (Player->PlayerIndex == PlayerIndex)
		{
			return Player;
		}
	}
	return nullptr;
}

const TArray<UBonkPlayer*>& ABonkGameModeBase::GetPlayers()
{
	return Players;
}

int32 ABonkGameModeBase::GetCurrentRound()
{
	return CurrentRound;
}

UBonkHUD* ABonkGameModeBase::GetHUD()
{
	return HUD;
}
