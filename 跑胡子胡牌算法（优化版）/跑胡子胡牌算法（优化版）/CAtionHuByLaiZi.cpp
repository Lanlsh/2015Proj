#include "stdafx.h"
#include "CAtionHuByLaiZi.h"

enum ENUM_Filter
{
	EF_SHUNZI = 1,
	EF_TST,
	EF_JIAOPAI,

	EF_TI,
	EF_PAO,
	EF_KAN,
	EF_JIANG
};

#define MENZI_LENGTH 7

//顺子分解

//二七十分解

//绞牌分解

CAtionHuByLaiZi::CAtionHuByLaiZi()
{
}


CAtionHuByLaiZi::~CAtionHuByLaiZi()
{
}

//获取是否可以胡牌
bool CAtionHuByLaiZi::GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo)
{
	if (vCard.empty())
	{
		return false;
	}

	bool bResult = false;
	bool isPeng = false;

	MZItem item[MENZI_LENGTH];
	//huSInfo.haveHowMuchMenZi = GethaveHowMuchMenZi(wChaiId, huSInfo.isHaveTiOrPao, huSInfo.huXiCount, item);
	//将操作牌加入到手牌中
	if (cOperator != Card_INVALID)
	{
		vCard.push_back(cOperator);
		if (std::count(vCard.begin(), vCard.end(), cOperator) == 3)
		{
			isPeng = true;
		}
	}
	std::sort(vCard.begin(), vCard.end());

	//再将手牌分为中的大小牌分离出来
	Card vCardBig[10] = { 0 };
	Card vCardSmall[10] = { 0 };
	DivideToTwoArray(vCard, vCardBig, vCardSmall);

	//先删除手上的跑牌
	//huSInfo.isHaveTiOrPao = RemovePaoPai(vCardSmall, vCardBig, item, huSInfo.haveHowMuchMenZi);

	Card cPengCard = Card_INVALID;
	if (isPeng)
	{
		cPengCard = cOperator;
	}

	//再删除手上的坎牌
	RemoveKanPai_New(vCardSmall, vCardBig, item, huSInfo.haveHowMuchMenZi, cPengCard);

	//if (cOperator != Card_INVALID)
	//{
	//	//如果胡的那张牌此时在手中有3张牌，则要扣掉坎牌和碰牌的息差
	//	if (isPeng)
	//	{
	//		if (cOperator < Card_Tiao_1)
	//		{
	//			huSInfo.huXiCount -= XN_Kan_Small - XN_Peng_Small;
	//		}
	//		else
	//		{
	//			huSInfo.huXiCount -= XN_Kan_Big - XN_Peng_Big;
	//		}
	//	}
	//}
	//检测牌型是否可以胡牌
	bResult = CheckHu(vCardSmall, vCardBig, item, huSInfo.haveHowMuchMenZi, huSInfo.isHaveTiOrPao, vCard.size(), huSInfo.huXiCount, huSInfo.vCard);

	return bResult;
}

bool CAtionHuByLaiZi::CheckHu(Card* vSmallCard, Card* vBigCard, MZItem item[], int& needMeziCount, bool isHaveTiOrPao, int handCardCount, int& huxiCount, CardVector& vCard)
{
	bool isCanHu = false;
	
	if ((!isHaveTiOrPao) && (handCardCount % 3 == 0))
	{
		//无将
		isCanHu = ComBineMenZi(vSmallCard, vBigCard, item, needMeziCount);
		if (isCanHu)
		{
			CardVector vTemp;
			huxiCount += GetHuxiByMenZi(item, vCard);
			if (huxiCount >= 10)
			{
				return true;
			}
		}
	}
	else
	{
		vector<int> vHuxi;
		std::vector<CardVector> lstVector;
		int tempHuxiCount = huxiCount;
		//有将
		bool isFindJiang = false;
		for (int i = 0; i < MAX_ARRAY_LENTH; i++)
		{
			if (vSmallCard[i] >= 2)
			{
				tempHuxiCount = huxiCount;
				isFindJiang = true;
				vSmallCard[i] -= 2;
				item[needMeziCount].cOpraterCard = i + 1;
				item[needMeziCount].i = EF_JIANG;
				item[needMeziCount].menzi[0] = i + 1;
				item[needMeziCount].menzi[1] = i + 1;

				if (needMeziCount == (MENZI_LENGTH - 1))
				{
					CardVector vTemp;
					huxiCount += GetHuxiByMenZi(item, vCard);
					if (huxiCount >= 10)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					isCanHu = ComBineMenZi(vSmallCard, vBigCard, item, needMeziCount + 1);
				}
				
				if (isCanHu)
				{
					CardVector vTemp;
					tempHuxiCount += GetHuxiByMenZi(item, vTemp);
					if (tempHuxiCount >= 10)
					{
						lstVector.push_back(vTemp);
						vHuxi.push_back(tempHuxiCount);
					}
				}

				vSmallCard[i] = 2;
			}
		}

		for (int i = 0; i < MAX_ARRAY_LENTH; i++)
		{
			if (vBigCard[i] >= 2)
			{
				tempHuxiCount = huxiCount;
				vBigCard[i] -= 2;
				item[needMeziCount].cOpraterCard = i + 0x11;
				item[needMeziCount].i = EF_JIANG;
				item[needMeziCount].menzi[0] = i + 0x11;
				item[needMeziCount].menzi[1] = i + 0x11;

				isCanHu = ComBineMenZi(vSmallCard, vBigCard, item, needMeziCount + 1);
				if (isCanHu)
				{
					CardVector vTemp;
					tempHuxiCount += GetHuxiByMenZi(item, vTemp);
					if (tempHuxiCount >= 10)
					{
						lstVector.push_back(vTemp);
						vHuxi.push_back(tempHuxiCount);
					}
				}

				vBigCard[i] = 2;
			}
		}

		if (vHuxi.size() > 0)
		{
			int idx = 0;
			for (int i = 1; i < vHuxi.size(); i++)
			{
				if (vHuxi[i] > vHuxi[i-1])
				{
					idx = i;
				}
			}

			huxiCount = vHuxi[idx];
			vCard = lstVector[idx];
			return true;
		}
	}

	return isCanHu;
}

bool CAtionHuByLaiZi::ComBineMenZi(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx)
{
	bool isOK = false;
	int index = 0;
	Card coperatorCard = Card_INVALID;
	index = SearchFirstValue(vSmallCard);
	bool bIsBigCard = false;

	if (index == MAX_ARRAY_LENTH)
	{
		index = SearchFirstValue(vBigCard);
		if (index == MAX_ARRAY_LENTH)
		{
			return false;
		}
		else
		{
			coperatorCard = index + 0x11;
			bIsBigCard = true;
		}
	}
	else
	{
		coperatorCard = index + 1;
	}

	item[idx].cOpraterCard = coperatorCard;

	Card vTempSmallCard[MAX_ARRAY_LENTH] = { 0 };
	Card vTempBigCard[MAX_ARRAY_LENTH] = { 0 };

	int cur_split = EF_SHUNZI;
	do
	{
		CardVector vTempHuCard;
		InitArray(vTempSmallCard, vSmallCard);
		InitArray(vTempBigCard, vBigCard);

		switch (cur_split)
		{
		case EF_SHUNZI:
			item[idx].i = EF_SHUNZI;
			isOK = RemoveShunZi(vTempSmallCard, vTempBigCard, item, idx, bIsBigCard, index);
			if ((idx < (MENZI_LENGTH - 1)) && isOK)
			{
				isOK = ComBineMenZi(vTempSmallCard, vTempBigCard, item, idx + 1);
			}
			break;
		case EF_TST:
			item[idx].i = EF_TST;
			isOK = RemoveTwoSevenTen(vTempSmallCard, vTempBigCard, item, idx, bIsBigCard, index);
			if ((idx < (MENZI_LENGTH - 1)) && isOK)
			{
				isOK = ComBineMenZi(vTempSmallCard, vTempBigCard, item, idx + 1);
			}
			break;
		case EF_JIAOPAI:
			item[idx].i = EF_JIAOPAI;
			isOK = RemoveJiaoPai(vTempSmallCard, vTempBigCard, item, idx, bIsBigCard, index);
			if ((idx < (MENZI_LENGTH - 1)) && isOK)
			{
				isOK = ComBineMenZi(vTempSmallCard, vTempBigCard, item, idx + 1);
			}
			break;
		}

		cur_split++;
	} while ((cur_split <= EF_JIAOPAI) && (!isOK));

	return isOK;
}

int CAtionHuByLaiZi::GethaveHowMuchMenZi(WORD wChairId, bool& isHaveTiOrPao, int& huxi, MZItem item[])
{
	isHaveTiOrPao = false;
	huxi = 0;
	return 0;
}

void CAtionHuByLaiZi::DivideToTwoArray(CardVector vHandCard, Card* vCardBig, Card* vCardSmall)
{
	if (vHandCard.empty() || (vCardBig == NULL) || (vCardSmall == NULL))
	{
		return;
	}

	int arrayLenth = vHandCard.size();
	for (int i = 0; i < arrayLenth; i++)
	{
		if ((vHandCard[i] < Card_Tiao_1) && (vHandCard[i] > Card_INVALID))
		{
			++vCardSmall[vHandCard[i] - 1];
		}
		else if ((vHandCard[i] >= Card_Tiao_1) && (vHandCard[i] <= Card_Tiao_10))
		{
			++vCardBig[vHandCard[i] - 0x11];
		}
	}
}

bool CAtionHuByLaiZi::RemovePaoPai(Card* vSmallCard, Card* vBigCard, MZItem item[], int& haveMuckMenZi)
{
	bool isHavePaoOrTi = false;
	for (int i = 0; i < MAX_ARRAY_LENTH; i++)
	{
		if (vSmallCard[i] == 4)
		{
			vSmallCard[i] = 0;

			item[haveMuckMenZi].cOpraterCard = vSmallCard[i];
			item[haveMuckMenZi].i = EF_PAO;
			item[haveMuckMenZi].menzi[0] = i + 1;
			item[haveMuckMenZi].menzi[1] = i + 1;
			item[haveMuckMenZi].menzi[2] = i + 1;
			item[haveMuckMenZi].menzi[3] = i + 1;
			haveMuckMenZi++;
			isHavePaoOrTi = true;
		}

		if (vBigCard[i] == 4)
		{
			vBigCard[i] = 0;
			item[haveMuckMenZi].cOpraterCard = vSmallCard[i];
			item[haveMuckMenZi].i = EF_PAO;
			item[haveMuckMenZi].menzi[0] = i + 0x11;
			item[haveMuckMenZi].menzi[1] = i + 0x11;
			item[haveMuckMenZi].menzi[2] = i + 0x11;
			item[haveMuckMenZi].menzi[3] = i + 0x11;
			haveMuckMenZi++;
			isHavePaoOrTi = true;
		}
	}

	return isHavePaoOrTi;
}

void CAtionHuByLaiZi::RemoveKanPai_New(Card* vSmallCard, Card* vBigCard, MZItem item[], int& haveMuckMenZi, Card cPengCard)
{
	for (int i = 0; i < MAX_ARRAY_LENTH; i++)
	{
		if (vSmallCard[i] == 3)
		{
			if (cPengCard == i + 1)
			{
				continue;
			}
			vSmallCard[i] = 0;

			item[haveMuckMenZi].cOpraterCard = i + 1;
			item[haveMuckMenZi].i = EF_KAN;
			item[haveMuckMenZi].menzi[0] = i + 1;
			item[haveMuckMenZi].menzi[1] = i + 1;
			item[haveMuckMenZi].menzi[2] = i + 1;
			haveMuckMenZi++;
		}

		if (vBigCard[i] == 3)
		{
			if (cPengCard == i + 0x11)
			{
				continue;
			}

			vBigCard[i] = 0;
			item[haveMuckMenZi].cOpraterCard = i + 0x11;
			item[haveMuckMenZi].i = EF_KAN;
			item[haveMuckMenZi].menzi[0] = i + 0x11;
			item[haveMuckMenZi].menzi[1] = i + 0x11;
			item[haveMuckMenZi].menzi[2] = i + 0x11;
			haveMuckMenZi++;
		}
	}
}

//删除顺子
bool CAtionHuByLaiZi::RemoveShunZi(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx)
{
	if ((firstCardIdx + 2) < MAX_ARRAY_LENTH)
	{
		int loopCount = 0;
		if (isBigCard)
		{
			loopCount = vBigCard[firstCardIdx] > vBigCard[firstCardIdx + 1] ? vBigCard[firstCardIdx + 1] : vBigCard[firstCardIdx];
			loopCount = loopCount > vBigCard[firstCardIdx + 2] ? vBigCard[firstCardIdx + 2] : loopCount;
			if (loopCount > 0)
			{
				vBigCard[firstCardIdx]--;
				vBigCard[firstCardIdx + 1]--;
				vBigCard[firstCardIdx + 2]--;
				item[idx].menzi[0] = firstCardIdx + 0x11;
				item[idx].menzi[1] = firstCardIdx + 0x12;
				item[idx].menzi[2] = firstCardIdx + 0x13;
				return true;
			}
		}
		else
		{
			loopCount = vSmallCard[firstCardIdx] > vSmallCard[firstCardIdx + 1] ? vSmallCard[firstCardIdx + 1] : vSmallCard[firstCardIdx];
			loopCount = loopCount > vSmallCard[firstCardIdx + 2] ? vSmallCard[firstCardIdx + 2] : loopCount;
			if (loopCount > 0)
			{
				vSmallCard[firstCardIdx]--;
				vSmallCard[firstCardIdx + 1]--;
				vSmallCard[firstCardIdx + 2]--;
				item[idx].menzi[0] = firstCardIdx + 1;
				item[idx].menzi[1] = firstCardIdx + 2;
				item[idx].menzi[2] = firstCardIdx + 3;
				return true;
			}
		}
	}

	return false;
}

//删除二七十
bool CAtionHuByLaiZi::RemoveTwoSevenTen(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx)
{
	//计算2-7-10大小字牌的数量

	if (firstCardIdx == 1)
	{
		int loopCount = 0;
		if (isBigCard)
		{
			loopCount = vBigCard[firstCardIdx] > vBigCard[firstCardIdx + 5] ? vBigCard[firstCardIdx + 5] : vBigCard[firstCardIdx];
			loopCount = loopCount > vBigCard[firstCardIdx + 8] ? vBigCard[firstCardIdx + 8] : loopCount;
			if (loopCount > 0)
			{
				vBigCard[1]--;
				vBigCard[6]--;
				vBigCard[9]--;
				item[idx].menzi[0] = Card_Tiao_2;
				item[idx].menzi[1] = Card_Tiao_7;
				item[idx].menzi[2] = Card_Tiao_10;
				return true;
			}
		}
		else
		{
			loopCount = vSmallCard[firstCardIdx] > vSmallCard[firstCardIdx + 5] ? vSmallCard[firstCardIdx + 5] : vSmallCard[firstCardIdx];
			loopCount = loopCount > vSmallCard[firstCardIdx + 8] ? vSmallCard[firstCardIdx + 8] : loopCount;
			if (loopCount > 0)
			{
				vSmallCard[1]--;
				vSmallCard[6]--;
				vSmallCard[9]--;
				item[idx].menzi[0] = Card_Wan_2;
				item[idx].menzi[1] = Card_Wan_7;
				item[idx].menzi[2] = Card_Wan_10;
				return true;
			}
		}
	}

	return false;
}

//删除绞牌
bool CAtionHuByLaiZi::RemoveJiaoPai(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx)
{
	//绞牌不算息，但也算一方门子
	//先查看小牌是否有一对或单只

	if (isBigCard)
	{
		if (vBigCard[firstCardIdx] == 1)
		{
			if (vSmallCard[firstCardIdx] == 2)
			{
				vBigCard[firstCardIdx]--;
				vSmallCard[firstCardIdx] = 0;
				item[idx].menzi[0] = firstCardIdx + 0x11;
				item[idx].menzi[1] = firstCardIdx + 1;
				item[idx].menzi[2] = firstCardIdx + 1;
			}
		}
		else if (vBigCard[firstCardIdx] == 2)
		{
			if ((vSmallCard[firstCardIdx] > 0) && (vSmallCard[firstCardIdx] < 3))
			{
				vBigCard[firstCardIdx] = 0;
				vSmallCard[firstCardIdx]--;
				item[idx].menzi[0] = firstCardIdx + 0x11;
				item[idx].menzi[1] = firstCardIdx + 0x11;
				item[idx].menzi[2] = firstCardIdx + 1;
			}
		}
	}
	else
	{
		if (vSmallCard[firstCardIdx] == 1)
		{
			if (vBigCard[firstCardIdx] == 2)
			{
				vSmallCard[firstCardIdx]--;
				vBigCard[firstCardIdx] = 0;
				item[idx].menzi[0] = firstCardIdx + 1;
				item[idx].menzi[1] = firstCardIdx + 0x11;
				item[idx].menzi[2] = firstCardIdx + 0x11;
				return true;
			}
		}
		else if (vSmallCard[firstCardIdx] == 2)
		{
			if ((vBigCard[firstCardIdx] > 0) && (vBigCard[firstCardIdx] < 3))
			{
				vSmallCard[firstCardIdx] = 0;
				vBigCard[firstCardIdx]--;
				item[idx].menzi[0] = firstCardIdx + 1;
				item[idx].menzi[1] = firstCardIdx + 1;
				item[idx].menzi[2] = firstCardIdx + 0x11;
				return true;
			}
		}
	}

	return false;
}

//获取门子胡息
int CAtionHuByLaiZi::GetHuxiByMenZi(MZItem item[], CardVector& vCard)
{
	int count = 0;
	for (int i = 0; i < MENZI_LENGTH; i++)
	{
		if ((item[i].i == EF_SHUNZI))
		{
			if (item[i].cOpraterCard == Card_Tiao_1)
			{
				count += XN_OneTwoThre_Big;
			}
			else if (item[i].cOpraterCard == Card_Wan_1)
			{
				count += XN_OneTwoThre_Small;
			}

			vCard.push_back(item[i].menzi[0]);
			vCard.push_back(item[i].menzi[1]);
			vCard.push_back(item[i].menzi[2]);
		}
		else if ((item[i].i == EF_TST))
		{
			if (item[i].cOpraterCard == Card_Tiao_2)
			{
				count += XN_TwoSevenTen_Big;
			}
			else if (item[i].cOpraterCard == Card_Wan_2)
			{
				count += XN_TwoSevenTen_Small;
			}

			vCard.push_back(item[i].menzi[0]);
			vCard.push_back(item[i].menzi[1]);
			vCard.push_back(item[i].menzi[2]);
		}
		else if ((item[i].i == EF_JIAOPAI))
		{
			vCard.push_back(item[i].menzi[0]);
			vCard.push_back(item[i].menzi[1]);
			vCard.push_back(item[i].menzi[2]);
		}
		else if ((item[i].i == EF_PAO))
		{
			if (item[i].cOpraterCard >= Card_Tiao_1)
			{
				count += XN_Pao_Big;
			}
			else
			{
				count += XN_Pao_Small;
			}

			vCard.push_back(item[i].menzi[0]);
			vCard.push_back(item[i].menzi[1]);
			vCard.push_back(item[i].menzi[2]);
			vCard.push_back(item[i].menzi[3]);
		}
		else if ((item[i].i == EF_KAN))
		{
			if (item[i].cOpraterCard >= Card_Tiao_1)
			{
				count += XN_Kan_Big;
			}
			else
			{
				count += XN_Kan_Small;
			}

			vCard.push_back(item[i].menzi[0]);
			vCard.push_back(item[i].menzi[1]);
			vCard.push_back(item[i].menzi[2]);
		}
		else if ((item[i].i == EF_TI))
		{
			if (item[i].cOpraterCard >= Card_Tiao_1)
			{
				count += XN_Ti_Big;
			}
			else
			{
				count += XN_Ti_Small;
			}

			vCard.push_back(item[i].menzi[0]);
			vCard.push_back(item[i].menzi[1]);
			vCard.push_back(item[i].menzi[2]);
			vCard.push_back(item[i].menzi[3]);
		}
		else if ((item[i].i == EF_JIANG))
		{
			vCard.push_back(item[i].menzi[0]);
			vCard.push_back(item[i].menzi[1]);
		}
	}

	return count;
}

//搜索第一次出现牌值的下标
int CAtionHuByLaiZi::SearchFirstValue(Card* card)
{
	for (int i = 0; i < MAX_ARRAY_LENTH; i++)
	{
		if (card[i] > 0)
			return i;
	}

	return MAX_ARRAY_LENTH;
}

//初始化数组
void CAtionHuByLaiZi::InitArray(Card* vdes, Card* vSource)
{
	if ((vdes == NULL) || (vSource == NULL))
	{
		return;
	}

	for (int i = 0; i < MAX_ARRAY_LENTH; i++)
	{
		vdes[i] = vSource[i];
	}
}