#include "cultivator.h"
#include "controller.h"
#include <iostream>
#include <algorithm>
#include <random>

#define RANDOM_MOVE


using namespace std;

int GetRandom(int range) {
	if (range <= 0) return 0;
	mt19937 rng(random_device{}());
	uniform_int_distribution<int> dist(0, range - 1);
	return dist(rng);
}

void PrintNews(vector<Log> news, vector<shared_ptr<Cultivator>> cults) {
	for (auto& n : news) {
		switch (n.type) {
		case LOG_MP:
			cout << cults[n.subject]->GetName() << "真气量回复至" << cults[n.subject]->GetCurrentMP() << endl;
			break;
		case LOG_ATK:
			cout << cults[n.subject]->GetName() << "武力值回复至" << cults[n.subject]->GetCurrentATK() << endl;
			break;
		case LOG_HP:
			cout << cults[n.subject]->GetName() << "健康值回复至" << cults[n.subject]->GetCurrentHP() << endl;
			break;
		case LOG_CONSUME:
			cout << cults[n.subject]->GetName() << "消耗" << n.amount << "点武力值" << endl;
			break;
		case LOG_SKIP:
			cout << cults[n.subject]->GetName() << "跳过回合" << endl;
			break;
		case LOG_MOVE:
			cout << cults[n.subject]->GetName() << "消耗" << n.amount << "点真气对" << cults[n.object]->GetName()
				<< "出招：" << n.name << ", 点数" << n.pointAtk << endl;
			break;
		case LOG_DAMAGE:
			cout << cults[n.subject]->GetName() << "对" << cults[n.object]->GetName() << "造成";
			if (n.pointAtk > 0)
				cout << Action::attributeText[n.attribute] << n.pointAtk << "点招式武力伤害" << endl;
			else
				cout << n.pointHp << "点招式物理伤害" << endl;
			break;
		case LOG_DODGE:
			cout << cults[n.object]->GetName() << "闪避成功" << endl;
			break;
		case LOG_FAIL:
			cout << cults[n.subject]->GetName() << "出招失败" << endl;
			break;
		case LOG_EFFECT:
			switch (n.effect) {
			case EFFECT_PENETRATE:
				cout << cults[n.subject]->GetName() << "对" << cults[n.object]->GetName() << "造成";
				if (n.pointAtk > 0) cout << Action::attributeText[n.attribute] << n.pointAtk << "点穿透武力伤害" << endl;
				else cout << n.pointHp << "点穿透物理伤害" << endl;
				break;
			case EFFECT_REBOUND:
				cout << cults[n.subject]->GetName() << "对" << cults[n.object]->GetName() << "造成";
				if (n.pointAtk > 0) cout << Action::attributeText[n.attribute] << n.pointAtk << "点反弹武力伤害" << endl;
				else cout << n.pointHp << "点反弹物理伤害" << endl;
				break;
			case EFFECT_ABSORB:
				cout << cults[n.subject]->GetName() << "吸收" << n.amount << "点真气" << endl;
				break;
			case EFFECT_LOCK:
				cout << cults[n.object]->GetName() << "被" << cults[n.subject]->GetName() << "禁锢" << endl;
				break;
			case EFFECT_DOT:
				cout << cults[n.object]->GetName() << "受到" << n.amount << "点持续武力伤害" << endl;
				break;
			case EFFECT_REBATE:
				cout << cults[n.subject]->GetName() << "恢复" << n.amount << "点真气" << endl;
				break;
			case EFFECT_RECOVER:
				cout << cults[n.subject]->GetName() << "恢复" << n.amount << "点武力值" << endl;
				break;
			case EFFECT_CURE:
				cout << cults[n.subject]->GetName() << "恢复" << n.amount << "点健康值" << endl;
				break;
			default: break;
			}
			break;
		default: break;
		}
	}
}

int main() {
	auto cult1 = make_shared<Cultivator>("张三", "./act/xiaohua.txt",
		1000, 1200, 1600, 20, 40);
	auto cult2 = make_shared<Cultivator>("李四", "./act/xiaohua.txt",
		1000, 1080, 1800, 22, 40);
	vector<shared_ptr<Cultivator>> cults = { cult1, cult2 };
	Controller game(cults);

	cout << "《五行斗诀》战斗开始！" << endl;
	cout << cult1->GetName() << " VS " << cult2->GetName() << endl;

	int turnCount = 0;
	while (cult1->IsAlive() && cult2->IsAlive()) {
		cout << "\n--- 第 " << ++turnCount << " 回合 ---" << endl;
		cout << cult1->GetName() << ": HP=" << cult1->GetCurrentHP()
			<< ", ATK=" << cult1->GetCurrentATK()
			<< ", MP=" << cult1->GetCurrentMP() << endl;
		cout << cult2->GetName() << ": HP=" << cult2->GetCurrentHP()
			<< ", ATK=" << cult2->GetCurrentATK()
			<< ", MP=" << cult2->GetCurrentMP() << endl;

		game.StartTurn();
		PrintNews(game.GetNews(), cults);

		shared_ptr<Action> act1, act2;
#ifdef RANDOM_MOVE
		int ac1 = (int)cult1->GetActions().size();
		int ac2 = (int)cult2->GetActions().size();
		if (GetRandom(4)) act1 = cult1->GetAction(GetRandom(ac1 + 6));
		else act1 = cult1->GetAction(GetRandom(ac1), GetRandom(ac1));
		if (GetRandom(4)) act2 = cult2->GetAction(GetRandom(ac2 + 6));
		else act2 = cult2->GetAction(GetRandom(ac2), GetRandom(ac2));
#elif defined(SPECIFIC_MOVE)
		act1 = cult1->GetAction(11);
		act2 = cult2->GetAction(4);
#endif

		game.ActionTurn({ {act1, 1}, {act2, 0} });
		PrintNews(game.GetNews(), cults);

		game.CheckTurn({ {act1, 1}, {act2, 0} });
		PrintNews(game.GetNews(), cults);

		game.EndTurn();
		PrintNews(game.GetNews(), cults);

		cout << "回合结束" << endl;
	}

	if (!cult1->IsAlive() && !cult2->IsAlive())
		cout << "\n战斗结束！同归于尽!" << endl;
	else
		cout << "\n战斗结束！胜利者: " << (cult1->IsAlive() ? cult1->GetName() : cult2->GetName()) << "!" << endl;

	return 0;
}
