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
//-------------��׼�ġ�---------	
	//׼��
	UPROPERTY()
	FCanvasIcon Crosshair[5];
	//׼������ �� ���� ֮��ļ�����
	UPROPERTY(EditDefaultsOnly, category = Crosshair)
	float CrosshairSpace = 5.f;
//--------------��Ѫ����----------
	//Ѫ����ʼ���꣨���Ͻǣ�
	UPROPERTY(EditDefaultsOnly, category = HealthBar)
	FVector2D HealthBarPos;
	//Ѫ������
	UPROPERTY()
	FCanvasIcon HealthbarBg;
	//Ѫ��
	UPROPERTY()
	FCanvasIcon Healthbar;
	//Ѫ����� +
	UPROPERTY()
	FCanvasIcon HealthIcon;
//--------------����ʱ����------------
	//��ʱ�����ϽǼ�϶�����Ͻǣ�
	UPROPERTY(EditDefaultsOnly, category = Timer)
	FVector2D TimerPos;
	//��ʱ������
	UPROPERTY()
	FCanvasIcon MatchTimerBg;
	//��ʱ����־
	UPROPERTY()
	FCanvasIcon TimerIcon;
	//��ʱ������
	UPROPERTY()
	UFont* TimerFont;

	FFontRenderInfo ShadowFont;

	FColor FontColor;
//--------------��������-------------
	//����ͼ�� ���½�ƫ��
	UPROPERTY(EditDefaultsOnly, category = HealthBar)
	FVector2D FirstWeaponPos;
	//��ǰ����ͼ�� ����
	UPROPERTY()
	FCanvasIcon FirstWeaponBg;
	//��ǰ����ͼ��
	UPROPERTY()
	FCanvasIcon FirstWeaponIcon;
	//��������
	UPROPERTY()
	FCanvasIcon FirstWeaponClipIcon;

//--------------�����ƺ�����-----------
	virtual void DrawHUD() override;

	void DrawCrosshair();

	void DrawHealthBar();

	void MakeUV(FCanvasIcon Icon, FVector2D& UV0, FVector2D& UV1, int16 U, int16 V, int16 UL, int16 VL);

	void DrawMatchTimer();

	FString MakeRemainTimeText(float time);

	void DrawWeapon();

	//�������ű���
	float ScaleUI;



};
