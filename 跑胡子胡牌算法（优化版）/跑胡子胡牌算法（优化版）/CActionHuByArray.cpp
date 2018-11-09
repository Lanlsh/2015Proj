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
	//��ȡ��ҵĸ����ƺ�Ϣ
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

//�Ƚ������Ʋ����뵽�����У��������е�˳�ӺͿ��ֳ�ȥ��������ۺ�ʣ���ƣ��������Ƽ��뵽�����У����Ƿ������333��332��332ʱ��Ҫ�����ƻ����ƣ�
//�����㷨�������������

//ɾ������Ԫ��
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

//ɾ��˳��Ԫ��
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

//ɾ������
void CActionHuByArray::RemovePeng(Card* vCard, CardVector& vHuCard, int& huxiByHand, bool isBigCard)
{
	//��Ϊ���Ѿ�ɾ���ˣ���ʱɾ������Ϊ������������һ������a�����������򵽳�a����ʱ�ܺϳ�һ���������ơ���ʱ���ֻ��һ��3��һ������
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


//ɾ��˳��
void CActionHuByArray::RemoveShunZi(Card* vCard, CardVector& vHuCard, int& huxiByHand, bool isBigCard)
{
	//if (ArrayValueIsZreo(vCard))
	//{
	//	return;
	//}
	//��ͨ��˳��
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

//ɾ������ʮ
void CActionHuByArray::RemoveTwoSevenTen(Card* vCard, CardVector& vHuCard, int& huxiByHand, bool isBigCard)
{
	//����2-7-10��С���Ƶ�����
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


//ɾ������
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

//ɾ������
void CActionHuByArray::RemoveJiaoPai(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard)
{
	//���Ʋ���Ϣ����Ҳ��һ������
	//�Ȳ鿴С���Ƿ���һ�Ի�ֻ
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
	//1. ˳�ӡ�����ʮ����������
	//ɾ��˳��
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand, false);
	//ɾ������ʮ
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand, false);
	//ɾ������
	//RemovePeng(vSmallCard, vHuCard, huxiByHand, false);
	//ɾ������
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}


void CActionHuByArray::SecondSmallCardSplit(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//2. ����ʮ��˳�ӡ���������
	//ɾ������ʮ
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand, false);
	//ɾ��˳��
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand, false);
	//ɾ������
	//RemovePeng(vSmallCard, vHuCard, huxiByHand, false);
	//ɾ������
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);
}

void CActionHuByArray::ThirdSmallCardSplit(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//3. ���ơ�����ʮ��˳�ӡ�
	//ɾ������
	//RemovePeng(vSmallCard, vHuCard, huxiByHand, false);
	//ɾ������
	RemoveJiaoPai(vSmallCard, vBigCard, vHuCard);

	//ɾ������ʮ
	RemoveTwoSevenTen(vSmallCard, vHuCard, huxiByHand, false);
	//ɾ��˳��
	RemoveShunZi(vSmallCard, vHuCard, huxiByHand, false);
}

//��С��
void CActionHuByArray::DoSplitSmallCard(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand, int splitType)
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

void CActionHuByArray::FirstBigCardSplit(Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//1. ����ʮ��˳�ӡ���
	//ɾ������ʮ
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand, true);
	//ɾ��˳��
	RemoveShunZi(vBigCard, vHuCard, huxiByHand, true);
	//ɾ������
	//RemovePeng(vBigCard, vHuCard, huxiByHand, true);
}

void CActionHuByArray::SecondBigCardSplit(Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//2. ˳�ӡ�����ʮ����
	//ɾ��˳��
	RemoveShunZi(vBigCard, vHuCard, huxiByHand, true);
	//ɾ������ʮ
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand, true);
	//ɾ������
	//RemovePeng(vBigCard, vHuCard, huxiByHand, true);
}

void CActionHuByArray::ThirdBigCardSplit(Card* vBigCard, CardVector& vHuCard, int& huxiByHand)
{
	//3. ��������ʮ��˳�ӡ�
	//ɾ������
	//RemovePeng(vBigCard, vHuCard, huxiByHand, true);
	//ɾ������ʮ
	RemoveTwoSevenTen(vBigCard, vHuCard, huxiByHand, true);
	//ɾ��˳��
	RemoveShunZi(vBigCard, vHuCard, huxiByHand, true);
}

bool CActionHuByArray::DoSplitBigCard(Card* vSmallCard, Card* vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHavePaoOrTi, bool isHaveJiang)
{
	//С���Ȳ�����ˣ�����ֱ�Ӳ�����
	//��С��ʣ����������0��ʣ���С��ֻ��Ϊ���ƣ������ܻ�����ʱ�����Ʋ���Ҫ�����������ܺ�

	//1. �鿴С�Ƶ�����
	//if (!ArrayValueIsZreo(vSmallCard) && !isHavePaoOrTi)
	//{
	//	return false;
	//}

	//��ִ��ƣ� ˳�ӡ�����ʮ����
	int splitCount = 1;
	int tempHuxiByHand = 0;;
	CardVector vTempHuCard;
	Card vTempBigCard[MAX_ARRAY_LENTH] = { 0 };
	do
	{
		tempHuxiByHand = huxiByHand;
		vTempHuCard = vHuCard;
		InitArray(vTempBigCard, vBigCard);

		//��С�ư���˳�ӡ�����ʮ���������Ʋ�֣����Ƿ��ܲ����,��ȥ��ִ���
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

		//���������ƻ���ƣ�����Ҫ��
		if (ArrayValueIsZreo(vTempBigCard))
		{
			int idx = IsOnlyJiang(vSmallCard);
			if (isHavePaoOrTi)
			{
				//�����ƻ����ƾͱ���Ҫ�н�
				if (isHaveJiang && (idx != -1))
				{
					//�����Ƽ��뵽����
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
				//û���ƻ���������Ҫ��
				vHuCard = vTempHuCard;
				huxiByHand = tempHuxiByHand;
				return true;
			}
		}
		else
		{
			//����������
			int idx = IsOnlyJiang(vTempBigCard);
			if (!isHaveJiang && (idx != -1))
			{
				//�����Ƽ��뵽����
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

	//���С�ư����ַ�ʽ���в�֣�1. ˳�ӡ�����ʮ����������  2. ����ʮ��˳�ӡ���������  3. ��������ʮ��˳�ӡ�����
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

		//��С��(С�Ʋ��꣬���ܺ���)
		if (!ArrayValueIsZreo(vTempSmallCard))
		{
			DoSplitSmallCard(vTempSmallCard, vTempBigCard, vTempHuCard, tempHuxiByHand, splitType);
		}

		if (ArrayValueIsZreo(vTempSmallCard))
		{
			//��ִ���
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
			//С�Ƶ�ʣ��������(�����ƻ�����ʱ�����뽫)
			int idx_small = IsOnlyJiang(vTempSmallCard);
			if (idx_small != -1)
			{
				//��ִ���
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

		//С�ƺʹ��Ƶ���ֵΪ�գ���˵�����Ϻ�������

		splitType++;
	} while (splitType <= 3);

	return isCanHu;
}

//��ȡ�Ƿ���Ժ���
bool CActionHuByArray::GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo)
{
	if (vCard.empty())
	{
		return false;
	}

	bool bResult = false;
	bool isPeng = false;
	//�������Ƽ��뵽������
	if (cOperator != Card_INVALID)
	{
		vCard.push_back(cOperator);
		if (std::count(vCard.begin(), vCard.end(), cOperator) == 3)
		{
			isPeng = true;
		}
	}
	std::sort(vCard.begin(), vCard.end());

	//�ٽ����Ʒ�Ϊ�еĴ�С�Ʒ������
	Card vCardBig[10] = { 0 };
	Card vCardSmall[10] = { 0 };
	DivideToTwoArray(vCard, vCardBig, vCardSmall);

	//��ɾ�����ϵ�����
	RemovePaoPai(vCardSmall, vCardBig, huSInfo.vCard, huSInfo.huXiByHand);

	//��ɾ�����ϵĿ���
	RemoveKanPai_New(vCardSmall, vCardBig, huSInfo.vCard, huSInfo.huXiByHand);

	//��������Ƿ���Ժ���
	bool bPaiXingIsOk = CheckHu(vCardSmall, vCardBig, huSInfo.vCard, huSInfo.huXiByHand, huSInfo.isHaveTiOrPao);
	if (bPaiXingIsOk)
	{
		//����Ϣ�Ƿ���10��Ϣ
		huSInfo.huXiCount += huSInfo.huXiByHand;
		if (cOperator != Card_INVALID)
		{
			//������������ƴ�ʱ��������3���ƣ���Ҫ�۵����ƺ����Ƶ�Ϣ��
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

//�������ֵ�Ƿ�ȫΪ0
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

//��������Ƿ�ֻ��һ�Խ�     ����ǣ��򷵻������±꣬���򷵻�-1
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

//��ʼ������
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

//���ö��ٺ�Ϣ�ɺ�
void CActionHuByArray::SetHowMuchHuXiForHu(int huxi)
{
	m_huxi = huxi;
}