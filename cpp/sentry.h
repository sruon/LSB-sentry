#pragma once

#include "utils/moduleutils.h"

class SentryModule : public CPPModule
{
public:
    ~SentryModule() override;

private:
    void OnInit() override;
};
