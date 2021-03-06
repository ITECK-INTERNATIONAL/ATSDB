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

#ifndef MANAGEDBOBJECTSTASKWIDGET_H
#define MANAGEDBOBJECTSTASKWIDGET_H

#include <taskwidget.h>

class ManageDBObjectsTask;
class DBObjectManagerWidget;

class ManageDBObjectsTaskWidget : public TaskWidget
{
    Q_OBJECT

  public slots:
    void expertModeChangedSlot();

  public:
    ManageDBObjectsTaskWidget(ManageDBObjectsTask& task, QWidget* parent = nullptr);

  protected:
    ManageDBObjectsTask& task_;

    DBObjectManagerWidget* object_manager_widget_{nullptr};
};

#endif  // MANAGEDBOBJECTSTASKWIDGET_H
