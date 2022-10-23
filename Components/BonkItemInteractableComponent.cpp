#include "BonkItemInteractableComponent.h"
#include "BonkItemInteractComponent.h"
#include "Bonk/Libraries/BonkMacros.h"
#include "Kismet/KismetSystemLibrary.h"

UBonkItemInteractableComponent::UBonkItemInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBonkItemInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBonkItemInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBonkItemInteractableComponent::SetPrimitiveComponent(UPrimitiveComponent* PrimitiveComponent)
{
	PrimitiveComp = PrimitiveComponent;
}

void UBonkItemInteractableComponent::TrySelect(UBonkItemInteractComponent* InteractComp)
{
	if (bIsHeld)
		return;
	
	// SelectedCount++;
	// if (SelectedCount != 1)
	// 	return;
	
	InteractComp->SelectedInteractable = this;
	LOG("Selected Interactable");
	OnSelect.Broadcast();
}

void UBonkItemInteractableComponent::TryDeselect(UBonkItemInteractComponent* InteractComp)
{
	InteractComp->SelectedInteractable = nullptr;
	// SelectedCount--;
	// if (SelectedCount > 0)
	// 	return;

	LOG("Deselected Interactable");
	OnDeselect.Broadcast();
}

void UBonkItemInteractableComponent::Pickup(UBonkItemInteractComponent* InteractComp)
{
	const FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		false);
	
	PrimitiveComp->SetSimulatePhysics(false);
	PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetOwner()->AttachToComponent(InteractComp->SkeletalComponentAttach, AttachmentRules, SocketName);

	HeldByInteract = InteractComp;
	LastHeldByInteract = InteractComp;
	bIsHeld = true;
	TryDeselect(InteractComp);
	OnPickupResponse.Broadcast(InteractComp);
}

void UBonkItemInteractableComponent::Drop(UBonkItemInteractComponent* InteractComp, FVector Force)
{
	GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	PrimitiveComp->SetSimulatePhysics(true);
	PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PrimitiveComp->AddAngularImpulseInDegrees(InteractComp->GetOwner()->GetActorRightVector() * TorqueOnDrop * Force.Length());
	PrimitiveComp->AddImpulse(Force);
	HeldByInteract = nullptr;
	bIsHeld = false;
	OnDropResponse.Broadcast(InteractComp);
}

void UBonkItemInteractableComponent::Action(UBonkItemInteractComponent* InteractComp)
{
	OnActionResponse.Broadcast(InteractComp);
}
