/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "componentsymbolvariantlistwidget.h"

#include <librepcb/common/widgets/editabletablewidget.h>
#include <librepcb/library/cmp/componentsymbolvariantlistmodel.h>

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace library {
namespace editor {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

ComponentSymbolVariantListWidget::ComponentSymbolVariantListWidget(
    QWidget* parent) noexcept
  : QWidget(parent),
    mView(new EditableTableWidget(this)),
    mModel(new ComponentSymbolVariantListModel(this)) {
  mView->setShowMoveButtons(true);
  mView->setModel(mModel.data());
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSymbolVariantListModel::COLUMN_NAME, QHeaderView::Stretch);
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSymbolVariantListModel::COLUMN_DESCRIPTION,
      QHeaderView::Stretch);
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSymbolVariantListModel::COLUMN_NORM, QHeaderView::Stretch);
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSymbolVariantListModel::COLUMN_SYMBOLCOUNT,
      QHeaderView::ResizeToContents);
  mView->horizontalHeader()->setSectionResizeMode(
      ComponentSymbolVariantListModel::COLUMN_ACTIONS,
      QHeaderView::ResizeToContents);
  connect(mView.data(), &EditableTableWidget::btnAddClicked, mModel.data(),
          &ComponentSymbolVariantListModel::addSymbolVariant);
  connect(mView.data(), &EditableTableWidget::btnRemoveClicked, mModel.data(),
          &ComponentSymbolVariantListModel::removeSymbolVariant);
  connect(mView.data(), &EditableTableWidget::btnMoveUpClicked, mModel.data(),
          &ComponentSymbolVariantListModel::moveSymbolVariantUp);
  connect(mView.data(), &EditableTableWidget::btnMoveDownClicked, mModel.data(),
          &ComponentSymbolVariantListModel::moveSymbolVariantDown);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mView.data());
}

ComponentSymbolVariantListWidget::~ComponentSymbolVariantListWidget() noexcept {
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentSymbolVariantListWidget::setReferences(
    UndoStack* undoStack, ComponentSymbolVariantList* list,
    IF_ComponentSymbolVariantEditorProvider* editorProvider) noexcept {
  mModel->setSymbolVariantList(list);
  mModel->setUndoStack(undoStack);
  mEditorProvider = editorProvider;
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace library
}  // namespace librepcb
