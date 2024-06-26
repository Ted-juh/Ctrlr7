/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "../UI/jucer_JucerDocumentEditor.h"

//==============================================================================
template <class ComponentType>
class ComponentUndoableAction    : public UndoableAction
{
public:
    ComponentUndoableAction (ComponentType* const comp,
                             ComponentLayout& layout_)
        : layout (layout_),
          componentIndex (layout_.indexOfComponent (comp))
    {
        jassert (comp != nullptr);
        jassert (componentIndex >= 0);
    }

    ComponentType* getComponent() const
    {
        ComponentType* const c = dynamic_cast<ComponentType*> (layout.getComponent (componentIndex));
        jassert (c != nullptr);
        return c;
    }

    int getSizeInUnits() override    { return 2; }

protected:
    ComponentLayout& layout;
    const int componentIndex;

    void changed() const
    {
        jassert (layout.getDocument() != nullptr);
        layout.getDocument()->changed();
    }

    void showCorrectTab() const
    {
        if (JucerDocumentEditor* const ed = JucerDocumentEditor::getActiveDocumentHolder())
            ed->showLayout();

        if (layout.getSelectedSet().getNumSelected() == 0)
            if (ComponentType* const c = dynamic_cast<ComponentType*> (layout.getComponent (componentIndex)))
                layout.getSelectedSet().selectOnly (getComponent());
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentUndoableAction)
};
