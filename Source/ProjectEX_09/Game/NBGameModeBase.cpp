// NBGameModeBase.cpp


#include "Game/NBGameModeBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "EngineUtils.h"
#include "NBGameStateBase.h"

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(NBPlayerController) == true)
	{
		NBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(NBPlayerController);

		ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			NBPS->PlayerNameString =
				TEXT("Player") +
				FString::FromInt(AllPlayerControllers.Num());

			switch (AllPlayerControllers.Num())
			{
			case 1:
				NBPS->ChatColor = FLinearColor::Red;
				break;

			case 2:
				NBPS->ChatColor = FLinearColor::Blue;
				break;

			case 3:
				NBPS->ChatColor = FLinearColor::Green;
				break;

			case 4:
				NBPS->ChatColor = FLinearColor::Yellow;
				break;

			default:
				NBPS->ChatColor = FLinearColor::White;
				break;
			}
		}

		ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameStateBase) == true)
		{
			NBGameStateBase->MulticastRPCBroadcastLoginMessage(NBPS->PlayerNameString);
		}
	}
}

FString ANBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ANBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			if (UniqueDigits.Contains(C) == true)
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ANBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void ANBGameModeBase::PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	if (IsGuessNumberString(InChatMessageString) == true)
	{
		FString GuessNumberString = InChatMessageString;
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		ANBPlayerState* PS =
			InChattingPlayerController->GetPlayerState<ANBPlayerState>();

		FString CombinedMessageString =
			PS->GetPlayerInfoString()
			+ TEXT(": ")
			+ GuessNumberString
			+ TEXT(" -> ")
			+ JudgeResultString;

		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPlayerController = *It;
			if (IsValid(NBPlayerController))
			{
				NBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString,
					PS->ChatColor);
				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(
			TEXT("다시 입력하세요"),
			FLinearColor::White);
	}
}
void ANBGameModeBase::IncreaseGuessCount(ANBPlayerController* InChattingPlayerController)
{
	ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPS) == true)
	{
		NBPS->CurrentGuessCount++;
	}
}
void ANBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& NBPlayerController : AllPlayerControllers)
	{
		ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			NBPS->CurrentGuessCount = 0;
			UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
		}
	}
}

void ANBGameModeBase::JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			if (IsValid(NBPS) == true)
			{
				FString CombinedMessageString = NBPS->PlayerNameString + TEXT(" has won the game.");
				NBPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
			if (IsValid(NBPS) == true)
			{
				if (NBPS->CurrentGuessCount < NBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& NBPlayerController : AllPlayerControllers)
			{
				NBPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}