#include "stdafx.h"
#include "CActionHuNormal.h"

CActionHu::CActionHu()
{
}


CActionHu::~CActionHu()
{
}

int CActionHu::GetFuZiHuXi(WORD wChairId, bool& isHaveTiOrPao)
{
	//获取玩家的附子牌胡息
	int huxi = 10;
	isHaveTiOrPao = true;
	return huxi;
}

void CActionHu::DivideToTwoArray(CardVector vHandCard, CardVector& vCardBig, CardVector& vCardSmall)
{
	if (vHandCard.empty())
	{
		return;
	}

	int arrayLenth = vHandCard.size();

	vCardBig.clear();
	vCardSmall.clear();

	for (int i = 0; i < arrayLenth; i++)
	{
		if ((vHandCard[i] < Card_Tiao_1) && (vHandCard[i] > Card_INVALID))
		{
			vCardSmall.push_back(vHandCard[i]);
		}
		else if ((vHandCard[i] >= Card_Tiao_1) && (vHandCard[i] <= Card_Tiao_10))
		{
			vCardBig.push_back(vHandCard[i]);
		}
	}
}

//先将操作牌不放入到手牌中，将手牌中的顺子和刻字出去，最后再综合剩余牌，将操作牌加入到手牌中，看是否能组成333或332（332时需要有提牌或跑牌）
//回溯算法：深度优先搜索

//删除坎牌元素
void CActionHu::DeleteKanPaiElement(CardVector& ShouPai, Card cardData)
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
void CActionHu::DeleteShunZiElement(CardVector& vCard, CardVector vDeleteElement, CardVector& vHuCard, int& huxiByHand)
{
	if (vCard.empty() || (vDeleteElement.size() != 3))
	{
		return;
	}

	bool isDeleteOne = false;
	bool isDeleteTwo = false;
	bool isDeleteThree = false;

	for (CardVector::iterator iter = vCard.begin(); iter != vCard.end();)
	{
		if ((vDeleteElement[0] == *iter) && (!isDeleteOne))
		{
			iter = vCard.erase(iter);
			isDeleteOne = true;
		}
		else if ((vDeleteElement[1] == *iter) && (!isDeleteTwo))
		{
			iter = vCard.erase(iter);
			isDeleteTwo = true;
		}
		else if ((vDeleteElement[2] == *iter) && (!isDeleteThree))
		{
			iter = vCard.erase(iter);
			isDeleteThree = true;
		}
		else
			iter++;

		if (isDeleteOne && isDeleteTwo && isDeleteThree)
		{
			vHuCard.push_back(vDeleteElement[0]);
			vHuCard.push_back(vDeleteElement[1]);
			vHuCard.push_back(vDeleteElement[2]);

			if ((vDeleteElement[0] == Card_Tiao_1) || (vDeleteElement[0] == Card_Wan_1))
			{
				if (vDeleteElement[0] >= Card_Tiao_1)
				{
					huxiByHand += XN_OneTwoThre_Big;
				}
				else
				{
					huxiByHand += XN_OneTwoThre_Small;
				}
			}
			else if (((vDeleteElement[0] == Card_Tiao_2) || (vDeleteElement[0] == Card_Wan_2)) && ((vDeleteElement[1] == Card_Tiao_7) || (vDeleteElement[1] == Card_Wan_7)))
			{
				if (vDeleteElement[0] >= Card_Tiao_1)
				{
					huxiByHand += XN_TwoSevenTen_Big;
				}
				else
				{
					huxiByHand += XN_TwoSevenTen_Small;
				}
			}
			break;
		}
	}
}

//删除碰牌
void CActionHu::RemovePeng(CardVector& vCard, CardVector& vHuCard, int& huxiByHand)
{
	//因为坎已经删除了，此时删除的碰为手中已有两张一样的牌a，别人摸到或到出a，此时能合成一方门子碰牌。此时最多只有一个3张一样的牌
	if (vCard.size() <= 0)
	{
		return;
	}

	for (CardVector::iterator iter = vCard.begin(); iter != vCard.end(); iter++)
	{
		if (std::count(vCard.begin(), vCard.end(), *iter) == 3)
		{
			if (*iter <= Card_Wan_10)
			{
				huxiByHand += XN_Peng_Small;
			}
			else
			{
				huxiByHand += XN_Peng_Big;
			}

			vHuCard.push_back(*iter);
			vHuCard.push_back(*iter);
			vHuCard.push_back(*iter);
			DeleteKanPaiElement(vCard, (Card)(*iter));
			break;
		}
	}
}


//删除顺子
void CActionHu::RemoveShunZi(CardVector& vCard, CardVector& vHuCard, int& huxiByHand)
{
	if (vCard.size() <= 0)
	{
		return;
	}
	//普通的顺子
	bool isFindHaveShunZi = false;
	CardVector vTemp;
	int findIndex = 0;
	std::sort(vCard.begin(), vCard.end());

	do
	{
		isFindHaveShunZi = false;
		if (findIndex >= vCard.size())
		{
			break;
		}

		for (int i = findIndex; i < vCard.size(); i++)
		{
			if ((i + 2) < vCard.size())
			{
				CardVector vTemp;
				vTemp.push_back(vCard[i]);
				vTemp.push_back(vCard[i] + 1);
				vTemp.push_back(vCard[i] + 2);
				int cardCount[3] = { 0 };
				for (int i = 0; i < 3; i++)
				{
					cardCount[i] = std::count(vCard.begin(), vCard.end(), vTemp[i]);
				}

				int loopCount = cardCount[0]>cardCount[1] ? cardCount[1] : cardCount[0];
				loopCount = loopCount > cardCount[2] ? cardCount[2] : loopCount;

				if (loopCount > 0)
				{
					isFindHaveShunZi = true;
					while (loopCount)
					{
						DeleteShunZiElement(vCard, vTemp, vHuCard, huxiByHand);
						loopCount--;
					}
					break;
				}
			}
			else
			{
				break;
			}

			findIndex++;
		}

	} while (isFindHaveShunZi);
}

//删除二七十
void CActionHu::RemoveTwoSevenTen(CardVector& vCard, CardVector& vHuCard, int& huxiByHand)
{
	//计算2-7-10大小字牌的数量
	if (vCard.size() <= 0)
	{
		return;
	}
	Card tempCard[] = { Card_Wan_2, Card_Wan_7, Card_Wan_10, Card_Tiao_2, Card_Tiao_7, Card_Tiao_10 };
	int cardCount[6] = { 0 };

	for (int i = 0; i<sizeof(tempCard) / sizeof(tempCard[0]); i++)
	{
		cardCount[i] = (int)std::count(vCard.begin(), vCard.end(), tempCard[i]);
	}

	int smallPai = (cardCount[0]>cardCount[1]) ? cardCount[1] : cardCount[0];
	smallPai = (smallPai > cardCount[2]) ? cardCount[2] : smallPai;

	int bigPai = (cardCount[3] > cardCount[4]) ? cardCount[4] : cardCount[3];
	bigPai = (bigPai > cardCount[5]) ? cardCount[5] : bigPai;

	if (smallPai > 0)
	{
		while (smallPai)
		{
			CardVector vTemp;
			vTemp.push_back(Card_Wan_2);
			vTemp.push_back(Card_Wan_7);
			vTemp.push_back(Card_Wan_10);
			DeleteShunZiElement(vCard, vTemp, vHuCard, huxiByHand);
			smallPai--;
		}
	}

	if (bigPai > 0)
	{
		while (bigPai)
		{
			CardVector vTemp;
			vTemp.push_back(Card_Tiao_2);
			vTemp.push_back(Card_Tiao_7);
			vTemp.push_back(Card_Tiao_10);
			DeleteShunZiElement(vCard, vTemp, vHuCard, huxiByHand);
			bigPai--;
		}
	}
}


//删除坎牌
void CActionHu::RemoveKanPai(CardVector& vCard, CardVector& vHuCard, int& huxiByHand)
{
	if (vCard.size() <= 0)
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

		std::sort(vCard.begin(), vCard.end());
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

void CActionHu::DeleteJiaoPaiElement(CardVector& vSmallCard, CardVector& vBigCard, CardVector vDeleteElementSmall, CardVector vDeleteElementBig)
{
	for (int i = 0; i < vDeleteElementSmall.size(); i++)
	{
		for (CardVector::iterator iter = vSmallCard.begin(); iter != vSmallCard.end(); iter++)
		{
			if (vDeleteElementSmall[i] == *iter)
			{
				vSmallCard.erase(iter);
				break;
			}
		}
	}

	for (int i = 0; i < vDeleteElementBig.size(); i++)
	{
		for (CardVector::iterator iter = vBigCard.begin(); iter != vBigCard.end(); iter++)
		{
			if (vDeleteElementBig[i] == *iter)
			{
				vBigCard.erase(iter);
				break;
			}
		}
	}
}

//删除绞牌
void CActionHu::RemoveJiaoPai(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard)
{
	//绞牌不算息，但也算一方门子
	//先查看小牌是否有一对或单只
	if ((vSmallCard.size() <= 0) || (vBigCard.size() <= 0))
	{
		return;
	}

	std::sort(vSmallCard.begin(), vSmallCard.end());
	std::sort(vBigCard.begin(), vBigCard.end());
	CardVector vDeleteElementSmall; //保存小字牌需要删除的牌值
	CardVector vDeleteElementBig; //保存大字牌需要删除的牌值

	for (int i = 0; i < vSmallCard.size();)
	{
		if (i >= vSmallCard.size())
		{
			break;
		}

		int count = std::count(vSmallCard.begin(), vSmallCard.end(), vSmallCard[i]);
		if (count == 1)
		{
			if (std::count(vBigCard.begin(), vBigCard.end(), vSmallCard[i] + 0x10) == 2)
			{
				//找到绞牌，保存删除绞牌
				vDeleteElementSmall.push_back(vSmallCard[i]);
				vDeleteElementBig.push_back(vSmallCard[i] + 0x10);
				vDeleteElementBig.push_back(vSmallCard[i] + 0x10);
				vHuCard.push_back(vSmallCard[i]);
				vHuCard.push_back(vSmallCard[i] + 0x10);
				vHuCard.push_back(vSmallCard[i] + 0x10);
			}

			i++;
		}
		else if (count == 2)
		{
			int searchSum = std::count(vBigCard.begin(), vBigCard.end(), vSmallCard[i] + 0x10);
			if ((searchSum > 0) && (searchSum < 3))
			{
				//找到绞牌，保存删除绞牌
				vDeleteElementBig.push_back(vSmallCard[i] + 0x10);
				vDeleteElementSmall.push_back(vSmallCard[i]);
				vDeleteElementSmall.push_back(vSmallCard[i]);
				vHuCard.push_back(vSmallCard[i] + 0x10);
				vHuCard.push_back(vSmallCard[i]);
				vHuCard.push_back(vSmallCard[i]);
			}

			i += 2;
		}
		else
			i++;
	}

	//删除绞牌元素
	if ((vDeleteElementBig.size() > 0) && (vDeleteElementSmall.size() > 0))
	{
		DeleteJiaoPaiElement(vSmallCard, vBigCard, vDeleteElementSmall, vDeleteElementBig);
	}

}


void CActionHu::FirstSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//1. 顺子、二七十、碰、绞牌
	//删除顺子
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand);
	//删除二七十
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand);
	//删除碰牌
	RemovePeng(vSmallCard, vHuCard, huxiByHand);
	//删除绞牌
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}


void CActionHu::SecondSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//2. 二七十、顺子、碰、绞牌
	//删除二七十
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand);
	//删除顺子
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand);
	//删除碰牌
	RemovePeng(vSmallCard, vHuCard, huxiByHand);
	//删除绞牌
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}

void CActionHu::ThirdSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//3. 碰、二七十、顺子、绞牌
	//删除碰牌
	RemovePeng(vSmallCard, vHuCard, huxiByHand);
	//删除二七十
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand);
	//删除顺子
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand);
	//删除绞牌
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}

//拆小牌
void CActionHu::DoSplitSmallCard(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, int splitType)
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

void CActionHu::FirstBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//1. 二七十、顺子、碰
	//删除二七十
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand);
	//删除顺子
	RemoveShunZi(vBigCard, vHuCard, huxiByHand);
	//删除碰牌
	RemovePeng(vBigCard, vHuCard, huxiByHand);
}

void CActionHu::SecondBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//2. 顺子、二七十、碰
	//删除顺子
	RemoveShunZi(vBigCard, vHuCard, huxiByHand);
	//删除二七十
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand);
	//删除碰牌
	RemovePeng(vBigCard, vHuCard, huxiByHand);
}

void CActionHu::ThirdBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//3. 碰、二七十、顺子、
	//删除碰牌
	RemovePeng(vBigCard, vHuCard, huxiByHand);
	//删除二七十
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand);
	//删除顺子
	RemoveShunZi(vBigCard, vHuCard, huxiByHand);
}

bool CActionHu::DoSplitBigCard(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHavePaoOrTi)
{
	//小牌先拆解完了，现在直接拆解大牌
	//若小牌剩余数量大于0，剩余的小牌只能为将牌（当有跑或提牌时，胡牌才需要将），否则不能胡
	bool isHaveJiang = false;
	//1. 查看小牌的数量
	if (vSmallCard.size() > 0 && !isHavePaoOrTi)
	{
		return false;
	}

	if (vSmallCard.size() > 0)
	{
		if (vSmallCard.size() != 2)
		{
			return false;
		}

		if (vSmallCard[0] != vSmallCard[1])
		{
			return false;
		}
		else
		{
			isHaveJiang = true;
		}
	}

	//拆分大牌： 顺子、二七十、碰
	int splitCount = 1;
	do
	{
		CardVector vTempBigCard = vBigCard;
		CardVector vTempHuCard = vHuCard;
		int tempHuxiByHand = huxiByHand;

		//将小牌按照顺子、二七十、碰、绞牌拆分，看是否能拆分完,再去拆分大牌
		switch (splitCount)
		{
		case 1:
			FirstBigCardSplit(vBigCard, vTempHuCard, huxiByHand);
			break;
		case 2:
			SecondBigCardSplit(vBigCard, vTempHuCard, huxiByHand);
			break;
		case 3:
			ThirdBigCardSplit(vBigCard, vTempHuCard, huxiByHand);
			break;
		default:
			break;
		}

		//不存在提牌或胡牌，则不需要将
		if (vBigCard.size() == 0)
		{
			if (isHavePaoOrTi)
			{
				//有提牌或跑牌就必须要有将
				if (isHaveJiang)
				{
					//将将牌加入到胡牌
					vTempHuCard.push_back(vSmallCard[0]);
					vTempHuCard.push_back(vSmallCard[0]);
					vHuCard = vTempHuCard;
					vSmallCard.clear();
					return true;
				}
			}
			else
			{
				//没提牌或跑牌则不需要将
				if (!isHaveJiang)
				{
					return true;
				}
			}
		}
		else
		{
			//多余牌做将
			if (!isHaveJiang && (vBigCard.size() == 2) && isHavePaoOrTi)
			{
				if (vBigCard[0] == vBigCard[1])
				{
					//将将牌加入到胡牌
					vTempHuCard.push_back(vBigCard[0]);
					vTempHuCard.push_back(vBigCard[0]);
					vHuCard = vTempHuCard;
					vBigCard.clear();
					return true;
				}
			}
		}

		splitCount++;
	} while (splitCount <= 3);

	return false;
}

bool CActionHu::CheckHu(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHaveTiOrPao)
{
	bool isCanHu = false;
	bool isCanSplitSmallCard = false;

	//拆分小牌按三种方式进行拆分：1. 顺子、二七十、碰、绞牌  2. 二七十、顺子、碰、绞牌  3. 碰、二七十、顺子、绞牌
	int splitType = 1;
	do
	{
		CardVector vTempSmallCard = vSmallCard;
		CardVector vTempBigCard = vBigCard;
		CardVector vTempHuCard = vHuCard;
		int tempHuxiByHand = huxiByHand;

		//拆小牌(小牌拆不完，则不能胡牌)
		if (vTempSmallCard.size() > 0)
		{
			DoSplitSmallCard(vTempSmallCard, vTempBigCard, vTempHuCard, tempHuxiByHand, splitType);
		}

		if (vTempSmallCard.size() == 0)
		{
			//拆分大牌
			if (vTempBigCard.size() > 0)
			{
				isCanHu = DoSplitBigCard(vTempSmallCard, vTempBigCard, vTempHuCard, tempHuxiByHand, isHaveTiOrPao);
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
			//小牌的剩余牌做将(有提牌或跑牌时，才需要将，否则就是333胡牌类型)
			if (isHaveTiOrPao && (vTempSmallCard.size() == 2))
			{
				if (vTempSmallCard[0] == vTempSmallCard[1])
				{
					//拆分大牌
					if (vTempBigCard.size() > 0)
					{
						isCanHu = DoSplitBigCard(vTempSmallCard, vTempBigCard, vTempHuCard, tempHuxiByHand, isHaveTiOrPao);
					}

					if (isCanHu)
					{
						vHuCard = vTempHuCard;
						huxiByHand = tempHuxiByHand;
						return isCanHu;
					}
				}
			}
		}

		//小牌和大牌的牌值为空，则说明符合胡牌类型

		splitType++;
	} while (splitType <= 3);

	return isCanHu;
}

//获取是否可以胡牌
bool CActionHu::GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo)
{
	bool bResult = false;

	//先删除手牌中所有的坎牌
	RemoveKanPai(vCard, huSInfo.vCard, huSInfo.huXiByHand);

	//将操作牌加入到手牌中
	if (cOperator != Card_INVALID)
	{
		vCard.push_back(cOperator);
		std::sort(vCard.begin(), vCard.end());
	}

	//再将手牌分为中的大小牌分离出来
	CardVector vCardBig;
	CardVector vCardSmall;
	DivideToTwoArray(vCard, vCardBig, vCardSmall);

	//检测牌型是否可以胡牌
	bool bPaiXingIsOk = CheckHu(vCardSmall, vCardBig, huSInfo.vCard, huSInfo.huXiByHand, huSInfo.isHaveTiOrPao);
	if (bPaiXingIsOk)
	{
		//检测胡息是否有10胡息
		huSInfo.huXiCount += huSInfo.huXiByHand;
		if (huSInfo.huXiCount >= 10)
		{
			bResult = true;
		}
	}
	return bResult;
}
