// NBChatInput.cpp


#include "UI/NBChatInput.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Player/NBPlayerController.h"

void UNBChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}

	CreateChatLogWidgets();
}

void UNBChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInput::CreateChatLogWidgets()
{
	if (IsValid(ChatLogScrollBox) == true)
	{
		
		return;
	}

	UPanelWidget* RootPanel = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (IsValid(RootPanel) == false)
	{
		return;
	}

	ChatLogScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ChatLogScrollBox"));
	ChatLogVerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ChatLogVerticalBox"));

	if (IsValid(ChatLogScrollBox) == false || IsValid(ChatLogVerticalBox) == false)
	{
		return;
	}

	ChatLogScrollBox->AddChild(ChatLogVerticalBox);

	UPanelSlot* AddedSlot = RootPanel->AddChild(ChatLogScrollBox);
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(AddedSlot);
	if (IsValid(CanvasSlot) == true)
	{
		
		CanvasSlot->SetAnchors(FAnchors(0.f, 1.f, 0.5f, 1.f));
		CanvasSlot->SetAlignment(FVector2D(0.f, 1.f));
		CanvasSlot->SetOffsets(FMargin(20.f, -360.f, 0.f, 300.f));
		CanvasSlot->SetAutoSize(false);
	}
}

void UNBChatInput::AddChatLine(const FString& InMessageString, const FLinearColor& InColor)
{
	if (IsValid(ChatLogVerticalBox) == false)
	{
		CreateChatLogWidgets();
	}

	if (IsValid(ChatLogVerticalBox) == false)
	{
		return;
	}

	UTextBlock* NewLine = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	if (IsValid(NewLine) == false)
	{
		return;
	}

	NewLine->SetText(FText::FromString(InMessageString));
	NewLine->SetColorAndOpacity(FSlateColor(InColor));

	FSlateFontInfo FontInfo = NewLine->GetFont();
	FontInfo.Size = 14;
	NewLine->SetFont(FontInfo);

	ChatLogVerticalBox->AddChildToVerticalBox(NewLine);

	
	while (ChatLogVerticalBox->GetChildrenCount() > MaxChatLines)
	{
		ChatLogVerticalBox->RemoveChildAt(0);
	}

	if (IsValid(ChatLogScrollBox) == true)
	{
		ChatLogScrollBox->ScrollToEnd();
	}
}

void UNBChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			ANBPlayerController* OwningNBPlayerController = Cast<ANBPlayerController>(OwningPlayerController);
			if (IsValid(OwningNBPlayerController) == true)
			{
				OwningNBPlayerController->SetChatMessageString(Text.ToString());

				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}
