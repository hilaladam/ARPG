﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPG_WeaponBase.h"
#include <Components/PrimitiveComponent.h>
#include "CharacterBase.h"
#include "ARPG_ItemCoreBase.h"
#include "SocketMoveTraceManager.h"
#include "HumanBase.h"
#include "ARPG_WeaponCoreBase.h"
#include "ARPG_Item_Log.h"
#include "ARPG_Battle_Log.h"
#include "ARPG_CharacterAnimType.h"
#include "ARPG_ActorFunctionLibrary.h"
#include "ARPG_DebugFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "ARPG_Item"

AARPG_WeaponBase::AARPG_WeaponBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UARPG_WeaponCoreBase>(GET_MEMBER_NAME_CHECKED(AARPG_WeaponBase, InnerItemCore)))
{
	AttackAnimSet = CreateDefaultSubobject<UARPG_AttackAnimSetNormal>(GET_MEMBER_NAME_CHECKED(AARPG_WeaponBase, AttackAnimSet));
}

void AARPG_WeaponBase::UseItemImpl_Implementation(class UARPG_ItemCoreBase* ItemCore, class ACharacterBase* ItemOwner, EUseItemInput UseItemInput) const
{
	ItemOwner->EquipWaepon(CastChecked<UARPG_WeaponCoreBase>(ItemCore), UseItemInput);
}

FText AARPG_WeaponBase::GetItemTypeDescImpl_Implementation(const class UXD_ItemCoreBase* ItemCore) const
{
	return LOCTEXT("ARPG_Weapon", "武器");
}

void AARPG_WeaponBase::WhenUse(class ACharacterBase* ItemOwner)
{
	Item_Display_LOG("%s装备武器%s", *UXD_DebugFunctionLibrary::GetDebugName(ItemOwner), *UXD_DebugFunctionLibrary::GetDebugName(this));
	if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		SocketMoveTracer = NewObject<USocketMoveTracer>(this, GET_MEMBER_NAME_CHECKED(AARPG_WeaponBase, SocketMoveTracer));
		SocketMoveTracer->OnTraceActorNative.BindUObject(this, &AARPG_WeaponBase::WhenAttackTracedActor);
		SocketMoveTracer->InitSocketMoveTracer(Root, SocketMoveTracerConfig);
	}
	Super::WhenUse(ItemOwner);
}

void AARPG_WeaponBase::WhenNotUse(class ACharacterBase* ItemOwner)
{
	Super::WhenNotUse(ItemOwner);
}

void AARPG_WeaponBase::WhenRemoveFromInventory_Implementation(class AActor* ItemOwner, class UXD_ItemCoreBase* ItemCore, int32 RemoveNumber, int32 ExistNumber) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(ItemOwner))
	{
		if (Human->RightWeapon && Human->RightWeapon->IsEqualWithItemCore(ItemCore))
		{
			if (Human->LeftWeapon && Human->LeftWeapon->IsEqualWithItemCore(ItemCore))
			{
				if (ExistNumber == 1)
				{
					Human->SetLeftWeapon(nullptr);
				}
			}
			if (ExistNumber <= 0)
			{
				Human->SetRightWeapon(nullptr);
			}
		}
		else if (ExistNumber == 0 && Human->LeftWeapon && Human->LeftWeapon->IsEqualWithItemCore(ItemCore))
		{
			Human->SetLeftWeapon(nullptr);
		}
	}
}

void AARPG_WeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AARPG_WeaponBase::EnableNearAttackTrace(const FApplyPointDamageParameter& ApplyPointDamageParameter, bool ClearIgnoreList /*= true*/)
{
	PointDamageParameter = ApplyPointDamageParameter;
	SocketMoveTracer->OnTraceActorNative.BindUObject(this, &AARPG_WeaponBase::WhenAttackTracedActor);
	SocketMoveTracer->EnableTrace(ClearIgnoreList);
}

void AARPG_WeaponBase::DisableNearAttackTrace()
{
	SocketMoveTracer->DisableTrace();
}

void AARPG_WeaponBase::WhenAttackTracedActor(UPrimitiveComponent* HitComponent, const FName& SocketName, AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& TraceResult)
{
	if (OtherActor && OtherActor != GetItemOwner())
	{
		if (ACharacterBase* WeaponOnwer = Cast<ACharacterBase>(GetItemOwner()))
		{
			Battle_Display_LOG("%s所持武器%s打击到%s", *UARPG_DebugFunctionLibrary::GetDebugName(WeaponOnwer), *UARPG_DebugFunctionLibrary::GetDebugName(this), *UARPG_DebugFunctionLibrary::GetDebugName(OtherActor));

			if (ACharacterBase* ReceiveDamageCharacter = Cast<ACharacterBase>(OtherActor))
			{
				PointDamageParameter.AddHitStunValue = GetHitStunValue(PointDamageParameter.AddHitStunValue);

				if (ReceiveDamageCharacter->ApplyPointDamage(GetPhysicsAttackValue(), (ReceiveDamageCharacter->GetActorLocation() - TraceResult.ImpactPoint).GetSafeNormal2D(), TraceResult, WeaponOnwer, this, nullptr, PointDamageParameter) > 0.f)
				{
					WeaponOnwer->NearAttackSuccessTimeDilation(0.2f);
				}
			}
		}
	}
}

bool AARPG_WeaponBase::TraceForExecuteOther()
{
	return ExecuteActionSet.TraceForExecuteOther(GetItemOwner());
}

void AARPG_WeaponBase::EnableFallingAttackTrace(const FApplyPointDamageParameter& ApplyPointDamageParameter, bool ClearIgnoreList /*= true*/)
{
	PointDamageParameter = ApplyPointDamageParameter;
	SocketMoveTracer->OnTraceActorNative.BindUObject(this, &AARPG_WeaponBase::WhenFallingAttackTracedActor);
	SocketMoveTracer->EnableTrace(ClearIgnoreList);
}


void AARPG_WeaponBase::DisableFallingAttackTrace()
{
	SocketMoveTracer->DisableTrace();
}

void AARPG_WeaponBase::WhenFallingAttackTracedActor(UPrimitiveComponent* HitComponent, const FName& SocketName, AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& TraceResult)
{
	WhenAttackTracedActor(HitComponent, SocketName, OtherActor, OtherComp, TraceResult);
}

void AARPG_WeaponBase::WhenInHand()
{
	ReceiveWhenInHand();
}

void AARPG_WeaponBase::WhenInWeaponBack()
{
	ReceiveWhenInWeaponBack();
}

void AARPG_WeaponBase::AttachWeaponTo(class USceneComponent* InParent, FName InSocketName)
{
	AttachToComponent(InParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
}

#undef LOCTEXT_NAMESPACE
