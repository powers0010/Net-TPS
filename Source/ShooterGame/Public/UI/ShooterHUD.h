// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include "ShooterHUD.generated.h"

/**
 * 
 */

/*UENUM()*/
enum /*class*/ EShooterCrosshairDirection /*: uint8*/
{
	Left = 0,
	Right,
	Center,
	Top,
	Bottom
};



UCLASS()
class SHOOTERGAME_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

public:
	AShooterHUD();
//-------------【准心】---------	
	//准心
	UPROPERTY()
	FCanvasIcon Crosshair[5];
	//准心中心 和 线条 之间的间隔宽度
	UPROPERTY(EditDefaultsOnly, category = Crosshair)
	float CrosshairSpace = 5.f;
//--------------【血条】----------
	//血条起始坐标（左上角）
	UPROPERTY(EditDefaultsOnly, category = HealthBar)
	FVector2D HealthBarPos;
	//血条背景
	UPROPERTY()
	FCanvasIcon HealthbarBg;
	//血条
	UPROPERTY()
	FCanvasIcon Healthbar;
	//血条标记 +
	UPROPERTY()
	FCanvasIcon HealthIcon;
//--------------【定时器】------------
	//定时器右上角间隙（右上角）
	UPROPERTY(EditDefaultsOnly, category = Timer)
	FVector2D TimerPos;
	//定时器背景
	UPROPERTY()
	FCanvasIcon MatchTimerBg;
	//定时器标志
	UPROPERTY()
	FCanvasIcon TimerIcon;
	//定时器字体
	UPROPERTY()
	UFont* TimerFont;

	FFontRenderInfo ShadowFont;

	FColor FontColor;
//--------------【武器】-------------
	//武器图标 右下角偏移
	UPROPERTY(EditDefaultsOnly, category = HealthBar)
	FVector2D FirstWeaponPos;
	//当前武器图标 背景
	UPROPERTY()
	FCanvasIcon FirstWeaponBg;
	//当前武器图标
	UPROPERTY()
	FCanvasIcon FirstWeaponIcon;
	//武器弹夹
	UPROPERTY()
	FCanvasIcon FirstWeaponClipIcon;

//--------------【绘制函数】-----------
	virtual void DrawHUD() override;

	void DrawCrosshair();

	void DrawHealthBar();

	void MakeUV(FCanvasIcon Icon, FVector2D& UV0, FVector2D& UV1, int16 U, int16 V, int16 UL, int16 VL);

	void DrawMatchTimer();

	FString MakeRemainTimeText(float time);

	void DrawWeapon();

	//绘制缩放比例
	float ScaleUI;



};
