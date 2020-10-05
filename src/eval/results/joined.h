#ifndef JOINEDEVALUATIONREQUIREMENTRESULT_H
#define JOINEDEVALUATIONREQUIREMENTRESULT_H

#include "eval/results/base.h"

namespace EvaluationRequirementResult
{

class Joined : public Base
{
public:
    Joined(std::shared_ptr<EvaluationRequirement> requirement,
           EvaluationManager& eval_man);

    virtual void join(std::shared_ptr<Base> other);

    virtual void print() = 0;
    virtual void addToReport (std::shared_ptr<EvaluationResultsReport::RootItem> root_item) = 0;

protected:
    std::vector<std::shared_ptr<Base>> results_;
};

}
#endif // JOINEDEVALUATIONREQUIREMENTRESULT_H