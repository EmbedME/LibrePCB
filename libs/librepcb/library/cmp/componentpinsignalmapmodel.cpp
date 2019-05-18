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
#include "componentpinsignalmapmodel.h"

#include <librepcb/common/undostack.h>

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace library {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

ComponentPinSignalMapModel::ComponentPinSignalMapModel(QObject* parent) noexcept
  : QAbstractTableModel(parent),
    mPinSignalMap(nullptr),
    mUndoStack(nullptr),
    mOnEditedSlot(*this, &ComponentPinSignalMapModel::pinSignalMapEdited) {
}

ComponentPinSignalMapModel::~ComponentPinSignalMapModel() noexcept {
  setPinSignalMap(nullptr);
  setUndoStack(nullptr);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentPinSignalMapModel::setPinSignalMap(
    ComponentPinSignalMap* map) noexcept {
  emit beginResetModel();

  if (mPinSignalMap) {
    mPinSignalMap->onEdited.detach(mOnEditedSlot);
  }

  mPinSignalMap = map;

  if (mPinSignalMap) {
    mPinSignalMap->onEdited.attach(mOnEditedSlot);
  }

  emit endResetModel();
}

void ComponentPinSignalMapModel::setUndoStack(UndoStack* stack) noexcept {
  mUndoStack = stack;
}

/*******************************************************************************
 *  Inherited from QAbstractItemModel
 ******************************************************************************/

int ComponentPinSignalMapModel::rowCount(const QModelIndex& parent) const {
  if (!parent.isValid() && mPinSignalMap) {
    return mPinSignalMap->count();
  }
  return 0;
}

int ComponentPinSignalMapModel::columnCount(const QModelIndex& parent) const {
  if (!parent.isValid()) {
    return _COLUMN_COUNT;
  }
  return 0;
}

QVariant ComponentPinSignalMapModel::data(const QModelIndex& index,
                                          int                role) const {
  if (!index.isValid() || !mPinSignalMap) {
    return QVariant();
  }

  std::shared_ptr<ComponentPinSignalMapItem> item =
      mPinSignalMap->value(index.row());
  if (!item) {
    return QVariant();
  }

  switch (index.column()) {
    // case COLUMN_SYMBOL: {
    //  Uuid                              uuid = item->getPadUuid();
    //  std::shared_ptr<const PackagePad> pad  = mPads.find(uuid);
    //  switch (role) {
    //    case Qt::DisplayRole:
    //      return pad ? *pad->getName() : uuid.toStr();
    //    case Qt::ToolTipRole:
    //      return uuid.toStr();
    //    default:
    //      return QVariant();
    //  }
    //}
    // case COLUMN_SIGNAL: {
    //  tl::optional<Uuid>                     uuid = item->getSignalUuid();
    //  std::shared_ptr<const ComponentSignal> sig =
    //      uuid ? mSignals.find(*uuid) : nullptr;
    //  switch (role) {
    //    case Qt::DisplayRole:
    //      return sig ? *sig->getName()
    //                 : (uuid ? uuid->toStr() : tr("(unconnected)"));
    //    case Qt::EditRole:
    //      return uuid ? uuid->toStr() : QVariant();  // NULL means
    //      unconnected!
    //    case Qt::ToolTipRole:
    //      return uuid ? uuid->toStr() : QVariant();
    //    case Qt::UserRole:
    //      return QVariant::fromValue(mComboBoxItems);
    //    default:
    //      return QVariant();
    //  }
    //}
    default:
      return QVariant();
  }

  return QVariant();
}

QVariant ComponentPinSignalMapModel::headerData(int             section,
                                                Qt::Orientation orientation,
                                                int             role) const {
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      switch (section) {
        case COLUMN_SYMBOL:
          return tr("Symbol");
        case COLUMN_PIN:
          return tr("Pin");
        case COLUMN_SIGNAL:
          return tr("Component Signal");
        case COLUMN_DISPLAY:
          return tr("Designator in Schematics");
      }
    }
  } else if (orientation == Qt::Vertical) {
    // if (mPinSignalMap && (role == Qt::DisplayRole)) {
    //  std::shared_ptr<ComponentPinSignalMapItem> item =
    //      mPinSignalMap->value(section);
    //  return item ? item->getPadUuid().toStr().left(8) : QVariant();
    //} else if (mPinSignalMap && (role == Qt::ToolTipRole)) {
    //  std::shared_ptr<ComponentPinSignalMapItem> item =
    //      mPinSignalMap->value(section);
    //  return item ? item->getPadUuid().toStr() : QVariant();
    //} else if (role == Qt::TextAlignmentRole) {
    //  return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    //} else if (role == Qt::FontRole) {
    //  QFont f = QAbstractTableModel::headerData(section, orientation, role)
    //                .value<QFont>();
    //  f.setStyleHint(QFont::Monospace);  // ensure fixed column width
    //  f.setFamily("Monospace");
    //  return f;
    //}
  }
  return QVariant();
}

Qt::ItemFlags ComponentPinSignalMapModel::flags(
    const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractTableModel::flags(index);
  if (index.isValid() && ((index.column() == COLUMN_SIGNAL) ||
                          (index.column() == COLUMN_DISPLAY))) {
    f |= Qt::ItemIsEditable;
  }
  return f;
}

bool ComponentPinSignalMapModel::setData(const QModelIndex& index,
                                         const QVariant& value, int role) {
  if (!mPinSignalMap) {
    return false;
  }
  try {
    // if ((index.column() == COLUMN_SIGNAL) && (role == Qt::EditRole)) {
    //  std::shared_ptr<DevicePadSignalMapItem> item =
    //      mPinSignalMap->value(index.row());
    //  if (!item) {
    //    return false;
    //  }
    //  QScopedPointer<CmdDevicePadSignalMapItemEdit> cmd(
    //      new CmdDevicePadSignalMapItemEdit(*item));
    //  cmd->setSignalUuid(Uuid::tryFromString(value.toString()));
    //  execCmd(cmd.take());
    //  return true;
    //}
  } catch (const Exception& e) {
    QMessageBox::critical(0, tr("Error"), e.getMsg());
  }
  return false;
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void ComponentPinSignalMapModel::pinSignalMapEdited(
    const ComponentPinSignalMap& map, int index,
    const std::shared_ptr<const ComponentPinSignalMapItem>& item,
    ComponentPinSignalMap::Event                            event) noexcept {
  Q_UNUSED(map);
  Q_UNUSED(item);
  switch (event) {
    case ComponentPinSignalMap::Event::ElementAdded:
      beginInsertRows(QModelIndex(), index, index);
      endInsertRows();
      break;
    case ComponentPinSignalMap::Event::ElementRemoved:
      beginRemoveRows(QModelIndex(), index, index);
      endRemoveRows();
      break;
    case ComponentPinSignalMap::Event::ElementEdited:
      dataChanged(this->index(index, 0), this->index(index, _COLUMN_COUNT - 1));
      break;
    default:
      qWarning() << "Unhandled switch-case in "
                    "ComponentPinSignalMapModel::pinSignalMapEdited()";
      break;
  }
}

void ComponentPinSignalMapModel::execCmd(UndoCommand* cmd) {
  if (mUndoStack) {
    mUndoStack->execCmd(cmd);
  } else {
    QScopedPointer<UndoCommand> cmdGuard(cmd);
    cmdGuard->execute();
  }
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb
