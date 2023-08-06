#include "stdafx.h"

#include "Utils/Util.h"


void Main()
{
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	uint16 a = 0b1100'1111'0000'1111;
	Print(U"{:016B}"_fmt(Utils::GetBits<1, 2>(a)));;
	Print(U"{:016B}"_fmt(Utils::GetBits<1, 6>(a)));;
	Print(U"{:016B}"_fmt(Utils::GetBits<1, 10>(a)));;
	Print(U"{:016B}"_fmt(Utils::GetBits<1, 14>(a)));;

	Print(U"{:016B}"_fmt(Utils::SetBits<1, 2>(a, uint16(0b11))));;
	Print(U"{:016B}"_fmt(Utils::SetBits<1, 6>(a, uint16(~0))));;
	Print(U"{:016B}"_fmt(Utils::SetBits<1, 10>(a, uint16(~0))));;
	Print(U"{:016B}"_fmt(Utils::SetBits<1, 14>(a, uint16(~0))));;

	while (System::Update())
	{
	}
}
