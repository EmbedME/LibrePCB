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

#ifndef LIBREPCB_LIBRARY_DEVICEPADSIGNALMAPMODEL_H
#define LIBREPCB_LIBRARY_DEVICEPADSIGNALMAPMODEL_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../cmp/componentsignal.h"
#include "../pkg/packagepad.h"
#include "devicepadsignalmap.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class UndoStack;

namespace library {

/*******************************************************************************
 *  Class DevicePadSignalMapModel
 ******************************************************************************/

/**
 * @brief The DevicePadSignalMapModel class
 */
class DevicePadSignalMapModel final : public QAbstractTableModel,
                                      private DevicePadSignalMap::IF_Observer {
  Q_OBJECT

public:
  enum Column { COLUMN_PAD, COLUMN_SIGNAL, _COLUMN_COUNT };

  // Constructors / Destructor
  DevicePadSignalMapModel() = delete;
  DevicePadSignalMapModel(const DevicePadSignalMapModel& other) noexcept;
  explicit DevicePadSignalMapModel(QObject* parent = nullptr) noexcept;
  ~DevicePadSignalMapModel() noexcept;

  // Setters
  void setPadSignalMap(DevicePadSignalMap* map) noexcept;
  void setUndoStack(UndoStack* stack) noexcept;
  void setSignalList(const ComponentSignalList& list) noexcept;
  void setPadList(const PackagePadList& list) noexcept;

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
  DevicePadSignalMapModel& operator=(
      const DevicePadSignalMapModel& rhs) noexcept;

private:
  virtual void listObjectAdded(
      const DevicePadSignalMap& list, int newIndex,
      const std::shared_ptr<DevicePadSignalMapItem>& ptr) noexcept override;
  virtual void listObjectRemoved(
      const DevicePadSignalMap& list, int oldIndex,
      const std::shared_ptr<DevicePadSignalMapItem>& ptr) noexcept override;
  void signalUuidChanged(const tl::optional<Uuid>& uuid) noexcept;
  void execCmd(UndoCommand* cmd);
  void updateComboBoxItems() noexcept;

private:  // Data
  DevicePadSignalMap*               mPadSignalMap;
  UndoStack*                        mUndoStack;
  ComponentSignalList               mSignals;
  PackagePadList                    mPads;
  QVector<QPair<QString, QVariant>> mComboBoxItems;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb

#endif  // LIBREPCB_LIBRARY_DEVICEPADSIGNALMAPMODEL_H