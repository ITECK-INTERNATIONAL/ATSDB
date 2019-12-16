#ifndef POSTPROCESSTASK_H
#define POSTPROCESSTASK_H

#include "configurable.h"
#include "task.h"
#include "postprocesstaskwidget.h"
#include "job.h"

#include <QObject>

#include <memory>

#include "boost/date_time/posix_time/posix_time.hpp"

class TaskManager;
class QProgressDialog;

class PostProcessTask: public Task, public Configurable
{
    Q_OBJECT

//signals:
//    void postProcessingDoneSignal ();

public slots:
    void postProcessingJobDoneSlot();

    //void postProcessingDoneSlot ();

public:
    PostProcessTask(const std::string& class_id, const std::string& instance_id,
                     TaskManager& task_manager);

    virtual void generateSubConfigurable (const std::string &class_id, const std::string &instance_id);

    virtual QWidget* widget ();

    virtual bool checkPrerequisites ();
    virtual bool isRecommended();
    virtual bool isRequired () { return true; }

    virtual bool canRun();
    void run ();

    static const std::string DONE_PROPERTY_NAME;

protected:
    std::unique_ptr<PostProcessTaskWidget> widget_;

    boost::posix_time::ptime start_time_;
    boost::posix_time::ptime stop_time_;

    std::vector <std::shared_ptr<Job>> postprocess_jobs_;
    QProgressDialog* postprocess_dialog_ {nullptr};
    size_t postprocess_job_num_{0};

    virtual void checkSubConfigurables () {}
};

#endif // POSTPROCESSTASK_H
