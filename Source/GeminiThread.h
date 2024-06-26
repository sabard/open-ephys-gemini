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

#ifndef GEMINITHREAD_H_DEFINED
#define GEMINITHREAD_H_DEFINED

#include <DataThreadHeaders.h>

namespace GeminiThreadNode {

class GeminiThread : public DataThread
{
public:
    /** Default parameters */
    // GUI
    const int DEFAULT_PORT = 51002;
    const float DEFAULT_SAMPLE_RATE = 30000.0f;
    const float DEFAULT_DATA_SCALE = 1.0f;
    const float DEFAULT_DATA_OFFSET = 0.0f;
    // internal
    const int DEFAULT_BUF_SIZE = 12000;
    const int DEFAULT_NUM_CHANNELS = 192;
    const int DEFAULT_NUM_SAMPLES = 30;

    /** Parameter limits */
    const float MIN_DATA_SCALE = 0.0f;
    const float MAX_DATA_SCALE = 9999.9f;
    const float MIN_DATA_OFFSET = 0;
    const float MAX_DATA_OFFSET = 65536;
    const float MIN_PORT = 1023;
    const float MAX_PORT = 65535;
    const float MIN_SAMPLE_RATE = 0;
    const float MAX_SAMPLE_RATE = 50000.0f;

    // socket
    int sockfd = -1;

    // label params
    int port;
    float sample_rate;
    float data_scale;
    float data_offset;

    // internal params
    int num_channels;
    int num_samp;

    // state vars
    bool connected = false;
    bool error_flag;
    int64 total_samples;

    // buffers
    std::vector<std::byte> read_buffer;
    std::vector<float> convbuf;
    Array<int64> sampleNumbers;
    Array<double> timestamps;
    Array<uint64> ttlEventWords;



	/** Constructor */
	GeminiThread(SourceNode* sn);

	/** Destructor */
	~GeminiThread();

    /** Create the DataThread object*/
    static DataThread* createDataThread(SourceNode* sn);

    // ------------------------------------------------------------
    //                  PURE VIRTUAL METHODS
    //     (must be implemented by all DataThreads)
    // ------------------------------------------------------------

	/** Called repeatedly to add any available data to the buffer */
	bool updateBuffer();

    /** Returns true if the data source is connected, false otherwise.*/
    bool foundInputSource();

    /** Initializes data transfer.*/
    bool startAcquisition();

    /** Stops data transfer.*/
    bool stopAcquisition();

    /* Passes the processor's info objects to DataThread, to allow them to be configured */
    void updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
        OwnedArray<EventChannel>* eventChannels,
        OwnedArray<SpikeChannel>* spikeChannels,
        OwnedArray<DataStream>* sourceStreams,
        OwnedArray<DeviceInfo>* devices,
        OwnedArray<ConfigurationObject>* configurationObjects);

    // ------------------------------------------------------------
    //                   VIRTUAL METHODS 
    //       (can optionally be overriden by sub-classes)
    // ------------------------------------------------------------

    /** Called when the chain updates, to add, remove or resize the sourceBuffers' DataBuffers as needed*/
    void resizeBuffers() override;

    /** Create the DataThread custom editor */
    std::unique_ptr<GenericEditor> createEditor(SourceNode* sn) override;

    // ** Allows the DataThread plugin to respond to messages sent by other processors */
    void handleBroadcastMessage(String msg) override;

    // ** Allows the DataThread plugin to handle a config message while acquisition is not active. */
    String handleConfigMessage(String msg) override;


    /** Connects to the socket */
    bool connectSocket();

    /** Disconnects the socket */
    void disconnectSocket();

    /** Returns if any errors were thrown during acquisition, such as invalid headers or unable to read from socket */
    bool errorFlag();
};

}

#endif
