#include "stdafx.h"
#include "CActionHuByArray.h"

CActionHuByArray::CActionHuByArray() :m_huxi(10)
{
}


CActionHuByArray::~CActionHuByArray()
{
}

int CActionHuByArray::GetFuZiHuXi(WORD wChairId, bool& isHaveTiOrPao)
{
	//获取玩家的附子牌胡息
	int huxi = 10;
	isHaveTiOrPao = false;
	return huxi;
}

void CActionHuByArray::DivideToTwoArray(CardVector vHandCard, Card* vCardBig, Card* vCardSmall)
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

//先将操作牌不放入到手牌中，将手牌中的顺子和刻字出去，最后再综合剩余牌，将操作牌加入到手牌中，看是否能组成333或332（332时需要有提牌或跑牌）
//回溯算法：深度优先搜索

//删除坎牌元素
void CActionHuByArray::DeleteKanPaiElement(CardVector& ShouPai, Card cardData)
{
	for (CardVector::iterator iter = ShouPai.begin(); iter != ShouPai.end(); iter++)
	{
		if (cardData == *iter)
		{
			iter = ShouPai.erase(iter, iter + 3);
			break;
		}
	}
}

//删除顺子元素
void CActionHuByArray::DeleteShunZiElement(Card* vCard, CardVector vDeleteElement, CardVector& vHuCard, int& huxiByHand, bool isBigCard, int count)
{
	if ((vCard == NULL) || (vDeleteElement.size() != 3))
	{
		return;
	}

	if (isBigCard)
	{
		vCard[vDeleteElement[0] - 0x11] -= count;
		vCard[vDeleteElement[1] - 0x11] -= count;
		vCard[vDeleteElement[2] - 0x11] -= count;
	}
	else
	{
		vCard[vDeleteElement[0] - 1] -= count;
		vCard[vDeleteElement[1] - 1] -= count;
		vCard[vDeleteElement[2] - 1] -= count;
	}

	for (int i = 0; i < count; i++)
	{
		vHuCard.push_back(vDeleteElement[0]);
		vHuCard.push_back(vDeleteElement[1]);
		vHuCard.push_back(vDeleteElement[2]);
	}

	if ((vDeleteElement[0] == Card_Tiao_1) || (vDeleteElement[0] == Card_Wan_1))
	{
		if (isBigCard)
		{
			huxiByHand += XN_OneTwoThre_Big*count;
		}
		else
		{
			huxiByHand += XN_OneTwoThre_Small*count;
		}
	}
	else if (((vDeleteElement[0] == Card_Tiao_2) || (vDeleteElement[0] == Card_Wan_2)) && ((vDeleteElement[1] == Card_Tiao_7) || (vDeleteElement[1] == Card_Wan_7)))
	{
		if (isBigCard)
		{
			huxiByHand += XN_TwoSevenTen_Big*count;
		}
		else
		{
			huxiByHand += XN_TwoSevenTen_Small*count;
		}
	}

}

//删除碰牌
void CActionHuByArray::RemovePeng(Card* vCard, CardVector& vHuCard, int& huxiByHand, bool isBigCard)
{
	//因为坎已经删除了，此时删除的碰为手中已有两张一样的牌a，别人摸到或到出a，此时能合成一方门子碰牌。此时最多只有一个3张一样的牌
	//if (ArrayValueIsZreo(vCard))
	//{
	//	return;
	//}

	for (int i = 0; i < MAX_ARRAY_LENTH; i++)
	{
		if (vCard[i] == 3)
		{
			int value = i + 1;
			if (isBigCard)
			{
				huxiByHand += XN_Peng_Small;
				value += 0x10;
			}
			else
			{
				huxiByHand += XN_Peng_Big;
			}

			vHuCard.push_back(value);
			vHuCard.push_back(value);
			vHuCard.push_back(value);
			vCard[i] = 0;
			break;
		}
	}
}


//删除顺子
void CActionHuByArray::RemoveShunZi(Card* vCard, CardVector& vHuCard, int& huxiByHand, bool isBigCard)
{
	//if (ArrayValueIsZreo(vCard))
	//{
	//	return;
	//}
	//普通的顺子
	bool isFindHaveShunZi = false;
	CardVector vTemp;

	for (int i = 0; i < MAX_ARRAY_LENTH;)
	{
		if ((i + 2) < MAX_ARRAY_LENTH)
		{
			int loopCount = vCard[i] > vCard[i + 1] ? vCard[i + 1] : vCard[i];
			loopCount = loopCount > vCard[i + 2] ? vCard[i + 2] : loopCount;

			if (loopCount > 0)
			{
				CardVector vTemp;
				if (isBigCard)
				{
					vTemp.push_back(i + 0x11);
					vTemp.push_back(i + 1 + 0x11);
					vTemp.push_back(i + 2 + 0x11);
				}
				else
				{
					vTemp.push_back(i + 1);
					vTemp.push_back(i + 2);
					vTemp.push_back(i + 3);
				}

				DeleteShunZiElement(vCard, vTemp, vHuCard, huxiByHand, isBigCard, loopCount);
				i += 3;
			}
			else
			{
				i++;
			}
		}
		else
		{
			break;
		}
	}

}

//删除二七十
void CActionHuByArray::RemoveTwoSevenTen(Card* vCard, CardVector& vHuCard, int& huxiByHand, bool isBigCard)
{
	//计算2-7-10大小字牌的数量
	//if (ArrayValueIsZreo(vCard))
	//{
	//	return;
	//}

	int cardCount = (vCard[1] > vCard[6]) ? vCard[6] : vCard[1];
	cardCount = (cardCount > vCard[9]) ? vCard[9] : cardCount;

	if (cardCount)
	{
		CardVector vTemp;
		if (isBigCard)
		{
			vTemp.push_back(Card_Tiao_2);
			vTemp.push_back(Card_Tiao_7);
			vTemp.push_back(Card_Tiao_10);
		}
		else
		{
			vTemp.push_back(Card_Wan_2);
			vTemp.push_back(Card_Wan_7);
			vTemp.push_back(Card_Wan_10);
		}

		DeleteShunZiElement(vCard, vTemp, vHuCard, huxiByHand, isBigCard, cardCount);
	}
}


//删除坎牌
void CActionHuByArray::RemoveKanPai(CardVector& vCard, CardVector& vHuCard, int& huxiByHand)
{
	if (vCard.empty())
	{
		return;
	}
	bool isFindHaveKanPai = false;

	int findIndex = 0;
	do
	{
		isFindHaveKanPai = false;
		if (findIndex + 2 >= vCard.size())
		{
			break;
		}

		for (int i = findIndex; i < vCard.size(); i++)
		{
			if (std::count(vCard.begin(), vCard.end(), vCard[i]) == 3)
			{
				isFindHaveKanPai = true;
				if (vCard[i] <= Card_Wan_10)
				{
					huxiByHand += XN_Kan_Small;
				}
				else
				{
					huxiByHand += XN_Kan_Big;
				}

				vHuCard.push_back(vCard[i]);
				vHuCard.push_back(vCard[i]);
				vHuCard.push_back(vCard[i]);
				DeleteKanPaiElement(vCard, vCard[i]);
				break;
			}

			findIndex++;
		}

	} while (isFindHaveKanPai);
}

void CActionHuByArray::RemovePaoPai(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huXiByHand)
{
	for (int i = 0; i < MAX_ARRAY_LENTH; i++)
	{
		if (vSmallCard[i] == 4)
		{
			vSmallCard[i] = 0;
			vHuCard.push_back(i + 1);
			vHuCard.push_back(i + 1);
			vHuCard.push_back(i + 1);
			vHuCard.push_back(i + 1);
			huXiByHand += XN_Pao_Small;
		}

		if (vBigCard[i] == 4)
		{
			vBigCard[i] = 0;
			vHuCard.push_back(i + 0x11);
			vHuCard.push_back(i + 0x11);
			vHuCard.push_back(i + 0x11);
			vHuCard.push_back(i + 0x11);
			huXiByHand += XN_Pao_Big;
		}
	}
}

void CActionHuByArray::RemoveKanPai_New(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huXiByHand)
{
	for (int i = 0; i < MAX_ARRAY_LENTH; i++)
	{
		if (vSmallCard[i] == 3)
		{
			vSmallCard[i] = 0;
			vHuCard.push_back(i + 1);
			vHuCard.push_back(i + 1);
			vHuCard.push_back(i + 1);
			huXiByHand += XN_Kan_Small;
		}

		if (vBigCard[i] == 3)
		{
			vBigCard[i] = 0;
			vHuCard.push_back(i + 0x11);
			vHuCard.push_back(i + 0x11);
			vHuCard.push_back(i + 0x11);
			huXiByHand += XN_Kan_Big;
		}
	}
}

//删除绞牌
void CActionHuByArray::RemoveJiaoPai(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard)
{
	//绞牌不算息，但也算一方门子
	//先查看小牌是否有一对或单只
	//if (ArrayValueIsZreo(vSmallCard) || ArrayValueIsZreo(vBigCard))
	//{
	//	return;
	//}

	for (int i = 0; i < MAX_ARRAY_LENTH; i++)
	{
		if (vSmallCard[i] == 1)
		{
			if (vBigCard[i] == 2)
			{
				vSmallCard[i] = 0;
				vBigCard[i] = 0;
				vHuCard.push_back(i + 1);
				vHuCard.push_back(i + 0x11);
				vHuCard.push_back(i + 0x11);
			}
		}
		else if (vSmallCard[i] == 2)
		{
			if ((vBigCard[i] > 0) && (vBigCard[i] < 3))
			{
				vSmallCard[i] = 0;
				vBigCard[i] -= 1;
				vHuCard.push_back(i + 1);
				vHuCard.push_back(i + 1);
				vHuCard.push_back(i + 0x11);
			}
		}
	}
}


void CActionHuByArray::FirstSmallCardSplit(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//1. 顺子、二七十、碰、绞牌
	//删除顺子
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand, false);
	//删除二七十
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand, false);
	//删除碰牌
	//RemovePeng(vSmallCard, vHuCard, huxiByHand, false);
	//删除绞牌
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}


void CActionHuByArray::SecondSmallCardSplit(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//2. 二七十、顺子、碰、绞牌
	//删除二七十
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand, false);
	//删除顺子
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand, false);
	//删除碰牌
	//RemovePeng(vSmallCard, vHuCard, huxiByHand, false);
	//删除绞牌
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}

void CActionHuByArray::ThirdSmallCardSplit(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//3. 绞牌、二七十、顺子、
	//删除碰牌
	//RemovePeng(vSmallCard, vHuCard, huxiByHand, false);
	//删除绞牌
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);

	//删除二七十
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand, false);
	//删除顺子
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand, false);
}

//拆小牌
void CActionHuByArray::DoSplitSmallCard(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand, int splitType)
{
	//将小牌按照顺子、二七十、碰、绞牌拆分，看是否能拆分完,再去拆分大牌
	switch (splitType)
	{
	case 1:
		FirstSmallCardSplit(vSmallCard, vBigCard, vHuCard, huxiByHand);
		break;
	case 2:
		SecondSmallCardSplit(vSmallCard, vBigCard, vHuCard, huxiByHand);
		break;
	case 3:
		ThirdSmallCardSplit(vSmallCard, vBigCard, vHuCard, huxiByHand);
		break;
	default:
		break;
	}
}

void CActionHuByArray::FirstBigCardSplit(Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//1. 二七十、顺子、碰
	//删除二七十
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand, true);
	//删除顺子
	RemoveShunZi(vBigCard, vHuCard, huxiByHand, true);
	//删除碰牌
	//RemovePeng(vBigCard, vHuCard, huxiByHand, true);
}

void CActionHuByArray::SecondBigCardSplit(Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//2. 顺子、二七十、碰
	//删除顺子
	RemoveShunZi(vBigCard, vHuCard, huxiByHand, true);
	//删除二七十
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand, true);
	//删除碰牌
	//RemovePeng(vBigCard, vHuCard, huxiByHand, true);
}

void CActionHuByArray::ThirdBigCardSplit(Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//3. 碰、二七十、顺子、
	//删除碰牌
	//RemovePeng(vBigCard, vHuCard, huxiByHand, true);
	//删除二七十
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand, true);
	//删除顺子
	RemoveShunZi(vBigCard, vHuCard, huxiByHand, true);
}

bool CActionHuByArray::DoSplitBigCard(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHavePaoOrTi, bool isHaveJiang)
{
	//小牌先拆解完了，现在直接拆解大牌
	//若小牌剩余数量大于0，剩余的小牌只能为将牌（当有跑或提牌时，胡牌才需要将），否则不能胡

	//1. 查看小牌的数量
	//if (!ArrayValueIsZreo(vSmallCard) && !isHavePaoOrTi)
	//{
	//	return false;
	//}

	//拆分大牌： 顺子、二七十、碰
	int splitCount = 1;
	int tempHuxiByHand = 0;;
	CardVector vTempHuCard;
	Card vTempBigCard[MAX_ARRAY_LENTH] = { 0 };
	do
	{
		tempHuxiByHand = huxiByHand;
		vTempHuCard = vHuCard;
		InitArray(vTempBigCard, vBigCard);

		//将小牌按照顺子、二七十、碰、绞牌拆分，看是否能拆分完,再去拆分大牌
		switch (splitCount)
		{
		case 1:
			FirstBigCardSplit(vTempBigCard, vTempHuCard, tempHuxiByHand);
			break;
		case 2:
			SecondBigCardSplit(vTempBigCard, vTempHuCard, tempHuxiByHand);
			break;
		case 3:
			//ThirdBigCardSplit(vTempBigCard, vTempHuCard, tempHuxiByHand);
			break;
		default:
			break;
		}

		//不存在提牌或胡牌，则不需要将
		if (ArrayValueIsZreo(vTempBigCard))
		{
			int idx = IsOnlyJiang(vSmallCard);
			if (isHavePaoOrTi)
			{
				//有提牌或跑牌就必须要有将
				if (isHaveJiang && (idx != -1))
				{
					//将将牌加入到胡牌
					vTempHuCard.push_back(idx + 1);
					vTempHuCard.push_back(idx + 1);
					vHuCard = vTempHuCard;
					huxiByHand = tempHuxiByHand;
					vSmallCard[idx] = 0;
					return true;
				}
			}
			else
			{
				if (isHaveJiang)
				{
					vTempHuCard.push_back(idx + 1);
					vTempHuCard.push_back(idx + 1);
				}
				//没提牌或跑牌则不需要将
				vHuCard = vTempHuCard;
				huxiByHand = tempHuxiByHand;
				return true;
			}
		}
		else
		{
			//多余牌做将
			int idx = IsOnlyJiang(vTempBigCard);
			if (!isHaveJiang && (idx != -1))
			{
				//将将牌加入到胡牌
				vTempHuCard.push_back(idx + 0x11);
				vTempHuCard.push_back(idx + 0x11);
				vHuCard = vTempHuCard;
				huxiByHand = tempHuxiByHand;
				vBigCard[idx] = 0;
				return true;
			}
		}

		splitCount++;
	} while (splitCount <= 3);

	return false;
}

bool CActionHuByArray::CheckHu(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHaveTiOrPao)
{
	bool isCanHu = false;
	bool isCanSplitSmallCard = false;

	//拆分小牌按三种方式进行拆分：1. 顺子、二七十、碰、绞牌  2. 二七十、顺子、碰、绞牌  3. 碰、二七十、顺子、绞牌
	int splitType = 1;
	Card vTempSmallCard[MAX_ARRAY_LENTH] = { 0 };
	Card vTempBigCard[MAX_ARRAY_LENTH] = { 0 };
	CardVector vTempHuCard;
	int tempHuxiByHand = 0;
	do
	{
		vTempHuCard = vHuCard;
		tempHuxiByHand = huxiByHand;
		InitArray(vTempSmallCard, vSmallCard);
		InitArray(vTempBigCard, vBigCard);

		//拆小牌(小牌拆不完，则不能胡牌)
		if (!ArrayValueIsZreo(vTempSmallCard))
		{
			DoSplitSmallCard(vTempSmallCard, vTempBigCard, vTempHuCard, tempHuxiByHand, splitType);
		}

		if (ArrayValueIsZreo(vTempSmallCard))
		{
			//拆分大牌
			if (!ArrayValueIsZreo(vTempBigCard))
			{
				isCanHu = DoSplitBigCard(vTempSmallCard, vTempBigCard, vTempHuCard, tempHuxiByHand, isHaveTiOrPao, false);
			}
			else
			{
				if (!isHaveTiOrPao)
				{
					isCanHu = true;
				}
			}

			if (isCanHu)
			{
				vHuCard = vTempHuCard;
				huxiByHand = tempHuxiByHand;
				return isCanHu;
			}
		}
		else
		{
			//小牌的剩余牌做将(有提牌或跑牌时，必须将)
			int idx_small = IsOnlyJiang(vTempSmallCard);
			if (idx_small != -1)
			{
				//拆分大牌
				if (!ArrayValueIsZreo(vTempBigCard))
				{
					isCanHu = DoSplitBigCard(vTempSmallCard, vTempBigCard, vTempHuCard, tempHuxiByHand, isHaveTiOrPao, true);
				}
				else
				{
					isCanHu = true;
					vTempHuCard.push_back(idx_small + 1);
					vTempHuCard.push_back(idx_small + 1);
				}

				if (isCanHu)
				{
					vHuCard = vTempHuCard;
					huxiByHand = tempHuxiByHand;
					return isCanHu;
				}

			}
		}

		//小牌和大牌的牌值为空，则说明符合胡牌类型

		splitType++;
	} while (splitType <= 3);

	return isCanHu;
}

//获取是否可以胡牌
bool CActionHuByArray::GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo)
{
	if (vCard.empty())
	{
		return false;
	}

	bool bResult = false;
	bool isPeng = false;
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
	RemovePaoPai(vCardSmall, vCardBig, huSInfo.vCard, huSInfo.huXiByHand);

	//再删除手上的坎牌
	RemoveKanPai_New(vCardSmall, vCardBig, huSInfo.vCard, huSInfo.huXiByHand);

	//检测牌型是否可以胡牌
	bool bPaiXingIsOk = CheckHu(vCardSmall, vCardBig, huSInfo.vCard, huSInfo.huXiByHand, huSInfo.isHaveTiOrPao);
	if (bPaiXingIsOk)
	{
		//检测胡息是否有10胡息
		huSInfo.huXiCount += huSInfo.huXiByHand;
		if (cOperator != Card_INVALID)
		{
			//如果胡的那张牌此时在手中有3张牌，则要扣掉坎牌和碰牌的息差
			if (isPeng)
			{
				if (cOperator < Card_Tiao_1)
				{
					huSInfo.huXiCount -= XN_Kan_Small - XN_Peng_Small;
				}
				else
				{
					huSInfo.huXiCount -= XN_Kan_Big - XN_Peng_Big;
				}
			}
		}

		if (huSInfo.huXiCount >= m_huxi)
		{
			bResult = true;
		}
	}
	return bResult;
}

//检查数组值是否全为0
bool CActionHuByArray::ArrayValueIsZreo(Card* array)
{
	if (array)
	{
		for (int i = 0; i < MAX_ARRAY_LENTH; i++)
		{
			if (array[i] > 0)
			{
				return false;
			}
		}

		return true;
	}

	return true;
}

//检查数组是否只有一对将     如果是，则返回数组下标，否则返回-1
int CActionHuByArray::IsOnlyJiang(Card* array)
{
	if (array)
	{
		int count = 0;
		int idx = -1;
		for (int i = 0; i < MAX_ARRAY_LENTH; i++)
		{
			if (array[i] == 2)
			{
				count++;
				idx = i;
				if (count > 1)
				{
					return -1;
				}
			}
			else if ((array[i] != 2) && (array[i] != 0))
			{
				return -1;
			}
		}

		if (count == 1)
		{
			return idx;
		}

	}

	return -1;
}

//初始化数组
void CActionHuByArray::InitArray(Card* vdes, Card* vSource)
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

//设置多少胡息可胡
void CActionHuByArray::SetHowMuchHuXiForHu(int huxi)
{
	m_huxi = huxi;
}