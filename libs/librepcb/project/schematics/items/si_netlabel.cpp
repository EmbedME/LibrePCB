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
#include "si_netlabel.h"

#include "../../circuit/circuit.h"
#include "../../circuit/netsignal.h"
#include "../../project.h"
#include "../schematic.h"
#include "si_netsegment.h"

#include <librepcb/common/graphics/graphicsscene.h>
#include <librepcb/common/scopeguard.h>

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace project {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

SI_NetLabel::SI_NetLabel(SI_NetSegment& segment, const SExpression& node)
  : SI_Base(segment.getSchematic()),
    mNetSegment(segment),
    mUuid(node.getChildByIndex(0).getValue<Uuid>()),
    mPosition(node.getChildByPath("position")),
    mRotation(node.getValueByPath<Angle>("rotation")) {
  init();
}

SI_NetLabel::SI_NetLabel(SI_NetSegment& segment, const Point& position,
                         const Angle& rotation)
  : SI_Base(segment.getSchematic()),
    mNetSegment(segment),
    mUuid(Uuid::createRandom()),
    mPosition(position),
    mRotation(rotation) {
  init();
}

void SI_NetLabel::init() {
  // create the graphics item
  mGraphicsItem.reset(new SGI_NetLabel(*this));
  mGraphicsItem->setPos(mPosition.toPxQPointF());
  mGraphicsItem->setRotation(-mRotation.toDeg());
}

SI_NetLabel::~SI_NetLabel() noexcept {
  mGraphicsItem.reset();
}

/*******************************************************************************
 *  Getters
 ******************************************************************************/

NetSignal& SI_NetLabel::getNetSignalOfNetSegment() const noexcept {
  return mNetSegment.getNetSignal();
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void SI_NetLabel::setPosition(const Point& position) noexcept {
  if (position != mPosition) {
    mPosition = position;
    mGraphicsItem->setPos(mPosition.toPxQPointF());
    updateAnchor();
  }
}

void SI_NetLabel::setRotation(const Angle& rotation) noexcept {
  if (rotation != mRotation) {
    mRotation = rotation;
    mGraphicsItem->setRotation(-mRotation.toDeg());
    mGraphicsItem->updateCacheAndRepaint();
    updateAnchor();
  }
}

/*******************************************************************************
 *  General Methods
 ******************************************************************************/

void SI_NetLabel::updateAnchor() noexcept {
  mGraphicsItem->setAnchor(mNetSegment.calcNearestPoint(mPosition));
}

void SI_NetLabel::addToSchematic() {
  if (isAddedToSchematic()) {
    throw LogicError(__FILE__, __LINE__);
  }
  mNameChangedConnection =
      connect(&getNetSignalOfNetSegment(), &NetSignal::nameChanged,
              [this]() { mGraphicsItem->updateCacheAndRepaint(); });
  mHighlightChangedConnection =
      connect(&getNetSignalOfNetSegment(), &NetSignal::highlightedChanged,
              [this]() { mGraphicsItem->update(); });
  SI_Base::addToSchematic(mGraphicsItem.data());
  mGraphicsItem->updateCacheAndRepaint();
  updateAnchor();
}

void SI_NetLabel::removeFromSchematic() {
  if (!isAddedToSchematic()) {
    throw LogicError(__FILE__, __LINE__);
  }
  disconnect(mNameChangedConnection);
  disconnect(mHighlightChangedConnection);
  SI_Base::removeFromSchematic(mGraphicsItem.data());
}

void SI_NetLabel::serialize(SExpression& root) const {
  root.appendChild(mUuid);
  root.appendChild(mPosition.serializeToDomElement("position"), true);
  root.appendChild("rotation", mRotation, false);
}

/*******************************************************************************
 *  Inherited from SI_Base
 ******************************************************************************/

QPainterPath SI_NetLabel::getGrabAreaScenePx() const noexcept {
  return mGraphicsItem->sceneTransform().map(mGraphicsItem->shape());
}

void SI_NetLabel::setSelected(bool selected) noexcept {
  SI_Base::setSelected(selected);
  mGraphicsItem->update();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace project
}  // namespace librepcb
