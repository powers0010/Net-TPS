// Fill out your copyright notice in the Description page of Project Settings.

#include "NetWorkGameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


UNetWorkGameInstance::UNetWorkGameInstance()
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UNetWorkGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UNetWorkGameInstance::OnStartOnlineGameComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UNetWorkGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UNetWorkGameInstance::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UNetWorkGameInstance::OnDestroySessionComplete);
}

bool UNetWorkGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSubsys = IOnlineSubsystem::Get();

	if (OnlineSubsys)
	{
		//typedef TSharedPtr < class IOnlineSession , ESPMode::ThreadSafe > IOnlineSessionPtr
		IOnlineSessionPtr Sessions = OnlineSubsys->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			SessionSettings->Set(SETTING_MAPNAME, FString("Untitled"), EOnlineDataAdvertisementType::ViaOnlineService);
			
			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			// Our delegate should get called when this is complete (doesn't need to be successful!)
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No OnlineSubsytem found!"));
	}
	return false;
}

//Function fired when a session create request has completed
void UNetWorkGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete!"));
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Sessions = OnlineSubsys->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
				Sessions->StartSession(SessionName);
			}
		}
	}
}

//Function fired when a session start request has completed
void UNetWorkGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnStartOnlineGameComplete!"));
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Sessions = OnlineSubsys->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Untitled", true, "listen");
	}
}

void UNetWorkGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserID, bool bIsLAN, bool bIsPresence)
{
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Sessions = OnlineSubsys->GetSessionInterface();
		if (Sessions.IsValid() && UserID.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;

			if (bIsPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			
			TSharedRef<FOnlineSessionSearch> SessionSearchRef = SessionSearch.ToSharedRef();
			Sessions->FindSessions(*UserID, SessionSearchRef);
		}
	}
	else
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}

void UNetWorkGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete!"));
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Sessions = OnlineSubsys->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			UE_LOG(LogTemp, Warning, TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num());
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num());
			}
			if (SessionSearch->SearchResults.Num() > 0)
			{
				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
					// This is something you can't do in Blueprint for example!
					UE_LOG(LogTemp, Warning, TEXT("Session Number: %d | Sessionname: %s"), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName));
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session Number: %d"), SearchIdx + 1);
					}
				}
			}
		}
	}
}

bool UNetWorkGameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserID, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccessful = false;
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Sessions = OnlineSubsys->GetSessionInterface();
		if (Sessions.IsValid() && UserID.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			bSuccessful = Sessions->JoinSession(*UserID, SessionName, SearchResult);
		}
	}
	return bSuccessful;
}

void UNetWorkGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete!"));
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Sessions = OnlineSubsys->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			// This is something the Blueprint Node "Join Session" does automatically!
			APlayerController * const PlayerController = GetFirstLocalPlayerController();

			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UNetWorkGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete!"));
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Sessions = OnlineSubsys->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "TextMap", true);
			}
		}
	}
}


//---------BlueprintCallable Functions to test this Setup
void UNetWorkGameInstance::StartOnlineGame()
{
	ULocalPlayer* const player = GetFirstGamePlayer();

	HostSession(player->GetPreferredUniqueNetId(), GameSessionName, true, true, 4);
}

void UNetWorkGameInstance::FindOnlineGames()
{
	ULocalPlayer* const player = GetFirstGamePlayer();

	FindSessions(player->GetPreferredUniqueNetId(), true, true);
}

void UNetWorkGameInstance::JoinOnlineGame()
{

	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
	FOnlineSessionSearchResult SearchResult;

	// If the Array is not empty, we can go through it
	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			// To avoid something crazy, we filter sessions from ourself
			if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
			{
				SearchResult = SessionSearch->SearchResults[i];

				// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
				// use a widget where you click on and have a reference for the GameSession it represents which you can use
				// here
				JoinSession(Player->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
				break;
			}
		}
	}
}

void UNetWorkGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

			Sessions->DestroySession(GameSessionName);
		}
	}
}