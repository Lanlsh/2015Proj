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
	Card_Wan_10 = 0x0a,

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
	Card_Tiao_10 = 0x1a
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

#define MAX_ARRAY_LENTH 10
#define MAX_SEARCH_LENGTH 20

typedef struct ST_HuStructInfo
{
	//������������ϵ��������7��4����ʱ��������Ҫ�����������ӣ�����3��5�����⣩
	CardVector vCard; //����ʱ������ɵ�����
	int huXiCount; //��Ϣ
	int haveHowMuchMenZi; //��ȥ�����ƺ���Ҫ��������
	bool isHaveTiOrPao; //�Ƿ������ƻ�����
}HuStructInfo;

typedef struct MenZiItem
{
	Card cOpraterCard; //������
	int i;	//ʹ�õڼ����ֽ�
	Card menzi[4];	//�ֽ����ֵ

	MenZiItem()
	{
		cOpraterCard = Card_INVALID;
		i = 0;
		memset(menzi, 0, sizeof(menzi));
	}
}MZItem;

class CAtionHuByLaiZi
{
public:
	CAtionHuByLaiZi();
	~CAtionHuByLaiZi();

	//��ȡ�Ƿ���Ժ���
	bool GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo);

	//���Ʒ�Ϊ���ƺ�С��
	void DivideToTwoArray(CardVector vHandCard, Card vCardBig[], Card vCardSmall[]);

	//ɾ�������ϵ�����
	bool RemovePaoPai(Card* vSmallCard, Card* vBigCard, MZItem item[], int& haveMuckMenZi);

	//ɾ�������ϵĿ���
	void RemoveKanPai_New(Card* vSmallCard, Card* vBigCard,  MZItem item[], int& haveMuckMenZi, Card cPengCard);

	//��ȡ���Ӻ�Ϣ
	int GethaveHowMuchMenZi(WORD wChairId, bool& isHaveTiOrPao, int& huxi, MZItem item[]);

	//ɾ��˳��Ԫ��
	void DeleteShunZiElement(Card* vCard, CardVector vDeleteElement, MZItem item[], int idx, bool isBigCard, int count);

	//ɾ��˳��
	bool RemoveShunZi(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx);

	//ɾ������ʮ
	bool RemoveTwoSevenTen(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx);

	//ɾ������
	bool RemoveJiaoPai(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx);

	//��ȡ���Ƶ�����
	bool CheckHu(Card* vSmallCard, Card* vBigCard, MZItem item[], int& needMeziCount, bool isHaveTiOrPao, int handCardCount, int& huxiCount, CardVector& vCard);

	//�齨����
	bool ComBineMenZi(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx);

	//��ȡ���Ӻ�Ϣ
	int GetHuxiByMenZi(MZItem item[], CardVector& vCard);

	//������һ�γ�����ֵ���±�
	int SearchFirstValue(Card* card);

	//��ʼ������
	void InitArray(Card* vdes, Card* vSource);
};

