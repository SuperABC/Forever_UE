#include "society.h"


using namespace std;

CalendarFactory* Society::calendarFactory = nullptr;

Society::Society() {
	if (!calendarFactory) {
		calendarFactory = new CalendarFactory();
	}
}

Society::~Society() {

}

void Society::LoadConfigs() const {
	calendarFactory->RemoveAll();
	//jobFactory->RemoveAll();
	//organizationFactory->RemoveAll();

	auto calendars = Config::GetEnables("calendar");
	for (auto calendar : calendars) {
		calendarFactory->SetConfig(calendar, true);
	}
	//auto jobs = Config::GetEnables("job");
	//for (auto job : jobs) {
	//	jobFactory->SetConfig(job, true);
	//}
	//auto organizations = Config::GetEnables("organization");
	//for (auto organization : organizations) {
	//	organizationFactory->SetConfig(organization, true);
	//}
}

void Society::InitCalendars(std::unordered_map<std::string, HMODULE>& modHandles,
    const std::vector<std::string>& dlls) {

    calendarFactory->RegisterCalendar(EmptyCalendar::GetId(),
        []() { return new EmptyCalendar(); },
        [](CalendarMod* calendar) { delete calendar; }
    );

    for (auto dll : dlls) {
        HMODULE modHandle;
        if (modHandles.find(dll) != modHandles.end()) {
            modHandle = modHandles[dll];
        }
        else {
            modHandle = LoadLibraryA(dll.data());
            modHandles[dll] = modHandle;
        }

        if (modHandle) {
            debugf("Log: %s loaded successfully.\n", dll.data());

            auto registerFunc =
                (RegisterModCalendarsFunc)GetProcAddress(modHandle, "RegisterModCalendars");
            if (registerFunc) {
                registerFunc(calendarFactory);
            }
        }
        else {
            debugf("Warning: Failed to load %s.\n", dll.data());
        }
    }
}

void Society::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}
