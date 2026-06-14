// NBChatInput.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NBChatInput.generated.h"

class UEditableTextBox;
class UScrollBox;
class UVerticalBox;

/**
 *
 */
UCLASS()
class PROJECTEX_09_API UNBChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	
	void AddChatLine(const FString& InMessageString, const FLinearColor& InColor);

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	
	void CreateChatLogWidgets();

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;

protected:
	
	UPROPERTY()
	TObjectPtr<UScrollBox> ChatLogScrollBox;

	
	UPROPERTY()
	TObjectPtr<UVerticalBox> ChatLogVerticalBox;

	
	UPROPERTY(EditDefaultsOnly)
	int32 MaxChatLines = 50;
};
