/*
 ------------------------------------------------------------------

 This file is part of the Open Ephys GUI
 Copyright (C) 2022 Open Ephys

 ------------------------------------------------------------------

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef GEMINITHREADEDITOR_H_DEFINED
#define GEMINITHREADEDITOR_H_DEFINED

#include <VisualizerEditorHeaders.h>

#include <EditorHeaders.h>

#include "GeminiThread.h"

namespace GeminiThreadNode {

class GeminiThreadEditor : public GenericEditor,
						   public Label::Listener,
                           public Button::Listener
{

public:
	/** The class constructor, used to initialize any members. */
	GeminiThreadEditor(GenericProcessor* parentNode, GeminiThread *thread);

	/** The class destructor, used to deallocate memory */
	~GeminiThreadEditor() { }

    /** Button listener callback, called by button when pressed. */
    void buttonClicked(Button* button);

    /** Called when label is changed */
    void labelTextChanged(Label* label);

    /** Called by processor graph in beginning of the acqusition, disables editor completly. */
    void startAcquisition();

    /** Called by processor graph at the end of the acqusition, reenables editor completly. */
    void stopAcquisition();

    /** Called when configuration is saved. Adds editors config to xml. */
	void saveCustomParametersToXml(XmlElement* xml) override;

	/** Called when configuration is loaded. Reads editors config from xml. */
    void loadCustomParametersFromXml(XmlElement* xml) override;


private:

    // Button that tries to connect to server
    ScopedPointer<UtilityButton> bindButton;

    // Changes colors and disables UI elements
    void disableInputs();

    // Button that disconnects from server
    ScopedPointer<UtilityButton> closeButton;

    // Changes colors and enables UI elements
    void enableInputs();

 	// Port
    ScopedPointer<Label> portLabel;
    ScopedPointer<Label> portInput;

    // Fs
    ScopedPointer<Label> sampleRateLabel;
    ScopedPointer<Label> sampleRateInput;

    // Scale
    ScopedPointer<Label> scaleLabel;
    ScopedPointer<Label> scaleInput;

    // Offset
    ScopedPointer<Label> offsetLabel;
    ScopedPointer<Label> offsetInput;

    // Parent node
    GeminiThread *node;
};

}

#endif
