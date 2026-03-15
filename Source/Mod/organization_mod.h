#pragma once

#include "../common/error.h"
#include "../common/utility.h"

#include "../common/error.h"
#include "../common/utility.h"

#include "organization_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModOrganization : public Organization {
public:
    static std::string GetId() { return "mod"; }
    virtual std::string GetType() const override { return "mod"; }
    virtual std::string GetName() const override { return "模组组织"; }

    static float GetPower() {
        return 1.0f;
    }

    virtual std::vector<std::pair<std::string, std::pair<int, int>>> ComponentRequirements() const override {
        return std::vector<std::pair<std::string, std::pair<int, int>>>();
    }

    virtual std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>> ArrageVacancies(
        const std::vector<std::pair<std::string, int>>& components) const override {
        return std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>>();
    }

    virtual void SetCalendar(CalendarFactory* factory) override {
        if (factory == nullptr) {
            THROW_EXCEPTION(NullPointerException, "Calendar factory is null.\n");
        }
        for (const auto& [component, vacancies] : GetJobs()) {
            for (const auto& [job, occupantId] : vacancies) {
                Calendar* calendar = factory->CreateCalendar("mod");
                if (calendar) {
                    job->SetCalendar(calendar);
                }
            }
        }
    }

    virtual void ArrangeRooms() override {
        for (const auto& [component, vacancies] : GetJobs()) {
            if (component == nullptr) {
                debugf("Warning: Organization component is null.\n");
                continue;
            }
            const auto& rooms = component->GetRooms();
            if (rooms.empty()) {
                debugf("Warning: Organization component has no rooms.\n");
                continue;
            }
            for (const auto& [job, occupantId] : vacancies) {
                job->SetPosition(rooms[0]);
            }
        }
    }
};

