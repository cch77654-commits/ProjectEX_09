// NBPlayerController,h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UNBChatInput;
class UUserWidget;

/**
 * 
 */
UCLASS()
class PROJECTEX_09_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANBPlayerController();

	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(
		const FString& InChatMessageString,
		const FLinearColor& InColor);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(
		const FString& InChatMessageString,
		const FLinearColor& InColor);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNBChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UNBChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
};