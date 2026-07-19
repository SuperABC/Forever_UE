#include "implement.h"

#include "../../Dependence/common/json.h"


PostImplement::PostImplement(Map* map, Populace* populace, Society* society,
	Story* story, Industry* industry, Traffic* traffic, Player* player) :
	map(map),
	populace(populace),
	society(society),
	story(story),
	industry(industry),
	traffic(traffic),
	player(player) {
}

PostImplement::~PostImplement() {

}

JsonValue* PostImplement::Post(JsonValue* request) {
	return new JsonValue();
}
