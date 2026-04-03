#include "product_mod.h"


// 经验产品
class ExperienceProduct : public ProductMod {
public:
	ExperienceProduct();
	virtual ~ExperienceProduct();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void SetProperty() override;

private:
	static int count;

	int id;
	std::string name;
};

