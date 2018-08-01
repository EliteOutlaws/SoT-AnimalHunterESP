
#include "Functions.h"
#include "cfg.h"
#include "Offsets.h"

IDirect3D9Ex* p_Object = 0;
IDirect3DDevice9Ex* p_Device = 0;
D3DPRESENT_PARAMETERS p_Params;

ID3DXLine* p_Line;
ID3DXFont* pFontSmall = 0;

int DirectXInit(HWND hWnd)
{
	if(FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(1);

	ZeroMemory(&p_Params, sizeof(p_Params));    
    p_Params.Windowed = TRUE;   
    p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;    
    p_Params.hDeviceWindow = hWnd;    
	p_Params.MultiSampleQuality   = D3DMULTISAMPLE_NONE;
    p_Params.BackBufferFormat = D3DFMT_A8R8G8B8 ;     
    p_Params.BackBufferWidth = Width;    
    p_Params.BackBufferHeight = Height;    
    p_Params.EnableAutoDepthStencil = TRUE;
    p_Params.AutoDepthStencilFormat = D3DFMT_D16;

	if(FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
		exit(1);

	if(!p_Line)
		D3DXCreateLine(p_Device, &p_Line);
		//p_Line->SetAntialias(1); *removed cuz crosshair was blurred*

	//D3DXCreateFont(p_Device, 18, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Calibri", &pFontSmall);
	D3DXCreateFont(p_Device, 12, 0, 400, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma", &pFontSmall);

	return 0;
}


 Vector2 me = Vector2(180, 180);
 Vector2 actor = Vector2(100, 220);
 float myangle = 180;
int Render()
{
	p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	p_Device->BeginScene();

	if(tWnd == GetForegroundWindow())
	{
		ReadData();
			
			auto World = mem.Read<ULONG_PTR>(WorldAddress);
			GNames = mem.Read<ULONG_PTR>(NamesAddress);
			ULONG_PTR owninggameinstance = mem.Read<ULONG_PTR>(World + Offsets::OwningGameInstance);
			ULONG_PTR LocalPlayers = mem.Read<ULONG_PTR>(owninggameinstance + Offsets::LocalPlayers);
			ULONG_PTR ULocalPlayer = mem.Read<ULONG_PTR>(LocalPlayers);
			ULONG_PTR PlayerController = mem.Read<ULONG_PTR>(ULocalPlayer + Offsets::PlayerController);
			auto LocalPlayer = mem.Read<ULONG_PTR>(PlayerController + Offsets::Pawn);
			auto LocalPlayeState = mem.Read<ULONG_PTR>(PlayerController + Offsets::PlayerState);
			auto HealthComponet = mem.Read<ULONG_PTR>(LocalPlayer + Offsets::HealthComponent);
			auto CameraManager = mem.Read<ULONG_PTR>(PlayerController + Offsets::PlayerCameraManager);
			auto RootComponent = mem.Read<ULONG_PTR>(LocalPlayer + Offsets::RootComponent);

			ULONG_PTR ULevel = mem.Read<ULONG_PTR>(World + Offsets::PersistentLevel);
			int ActorCount = mem.Read<int>(ULevel + Offsets::ActorsTArrayCount);

		//	cfg.SaveCfg();
		//	cfg.LoadCfg();

			auto LocalNamePointer = mem.Read<ULONG_PTR>(LocalPlayeState + Offsets::PlayerName);
			auto LocalName = mem.Read<textx>(LocalNamePointer);
			std::wstring mename = LocalName.word;
			using convert_type = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_type, wchar_t> converter;
			//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
			std::string menamestring = converter.to_bytes(mename);


			for (int i = 0; i < ActorCount; i++)
			{
				ULONG_PTR ActorList = mem.Read<ULONG_PTR>(ULevel + Offsets::ActorsTArray);
				ULONG_PTR Actor = mem.Read<ULONG_PTR>(ActorList + (i * 0x8));
				if (!Actor)
					continue;

				int ActorID = mem.Read<int>(Actor + Offsets::Id);
				auto ActorRootComponet = mem.Read<ULONG_PTR>(Actor + Offsets::RootComponent);
				auto Actorrelativelocation = mem.Read<Vector3>(ActorRootComponet + Offsets::RelativeLocation);//owninggameinstance.LocalPlayersPTR->LocalPlayers->PlayerController->PlayerState->RootComponent->RelativeLocation_0;
				auto ActorYaw = mem.Read<float>(ActorRootComponet + Offsets::RelativeRotationYaw);//owninggameinstance.LocalPlayersPTR->LocalPlayers->PlayerController->PlayerState->RootComponent->RelativeLocation_0;

				auto chunk = ActorID / 0x4000;
				auto fNamePtr = mem.Read<ULONG_PTR>(GNames + chunk * 8);
				auto fName = mem.Read<ULONG_PTR>(fNamePtr + 8 * (ActorID % 0x4000));
				auto rs = mem.Read<text>(fName + 16);
				std::string name = rs.word;

				AActors info;

				info.namesize = GetTextWidth(name.c_str(), pFontSmall);
				//if (name.find("BP_PlayerPirate_C") != std::string::npos || name.find("BP_TreasureChest_P") != std::string::npos || name.find("BP_BountyRewardSkull_P") != std::string::npos || name.find("BP_ShipwreckTreasureChest_P") != std::string::npos || (name.find("BP_MerchantCrate") != std::string::npos && name.find("Proxy") != std::string::npos) || name.find("BP_SmallShipTemplate_C") != std::string::npos || name.find("BP_LargeShipTemplate_C") != std::string::npos || name.find("Skeleton") != std::string::npos)
				if (name.find("BP_Chicken_") != std::string::npos)
				{

					info.type = chicken;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("Common") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "White Chicken";
						info.rareity = Common;
					}
					if (name.find("Rare") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Red Spotted Chicken";
						info.rareity = Rare;
					}
					if (name.find("Legendary") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Black Chicken";
						info.rareity = Legendary;
					}
					if (name.find("Mythical") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Golden Chicken";
						info.rareity = Mythical;
					}

					ActorArray.push_back(info);
				}
				// Pigs
				else if (name.find("BP_Pig_") != std::string::npos)
				{

					info.type = pig;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("Common") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Pink Pig";
						info.rareity = Common;
					}
					if (name.find("Rare") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Rare Pig";
						info.rareity = Rare;
					}
					if (name.find("Legendary") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Black Pig";
						info.rareity = Legendary;
					}
					if (name.find("Mythical") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Golden Pig";
						info.rareity = Mythical;
					}

					ActorArray.push_back(info);
				}
				// Snakes
				else if (name.find("BP_Snake_") != std::string::npos)
				{

					info.type = snake;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("Common") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Common Snake";
						info.rareity = Common;
					}
					if (name.find("Rare") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Rare Snake";
						info.rareity = Rare;
					}
					if (name.find("Legendary") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Black Snake";
						info.rareity = Legendary;
					}
					if (name.find("Mythical") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Golden Snake";
						info.rareity = Mythical;
					}

					ActorArray.push_back(info);
				}
				else if (name.find("BP_SmallShipNetProxy") != std::string::npos || name.find("BP_MediumShipNetProxy") != std::string::npos || name.find("BP_LargeShipNetProxy") != std::string::npos)
				{
					info.id = ActorID;
					info.name = "Ship";
					info.type = ship;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 300);
					info.yaw = ActorYaw;
					ActorArray.push_back(info);
				}
				else if (name.find("BP_SmallShipTemplate_C") != std::string::npos || name.find("BP_MediumShipTemplate_C") != std::string::npos || name.find("BP_LargeShipTemplate_C") != std::string::npos)
				{
					info.id = ActorID;
					info.name = "Ship";
					info.type = ship;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 300);
					info.yaw = ActorYaw;
					ActorArray.push_back(info);
				}
			}

			myLocation = mem.Read<Vector3>(RootComponent + Offsets::RelativeLocation);
			myAngles = mem.Read<Vector3>(CameraManager + Offsets::CameraRotation);
			Cameralocation = mem.Read<Vector3>(CameraManager + Offsets::CameraLocation);
			CameraFov = mem.Read<float>(CameraManager + Offsets::CameraFOV);
			auto myhealth  = mem.Read<float>(HealthComponet + Offsets::CurrentHealth);
			auto maxhealth = mem.Read<float>(HealthComponet + Offsets::MaxHealth);
			Sleep(2);


			//colored rects
			//FillRGB(30, 30, 300, 300, 255, 255, 255, 100);
			//DrawLine(30, 180, 330, 180, 0, 0, 0, 255);
			//DrawLine(180, 30, 180, 330, 0, 0, 0, 255);
			FillRGB(180, 30, 1, 300, 75, 68, 67, 90);
			FillRGB(30, 180, 300, 1, 75, 68, 67, 90);

			for (int i = 0; i < ActorArray.size(); i++)
			{

				float angle = getAngle(Vector2(myLocation.x, myLocation.y), Vector2(ActorArray.at(i).Location.x, ActorArray.at(i).Location.y));
				Vector2 dist = Vector2((myLocation.x - ActorArray.at(i).Location.x) / 100, (myLocation.y - ActorArray.at(i).Location.y) / 100);
				
			
				int distance = Vector2(myLocation.x, myLocation.y).DistTo(Vector2(ActorArray.at(i).Location.x, ActorArray.at(i).Location.y)) / 100;

				ActorArray.at(i).name = ActorArray.at(i).name + " [" + std::to_string(distance) + "m]";

				Vector2 point2dist = me - dist;

				Vector2 ScreenPoint = RotatePoint(point2dist, me, -myAngles.y - 90, false);
				if (ScreenPoint.x < 30)
					ScreenPoint.x = 30;
				if (ScreenPoint.y < 30)
					ScreenPoint.y = 30;

				if (ScreenPoint.x > 330)
					ScreenPoint.x = 330;
				if (ScreenPoint.y > 330)
					ScreenPoint.y = 330;


				if (ActorArray.at(i).type == ship)
				{
					FillRGB(ScreenPoint.x - 5, ScreenPoint.y - 5, 10, 10, 0, 255, 0, 255);
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString((char*)ActorArray.at(i).name.c_str(), ScreenPoint.x, ScreenPoint.y, 0, 255, 0, pFontSmall);
				}

				else if (ActorArray.at(i).rareity == Common)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 165, 42, 42, 255);
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 165, 42, 42, pFontSmall);
				}

				else if (ActorArray.at(i).type == animalcrate)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 230, 230, 250, 255);
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 230, 230, 250, pFontSmall);
				}
				else if (ActorArray.at(i).type == gunpowder)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 0, 0, 255);
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 255, 0, 0, pFontSmall);
				}
				else if (ActorArray.at(i).rareity == Rare)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 0, 255, 255, 255);
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 0, 255, 255, pFontSmall);
				}
				else if (ActorArray.at(i).rareity == Legendary)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 105, 180, 255);
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 255, 105, 180, pFontSmall);
				}
				else if (ActorArray.at(i).rareity == Mythical)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 75, 0, 130, 255);
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 75, 0, 130, pFontSmall);
			
				}
				else
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 215, 0, 255);
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 255, 215, 05, pFontSmall);
				}

				ActorArray.erase(ActorArray.begin() + i);

			}//END DRAWING LOOP
	}//END IF GAME WINDOW FOCUSED

	//----------------
	// KEYBOARD INPUT
	//----------------
	if (GetAsyncKeyState(VK_F9) & 1)
	{

		exit(1);
	}

	p_Device->EndScene();
	p_Device->PresentEx(0, 0, 0, 0, 0);
	return 0;
}
