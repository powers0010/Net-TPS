// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Engine/GameInstance.h"
#include "UnrealNetwork.h"
#include "Online.h"
#include "NetWorkGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UNetWorkGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
//-----------BlueprintCallable Functions to test this Setup
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void StartOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void FindOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void JoinOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void DestroySessionAndLeaveGame();
//---------------------------------Create Session-------------------------------------
	UNetWorkGameInstance();
		/**
		*	Function to host a game
		*	@Param		UserID			User that started the request
		*	@Param		SessionName		Name of the Session
		*	@Param		bIsLAN			Is this is LAN Game?
		*	@Param		bIsPresence		"Is the Session to create a presence Session"
		*	@Param		MaxNumPlayers	        Number of Maximum allowed players on this "Session" (Server)
		*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

	//Delegate
	//Delegate for session created
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	//Delegate called when session started
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	//Handles to registered delegates for creating/starting a session
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	//SessionSettings
	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	//Function fired when a session create request has completed
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	//Function fired when a session start request has completed
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

//--------------------------Find Session-----------------------------------------
	/*
	*	Find an online session
	*
	*	@param UserId user that initiated the request
	*	@param bIsLAN are we searching LAN matches
	*	@param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserID, bool bIsLAN, bool bIsPresence);

	//Delegate
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	//Handle
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	//A variable for our SearchSettings which will also contain our SearchResults, once this search is complete and successful:
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	//Function To Delegate
	void OnFindSessionsComplete(bool bWasSuccessful);

//---------------------------------Joining Session-------------------------------------
	/**
	*	Joins a session via a search result
	*
	*	@param SessionName name of session
	*	@param SearchResult Session to join
	*
	*	@return bool true if successful, false otherwise
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserID, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	//Delegate
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	//Handle
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	
	/*	Delegate Function
	*	Delegate fired when a session join request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

//---------------------------------Destroying a Session-------------------------------------
	//Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	//Handle to registered delegate for destroying a session
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	/**
	*	Delegate fired when a destroying an online session has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
};
