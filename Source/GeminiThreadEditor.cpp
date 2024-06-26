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

#include "GeminiThreadEditor.h"

using namespace GeminiThreadNode;

GeminiThreadEditor::GeminiThreadEditor(GenericProcessor* parentNode, GeminiThread *thread) : GenericEditor(parentNode)
{
	node = thread;

	desiredWidth = 180;

	// Add bind button
    bindButton = new UtilityButton("BIND", Font("Small Text", 12, Font::bold));
    bindButton->setRadius(3.0f);
    bindButton->setBounds(15, 35, 70, 20);
    bindButton->addListener(this);
    addAndMakeVisible(bindButton);

    // Add close button
    closeButton = new UtilityButton("CLOSE", Font("Small Text", 12, Font::bold));
    closeButton->setRadius(3.0f);
    closeButton->setBounds(100, 35, 70, 20);
    closeButton->addListener(this);
    closeButton->setEnabled(false);
    closeButton->setAlpha(0.2f);
    addAndMakeVisible(closeButton);

    // Port
    portLabel = new Label("Port", "Port");
    portLabel->setFont(Font("Small Text", 10, Font::plain));
    portLabel->setBounds(10, 63, 70, 8);
    portLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(portLabel);

    portInput = new Label("Port", String(node->port));
    portInput->setFont(Font("Small Text", 10, Font::plain));
    portInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    portInput->setEditable(true);
    portInput->addListener(this);
    portInput->setBounds(15, 73, 70, 15);
    addAndMakeVisible(portInput);

    // Frequency
    sampleRateLabel = new Label("FREQ (HZ)", "Freq (Hz)");
    sampleRateLabel->setFont(Font("Small Text", 10, Font::plain));
    sampleRateLabel->setBounds(10, 98, 85, 8);
    sampleRateLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(sampleRateLabel);

    sampleRateInput = new Label("Fs (Hz)", String((int) node->sample_rate));
    sampleRateInput->setFont(Font("Small Text", 10, Font::plain));
    sampleRateInput->setBounds(15, 108, 70, 15);
    sampleRateInput->setEditable(true);
    sampleRateInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    sampleRateInput->addListener(this);
    addAndMakeVisible(sampleRateInput);

    // Scale
    scaleLabel = new Label("Scale", "Scale");
    scaleLabel->setFont(Font("Small Text", 10, Font::plain));
    scaleLabel->setBounds(95, 63, 85, 8);
    scaleLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(scaleLabel);

    scaleInput = new Label("Scale", String(node->data_scale));
    scaleInput->setFont(Font("Small Text", 10, Font::plain));
    scaleInput->setBounds(100, 73, 70, 15);
    scaleInput->setEditable(true);
    scaleInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    scaleInput->addListener(this);
    addAndMakeVisible(scaleInput);

    // Offset
    offsetLabel = new Label("Offset", "Offset");
    offsetLabel->setFont(Font("Small Text", 10, Font::plain));
    offsetLabel->setBounds(95, 98, 85, 8);
    offsetLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(offsetLabel);

    offsetInput = new Label("Offset", String(node->data_offset));
    offsetInput->setFont(Font("Small Text", 10, Font::plain));
    offsetInput->setBounds(100, 108, 70, 15);
    offsetInput->setEditable(true);
    offsetInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    offsetInput->addListener(this);
    addAndMakeVisible(offsetInput);

}

void GeminiThreadEditor::enableInputs()
{
    bindButton->setEnabled(true);
    bindButton->setAlpha(1.0f);
    closeButton->setEnabled(false);
    closeButton->setAlpha(0.2f);

    portInput->setEnabled(true);
    sampleRateInput->setEnabled(true);
    scaleInput->setEnabled(true);
    offsetInput->setEnabled(true);
}

void GeminiThreadEditor::disableInputs()
{
    bindButton->setEnabled(false);
    bindButton->setAlpha(0.2f);
    closeButton->setEnabled(true);
    closeButton->setAlpha(1.0f);

    portInput->setEnabled(false);
    sampleRateInput->setEnabled(false);
    scaleInput->setEnabled(false);
    offsetInput->setEnabled(false);
}

void GeminiThreadEditor::buttonClicked(Button* button)
{
    if (button == bindButton && !acquisitionIsActive)
    {
        node->port = portInput->getText().getIntValue();

        if (node->connectSocket())
        {
            disableInputs();
        }

        CoreServices::updateSignalChain(this);
    }
    else if (button == closeButton && !acquisitionIsActive)
    {
        node->disconnectSocket();
        enableInputs();
    }
}

void GeminiThreadEditor::labelTextChanged(Label* label)
{
	if (label == sampleRateInput)
    {
        float sampleRate = sampleRateInput->getText().getFloatValue();

        if (sampleRate > node->MIN_SAMPLE_RATE && sampleRate < node->MAX_SAMPLE_RATE)
        {
            node->sample_rate = sampleRate;
            CoreServices::updateSignalChain(this);
        }
        else {
            sampleRateInput->setText(String(node->sample_rate), dontSendNotification);
        }
    }
    else if (label == portInput)
    {
        int port = portInput->getText().getIntValue();

        if (port > node->MIN_PORT && port < node->MAX_PORT)
        {
            node->port = port;
        }
        else {
            portInput->setText(String(node->port), dontSendNotification);
        }
    }
    else if (label == scaleInput)
    {
        float scale = scaleInput->getText().getFloatValue();

        if (scale > node->MIN_DATA_SCALE && scale < node->MAX_DATA_SCALE)
        {
            node->data_scale = scale;
            CoreServices::updateSignalChain(this);
        }
        else {
            scaleInput->setText(String(node->data_scale), dontSendNotification);
        }
    }
    else if (label == offsetInput)
    {
        int offset = offsetInput->getText().getIntValue();

        if (offset >= node->MIN_DATA_OFFSET && offset < node->MAX_DATA_OFFSET)
        {
            node->data_offset = offset;
        }
        else {
            offsetInput->setText(String(node->data_offset), dontSendNotification);
        }
    }
}


void GeminiThreadEditor::startAcquisition()
{
    closeButton->setEnabled(false);
    closeButton->setAlpha(0.2f);
}

void GeminiThreadEditor::stopAcquisition()
{
    if (node->errorFlag())
    {
        node->disconnectSocket();
        enableInputs();
    }
    else
    {
        closeButton->setEnabled(true);
        closeButton->setAlpha(1.0f);
    }
}

void GeminiThreadEditor::saveCustomParametersToXml(XmlElement* xmlNode)
{
    XmlElement* parameters = xmlNode->createNewChildElement("PARAMETERS");

    parameters->setAttribute("port", portInput->getText());
    parameters->setAttribute("fs", sampleRateInput->getText());
    parameters->setAttribute("scale", scaleInput->getText());
    parameters->setAttribute("offset", offsetInput->getText());
}

void GeminiThreadEditor::loadCustomParametersFromXml(XmlElement* xmlNode)
{
    forEachXmlChildElement(*xmlNode, subNode)
    {
        if (subNode->hasTagName("PARAMETERS"))
        {
            portInput->setText(subNode->getStringAttribute("port", ""), dontSendNotification);
            node->port = subNode->getIntAttribute("port", node->DEFAULT_PORT);

            sampleRateInput->setText(subNode->getStringAttribute("fs", ""), dontSendNotification);
            node->sample_rate = subNode->getDoubleAttribute("fs", node->DEFAULT_SAMPLE_RATE);

            scaleInput->setText(subNode->getStringAttribute("scale", ""), dontSendNotification);
            node->data_scale = subNode->getDoubleAttribute("scale", node->DEFAULT_DATA_SCALE);

            offsetInput->setText(subNode->getStringAttribute("offset", ""), dontSendNotification);
            node->data_offset = subNode->getIntAttribute("offset", node->DEFAULT_DATA_OFFSET);
        }
    }
}
