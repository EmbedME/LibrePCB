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
#include <librepcb/library/sym/symbol.h>

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
    mSymbolVariant(nullptr),
    mSymbols(nullptr),
    mUndoStack(nullptr),
    mOnEditedSlot(*this, &ComponentPinSignalMapModel::pinSignalMapEdited) {
  foreach (const CmpSigPinDisplayType& type,
           CmpSigPinDisplayType::getAllTypes()) {
    mDisplayTypeComboBoxItems.append(
        qMakePair(type.getNameTr(), QVariant(type.toString())));
  }
}

ComponentPinSignalMapModel::~ComponentPinSignalMapModel() noexcept {
  setSymbolVariant(nullptr);
  setSymbols(nullptr);
  setUndoStack(nullptr);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentPinSignalMapModel::setSymbolVariant(
    ComponentSymbolVariant* variant) noexcept {
  emit beginResetModel();

  if (mSymbolVariant) {
    // mSymbolVariant->onEdited.detach(mOnEditedSlot);
  }

  mSymbolVariant = variant;

  if (mSymbolVariant) {
    // mSymbolVariant->onEdited.attach(mOnEditedSlot);
  }

  emit endResetModel();
}

void ComponentPinSignalMapModel::setSymbols(
    const QHash<Uuid, std::shared_ptr<const Symbol>>* symbols) noexcept {
  mSymbols = symbols;
  emit dataChanged(index(0, COLUMN_SYMBOL),
                   index(rowCount() - 1, COLUMN_SYMBOL));
}

void ComponentPinSignalMapModel::setSignalList(
    const ComponentSignalList& list) noexcept {
  mSignals = list;
  updateSignalComboBoxItems();
  emit dataChanged(index(0, COLUMN_SIGNAL), index(rowCount() - 1, COLUMN_PIN));
}

void ComponentPinSignalMapModel::setUndoStack(UndoStack* stack) noexcept {
  mUndoStack = stack;
}

/*******************************************************************************
 *  Inherited from QAbstractItemModel
 ******************************************************************************/

int ComponentPinSignalMapModel::rowCount(const QModelIndex& parent) const {
  int count = 0;
  if (!parent.isValid() && mSymbolVariant) {
    for (const auto& item : mSymbolVariant->getSymbolItems()) {
      count += item.getPinSignalMap().count();
    }
  }
  return count;
}

int ComponentPinSignalMapModel::columnCount(const QModelIndex& parent) const {
  if (!parent.isValid()) {
    return _COLUMN_COUNT;
  }
  return 0;
}

QVariant ComponentPinSignalMapModel::data(const QModelIndex& index,
                                          int                role) const {
  if (!index.isValid() || !mSymbolVariant) {
    return QVariant();
  }

  int                                               count = 0;
  std::shared_ptr<const ComponentSymbolVariantItem> symbolItem;
  std::shared_ptr<const ComponentPinSignalMapItem>  mapItem;
  for (int i = 0; i < mSymbolVariant->getSymbolItems().count(); ++i) {
    symbolItem = mSymbolVariant->getSymbolItems().value(i);
    if (index.row() < (count + symbolItem->getPinSignalMap().count())) {
      mapItem = symbolItem->getPinSignalMap().value(index.row() - count);
      break;
    } else {
      count += symbolItem->getPinSignalMap().count();
    }
  }
  if (!symbolItem || !mapItem) {
    return QVariant();
  }

  switch (index.column()) {
    case COLUMN_SYMBOL: {
      Uuid                          symbolUuid = symbolItem->getSymbolUuid();
      std::shared_ptr<const Symbol> symbol     = mSymbols->value(symbolUuid);
      switch (role) {
        case Qt::DisplayRole:
          return symbol ? *symbol->getNames().getDefaultValue()
                        : symbolUuid.toStr();
        case Qt::ToolTipRole:
          return symbolUuid.toStr();
        default:
          return QVariant();
      }
    }
    case COLUMN_PIN: {
      Uuid                             symbolUuid = symbolItem->getSymbolUuid();
      std::shared_ptr<const Symbol>    symbol     = mSymbols->value(symbolUuid);
      Uuid                             pinUuid    = mapItem->getPinUuid();
      std::shared_ptr<const SymbolPin> pin =
          symbol ? symbol->getPins().find(pinUuid) : nullptr;
      switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
          return QString("%1::%2")
              .arg(*symbolItem->getSuffix())
              .arg(pin ? *pin->getName() : pinUuid.toStr());
        default:
          return QVariant();
      }
    }
    case COLUMN_SIGNAL: {
      tl::optional<Uuid>                     uuid = mapItem->getSignalUuid();
      std::shared_ptr<const ComponentSignal> sig =
          uuid ? mSignals.find(*uuid) : nullptr;
      switch (role) {
        case Qt::DisplayRole:
          return sig ? *sig->getName()
                     : (uuid ? uuid->toStr() : tr("(unconnected)"));
        case Qt::EditRole:
        case Qt::ToolTipRole:
          return uuid ? uuid->toStr() : QVariant();  // NULL means unconnected!
        case Qt::UserRole:
          return QVariant::fromValue(mSignalComboBoxItems);
        default:
          return QVariant();
      }
    }
    case COLUMN_DISPLAY: {
      switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
          return mapItem->getDisplayType().getNameTr();
        case Qt::EditRole:
          return mapItem->getDisplayType().toString();
        case Qt::UserRole:
          return QVariant::fromValue(mDisplayTypeComboBoxItems);
        default:
          return QVariant();
      }
    }
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
  if (!mSymbolVariant) {
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

void ComponentPinSignalMapModel::updateSignalComboBoxItems() noexcept {
  mSignalComboBoxItems.clear();
  for (const ComponentSignal& sig : mSignals) {
    mSignalComboBoxItems.append(
        qMakePair(*sig.getName(), QVariant(sig.getUuid().toStr())));
  }
  QCollator collator;
  collator.setCaseSensitivity(Qt::CaseInsensitive);
  collator.setIgnorePunctuation(false);
  collator.setNumericMode(true);
  qSort(mSignalComboBoxItems.begin(), mSignalComboBoxItems.end(),
        [&collator](const QPair<QString, QVariant>& lhs,
                    const QPair<QString, QVariant>& rhs) {
          return collator(lhs.first, rhs.first);
        });
  mSignalComboBoxItems.insert(
      0, qMakePair(QString("(%1)").arg(tr("unconnected")), QVariant()));
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb
