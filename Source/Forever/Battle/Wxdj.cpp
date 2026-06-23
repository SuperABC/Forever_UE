#include "Battle/Wxdj.h"


using namespace std;

UWxdj::UWxdj() {
	controller = make_shared<Controller>();
	turnCount = 0;
	turnStage = STAGE_START;
}

UWxdj::~UWxdj() {

}

void UWxdj::AddCultivator(const FString& name, const FString& file,
	int hp, int atk, int mp, int agi, int recover) {
	controller->AddCultivator(make_shared<Cultivator>(TCHAR_TO_UTF8(*name), TCHAR_TO_UTF8(*file),
		hp, atk, mp, agi, recover));
	currentActions.emplace_back();
}

FString UWxdj::GetLog() {
	auto news = controller->GetNews();
	FString log;
	for (auto& n : news) {
		switch (n.type) {
		case LOG_MP:
			log += FString::Printf(TEXT("%s真气量回复至%d\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), controller->GetCultivator(n.subject)->GetCurrentMP());
			break;
		case LOG_ATK:
			log += FString::Printf(TEXT("%s武力值回复至%d\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), controller->GetCultivator(n.subject)->GetCurrentATK());
			break;
		case LOG_HP:
			log += FString::Printf(TEXT("%s健康值回复至%d\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), controller->GetCultivator(n.subject)->GetCurrentHP());
			break;
		case LOG_CONSUME:
			log += FString::Printf(TEXT("%s消耗%d点武力值\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), n.amount);
			break;
		case LOG_SKIP:
			log += FString::Printf(TEXT("%s跳过回合\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()));
			break;
		case LOG_MOVE:
			log += FString::Printf(TEXT("%s消耗%d点真气对%s出招：%s, 点数%d\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), n.amount, UTF8_TO_TCHAR(controller->GetCultivator(n.object)->GetName().c_str()), UTF8_TO_TCHAR(n.name.c_str()), n.pointAtk);
			break;
		case LOG_DAMAGE:
			log += FString::Printf(TEXT("%s对%s造成"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), UTF8_TO_TCHAR(controller->GetCultivator(n.object)->GetName().c_str()));
			if (n.pointAtk > 0)
				log += FString::Printf(TEXT("%s%d点招式武力伤害\n"), UTF8_TO_TCHAR(Action::attributeText[n.attribute].c_str()), n.pointAtk);
			else
				log += FString::Printf(TEXT("%d点招式物理伤害\n"), n.pointHp);
			break;
		case LOG_DODGE:
			log += FString::Printf(TEXT("%s闪避成功\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.object)->GetName().c_str()));
			break;
		case LOG_FAIL:
			log += FString::Printf(TEXT("%s出招失败\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()));
			break;
		case LOG_EFFECT:
			switch (n.effect) {
			case EFFECT_PENETRATE:
				log += FString::Printf(TEXT("%s对%s造成"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), UTF8_TO_TCHAR(controller->GetCultivator(n.object)->GetName().c_str()));
				if (n.pointAtk > 0) log += FString::Printf(TEXT("%s%d点穿透武力伤害\n"), UTF8_TO_TCHAR(Action::attributeText[n.attribute].c_str()), n.pointAtk);
				else log += FString::Printf(TEXT("%d点穿透物理伤害\n"), n.pointHp);
				break;
			case EFFECT_REBOUND:
				log += FString::Printf(TEXT("%s对%s造成"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), UTF8_TO_TCHAR(controller->GetCultivator(n.object)->GetName().c_str()));
				if (n.pointAtk > 0) log += FString::Printf(TEXT("%s%d点反弹武力伤害\n"), UTF8_TO_TCHAR(Action::attributeText[n.attribute].c_str()), n.pointAtk);
				else log += FString::Printf(TEXT("%d点反弹物理伤害\n"), n.pointHp);
				break;
			case EFFECT_ABSORB:
				log += FString::Printf(TEXT("%s吸收%d点真气\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), n.amount);
				break;
			case EFFECT_LOCK:
				log += FString::Printf(TEXT("%s被%s禁锢\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.object)->GetName().c_str()), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()));
				break;
			case EFFECT_DOT:
				log += FString::Printf(TEXT("%s受到%d点持续武力伤害\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.object)->GetName().c_str()), n.amount);
				break;
			case EFFECT_REBATE:
				log += FString::Printf(TEXT("%s恢复%d点真气\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), n.amount);
				break;
			case EFFECT_RECOVER:
				log += FString::Printf(TEXT("%s恢复%d点武力值\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), n.amount);
				break;
			case EFFECT_CURE:
				log += FString::Printf(TEXT("%s恢复%d点健康值\n"), UTF8_TO_TCHAR(controller->GetCultivator(n.subject)->GetName().c_str()), n.amount);
				break;
			default: break;
			}
			break;
		default: break;
		}
	}
	return log;
}

bool UWxdj::NextStage() {
	switch (turnStage) {
	case STAGE_START:
		controller->StartTurn();
		turnStage = STAGE_ACTION;
		return true;
	case STAGE_ACTION:
		return false;
	case STAGE_CHECK:
		controller->CheckTurn(currentActions);
		turnStage = STAGE_END;
		return true;
	case STAGE_END:
		controller->EndTurn();
		for (auto& [action, _] : currentActions) action.reset();
		turnStage = STAGE_START;
		turnCount++;
		return true;
	default:
		break;
	}
	return true;
}

void UWxdj::SelectMove(int idx, int subject, int object) {
	if (turnStage != STAGE_ACTION) return;

	auto selected = controller->GetCultivator(subject)->GetAction(idx);
	currentActions[subject] = { selected, object };

	for (auto& [action, _] : currentActions) {
		if (action == nullptr)return;
	}
	turnStage = STAGE_CHECK;
}
