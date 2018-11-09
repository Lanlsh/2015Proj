#pragma once

typedef int Card;
typedef vector<Card> CardVector;

/*********************************
(1)分两个数组： 大字数组a[20]   小字数组b[20]
(2)大字：坎、吃、碰、顺子
(3)小字：坎、吃、碰、顺子
(4)剩余牌是否能成为对、绞牌、顺子
(5)再加上附子牌的胡息，判断胡息是否足够
**********************************/
enum Card_Number
{
	Card_INVALID = 0,

	//万
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

	//条
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
	//手里的与桌面上的牌组合有7（4人玩时，门子数要调整）方门子（特殊3提5坎例外）
	CardVector vCard; //胡牌时手牌组成的牌型
	int huXiCount; //胡息
	int haveHowMuchMenZi; //除去附子牌后还需要几方门子
	bool isHaveTiOrPao; //是否有提牌或跑牌
}HuStructInfo;

typedef struct MenZiItem
{
	Card cOpraterCard; //操作牌
	int i;	//使用第几步分解
	Card menzi[4];	//分解的牌值

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

	//获取是否可以胡牌
	bool GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo);

	//将牌分为大牌和小牌
	void DivideToTwoArray(CardVector vHandCard, Card vCardBig[], Card vCardSmall[]);

	//删除手牌上的跑牌
	bool RemovePaoPai(Card* vSmallCard, Card* vBigCard, MZItem item[], int& haveMuckMenZi);

	//删除手牌上的坎牌
	void RemoveKanPai_New(Card* vSmallCard, Card* vBigCard,  MZItem item[], int& haveMuckMenZi, Card cPengCard);

	//获取附子胡息
	int GethaveHowMuchMenZi(WORD wChairId, bool& isHaveTiOrPao, int& huxi, MZItem item[]);

	//删除顺子元素
	void DeleteShunZiElement(Card* vCard, CardVector vDeleteElement, MZItem item[], int idx, bool isBigCard, int count);

	//删除顺子
	bool RemoveShunZi(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx);

	//删除二七十
	bool RemoveTwoSevenTen(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx);

	//删除绞牌
	bool RemoveJiaoPai(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx, bool isBigCard, int firstCardIdx);

	//获取胡牌的牌型
	bool CheckHu(Card* vSmallCard, Card* vBigCard, MZItem item[], int& needMeziCount, bool isHaveTiOrPao, int handCardCount, int& huxiCount, CardVector& vCard);

	//组建门子
	bool ComBineMenZi(Card* vSmallCard, Card* vBigCard, MZItem item[], int idx);

	//获取门子胡息
	int GetHuxiByMenZi(MZItem item[], CardVector& vCard);

	//搜索第一次出现牌值的下标
	int SearchFirstValue(Card* card);

	//初始化数组
	void InitArray(Card* vdes, Card* vSource);
};

