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
	//��ȡ��ҵĸ����ƺ�Ϣ
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

//�Ƚ������Ʋ����뵽�����У��������е�˳�ӺͿ��ֳ�ȥ��������ۺ�ʣ���ƣ��������Ƽ��뵽�����У����Ƿ������333��332��332ʱ��Ҫ�����ƻ����ƣ�
//�����㷨�������������

//ɾ������Ԫ��
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

//ɾ��˳��Ԫ��
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

//ɾ������
void CActionHu::RemovePeng(CardVector& vCard, CardVector& vHuCard, int& huxiByHand)
{
	//��Ϊ���Ѿ�ɾ���ˣ���ʱɾ������Ϊ������������һ������a�����������򵽳�a����ʱ�ܺϳ�һ���������ơ���ʱ���ֻ��һ��3��һ������
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


//ɾ��˳��
void CActionHu::RemoveShunZi(CardVector& vCard, CardVector& vHuCard, int& huxiByHand)
{
	if (vCard.size() <= 0)
	{
		return;
	}
	//��ͨ��˳��
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

//ɾ������ʮ
void CActionHu::RemoveTwoSevenTen(CardVector& vCard, CardVector& vHuCard, int& huxiByHand)
{
	//����2-7-10��С���Ƶ�����
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


//ɾ������
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

//ɾ������
void CActionHu::RemoveJiaoPai(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard)
{
	//���Ʋ���Ϣ����Ҳ��һ������
	//�Ȳ鿴С���Ƿ���һ�Ի�ֻ
	if ((vSmallCard.size() <= 0) || (vBigCard.size() <= 0))
	{
		return;
	}

	std::sort(vSmallCard.begin(), vSmallCard.end());
	std::sort(vBigCard.begin(), vBigCard.end());
	CardVector vDeleteElementSmall; //����С������Ҫɾ������ֵ
	CardVector vDeleteElementBig; //�����������Ҫɾ������ֵ

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
				//�ҵ����ƣ�����ɾ������
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
				//�ҵ����ƣ�����ɾ������
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

	//ɾ������Ԫ��
	if ((vDeleteElementBig.size() > 0) && (vDeleteElementSmall.size() > 0))
	{
		DeleteJiaoPaiElement(vSmallCard, vBigCard, vDeleteElementSmall, vDeleteElementBig);
	}

}


void CActionHu::FirstSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//1. ˳�ӡ�����ʮ����������
	//ɾ��˳��
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand);
	//ɾ������ʮ
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand);
	//ɾ������
	RemovePeng(vSmallCard, vHuCard, huxiByHand);
	//ɾ������
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}


void CActionHu::SecondSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//2. ����ʮ��˳�ӡ���������
	//ɾ������ʮ
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand);
	//ɾ��˳��
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand);
	//ɾ������
	RemovePeng(vSmallCard, vHuCard, huxiByHand);
	//ɾ������
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}

void CActionHu::ThirdSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//3. ��������ʮ��˳�ӡ�����
	//ɾ������
	RemovePeng(vSmallCard, vHuCard, huxiByHand);
	//ɾ������ʮ
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand);
	//ɾ��˳��
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand);
	//ɾ������
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}

//��С��
void CActionHu::DoSplitSmallCard(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, int splitType)
{
	//��С�ư���˳�ӡ�����ʮ���������Ʋ�֣����Ƿ��ܲ����,��ȥ��ִ���
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
	//1. ����ʮ��˳�ӡ���
	//ɾ������ʮ
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand);
	//ɾ��˳��
	RemoveShunZi(vBigCard, vHuCard, huxiByHand);
	//ɾ������
	RemovePeng(vBigCard, vHuCard, huxiByHand);
}

void CActionHu::SecondBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//2. ˳�ӡ�����ʮ����
	//ɾ��˳��
	RemoveShunZi(vBigCard, vHuCard, huxiByHand);
	//ɾ������ʮ
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand);
	//ɾ������
	RemovePeng(vBigCard, vHuCard, huxiByHand);
}

void CActionHu::ThirdBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//3. ��������ʮ��˳�ӡ�
	//ɾ������
	RemovePeng(vBigCard, vHuCard, huxiByHand);
	//ɾ������ʮ
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand);
	//ɾ��˳��
	RemoveShunZi(vBigCard, vHuCard, huxiByHand);
}

bool CActionHu::DoSplitBigCard(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHavePaoOrTi)
{
	//С���Ȳ�����ˣ�����ֱ�Ӳ�����
	//��С��ʣ����������0��ʣ���С��ֻ��Ϊ���ƣ������ܻ�����ʱ�����Ʋ���Ҫ�����������ܺ�
	bool isHaveJiang = false;
	//1. �鿴С�Ƶ�����
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

	//��ִ��ƣ� ˳�ӡ�����ʮ����
	int splitCount = 1;
	do
	{
		CardVector vTempBigCard = vBigCard;
		CardVector vTempHuCard = vHuCard;
		int tempHuxiByHand = huxiByHand;

		//��С�ư���˳�ӡ�����ʮ���������Ʋ�֣����Ƿ��ܲ����,��ȥ��ִ���
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

		//���������ƻ���ƣ�����Ҫ��
		if (vBigCard.size() == 0)
		{
			if (isHavePaoOrTi)
			{
				//�����ƻ����ƾͱ���Ҫ�н�
				if (isHaveJiang)
				{
					//�����Ƽ��뵽����
					vTempHuCard.push_back(vSmallCard[0]);
					vTempHuCard.push_back(vSmallCard[0]);
					vHuCard = vTempHuCard;
					vSmallCard.clear();
					return true;
				}
			}
			else
			{
				//û���ƻ���������Ҫ��
				if (!isHaveJiang)
				{
					return true;
				}
			}
		}
		else
		{
			//����������
			if (!isHaveJiang && (vBigCard.size() == 2) && isHavePaoOrTi)
			{
				if (vBigCard[0] == vBigCard[1])
				{
					//�����Ƽ��뵽����
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

	//���С�ư����ַ�ʽ���в�֣�1. ˳�ӡ�����ʮ����������  2. ����ʮ��˳�ӡ���������  3. ��������ʮ��˳�ӡ�����
	int splitType = 1;
	do
	{
		CardVector vTempSmallCard = vSmallCard;
		CardVector vTempBigCard = vBigCard;
		CardVector vTempHuCard = vHuCard;
		int tempHuxiByHand = huxiByHand;

		//��С��(С�Ʋ��꣬���ܺ���)
		if (vTempSmallCard.size() > 0)
		{
			DoSplitSmallCard(vTempSmallCard, vTempBigCard, vTempHuCard, tempHuxiByHand, splitType);
		}

		if (vTempSmallCard.size() == 0)
		{
			//��ִ���
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
			//С�Ƶ�ʣ��������(�����ƻ�����ʱ������Ҫ�����������333��������)
			if (isHaveTiOrPao && (vTempSmallCard.size() == 2))
			{
				if (vTempSmallCard[0] == vTempSmallCard[1])
				{
					//��ִ���
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

		//С�ƺʹ��Ƶ���ֵΪ�գ���˵�����Ϻ�������

		splitType++;
	} while (splitType <= 3);

	return isCanHu;
}

//��ȡ�Ƿ���Ժ���
bool CActionHu::GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo)
{
	bool bResult = false;

	//��ɾ�����������еĿ���
	RemoveKanPai(vCard, huSInfo.vCard, huSInfo.huXiByHand);

	//�������Ƽ��뵽������
	if (cOperator != Card_INVALID)
	{
		vCard.push_back(cOperator);
		std::sort(vCard.begin(), vCard.end());
	}

	//�ٽ����Ʒ�Ϊ�еĴ�С�Ʒ������
	CardVector vCardBig;
	CardVector vCardSmall;
	DivideToTwoArray(vCard, vCardBig, vCardSmall);

	//��������Ƿ���Ժ���
	bool bPaiXingIsOk = CheckHu(vCardSmall, vCardBig, huSInfo.vCard, huSInfo.huXiByHand, huSInfo.isHaveTiOrPao);
	if (bPaiXingIsOk)
	{
		//����Ϣ�Ƿ���10��Ϣ
		huSInfo.huXiCount += huSInfo.huXiByHand;
		if (huSInfo.huXiCount >= 10)
		{
			bResult = true;
		}
	}
	return bResult;
}
