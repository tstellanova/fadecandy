/*
 * Abstract base class for USB-attached devices.
 * 
 * Copyright (c) 2013 Micah Elizabeth Scott
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "usbdevice.h"
#include <iostream>


USBDevice::USBDevice(libusb_device *device, bool verbose)
    : mDevice(libusb_ref_device(device)),
      mHandle(0),
      mVerbose(verbose)
{}

USBDevice::~USBDevice()
{
    if (mHandle) {
        libusb_close(mHandle);
    }
    if (mDevice) {
        libusb_unref_device(mDevice);
    }
}

bool USBDevice::probeAfterOpening()
{
    // By default, any device is supported by the time we get to opening it.
    return true;
}

void USBDevice::writeColorCorrection(const Value &color)
{
    // Optional. By default, ignore color correction messages.
}

bool USBDevice::matchConfigurationWithTypeAndSerial(const Value &config, const char *type, const char *serial)
{
    if (!config.IsObject()) {
        return false;
    }

    const Value &vtype = config["type"];
    const Value &vserial = config["serial"];

    if (!vtype.IsString() || strcmp(vtype.GetString(), type)) {
        // Wrong type
        return false;
    }

    if (!vserial.IsNull()) {
        // Not a wildcard serial number?
        // If a serial was not specified, it matches any device.

        if (!vserial.IsString()) {
            // Non-string serial number. Bad form.
            return false;
        }

        if (strcmp(vserial.GetString(), serial)) {
            // Not a match
            return false;
        }
    }

    return true;
}


const USBDevice::Value *USBDevice::findConfigMap(const Value &config)
{
    const Value &vmap = config["map"];

    if (vmap.IsArray()) {
        // The map is optional, but if it exists it needs to be an array.
        return &vmap;
    }
    
    if (!vmap.IsNull() && mVerbose) {
        std::clog << "Device configuration 'map' must be an array.\n";
    }

    return 0;
}
