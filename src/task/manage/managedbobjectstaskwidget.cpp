/*
 * This file is part of ATSDB.
 *
 * ATSDB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ATSDB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with ATSDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "managedbobjectstaskwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "atsdb.h"
#include "dbobjectmanager.h"
#include "dbobjectmanagerwidget.h"

ManageDBObjectsTaskWidget::ManageDBObjectsTaskWidget(ManageDBObjectsTask& task, QWidget* parent)
    : TaskWidget(parent), task_(task)
{
    QVBoxLayout* main_layout_ = new QVBoxLayout();

    object_manager_widget_ = ATSDB::instance().objectManager().widget();
    main_layout_->addWidget(object_manager_widget_);

    expertModeChangedSlot();

    setLayout(main_layout_);
}

void ManageDBObjectsTaskWidget::expertModeChangedSlot() {}
