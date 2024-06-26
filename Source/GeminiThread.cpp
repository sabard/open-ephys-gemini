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

#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#include "GeminiThread.h"
#include "GeminiThreadEditor.h"

using namespace GeminiThreadNode;

DataThread* GeminiThread::createDataThread(SourceNode *sn)
{
    return new GeminiThread(sn);
}

GeminiThread::GeminiThread(SourceNode* sn) : DataThread(sn)
{
    port = DEFAULT_PORT;
    sample_rate = DEFAULT_SAMPLE_RATE;
    data_scale = DEFAULT_DATA_SCALE;
    data_offset = DEFAULT_DATA_OFFSET;

    total_samples = 0;
    eventState = 0;

    num_channels = DEFAULT_NUM_CHANNELS;
    num_samp = DEFAULT_NUM_SAMPLES;

    sourceBuffers.add(new DataBuffer(num_channels, DEFAULT_BUF_SIZE));
}

std::unique_ptr<GenericEditor> GeminiThread::createEditor(SourceNode* sn)
{

    std::unique_ptr<GeminiThreadEditor> editor = std::make_unique<GeminiThreadEditor>(sn, this);

    return editor;

}

GeminiThread::~GeminiThread()
{
}

bool GeminiThread::connectSocket()
{
    disconnectSocket();

    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        LOGC("GeminiThread failed to connect");
        CoreServices::sendStatusMessage("GeminiThread: Socket could not connect.");
        connected = false;
        return false;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(DEFAULT_PORT);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
            sizeof(servaddr)) < 0 )
    {
        LOGC("GeminiThread failed to connect");
        CoreServices::sendStatusMessage("GeminiThread: Socket could not connect.");
        connected = false;
        return false;
    }

    LOGC("GeminiThread connected.");
    CoreServices::sendStatusMessage("GeminiThread: Socket connected and ready to receive data.");

    connected = true;
    return true;
}


void GeminiThread::disconnectSocket()
{
    if (sockfd != -1)
    {
        LOGD("Disconnecting socket.");

        close(sockfd);

        connected = false;

        CoreServices::sendStatusMessage("GeminiThread: Socket disconnected.");
    }
}

bool GeminiThread::updateBuffer()
{
    int n;
    socklen_t len;
    struct sockaddr_in addr;
    char buffer[DEFAULT_BUF_SIZE];

    memset(&addr, 0, sizeof(addr));
    len = sizeof(addr);  // len is value/result

    printf("Gemini buffer update\n");

    n  = recvfrom(sockfd, (char *)buffer, DEFAULT_BUF_SIZE,
            0, ( struct sockaddr *) &addr,
            &len);

    printf("%d\n", n);

    for (int i = 0; i < num_samp; i++) {
        sampleNumbers.set(i, total_samples++);
        ttlEventWords.set(i, eventState);
    }

    sourceBuffers[0]->addToBuffer(convbuf.data(),
        sampleNumbers.getRawDataPointer(),
        timestamps.getRawDataPointer(),
        ttlEventWords.getRawDataPointer(),
        num_samp,
        1
    );

    return true;
}

bool GeminiThread::foundInputSource()
{
    return connected;
}

bool GeminiThread::startAcquisition()
{
    resizeBuffers();

    total_samples = 0;
    eventState = 0;

    error_flag = false;

    startThread();

    return true;
}

bool GeminiThread::stopAcquisition()
{
    if (isThreadRunning())
    {
        signalThreadShouldExit();
    }

    waitForThreadToExit(500);

    sourceBuffers[0]->clear();
    return true;
}


void GeminiThread::updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
    OwnedArray<EventChannel>* eventChannels,
    OwnedArray<SpikeChannel>* spikeChannels,
    OwnedArray<DataStream>* sourceStreams,
    OwnedArray<DeviceInfo>* devices,
    OwnedArray<ConfigurationObject>* configurationObjects)
{


}

bool GeminiThread::errorFlag()
{
    return error_flag;
}


void GeminiThread::resizeBuffers()
{
    sourceBuffers[0]->resize(num_channels, DEFAULT_BUF_SIZE);
    // read_buffer.resize(num_channels * num_samp * element_size + HEADER_SIZE);
    convbuf.resize(num_channels * num_samp);
    sampleNumbers.resize(num_samp);
    timestamps.clear();
    timestamps.insertMultiple(0, 0.0, num_samp);
    ttlEventWords.resize(num_samp);
}

void GeminiThread::handleBroadcastMessage(String msg)
{


}

String GeminiThread::handleConfigMessage(String msg)
{

    return "";
}
