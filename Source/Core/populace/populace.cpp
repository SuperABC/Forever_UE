#include "populace.h"


using namespace std;

Populace::Populace() {

}

Populace::~Populace() {

}

void Populace::LoadConfigs() const {
	//assetFactory->RemoveAll();
	//nameFactory->RemoveAll();
	//schedulerFactory->RemoveAll();

	//auto assets = Config::GetEnables("asset");
	//for (auto asset : assets) {
	//	assetFactory->SetConfig(asset, true);
	//}
	//auto names = Config::GetEnables("name");
	//if (names.size() != 1) {
	//	THROW_EXCEPTION(RuntimeException, "There should be one and only one enabled name.\n");
	//}
	//nameFactory->SetConfig(names[0], true);
	//auto schedulers = Config::GetEnables("scheduler");
	//for (auto scheduler : schedulers) {
	//	schedulerFactory->SetConfig(scheduler, true);
	//}
}
