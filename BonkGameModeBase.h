// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BonkGameModeBase.generated.h"

class UBonkPlayer;
class ABonkCameraActor;
class ABonkPlayerPawn;
class APlayerStart;
class UBonkHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreRoundStart, int32, RoundNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundStart, int32, RoundNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundEnd, int32, RoundNumber);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, int32, PlayerIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerReady, int32, PlayerIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopEntered);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamRoundWin, const FText&, TeamName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamMatchWin, const FText&, TeamName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerSwitchedTeam, int32, PlayerIndex, int32, TeamIndex);

USTRUCT(BlueprintType)
struct FTeamStructure
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TeamName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FColor TeamColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UBonkPlayer*> TeamPlayers;

	FTeamStructure()
	{
		WinCount = 0;
	}
};

UCLASS()
class BONK_API ABonkGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ABonkGameModeBase();
	
	

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Bonk GameMode", WorldContext="WorldContext"))
	static ABonkGameModeBase* Get(UObject* WorldContext);
	
	// Use for first time start match, will call StartRound.
	UFUNCTION(BlueprintCallable)
	void StartMatch();

	// Used to setup the round (like camera switch, spawning players, incrementing the round) and let something else (like a countdown) handle starting of the round.
	UFUNCTION(BlueprintCallable)
	void StartPreRound();
	
	UFUNCTION(BlueprintCallable)
	void StartRound();
	
	UFUNCTION(BlueprintCallable)
	void EndRound();

	UFUNCTION(BlueprintCallable)
	void GoToShop();

	void EndMatch(const FTeamStructure& WinningTeam);

	UPROPERTY(BlueprintAssignable)
	FOnPreRoundStart OnPreRoundStart;
	UPROPERTY(BlueprintAssignable)
	FOnRoundStart OnRoundStart;
	UPROPERTY(BlueprintAssignable)
	FOnRoundEnd OnRoundEnd;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerJoined OnPlayerJoined;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerReady OnPlayerReady;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerSwitchedTeam OnPlayerSwitchedTeam;
	
	UPROPERTY(BlueprintAssignable)
	FOnTeamRoundWin OnTeamRoundWin;
	UPROPERTY(BlueprintAssignable)
	FOnTeamMatchWin OnTeamMatchWin;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnShopEntered OnShopEntered;

	UFUNCTION(BlueprintCallable)
	void SwitchTeamForPlayer(int32 PlayerIndex, int32 NewTeamIndex);
	// Indicates that a player has chosen their team and is ready to start the match.
	UFUNCTION(BlueprintCallable)
	void LockInPlayer(int32 Index);

	UFUNCTION(BlueprintPure)
	bool CheckAllPlayersLockedIn();
	UFUNCTION(BlueprintPure)
	bool CanStartMatch();

	// Testing
	UFUNCTION(BlueprintCallable)
	void PlayerDied(int32 PlayerIndex, int32 PlayerKillerIndex);

	UPROPERTY(EditAnywhere)
	bool bIsDebugging;
	
	UFUNCTION(BlueprintPure)
	UBonkPlayer* GetPlayer(int32 PlayerIndex);
	
	UFUNCTION(BlueprintPure)
	const TArray<UBonkPlayer*>& GetPlayers();
	
	UFUNCTION(BlueprintPure)
	const FTeamStructure& GetTeam(int32 TeamIndex);
	
	UFUNCTION(BlueprintPure)
	const TArray<FTeamStructure>& GetTeams();
	
	UFUNCTION(BlueprintPure)
	int32 GetCurrentRound();
	
	UFUNCTION(BlueprintPure)
	UBonkHUD* GetHUD();

	UFUNCTION(BlueprintCallable)
	void EnablePlayerInputs();
	UFUNCTION(BlueprintCallable)
	void DisablePlayerInputs();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 VolumeIndex;
	
protected:
	
	virtual void BeginPlay() override;
	
private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABonkPlayerPawn> PlayerPawnClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBonkHUD> HUDWidgetClass;

	UPROPERTY(EditAnywhere)
	TArray<FTeamStructure> Teams;
	UPROPERTY(EditAnywhere)
	TArray<UBonkPlayer*> Players;
	UPROPERTY(VisibleAnywhere)
	TArray<APlayerStart*> PlayerStarts;
	UPROPERTY(VisibleAnywhere)
	TArray<ACameraActor*> Cameras;
	UPROPERTY(VisibleAnywhere)
	ABonkCameraActor* GameCamera;
	UPROPERTY(VisibleAnywhere)
	UBonkHUD* HUD;

	UPROPERTY(VisibleAnywhere)
	FName CurrentCamera;
	
	UPROPERTY(VisibleAnywhere)
	int32 CurrentRound;

	// Amount of round wins required to win the match.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 RoundsToWin;
	
	// The minimum amount of players required to start a match.
	UPROPERTY(EditAnywhere)
	int32 MinimumPlayers;

	UPROPERTY(VisibleAnywhere)
	bool bIsRoundLive;

	// This will create the UObject that will hold the player's match data.
	void RegisterPlayer(int32 Index);

	// Creates the pawns used to gather player inputs during the lobby phase.
	void CreateLobbyPawns();
	void CreateHUD();

	// We always want a player 0 in the lobby.
	void AddPlayerZeroToLobby();

	void DestroyPlayerPawns();
	void DestroyLobbyPawns();
	void DestroyCameras();

	// This only needs to be called right before the match starts. Assigns all the players and locks them to their chosen team.
	void AddPlayersToTeams();

	void GatherPlayerStarts();
	void GatherCameras();
	
	void TransitionToCamera(const FName& Tag);

	void SpawnPlayers();

	// This is a workaround to prevent the game camera being switched when Player 0's pawn gets destroyed.
	// By default the PlayerController will try to find a new ViewTarget whenever they lose possession of something.
	// Since we only use a single locked camera we don't want this behaviour.
	UFUNCTION()
	void RescueCamera(AActor* Actor);

	bool CheckForRoundWin(FTeamStructure& OutWinningTeam);
	bool CheckForMatchWin(FTeamStructure& OutWinningTeam);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDied, int32, PlayerIndex);
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDied OnPlayerDied;
};
