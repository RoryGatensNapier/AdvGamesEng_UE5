// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_StoryChapter.generated.h"

/**
 * 
 */
UCLASS()
class AGE_DEMO_API UDA_StoryChapter : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> Images;

	UPROPERTY(EditAnywhere)
	UStringTable* Lines;
	
	UFUNCTION()
	TArray<UTexture2D*> getImages() { return Images; }

	UFUNCTION()
	UStringTable* getLines() { return Lines; }

};
