// Fill out your copyright notice in the Description page of Project Settings.


#include "CHCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ACHCharacter::ACHCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraCom"));
	CameraComp->SetupAttachment(SpringArm);
	
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	

}

// Called when the game starts or when spawned
void ACHCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

// Called every frame
void ACHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

