// 跑胡子胡牌算法（优化版）.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include "CActionHuNormal.h"
//#include "CActionHuByArray.h"
#include "CAtionHuByLaiZi.h"
#include "time.h"

int CARDARRAY[] = { Card_Wan_2, Card_Tiao_2, Card_Tiao_2, Card_Wan_3, Card_Wan_4, Card_Wan_5, Card_Wan_6, Card_Wan_6, Card_Wan_6,
Card_Wan_7, Card_Wan_7, Card_Wan_7, Card_Wan_8, Card_Wan_8, Card_Wan_8, Card_Wan_9, Card_Wan_9, Card_Wan_9, Card_Tiao_5, Card_Tiao_5, Card_Tiao_5 };

//int CARDARRAY[] = { Card_Wan_2, Card_Wan_2, Card_Wan_3, Card_Wan_4};

void Test()
{
	clock_t start, finish;
	double totaltime;
	int count = 1;
	int tmpCount = count;

	CardVector vCard;
	for (int i = 0; i < sizeof(CARDARRAY) / sizeof(CARDARRAY[0]); i++)
	{
		vCard.push_back(CARDARRAY[i]);
	}

	//start = clock();
	//while (count)
	//{
	//	for (int i = 0; i < 20; i++)
	//	{
	//		CActionHuByArray actionHu;
	//		HuStructInfo huSInfo;
	//		huSInfo.vCard.clear();
	//		huSInfo.isHaveTiOrPao = false;
	//		huSInfo.huXiCount = 0;
	//		huSInfo.huXiByHand = 0;
	//		huSInfo.huXiByFZ = actionHu.GetFuZiHuXi(0, huSInfo.isHaveTiOrPao);
	//		huSInfo.huXiCount += huSInfo.huXiByFZ;
	//		Card card = i + 1;
	//		if (i > 9)
	//		{
	//			card = i - 10 + 0x11;
	//		}
	//		bool result = actionHu.GetResult(card, vCard, 0, huSInfo);
	//		//if (result)
	//		//{
	//		//	cout << endl << "result: " << result << "    "<<"Card: "<<card<<"      ";
	//		//	cout << "huSInfo.huXiCount: " << huSInfo.huXiCount << "     ";
	//		//	cout << "huSInfo.vCard:" << "  ";
	//		//	for (int i = 0; i < huSInfo.vCard.size(); i++)
	//		//	{
	//		//		cout << " " << huSInfo.vCard[i];
	//		//	}
	//		//}
	//	}

	//	count--;
	//}
	//finish = clock();

	//CActionHuByArray actionHu;
	//HuStructInfo huSInfo;
	//huSInfo.vCard.clear();
	//huSInfo.isHaveTiOrPao = false;
	//huSInfo.huXiCount = 0;
	//huSInfo.huXiByHand = 0;
	//huSInfo.huXiByFZ = actionHu.GetFuZiHuXi(17, huSInfo.isHaveTiOrPao);
	//huSInfo.huXiCount += huSInfo.huXiByFZ;
	//bool result = actionHu.GetResult(Card_Tiao_3, vCard, 0, huSInfo);

	start = clock();
	//while (count)
	//{
	//	for (int i = 0; i < 20; i++)
	//	{
	//		CAtionHuByLaiZi actionHu;
	//		HuStructInfo huSInfo;
	//		huSInfo.vCard.clear();
	//		huSInfo.isHaveTiOrPao = false;
	//		huSInfo.huXiCount = 0;
	//		huSInfo.haveHowMuchMenZi = 0;
	//		Card card = i + 1;
	//		if (i > 9)
	//		{
	//			card = i - 10 + 0x11;
	//		}
	//		bool result = actionHu.GetResult(card, vCard, 0, huSInfo);
	//		if (result)
	//		{
	//			cout <<endl<< "result: " << result << "    "<<"Card: "<<card<<"      ";
	//			cout << "huSInfo.huXiCount: " << huSInfo.huXiCount << "     ";
	//			cout << "huSInfo.vCard:"<<"  ";
	//			for (int i = 0; i < huSInfo.vCard.size(); i++)
	//			{
	//				cout << " " << huSInfo.vCard[i];
	//			}
	//		}
	//	}

	//	count--;
	//}
	finish = clock();

	CAtionHuByLaiZi actionHu;
	HuStructInfo huSInfo;
	huSInfo.vCard.clear();
	huSInfo.isHaveTiOrPao = false;
	huSInfo.huXiCount = 10;
	huSInfo.haveHowMuchMenZi = 0;
	bool result = actionHu.GetResult(Card_INVALID, vCard, 0, huSInfo);
	
	cout << "result: " << result << endl;
	cout << "huSInfo.huXiCount: " << huSInfo.huXiCount << endl;
	cout << "huSInfo.vCard:";
	for (int i = 0; i < huSInfo.vCard.size(); i++)
	{
		cout << " " << huSInfo.vCard[i];
	}
	
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\nstart: " << start << endl;
	cout << "finish: " << finish << endl;
	cout << "此程序的运行" << tmpCount << "次时间为" << totaltime << "秒！" << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Test();
	system("pause");
	return 0;
}

