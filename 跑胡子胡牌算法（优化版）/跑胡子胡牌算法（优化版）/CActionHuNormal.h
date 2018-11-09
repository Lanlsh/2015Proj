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
	Card_Wan_10 = 0x10,

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
	//手里的与桌面上的牌组合有7（4人玩时，门子数要调整）方门子（特殊3提5坎例外）
	CardVector vCard; //胡牌时手牌组成的牌型
	int huXiCount; //胡息
	int huXiByHand; //手牌中的胡息
	int huXiByFZ; //附子牌中的胡息
	bool isHaveTiOrPao; //是否有提牌或跑牌
}HuStructInfo;


class CActionHu
{
public:
	CActionHu();
	~CActionHu();

	//获取是否可以胡牌
	bool GetResult(Card cOperator, CardVector vCard, WORD wChaiId, HuStructInfo& huSInfo);

	//获取胡牌的牌型
	bool CheckHu(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHaveTiOrPao);

	//获取附子胡息
	int GetFuZiHuXi(WORD wChairId, bool& isHaveTiOrPao);

	//将牌分为大牌和小牌
	void DivideToTwoArray(CardVector vHandCard, CardVector& vCardBig, CardVector& vCardSmall);
	
	//删除坎牌元素
	void DeleteKanPaiElement(CardVector& ShouPai, Card cardData);

	//删除顺子元素
	void DeleteShunZiElement(CardVector& vCard, CardVector vDeleteElement, CardVector& vHuCard, int& huxiByHand);

	//删除碰牌
	void RemovePeng(CardVector& vCard, CardVector& vHuCard, int& huxiByHand);

	//删除顺子
	void RemoveShunZi(CardVector& vCard, CardVector& vHuCard, int& huxiByHand);

	//删除二七十
	void RemoveTwoSevenTen(CardVector& vCard, CardVector& vHuCard, int& huxiByHand);

	//删除坎牌
	void RemoveKanPai(CardVector& vCard, CardVector& vHuCard, int& huxiByHand);

	//删除元素
	void DeleteJiaoPaiElement(CardVector& vSmallCard, CardVector& vBigCard, CardVector vDeleteElementSmall, CardVector vDeleteElementBig);

	//删除绞牌
	void RemoveJiaoPai(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard);

	//第一种顺序拆分小牌  1. 顺子、二七十、碰、绞牌
	void FirstSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//第二种顺序拆分小牌  2. 二七十、顺子、碰、绞牌
	void SecondSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//第三种顺序拆分小牌  3. 碰、二七十、顺子、绞牌
	void ThirdSmallCardSplit(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//拆小牌
	void DoSplitSmallCard(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, int splitType);

	//第一种顺序拆分大牌  1. 二七十、顺子、碰
	void FirstBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//第二种顺序拆分大牌  2. 顺子、二七十、碰
	void SecondBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//第三种顺序拆分大牌  3. 碰、二七十、顺子、
	void ThirdBigCardSplit(CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand);

	//拆大牌
	bool DoSplitBigCard(CardVector& vSmallCard, CardVector& vBigCard, CardVector& vHuCard, int& huxiByHand, bool isHavePaoOrTi);
};

