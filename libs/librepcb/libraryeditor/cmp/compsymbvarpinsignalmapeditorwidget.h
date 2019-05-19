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

#ifndef LIBREPCB_LIBRARY_EDITOR_COMPSYMBVARPINSIGNALMAPEDITORWIDGET_H
#define LIBREPCB_LIBRARY_EDITOR_COMPSYMBVARPINSIGNALMAPEDITORWIDGET_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <librepcb/library/cmp/componentsignal.h>
#include <librepcb/library/cmp/componentsymbolvariant.h>

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class SortFilterProxyModel;

namespace workspace {
class Workspace;
}

namespace library {

class Symbol;
class ComponentPinSignalMapModel;

namespace editor {

/*******************************************************************************
 *  Class CompSymbVarPinSignalMapEditorWidget
 ******************************************************************************/

/**
 * @brief The CompSymbVarPinSignalMapEditorWidget class
 */
class CompSymbVarPinSignalMapEditorWidget final : public QWidget {
  Q_OBJECT

public:
  // Constructors / Destructor
  explicit CompSymbVarPinSignalMapEditorWidget(
      QWidget* parent = nullptr) noexcept;
  CompSymbVarPinSignalMapEditorWidget(
      const CompSymbVarPinSignalMapEditorWidget& other) = delete;
  ~CompSymbVarPinSignalMapEditorWidget() noexcept;

  // General Methods
  void setReferences(const workspace::Workspace&                       ws,
                     const ComponentSignalList&                        sigs,
                     const QHash<Uuid, std::shared_ptr<const Symbol>>* symbols,
                     ComponentSymbolVariant& variant) noexcept;

  // Operator Overloadings
  CompSymbVarPinSignalMapEditorWidget& operator       =(
      const CompSymbVarPinSignalMapEditorWidget& rhs) = delete;

private:
  QScopedPointer<QTableView>                 mView;
  QScopedPointer<ComponentPinSignalMapModel> mModel;
  QScopedPointer<SortFilterProxyModel>       mProxy;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace library
}  // namespace librepcb

#endif  // LIBREPCB_LIBRARY_EDITOR_COMPSYMBVARPINSIGNALMAPEDITORWIDGET_H
