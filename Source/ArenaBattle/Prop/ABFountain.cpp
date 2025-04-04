// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/ABFountain.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Net/UnrealNetwork.h"
#include "ArenaBattle.h"
#include "EngineUtils.h"

// Sets default values
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));

	RootComponent = Body;
	Water->SetupAttachment(Body);
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 132.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'"));
	if (BodyMeshRef.Object)
	{
		Body->SetStaticMesh(BodyMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	if (WaterMeshRef.Object)
	{
		Water->SetStaticMesh(WaterMeshRef.Object);
	}

	bReplicates = true;
	SetNetUpdateFrequency(1.f);
	SetNetCullDistanceSquared(4000000.f);  // 2000 * 2000 = 20미터
	//NetDormancy = DORM_Initial;
}

// Called when the game starts or when spawned
void AABFountain::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
	/*
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				// 4 * 1000 = 4000Byte로 TotalNetBandWidth를 넘기면 [LogNetTraffic] Saturated 발생
				//BigData.Init(BigDataElement, 1000);

				// 값이 변경될 때만 리플리케이션
				//BigDataElement += 1.0f;

				//ServerLightColor = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f);
				//OnRep_ServerLightColor();

				const FLinearColor NewLightColor = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f);
				//MulticastRPCChangeLightColor(NewLightColor);
				
				// 오너가 설정되기 전엔 서버에서만 실행됨
				// 오너가 클라이언트 1번으로 설정되면 클라이언트에서만 실행됨
				// 물론 다른 클라이언트에서도 실행되지 않음
				ClientRPCChangeLightColor(NewLightColor);
			}
		), 1.0f, true, 0.0f);

		FTimerHandle Handle2;
		GetWorld()->GetTimerManager().SetTimer(Handle2, FTimerDelegate::CreateLambda([&]
			{
				//FlushNetDormancy(); // 휴면 상태 해지

				// 서버가 구동된 직후 설정하면 클라이언트가 존재하지 않아 오너십을 설정할 수 없음
				//for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
				//{
				//	APlayerController* PlayerController = Iterator->Get();
				//	if (PlayerController && !PlayerController->IsLocalPlayerController()) // 서버는 제외
				//	{
				//		// 클라이언트 1번을 오너로 설정
				//		SetOwner(PlayerController);
				//		break;
				//	}
				//}

				// EngineUtils.h를 포함하여 TActorRange<>() 함수로 원하는 타입의 액터를 가져올 수 있음
				for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
				{
					if (PlayerController && !PlayerController->IsLocalPlayerController())
					{
						SetOwner(PlayerController);
						break;
					}
				}
			}
		), 10.0f, false, -1.0f); // Loop는 false, Delay가 0이면 바로 실행하는데 마이너스면 10초 뒤에 실행

		FTimerHandle Handle3;
		GetWorld()->GetTimerManager().SetTimer(Handle3, FTimerDelegate::CreateLambda([&]
			{
				// 프로퍼티 리플리케이션, 클라이언트가 나중에 들어와도 동기화
				//ServerLightColor = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f);
				//OnRep_ServerLightColor();

				// Multicast RPC, 클라이언트가 나중에 들어오면 버려짐
				//const FLinearColor NewLightColor = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f);
				//MulticastRPCChangeLightColor(NewLightColor);
			}
		), 5.0f, false, -1.0f);
	*/
	}
	else
	{
		// Server RPC를 호출하기 위해선 오너십이 있어야함
		// 하지만 클라이언트에서 Owner를 설정해봤자 서버엔 변동이 없어 의미없음
		//SetOwner(GetWorld()->GetFirstPlayerController());

		//FTimerHandle Handle;
		//GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		//	{
		//		ServerRPCChangeLightColor();
		//	}
		//), 1.0f, true, 0.0f);
	}
}

// Called every frame
void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		AddActorLocalRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
		ServerRotationYaw = RootComponent->GetComponentRotation().Yaw;
	}
	else
	{
		ClientTimeSinceUpdate += DeltaTime;
		if (ClientTimeBetweenLastUpdate < KINDA_SMALL_NUMBER)
		{
			return;
		}

		const float EstimateRotationYaw = ServerRotationYaw + RotationRate * ClientTimeBetweenLastUpdate;
		const float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdate;

		FRotator ClientRotator = RootComponent->GetComponentRotation();
		const float ClientNewYaw = FMath::Lerp(ServerRotationYaw, EstimateRotationYaw, LerpRatio);
		ClientRotator.Yaw = ClientNewYaw;
		RootComponent->SetWorldRotation(ClientRotator);
	}
}

void AABFountain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AABFountain, ServerRotationYaw);
	//DOREPLIFETIME(AABFountain, BigData);
	DOREPLIFETIME(AABFountain, ServerLightColor);
}
/*
void AABFountain::OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::OnActorChannelOpen(InBunch, Connection);

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
}

bool AABFountain::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	bool NetRelevantResult = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	if (!NetRelevantResult)
	{
		AB_LOG(LogABNetwork, Log, TEXT("Not Relevant:[%s] %s"), *RealViewer->GetName(), *SrcLocation.ToCompactString());
	}

	return NetRelevantResult;
}

void AABFountain::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::PreReplication(ChangedPropertyTracker);
}
*/
void AABFountain::OnRep_ServerRotationYaw()
{
	//AB_LOG(LogABNetwork, Log, TEXT("Yaw : %f"), ServerRotationYaw);

	FRotator NewRotator = RootComponent->GetComponentRotation();
	NewRotator.Yaw = ServerRotationYaw;
	RootComponent->SetWorldRotation(NewRotator);

	ClientTimeBetweenLastUpdate = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0.f;
}

void AABFountain::OnRep_ServerLightColor()
{
	if (!HasAuthority())
	{
		AB_LOG(LogABNetwork, Log, TEXT("LightColor : %s"), *ServerLightColor.ToString());
	}

	UPointLightComponent* PointLight = Cast<UPointLightComponent>(GetComponentByClass(UPointLightComponent::StaticClass()));
	if (PointLight)
	{
		PointLight->SetLightColor(ServerLightColor);
	}
}

void AABFountain::MulticastRPCChangeLightColor_Implementation(const FLinearColor& NewLightColor)
{
	AB_LOG(LogABNetwork, Log, TEXT("LightColor : %s"), *NewLightColor.ToString());

	UPointLightComponent* PointLight = Cast<UPointLightComponent>(GetComponentByClass(UPointLightComponent::StaticClass()));
	if (PointLight)
	{
		PointLight->SetLightColor(NewLightColor);
	}
}

bool AABFountain::ServerRPCChangeLightColor_Validate()
{
	// Validation이 false라면 즉시 클라이언트의 접속을 끊음
	// 해당 클라이언트의 넷모드는 Standalone이 됨
	//return false;

	return true;
}

void AABFountain::ServerRPCChangeLightColor_Implementation()
{
	const FLinearColor NewLightColor = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f);
	MulticastRPCChangeLightColor(NewLightColor);
}

void AABFountain::ClientRPCChangeLightColor_Implementation(const FLinearColor& NewLightColor)
{
	AB_LOG(LogABNetwork, Log, TEXT("LightColor : %s"), *NewLightColor.ToString());

	UPointLightComponent* PointLight = Cast<UPointLightComponent>(GetComponentByClass(UPointLightComponent::StaticClass()));
	if (PointLight)
	{
		PointLight->SetLightColor(NewLightColor);
	}
}
