// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ShooterGame : ModuleRules
{
	public ShooterGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "UMG", "CoreUObject", "Engine", "InputCore", "AIModule", "GameplayTasks", "OnlineSubsystem", "OnlineSubsystemUtils"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        DynamicallyLoadedModuleNames.AddRange(new string[] {"OnlineSubsystemNull" } );

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
