#include "viewpointsreportgenerator.h"
#include "viewpointsreportgeneratordialog.h"
#include "viewmanager.h"
#include "viewpointswidget.h"
#include "viewpointstablemodel.h"
#include "viewpoint.h"
#include "logger.h"
#include "stringconv.h"
#include "dbobjectmanager.h"
#include "atsdb.h"
#include "global.h"
#include "dbinterface.h"
#include "sqliteconnection.h"
#include "mysqlppconnection.h"
#include "files.h"
#include "latexdocument.h"
#include "latexvisitor.h"
#include "system.h"

#if USE_EXPERIMENTAL_SOURCE == true
#include "osgview.h"
#endif

#include "boost/date_time/posix_time/posix_time.hpp"

#include <QCoreApplication>
#include <QApplication>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>

using namespace std;
using namespace Utils;

ViewPointsReportGenerator::ViewPointsReportGenerator(const std::string& class_id, const std::string& instance_id,
                                                     ViewManager& view_manager)
    : Configurable(class_id, instance_id, &view_manager), view_manager_(view_manager)
{
    registerParameter("author", &author_, "");
    registerParameter("abstract", &abstract_, "");

    SQLiteConnection* sql_con = dynamic_cast<SQLiteConnection*>(&ATSDB::instance().interface().connection());

    if (sql_con)
    {
        report_path_ = Files::getDirectoryFromPath(sql_con->lastFilename())+"/report_"
                + Files::getFilenameFromPath(sql_con->lastFilename()) + "/";
    }
    else
    {
        MySQLppConnection* mysql_con = dynamic_cast<MySQLppConnection*>(&ATSDB::instance().interface().connection());
        assert (mysql_con);
        report_path_ = HOME_PATH+"/report_"+mysql_con->usedDatabase() + "/";
    }

    report_filename_ = "report.tex";


    loginf << "ViewPointsReportGenerator: constructor: report path '" << report_path_ << "'"
           << " filename '"  << report_filename_ << "'";

    registerParameter("time_before_screenshot_ms", &time_before_screenshot_ms_, 0);
    registerParameter("export_all_unsorted", &export_all_unsorted_, false);
    registerParameter("run_pdflatex", &run_pdflatex_, true);
    registerParameter("open_created_pdf", &open_created_pdf_, false);

    pdflatex_found_ = System::exec("which pdflatex").size(); // empty if none

    if (!pdflatex_found_)
    {
        run_pdflatex_ = false;
        open_created_pdf_ = false;
    }
}


void ViewPointsReportGenerator::generateSubConfigurable(const std::string& class_id,
                                                        const std::string& instance_id)
{
    throw std::runtime_error("ViewPointsReportGenerator: generateSubConfigurable: unknown class_id " +
                             class_id);
}

void ViewPointsReportGenerator::checkSubConfigurables()
{
    // move along sir
}


ViewPointsReportGeneratorDialog& ViewPointsReportGenerator::dialog()
{
    if (!dialog_)
        dialog_.reset(new ViewPointsReportGeneratorDialog(*this));

    return *dialog_;
}

void ViewPointsReportGenerator::run ()
{
    loginf << "ViewPointsReportGenerator: run";

    assert (dialog_);
    dialog_->setRunning(true);

    LatexDocument doc (report_path_, report_filename_);
    doc.title("ATSDB View Points Report");

    if (author_.size())
        doc.author(author_);

    if (abstract_.size())
        doc.abstract(abstract_);

    LatexVisitor visitor (doc);

    cancel_ = false;
    running_ = true;
    pdf_created_ = false;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    boost::posix_time::ptime start_time;
    boost::posix_time::ptime stop_time;
    boost::posix_time::time_duration time_diff;

    boost::posix_time::ptime vp_start_time;

    start_time = boost::posix_time::microsec_clock::local_time();

    ViewPointsWidget* vp_widget = view_manager_.viewPointsWidget();
    assert (vp_widget);
    ViewPointsTableModel* table_model = vp_widget->tableModel();
    assert (table_model);

    std::vector<unsigned int> vp_ids;

    if (export_all_unsorted_)
        vp_ids = vp_widget->viewPoints();
    else
        vp_ids = vp_widget->viewedViewPoints();

    string status_str, elapsed_time_str, remaining_time_str;
    DBObjectManager& obj_man = ATSDB::instance().objectManager();

    unsigned int vp_cnt = 0;
    unsigned int vp_size = vp_ids.size();
    double ms;
    double ms_per_vp;

#if USE_EXPERIMENTAL_SOURCE == true
    OSGView::instant_display_ = true;
#endif

    for (auto vp_id : vp_ids)
    {
        while (QCoreApplication::hasPendingEvents())
            QCoreApplication::processEvents();

        if (cancel_)
        {
            loginf << "ViewPointsReportGenerator: run: cancel";
            break;
        }

        loginf << "ViewPointsReportGenerator: run: setting vp " << vp_id;
        view_manager_.setCurrentViewPoint(vp_id);

        while (obj_man.loadInProgress() || QCoreApplication::hasPendingEvents())
            QCoreApplication::processEvents();

        // do stuff
        assert (table_model->existsViewPoint(vp_id));
        ViewPoint& view_point = table_model->viewPoint(vp_id);

        view_point.accept(visitor);
        visitor.imagePrefix("vp_"+to_string(vp_id));

        for (auto& view_it : view_manager_.getViews())
        {
#if USE_EXPERIMENTAL_SOURCE == true
            if (dynamic_cast<OSGView*>(view_it.second))
            {
                view_it.second->showInTabWidget();

                // wait maybe
                if (time_before_screenshot_ms_)
                {
                    vp_start_time = boost::posix_time::microsec_clock::local_time();

                    while ((boost::posix_time::microsec_clock::local_time()-vp_start_time).total_milliseconds()
                           < time_before_screenshot_ms_)
                    {
                        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                    }
                }
            }
#endif

            view_it.second->accept(visitor);
        }

        // update status
        stop_time = boost::posix_time::microsec_clock::local_time();

        time_diff = stop_time - start_time;
        ms = time_diff.total_milliseconds();
        elapsed_time_str =
                String::timeStringFromDouble(ms / 1000.0, false);

        status_str = "Writing view point "+to_string(vp_cnt+1)+"/"+to_string(vp_size);

        if (vp_cnt && ms > 0)
        {
            ms_per_vp = ms/(double)vp_cnt;

            remaining_time_str = String::timeStringFromDouble((vp_size-vp_cnt) * ms_per_vp / 1000.0, false);

            loginf << "ViewPointsReportGenerator: run: setting vp " << vp_id
                   << " done after " << elapsed_time_str << " remaining " << remaining_time_str;
        }
        else
        {
            loginf << "ViewPointsReportGenerator: run: setting vp " << vp_id
                   << " done after " << elapsed_time_str;

        }

        dialog_->setElapsedTime(elapsed_time_str);
        dialog_->setProgress(0, vp_size, vp_cnt);
        dialog_->setStatus(status_str);
        dialog_->setRemainingTime(remaining_time_str);

        ++vp_cnt;
    }

    if (cancel_)
    {
        dialog_->setProgress(0, vp_size, 0);
        dialog_->setStatus("Writing view points cancelled");
        dialog_->setRemainingTime(String::timeStringFromDouble(0, false));

        while (QCoreApplication::hasPendingEvents())
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    else // proceed
    {
        dialog_->setProgress(0, vp_size, vp_size);
        dialog_->setStatus("Writing view points done");
        dialog_->setRemainingTime(String::timeStringFromDouble(0, false));

        while (QCoreApplication::hasPendingEvents())
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        doc.write();

        if (run_pdflatex_)
        {
            std::string command_out;
            std::string command = "cd "+report_path_+" && pdflatex --interaction=nonstopmode "+report_filename_
                    +" | awk 'BEGIN{IGNORECASE = 1}/warning|!/,/^$/;'";

            loginf << "ViewPointsReportGenerator: run: running pdflatex";
            dialog_->setStatus("Running pdflatex");

            while (QCoreApplication::hasPendingEvents())
                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            logdbg << "ViewPointsReportGenerator: run: cmd '" << command << "'";

            command_out = System::exec(command);

            while (command_out.find("Rerun to get outlines right") != std::string::npos)
            {
                loginf << "ViewPointsReportGenerator: run: re-running pdflatex";
                dialog_->setStatus("Re-running pdflatex");

                while (QCoreApplication::hasPendingEvents())
                    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

                command_out = System::exec(command);
            }

            loginf << "ViewPointsReportGenerator: run: result '" << command_out << "'";

            if (!command_out.size()) // no warnings
            {
                pdf_created_ = true;

                dialog_->setStatus("Running pdflatex done");

                if (open_created_pdf_)
                {
                    std::string fullpath = report_path_+report_filename_;

                    if (String::hasEnding(fullpath, ".tex"))
                    {
                        String::replace(fullpath, ".tex", ".pdf");

                        loginf << "ViewPointsReportGenerator: run: opening '" << fullpath << "'";

                        QDesktopServices::openUrl(QUrl(fullpath.c_str()));
                    }
                    else
                        logerr << "ViewPointsReportGenerator: run: opening not possible since wrong file ending";
                }
            }
            else // show warnings
            {
                QMessageBox msgBox;
                msgBox.setText("PDF Latex failed with warnings:\n\n"+QString(command_out.c_str()));
                msgBox.exec();
            }
        }
    }
    dialog_->setRunning(false);
    dialog_->close();

    QApplication::restoreOverrideCursor();

#if USE_EXPERIMENTAL_SOURCE == true
    OSGView::instant_display_ = false;
#endif

    if (show_done_)
    {
        QMessageBox msgBox;
        if (cancel_)
            msgBox.setText("Export View Points as PDF Cancelled");
        else
            msgBox.setText("Export View Points as PDF Done");

        msgBox.exec();
    }

    running_ = false;
}

void ViewPointsReportGenerator::cancel ()
{
    loginf << "ViewPointsReportGenerator: cancel";

    cancel_ = true;

    if (!running_)
        dialog_->close();
}

std::string ViewPointsReportGenerator::reportPath() const
{
    return report_path_;
}

void ViewPointsReportGenerator::reportPath(const std::string& path)
{
    loginf << "ViewPointsReportGenerator: reportPath: '" << path << "'";
    report_path_ = path;

    if (dialog_)
        dialog_->updateFileInfo();
}

std::string ViewPointsReportGenerator::reportFilename() const
{
    return report_filename_;
}

void ViewPointsReportGenerator::reportFilename(const std::string& filename)
{
    loginf << "ViewPointsReportGenerator: reportFilename: '" << filename << "'";
    report_filename_ = filename;

    if (dialog_)
        dialog_->updateFileInfo();
}

void ViewPointsReportGenerator::reportPathAndFilename(const std::string& str)
{
    report_path_ = Files::getDirectoryFromPath(str) + "/";
    report_filename_ = Files::getFilenameFromPath(str);

    loginf << "ViewPointsReportGenerator: reportPathAndFilename: path '" << report_path_
           << "' filename '" << report_filename_ << "'";

    if (dialog_)
        dialog_->updateFileInfo();
}

bool ViewPointsReportGenerator::isRunning() const
{
    return running_;
}

void ViewPointsReportGenerator::showDone(bool show_done)
{
    show_done_ = show_done;
}

std::string ViewPointsReportGenerator::author() const
{
    return author_;
}

void ViewPointsReportGenerator::author(const std::string& author)
{
    author_ = author;
}

std::string ViewPointsReportGenerator::abstract() const
{
    return abstract_;
}

void ViewPointsReportGenerator::abstract(const std::string& abstract)
{
    abstract_ = abstract;
}

bool ViewPointsReportGenerator::exportAllUnsorted() const
{
    return export_all_unsorted_;
}

void ViewPointsReportGenerator::exportAllUnsorted(bool value)
{
    export_all_unsorted_ = value;
}

bool ViewPointsReportGenerator::runPDFLatex() const
{
    return run_pdflatex_;
}

void ViewPointsReportGenerator::runPDFLatex(bool value)
{
    run_pdflatex_ = value;
}

bool ViewPointsReportGenerator::pdfLatexFound() const
{
    return pdflatex_found_;
}

bool ViewPointsReportGenerator::openCreatedPDF() const
{
    return open_created_pdf_;
}

void ViewPointsReportGenerator::openCreatedPDF(bool value)
{
    open_created_pdf_ = value;
}

unsigned int ViewPointsReportGenerator::timeBeforeScreenshot() const
{
    return time_before_screenshot_ms_;
}

void ViewPointsReportGenerator::timeBeforeScreenshot(unsigned int value_ms)
{
    time_before_screenshot_ms_ = value_ms;
}

