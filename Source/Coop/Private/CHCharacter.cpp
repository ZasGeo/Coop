// Fill out your copyright notice in the Description page of Project Settings.


#include "CHCharacter.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Coop.h"
#include "CHWeapon.h"
#include "CHHealthComponent.h"
#include "UnrealNetwork.h"

// Sets default values
ACHCharacter::ACHCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::COLLISION_WEAPON, ECollisionResponse::ECR_Ignore);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraCom"));
	CameraComp->SetupAttachment(SpringArm);
	
	HealthComp = CreateDefaultSubobject<UCHHealthComponent>(TEXT("HealthComp"));

	HealthComp->OnChangeHealth.AddDynamic(this, &ACHCharacter::OnHealthChanged);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	
}

// Called when the game starts or when spawned
void ACHCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	
	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		CurrentWeapon = GetWorld()->SpawnActor<ACHWeapon>(StartWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocket);
		}
	}
	

}

void ACHCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector()*AxisValue);
}

void ACHCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector()*AxisValue);
}

void ACHCharacter::BeginCrouch()
{
	Crouch();
}

void ACHCharacter::EndCrouch()
{
	UnCrouch();
}

void ACHCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ACHCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ACHCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ACHCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ACHCharacter::OnHealthChanged(UCHHealthComponent* HealthComponent, float Health, float DeltaHealth, const UDamageType* DamageType,AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bIsDied)
	{
		bIsDied = true;

		GetMovementComponent()->StopMovementImmediately();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
	}
}

// Called every frame
void ACHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void ACHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ACHCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACHCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ACHCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACHCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACHCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACHCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACHCharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ACHCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ACHCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACHCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACHCharacter::StopFire);
}

FVector ACHCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ACHCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACHCharacter, CurrentWeapon);
	DOREPLIFETIME(ACHCharacter, bIsDied);
}