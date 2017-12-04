// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterHUD.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "ShooterPlayerController.h"
#include "ShooterCharacter.h"
#include "ShooterGameState.h"
#include "Weapon.h"


AShooterHUD::AShooterHUD() : HealthBarPos(FVector2D(5.f, 5.f)), TimerPos(FVector2D(10.f, 5.f)), FirstWeaponPos(FVector2D(15.f, 65.f))
{
	static ConstructorHelpers::FObjectFinder<UTexture> HUDMainTextureOb(TEXT("/Game/UI/HUD/HUDMain"));
	
	//Crosshair
	Crosshair[EShooterCrosshairDirection::Left]		= UCanvas::MakeIcon(HUDMainTextureOb.Object, 43, 402, 25, 9);
	Crosshair[EShooterCrosshairDirection::Right]	= UCanvas::MakeIcon(HUDMainTextureOb.Object, 88, 402, 25, 9);
	Crosshair[EShooterCrosshairDirection::Top]		= UCanvas::MakeIcon(HUDMainTextureOb.Object, 74, 371, 9, 25);
	Crosshair[EShooterCrosshairDirection::Bottom]	= UCanvas::MakeIcon(HUDMainTextureOb.Object, 74, 415, 9, 25);
	Crosshair[EShooterCrosshairDirection::Center]	= UCanvas::MakeIcon(HUDMainTextureOb.Object, 75, 403, 7, 7);

	//Timer
	MatchTimerBg = UCanvas::MakeIcon(HUDMainTextureOb.Object, 262, 12, 255, 62);
	TimerIcon = UCanvas::MakeIcon(HUDMainTextureOb.Object, 381, 93, 24, 24);
	//Font
	static ConstructorHelpers::FObjectFinder<UFont> ShooterFont(TEXT("/Game/UI/HUD/Roboto51"));
	TimerFont = ShooterFont.Object;

	static ConstructorHelpers::FObjectFinder<UTexture> HealthTexture(TEXT("/Game/UI/HUD/HUDAssets02"));

	//HealthbarBackground
	HealthbarBg = UCanvas::MakeIcon(HealthTexture.Object, 67, 162, 372, 50);
	Healthbar = UCanvas::MakeIcon(HealthTexture.Object, 67, 212, 372, 50);
	HealthIcon = UCanvas::MakeIcon(HealthTexture.Object, 76, 262, 28, 28);

	//Weapon
	FirstWeaponBg = UCanvas::MakeIcon(HUDMainTextureOb.Object, 543, 17, 441, 81);
	FirstWeaponIcon = UCanvas::MakeIcon(HUDMainTextureOb.Object, 282, 389, 147, 67);
	FirstWeaponClipIcon = UCanvas::MakeIcon(HUDMainTextureOb.Object, 148, 151, 60, 51);

	ShadowFont.bEnableShadow = true;
	FontColor = FColor(110, 124, 131, 255);
}

void AShooterHUD::DrawHUD()
{
	ScaleUI = Canvas->ClipY / 1080.f;

	AShooterPlayerController* playerController = Cast<AShooterPlayerController>(PlayerOwner);

	if (playerController)
	{
		DrawCrosshair();
		DrawHealthBar();
		DrawMatchTimer();
		DrawWeapon();
	}
}

void AShooterHUD::DrawCrosshair()
{
		float CenterX, CenterY;
		CenterX = Canvas->ClipX / 2.f;
		CenterY = Canvas->ClipY / 2.f;

		//DrawIcon
		Canvas->DrawIcon(Crosshair[EShooterCrosshairDirection::Center], CenterX - ScaleUI * Crosshair[EShooterCrosshairDirection::Center].UL / 2.f,
			CenterY - ScaleUI *Crosshair[EShooterCrosshairDirection::Center].VL / 2.f, ScaleUI);

		Canvas->DrawIcon(Crosshair[EShooterCrosshairDirection::Left], CenterX - ScaleUI * (Crosshair[EShooterCrosshairDirection::Center].UL / 2.f + Crosshair[EShooterCrosshairDirection::Left].UL + CrosshairSpace),
			CenterY - ScaleUI * (Crosshair[EShooterCrosshairDirection::Left].VL / 2.f), ScaleUI);
		Canvas->DrawIcon(Crosshair[EShooterCrosshairDirection::Right], CenterX + ScaleUI * (Crosshair[EShooterCrosshairDirection::Center].UL / 2.f + CrosshairSpace),
			CenterY - ScaleUI * (Crosshair[EShooterCrosshairDirection::Right].VL / 2.f), ScaleUI);

		Canvas->DrawIcon(Crosshair[EShooterCrosshairDirection::Top], CenterX - ScaleUI * Crosshair[EShooterCrosshairDirection::Top].UL / 2.f,
			CenterY - ScaleUI * (Crosshair[EShooterCrosshairDirection::Center].UL / 2.f + CrosshairSpace + Crosshair[EShooterCrosshairDirection::Top].VL), ScaleUI);
		Canvas->DrawIcon(Crosshair[EShooterCrosshairDirection::Bottom], CenterX - ScaleUI * Crosshair[EShooterCrosshairDirection::Bottom].UL / 2.f,
			CenterY + ScaleUI * (Crosshair[EShooterCrosshairDirection::Center].UL / 2.f + CrosshairSpace), ScaleUI);
}

void AShooterHUD::DrawHealthBar()
{
	Canvas->DrawIcon(HealthbarBg, HealthBarPos.X*ScaleUI, HealthBarPos.Y*ScaleUI, ScaleUI);

	AShooterCharacter* ShooterCharactor = Cast<AShooterCharacter>(GetOwningPawn());
	if (ShooterCharactor)
	{
		float HealthPercent = (float)ShooterCharactor->GetCurHealth() / ShooterCharactor->GetMaxHealth();

		FCanvasTileItem TileItem(HealthBarPos, Healthbar.Texture->Resource, FVector2D(Healthbar.UL*HealthPercent*ScaleUI, Healthbar.VL*ScaleUI), FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		MakeUV(Healthbar, TileItem.UV0, TileItem.UV1, Healthbar.U, Healthbar.V, Healthbar.UL, Healthbar.VL);

		Canvas->DrawItem(TileItem);
		//	UE_LOG(LogTemp, Warning, TEXT("Health: %d %d %f"), ShooterCharactor->GetCurHealth(), ShooterCharactor->GetMaxHealth(), HealthPercent);
	}
}

void AShooterHUD::MakeUV(FCanvasIcon Icon, FVector2D& UV0, FVector2D& UV1, int16 U, int16 V, int16 UL, int16 VL)
{
	if (Icon.Texture)
	{
		float Width = Icon.Texture->GetSurfaceWidth();
		float Height = Icon.Texture->GetSurfaceHeight();

		UV0 = FVector2D(U / Width, V / Height);
		UV1 = FVector2D((U + UL) / Width, (V + VL) / Height);
	}
}

void AShooterHUD::DrawMatchTimer()
{
	AShooterGameState* GameState = Cast<AShooterGameState>(GetWorld()->GetGameState());

	if (GameState)
	{
		//TimerBg
		Canvas->DrawIcon(MatchTimerBg, Canvas->ClipX - Canvas->OrgX - TimerPos.X * ScaleUI - MatchTimerBg.UL *ScaleUI,
			TimerPos.Y*ScaleUI, ScaleUI);
		Canvas->DrawIcon(TimerIcon, Canvas->ClipX - (TimerPos.X + MatchTimerBg.UL - TimerIcon.UL/2.f)* ScaleUI,
			(TimerPos.Y + MatchTimerBg.VL / 2.f - TimerIcon.VL / 2.f)*ScaleUI, ScaleUI);

		//绘制剩余时间
		FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), TimerFont, FontColor);
		TextItem.EnableShadow(FLinearColor::Black);
		TextItem.FontRenderInfo = ShadowFont;
		TextItem.SetColor(FontColor);
		float TimeScale = 0.57;
		TextItem.Scale = FVector2D(TimeScale*ScaleUI, TimeScale*ScaleUI);
		TextItem.Font = TimerFont;

		FString TimeText = MakeRemainTimeText(GameState->GameRemainTime);
		TextItem.Text = FText::FromString(TimeText);

		//获取字体绘制的尺寸
		float SizeX, SizeY;
		Canvas->StrLen(TimerFont, TimeText, SizeX, SizeY);

		TextItem.Position = FVector2D(Canvas->ClipX - (TimerPos.X + MatchTimerBg.UL - 1.5f*TimerIcon.UL - 10.f)*ScaleUI,
			(TimerPos.Y + MatchTimerBg.VL / 2.f - SizeY*TimeScale /2.f)*ScaleUI);

		Canvas->DrawItem(TextItem);

	}
}

FString AShooterHUD::MakeRemainTimeText(float time)
{
	int32 TotalTime = FMath::Max(0, FMath::TruncToInt(time)%3600);
	int32 TimeMin = TotalTime / 60;
	int32 TimeSec = TotalTime % 60;

	FString text = FString::Printf(TEXT("%02d:%02d"), TimeMin, TimeSec);
	return text;
}

void AShooterHUD::DrawWeapon()
{
	AShooterCharacter* ShooterCharactor = Cast<AShooterCharacter>(GetOwningPawn());
	AWeapon* CurrentWeapon = ShooterCharactor ? ShooterCharactor->GetCurrentWeapon() : nullptr;
	if (ShooterCharactor && CurrentWeapon)
	{
	//---------------【常量定义】-------------------------
		//武器 图标 盒子
		const float WeaponBoxWidth = 160.f;
		//弹夹 偏移
		const float WeaponClipOffset = 30.f;
		//弹夹数量
		const float WeaponClipAmount = 4;
		//子弹数量 文字盒子宽度
		const float TextBoxWidht = 65.f;

	//--------------------【绘制背景】---------------------
		const float WeaponBgPosX = Canvas->ClipX - (FirstWeaponPos.X + FirstWeaponBg.UL)*ScaleUI;
		const float WeaponBgPosY = Canvas->ClipY - (FirstWeaponPos.Y + FirstWeaponBg.VL)*ScaleUI;
		Canvas->DrawIcon(FirstWeaponBg, WeaponBgPosX, WeaponBgPosY, ScaleUI);

	//---------------------【绘制武器】----------------------------
		const float WeaponIconPosX = Canvas->ClipX - Canvas->OrgX - (FirstWeaponPos.X + WeaponBoxWidth / 2.f + FirstWeaponIcon.UL / 2.f)*ScaleUI;
		const float WeaponIconPosY = WeaponBgPosY + (FirstWeaponBg.VL / 2.f - FirstWeaponIcon.VL / 2.f)*ScaleUI;
		Canvas->DrawIcon(FirstWeaponIcon, WeaponIconPosX, WeaponIconPosY, ScaleUI);

	//---------------------【子弹数量 文字部分】----------------
		float TextScale = 0.5;
		float SizeX, SizeY;
		int32 CurrentAmmoAmount = CurrentWeapon->GetCurrentAmmoAmount();
		int32 MaxAmmoAmount = CurrentWeapon->GetMaxAmmoAmount();
		FString AmountString = FString::Printf(TEXT("%d/%d"), CurrentAmmoAmount, MaxAmmoAmount);
		FText AmountText = FText::FromString(AmountString);
		Canvas->StrLen(TimerFont, AmountString, SizeX, SizeY);

		const float AmmoBoxPosX = Canvas->ClipX - (FirstWeaponPos.X + WeaponBoxWidth + TextBoxWidht/2.f + SizeX/2.f)*ScaleUI;
		const float AmmoBoxPosY = WeaponBgPosY + (FirstWeaponBg.VL/2.f - SizeY/2.f -5.f)*ScaleUI;

		FCanvasTextItem AmountTextItem(FVector2D(AmmoBoxPosX, AmmoBoxPosY), AmountText, TimerFont, FontColor );
		AmountTextItem.Scale = FVector2D(TextScale, TextScale);
		AmountTextItem.BlendMode = SE_BLEND_Translucent;

		Canvas->DrawItem(AmountTextItem);

	//---------------------【剩余弹夹数量 文字部分】----------------
		int32 ClipAmount = CurrentWeapon->GetClipAmount();
		AmountString = FString::Printf(TEXT("Clip:%d"), ClipAmount);
		AmountText = FText::FromString(AmountString);
		Canvas->StrLen(TimerFont, AmountString, SizeX, SizeY);

		const float TimeBoxPosX = Canvas->ClipX - (FirstWeaponPos.X + WeaponBoxWidth + TextBoxWidht / 2.f + SizeX / 2.f - 20.f)*ScaleUI;
		const float TimeBoxPosY = WeaponBgPosY + (FirstWeaponBg.VL / 2.f)*ScaleUI;
		TextScale = 0.3;
		FCanvasTextItem ClipAmountTextItem(FVector2D(TimeBoxPosX, TimeBoxPosY), AmountText, TimerFont, FontColor);
		ClipAmountTextItem.Scale = FVector2D(TextScale, TextScale);
		ClipAmountTextItem.BlendMode = SE_BLEND_Translucent;

		Canvas->DrawItem(ClipAmountTextItem);



	//------------------【绘制弹夹】-----------------------------------------
		const float WeaponClipWidth = FirstWeaponClipIcon.UL + WeaponClipOffset*(WeaponClipAmount - 1);
		const float WeaponClipPosX = WeaponBgPosX + WeaponClipOffset*ScaleUI;
		const float WeaponClipPosY = WeaponBgPosY + (FirstWeaponBg.VL / 2.f - FirstWeaponClipIcon.VL / 2.f)*ScaleUI;
		const float AmmoPerClip = MaxAmmoAmount / WeaponClipAmount;
		for (int i = 0; i < WeaponClipAmount; i++)
		{
			int32 color = 255;
			if (i*AmmoPerClip > CurrentAmmoAmount)
			{
				color = 128;				
			}
			if (CurrentAmmoAmount ==0)
			{
				color = 128;
			}
			Canvas->SetDrawColor(color, color, color, color);
			Canvas->DrawIcon(FirstWeaponClipIcon, WeaponClipPosX + i*WeaponClipOffset*ScaleUI, WeaponClipPosY, ScaleUI);
		}
	}
}