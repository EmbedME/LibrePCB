/*
 * EDA4U - Professional EDA for everyone!
 * Copyright (C) 2013 Urban Bruhin
 * http://eda4u.ubruhin.ch/
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

#ifndef PROJECT_BES_BASE_H
#define PROJECT_BES_BASE_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/

#include <QtCore>
#include "boardeditorevent.h"
#include "../boardeditor.h"
#include "ui_boardeditor.h"
#include <eda4ucommon/units/all_length_units.h>
#include <eda4ucommon/graphics/graphicsview.h>

/*****************************************************************************************
 *  Forward Declarations
 ****************************************************************************************/

namespace project {
class Project;
class Circuit;
}

/*****************************************************************************************
 *  Class BES_Base
 ****************************************************************************************/

namespace project {

/**
 * @brief The BES_Base (BoardEditorState Base) class
 */
class BES_Base : public QObject
{
        Q_OBJECT

    public:

        /// process() return values
        enum ProcRetVal {
            ForceStayInState,   ///< event handled, stay in the current state
            ForceLeaveState,    ///< event handled, leave the current state
            PassToParentState,  ///< event unhandled, pass it to the parent
        };

        // Constructors / Destructor
        explicit BES_Base(BoardEditor& editor, Ui::BoardEditor& editorUi,
                          GraphicsView& editorGraphicsView);
        virtual ~BES_Base();

        // General Methods
        virtual ProcRetVal process(BEE_Base* event) noexcept = 0;
        virtual bool entry(BEE_Base* event) noexcept {Q_UNUSED(event); return true;}
        virtual bool exit(BEE_Base* event) noexcept {Q_UNUSED(event); return true;}

    protected:

        // General Attributes which are needed by some state objects
        Project& mProject;
        Circuit& mCircuit;
        BoardEditor& mEditor;
        Ui::BoardEditor& mEditorUi; ///< allows access to BoardEditor UI
        GraphicsView& mEditorGraphicsView; ///< allows access to the board editor graphics view
};

} // namespace project

#endif // PROJECT_BES_BASE_H