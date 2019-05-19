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

#ifndef LIBREPCB_LIBRARY_COMPONENTPINSIGNALMAPMODEL_H
#define LIBREPCB_LIBRARY_COMPONENTPINSIGNALMAPMODEL_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "componentsymbolvariant.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class UndoStack;

namespace library {

/*******************************************************************************
 *  Class ComponentPinSignalMapModel
 ******************************************************************************/

/**
 * @brief The ComponentPinSignalMapModel class
 */
class ComponentPinSignalMapModel final : public QAbstractTableModel {
  Q_OBJECT

public:
  enum Column {
    COLUMN_SYMBOL,
    COLUMN_PIN,
    COLUMN_SIGNAL,
    COLUMN_DISPLAY,
    _COLUMN_COUNT
  };

  // Constructors / Destructor
  ComponentPinSignalMapModel() = delete;
  ComponentPinSignalMapModel(const ComponentPinSignalMapModel& other) noexcept;
  explicit ComponentPinSignalMapModel(QObject* parent = nullptr) noexcept;
  ~ComponentPinSignalMapModel() noexcept;

  // Setters
  void setSymbolVariant(ComponentSymbolVariant* variant) noexcept;
  void setUndoStack(UndoStack* stack) noexcept;

  // Inherited from QAbstractItemModel
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant      data(const QModelIndex& index,
                     int                role = Qt::DisplayRole) const override;
  QVariant      headerData(int section, Qt::Orientation orientation,
                           int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  bool          setData(const QModelIndex& index, const QVariant& value,
                        int role = Qt::EditRole) override;

  // Operator Overloadings
  ComponentPinSignalMapModel& operator=(
      const ComponentPinSignalMapModel& rhs) noexcept;

private:
  void pinSignalMapEdited(
      const ComponentPinSignalMap& map, int index,
      const std::shared_ptr<const ComponentPinSignalMapItem>& item,
      ComponentPinSignalMap::Event                            event) noexcept;
  void execCmd(UndoCommand* cmd);

private:  // Data
  ComponentSymbolVariant* mSymbolVariant;
  UndoStack*              mUndoStack;

  // Slots
  ComponentPinSignalMap::OnEditedSlot mOnEditedSlot;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb

#endif  // LIBREPCB_LIBRARY_COMPONENTPINSIGNALMAPMODEL_H
