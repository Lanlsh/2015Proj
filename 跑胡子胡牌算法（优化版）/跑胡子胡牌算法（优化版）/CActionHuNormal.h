#pragma once

typedef int Card;
typedef vector<Card> CardVector;

/*********************************
(1)���������飺 ��������a[20]   С������b[20]
(2)���֣������ԡ�����˳��
(3)С�֣������ԡ�����˳��
(4)ʣ�����Ƿ��ܳ�Ϊ�ԡ����ơ�˳��
(5)�ټ��ϸ����Ƶĺ�Ϣ���жϺ�Ϣ�Ƿ��㹻
**********************************/
enum Card_Number
{
	Card_INVALID = 0,

	//��
	Card_Wan_1 = 0x01,
	Card_Wan_2 = 0x02,
	Card_Wan_3 = 0x03,
	Card_Wan_4 = 0x04,
	Card_Wan_5 = 0x05,
	Card_Wan_6 = 0x06,
	Card_Wan_7 = 0x07,
	Card_Wan_8 = 0x08,
	Card_Wan_9 = 0x09,
	Card_Wan_10 = 0x10,

	//��
	Card_Tiao_1 = 0x11,
	Card_Tiao_2 = 0x12,
	Card_Tiao_3 = 0x13,
	Card_Tiao_4 = 0x14,
	Card_Tiao_5 = 0x15,
	Card_Tiao_6 = 0x16,
	Card_Tiao_7 = 0x17,
	Card_Tiao_8 = 0x18,
	Card_Tiao_9 = 0x19,
	Card_Tiao_10 = 0x20
};

enum XiNum
{
	XN_Peng_Small = 1,
	XN_Peng_Big = 3,
	XN_Wei_Small = 3,
	XN_Wei_Big = 6,
	XN_Pao_Small = 6,
	XN_Pao_Big = 9,
	XN_Ti_Small = 9,
	XN_Ti_Big = 12,
	XN_OneTwoThre_Small = 3,
	XN_OneTwoThre_Big = 6,
	XN_TwoSevenTen_Small = 3,
	XN_TwoSevenTen_Big = 6,
	XN_Kan_Small = 3,
	XN_Kan_Big = 6
};

typedef struct ST_HuStructInfo
{
	//������������ϵ��������7��4����ʱ��������Ҫ�����������ӣ�����3��5�����⣩
	CardVector vCard; //����ʱ������ɵ�����
	int huXiCount; //��Ϣ
	int huXiByHand; //�����еĺ�Ϣ
	int huXiByFZ; //�������еĺ�Ϣ
	bool isHaveTiOrPao; //�Ƿ������ƻ�����
}HuStructInfo;


class CActionHu
{
public:
	CActionHu();
	~CActionHu();

	//��ȡ�Ƿ���Ժ���
	bool GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo);

	//��ȡ���Ƶ�����
	bool CheckHu(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHaveTiOrPao);

	//��ȡ���Ӻ�Ϣ
	int GetFuZiHuXi(WORD wChairId, bool& isHaveTiOrPao);

	//���Ʒ�Ϊ���ƺ�С��
	void DivideToTwoArray(CardVector vHandCard, CardVector& vCardBig, CardVector& vCardSmall);
	
	//ɾ������Ԫ��
	void DeleteKanPaiElement(CardVector& ShouPai, Card cardData);

	//ɾ��˳��Ԫ��
	void DeleteShunZiElement(CardVector& vCard, CardVector vDeleteElement, CardVector& vHuCard, int& huxiByHand);

	//ɾ������
	void RemovePeng(CardVector& vCard, CardVector& vHuCard, int& huxiByHand);

	//ɾ��˳��
	void RemoveShunZi(CardVector& vCard, CardVector& vHuCard, int& huxiByHand);

	//ɾ������ʮ
	void RemoveTwoSevenTen(CardVector& vCard, CardVector& vHuCard, int& huxiByHand);

	//ɾ������
	void RemoveKanPai(CardVector& vCard, CardVector& vHuCard, int& huxiByHand);

	//ɾ��Ԫ��
	void DeleteJiaoPaiElement(CardVector& vSmallCard, CardVector& vBigCard, CardVector vDeleteElementSmall, CardVector vDeleteElementBig);

	//ɾ������
	void RemoveJiaoPai(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard);

	//��һ��˳����С��  1. ˳�ӡ�����ʮ����������
	void FirstSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//�ڶ���˳����С��  2. ����ʮ��˳�ӡ���������
	void SecondSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//������˳����С��  3. ��������ʮ��˳�ӡ�����
	void ThirdSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//��С��
	void DoSplitSmallCard(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, int splitType);

	//��һ��˳���ִ���  1. ����ʮ��˳�ӡ���
	void FirstBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//�ڶ���˳���ִ���  2. ˳�ӡ�����ʮ����
	void SecondBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//������˳���ִ���  3. ��������ʮ��˳�ӡ�
	void ThirdBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//�����
	bool DoSplitBigCard(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHavePaoOrTi);
};

