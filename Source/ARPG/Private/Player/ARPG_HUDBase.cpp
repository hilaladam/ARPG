﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPG_HUDBase.h"
#include <Components/SphereComponent.h>
#include <GameFramework/PlayerController.h>
#include "ARPG_InteractInterface.h"
#include "CharacterBase.h"



void AARPG_HUDBase::BeginPlay()
{
	Super::BeginPlay();
	AttachToActor(GetOwningPlayerController()->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
}

void AARPG_HUDBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPlayerController()->GetPawn()))
	{
		if ((OldCharacterLocation - Character->GetActorLocation()).Size2D() >= InteractableActorsUpdateDistance)
		{
			Algo::Sort(PotentialInteractableActors, [&](AActor* LHS, AActor* RHS)
			{
				if (!Character->CanInteract(RHS))
				{
					return true;
				}
				return Character->GetHorizontalDistanceTo(LHS) < Character->GetHorizontalDistanceTo(RHS);
			});
			OldCharacterLocation = Character->GetActorLocation();
		}

		TSet<AActor*> PreShowHintActors(ShowHintActors);
		ShowHintActors.Empty();
		for (AActor* PotentialInteractableActor : PotentialInteractableActors)
		{
			if (IARPG_InteractInterface::CanShowHintInfo(PotentialInteractableActor, Character))
			{
				ShowHintActors.Add(PotentialInteractableActor);
			}
		}

		for (AActor* EnableInteractActor : TSet<AActor*>(ShowHintActors).Difference(PreShowHintActors))
		{
			OnActorEnableHint.Broadcast(EnableInteractActor);
		}
		for (AActor* DisableInteractActor : PreShowHintActors.Difference(TSet<AActor*>(ShowHintActors)))
		{
			OnActorDisableHint.Broadcast(DisableInteractActor);
		}
	}
}

AARPG_HUDBase::AARPG_HUDBase()
{
	HintInfoCollector = CreateDefaultSubobject<USphereComponent>(GET_MEMBER_NAME_CHECKED(AARPG_HUDBase, HintInfoCollector));
	{
		HintInfoCollector->SetSphereRadius(400.f);
		HintInfoCollector->OnComponentBeginOverlap.AddDynamic(this, &AARPG_HUDBase::WhenHintInfoCollectorBeginOverlap);
		HintInfoCollector->OnComponentEndOverlap.AddDynamic(this, &AARPG_HUDBase::WhenHintInfoCollectorEndOverlap);
	}
}

void AARPG_HUDBase::WhenHintInfoCollectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UARPG_InteractInterface>())
	{
		PotentialInteractableActors.Add(OtherActor);
	}
}

void AARPG_HUDBase::WhenHintInfoCollectorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<UARPG_InteractInterface>())
	{
		PotentialInteractableActors.Remove(OtherActor);
	}
}

AActor* AARPG_HUDBase::GetNearestShowHintActor() const
{
	if (ShowHintActors.Num() > 0)
	{
		return ShowHintActors[0];
	}
	return nullptr;
}
