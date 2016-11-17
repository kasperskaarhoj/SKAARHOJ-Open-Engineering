/*
Copyright 2016 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Unisketch for Arduino

The Skaarhoj Unisketch library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The Skaarhoj Unisketch library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the Skaarhoj Unisketch library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/

/************************************
 *
 * GRAPHICS
 *
 ************************************/
WebServer webserver("", 80);

void scriptCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  server.httpSuccess("", "Content-Encoding: gzip\r\n");
  if (type == WebServer::GET) {
    server.writeP(JSscriptData, sizeof(JSscriptData));
  }
}

// commands are functions that get called by the webserver framework
// they can read any posted data from client, and they output to server
void webDefaultView(WebServer &server, WebServer::ConnectionType type) {

  if (type == WebServer::POST) {
    uint16_t globalConfigMemPtr = 2;
    int16_t HWCPtr = -1;
    int16_t HWCid = -1;
    int16_t stateIndex = -1;
    int16_t actionIndex = -1;
    int16_t len = -1;
    int16_t cStateIndex = -1;
    uint16_t lastStateLengthIdx = 0xFFFF;
    uint16_t lastHWCLengthIdx = 0xFFFF;
    uint16_t valueDeviceIdx = 0xFFFF;
    uint8_t logic = 0;

    bool repeat;
    bool endConfigParsing = false;
    char fieldName[20], fieldValue[22]; // fieldName is only 15 chars below HWcID 101, but reserving a few extra bytes to be sure
    uint8_t stNamLen = 0;

    memset(globalConfigMem, 0, SK_CONFIG_MEMORY_SIZE);
    // NOTICE: The ORDER of incoming fields is very important for proper processing below!!

    uint8_t formAction = 0;
    uint8_t presetValue = 0;
    uint8_t presetTitleLen = 0;
    do {
      repeat = server.readPOSTparam(fieldName, 20, fieldValue, 22);
      // Serial << F("Field:") << fieldName << F(" Value:") << fieldValue << "\n";

      if (!endConfigParsing) {
        if (!strncmp_P(fieldName, PSTR("sellgic"), 7)) {
          logic = atoi(fieldValue);
        }

        if (!strncmp_P(fieldName, PSTR("selfnc_"), 7)) {
          HWCid = atoi(strtok(fieldName + 7, "_"));
          stateIndex = atoi(strtok(NULL, "_"));
          actionIndex = atoi(strtok(NULL, "_"));
          if (HWCid == HWCPtr + 1) {
            if (lastHWCLengthIdx < SK_CONFIG_MEMORY_SIZE) {
              globalConfigMem[lastHWCLengthIdx] = (globalConfigMemPtr - lastHWCLengthIdx - 1);
            }
            lastHWCLengthIdx = globalConfigMemPtr;
            HWCPtr++;
            cStateIndex = -1;
            if (globalConfigMemPtr < SK_CONFIG_MEMORY_SIZE)
              globalConfigMem[globalConfigMemPtr++] = 255; // Length, to be updated
          }
          if (cStateIndex < stateIndex) { // If state index has changed
            while (cStateIndex < stateIndex) {
              if (lastStateLengthIdx < SK_CONFIG_MEMORY_SIZE) {
                globalConfigMem[lastStateLengthIdx] = (globalConfigMemPtr - lastStateLengthIdx - (cStateIndex == -1 ? 1 : 0) - 1);
              }
              lastStateLengthIdx = globalConfigMemPtr;
              cStateIndex++;
              logic = 0;
              if (globalConfigMemPtr < SK_CONFIG_MEMORY_SIZE)
                globalConfigMem[globalConfigMemPtr++] = 255; // Length, to be updated
            }
          }

          // Lets set the length of the previous action in the state behaviour
          if (len != -1 && valueDeviceIdx < SK_CONFIG_MEMORY_SIZE) {
            globalConfigMem[valueDeviceIdx] = ((globalConfigMem[valueDeviceIdx] & 31) | (((len - 2) & 7) << 5));
          }
          valueDeviceIdx = globalConfigMemPtr;
          if (globalConfigMemPtr < SK_CONFIG_MEMORY_SIZE)
            globalConfigMem[globalConfigMemPtr++] = (atoi(strtok(fieldValue, "_")) & 0xF) | (logic << 4); // Device + logic
          if (globalConfigMemPtr < SK_CONFIG_MEMORY_SIZE)
            globalConfigMem[globalConfigMemPtr++] = atoi(strtok(NULL, "_")); // Function
          len = 2;
        }

        if (!strncmp_P(fieldName, PSTR("selval_"), 7)) { // TODO: Check it's the values matching the previous function...
          if (len < 7 + 2) {
            len++;
            if (globalConfigMemPtr < SK_CONFIG_MEMORY_SIZE)
              globalConfigMem[globalConfigMemPtr++] = atoi(fieldValue);
          }
        }

        if (!strncmp_P(fieldName, PSTR("ctrlIP_"), 7)) {
          endConfigParsing = true;

          if (len != -1 && valueDeviceIdx < SK_CONFIG_MEMORY_SIZE) {
            globalConfigMem[valueDeviceIdx] = ((globalConfigMem[valueDeviceIdx] & 31) | (((len - 2) & 7) << 5));
          }
          if (lastStateLengthIdx < SK_CONFIG_MEMORY_SIZE) {
            globalConfigMem[lastStateLengthIdx] = (globalConfigMemPtr - lastStateLengthIdx - (cStateIndex == -1 ? 1 : 0) - 1);
          }
          if (lastHWCLengthIdx < SK_CONFIG_MEMORY_SIZE) {
            globalConfigMem[lastHWCLengthIdx] = (globalConfigMemPtr - lastHWCLengthIdx - 1);
          }
          globalConfigMem[globalConfigMemPtr++] = 255;           // Termination
          globalConfigMem[0] = highByte(globalConfigMemPtr - 2); // Subtracting 2 because we started out two higher...
          globalConfigMem[1] = lowByte(globalConfigMemPtr - 2);  // Subtracting 2 because we started out two higher...
          // Serial << F("Conf.String=") << globalConfigMemPtr << F("\n");
        }
      }
      // If this happens, we are done with the regular config:
      uint8_t IPidx, g;
      if (!strncmp_P(fieldName, PSTR("ctrlIP_"), 7)) {
        IPidx = atoi(strtok(fieldName + 7, "_"));
        if (IPidx < 4)
          globalConfigMem[globalConfigMemPtr + IPidx] = atoi(fieldValue);
      }
      if (!strncmp_P(fieldName, PSTR("subnet_"), 7)) {
        IPidx = atoi(strtok(fieldName + 7, "_"));
        if (IPidx < 4)
          globalConfigMem[globalConfigMemPtr + 4 + IPidx] = atoi(fieldValue);
      }
      if (!strncmp_P(fieldName, PSTR("devIP_"), 6)) {
        g = atoi(strtok(fieldName + 6, "-"));
        IPidx = atoi(strtok(NULL, "-"));
        if (IPidx < 4 && g < SK_DEVICES)
          globalConfigMem[globalConfigMemPtr + 8 + 1 + g * 5 + IPidx + 1] = atoi(fieldValue);
        globalConfigMem[globalConfigMemPtr + 8] = SK_DEVICES * 5;
      }
      if (!strncmp_P(fieldName, PSTR("devEn_"), 6)) {
        g = atoi(fieldName + 6);
        if (g < SK_DEVICES)
          globalConfigMem[globalConfigMemPtr + 8 + 1 + g * 5] = 1;
      }
      if (!strncmp_P(fieldName, PSTR("stNam_"), 6)) {
        // Serial << F("Field:") << fieldName << F(" Value:") << fieldValue << "\n";
        // Serial << globalConfigMem[globalConfigMemPtr+8+1+SK_DEVICES*5] << "\n";
        // Serial << globalConfigMem[globalConfigMemPtr+8+1+SK_DEVICES*5+1] << "\n";
        memcpy(globalConfigMem + globalConfigMemPtr + 8 + 1 + SK_DEVICES * 5 + 2 + globalConfigMem[globalConfigMemPtr + 8 + 1 + SK_DEVICES * 5], (void *)fieldValue, strlen(fieldValue));
        // Serial << (char *)(globalConfigMem + globalConfigMemPtr+8+1+SK_DEVICES*5+2 + globalConfigMem[globalConfigMemPtr+8+1+SK_DEVICES*5]) << "\n";
        globalConfigMem[globalConfigMemPtr + 8 + 1 + SK_DEVICES * 5] += strlen(fieldValue) + 1;
        globalConfigMem[globalConfigMemPtr + 8 + 1 + SK_DEVICES * 5 + 1]++;
        // Serial << globalConfigMem[globalConfigMemPtr+8+1+SK_DEVICES*5] << "\n";
        // Serial << globalConfigMem[globalConfigMemPtr+8+1+SK_DEVICES*5+1] << "\n";
      }

      // Preset submit button:
      if (!strncmp_P(fieldName, PSTR("prS"), 3)) {
        if (!strncmp_P(fieldValue, PSTR("Load"), 4)) {
          formAction = 1;
        }
        if (!strncmp_P(fieldValue, PSTR("Save"), 4)) {
          formAction = 2;
        }
        if (!strncmp_P(fieldValue, PSTR("Dele"), 4)) {
          formAction = 3;
        }
      }
      if (!strncmp_P(fieldName, PSTR("prN"), 3)) {
        presetValue = atoi(fieldValue);
      }
      if (!strncmp_P(fieldName, PSTR("prTl"), 4)) {
        presetTitleLen = strlen(fieldValue);
        //		  Serial << "::" << (globalConfigMemPtr+8+1+SK_DEVICES*5+2 + globalConfigMem[globalConfigMemPtr+8+1+SK_DEVICES*5]) << "::";
        memcpy(globalConfigMem + globalConfigMemPtr + 8 + 1 + SK_DEVICES * 5 + 2 + globalConfigMem[globalConfigMemPtr + 8 + 1 + SK_DEVICES * 5], (void *)fieldValue, strlen(fieldValue));
      }
    } while (repeat);

    switch (formAction) {
    case 1:
      if (loadPreset(presetValue == 0 ? 255 : presetValue) && presetValue != 0) {
        setCurrentPreset(presetValue);
        Serial << F("Set preset to: ") << presetValue << "\n";
      }
      break;
    case 3:
      savePreset(presetValue, 0); // Deletes
      loadPreset();
      break;
    case 2:
    default:
      savePreset(formAction == 2 ? presetValue : 0, globalConfigMemPtr + 8 + 1 + SK_DEVICES * 5 + 2 + globalConfigMem[globalConfigMemPtr + 8 + 1 + SK_DEVICES * 5] + (presetTitleLen + 1));
      break;
    }
  }

  P(htmlHead) = "<html><head><title>SKAARHOJ Controller</title>"
                "<style id=\"st\"></style>"
                "</head><body><form action=\"/\" method=\"post\">";

  server.httpSuccess();
  server.printP(htmlHead);

  // SVG & Functions
  server << F("<h2>Controller Functions</h2>");
  server << F("<div style=\"text-align:center;\">");
  server.printP(htmlSVG);
  server << F("</div>");
  server << F("<div id=\"func\"></div>");

  // Device Settings:
  server << F("<h2 style=\"margin-top:150;\">Devices Settings</h2><div id=\"ipset\"></div>");

  // States: DONT CHANGE ORDER!
  server << F("<h2 style=\"margin-top:150;\">States</h2><div id=\"state\"></div>");

  // Output config from memory:
  server << F("<script>var HWv=[");
  uint16_t ptr = 2;
  int16_t HWcIndex = -1;
  while (ptr < SK_CONFIG_MEMORY_SIZE && globalConfigMem[ptr] != 255) {
    uint8_t HWcSegmentLength = globalConfigMem[ptr];
    uint16_t HWcSegmentStartPtr = ptr;
    HWcIndex++;
    int16_t stateIndex = -1;
    ptr++;
    server << "[";

    while (ptr < HWcSegmentStartPtr + HWcSegmentLength + 1) {
      uint8_t stateSegmentLength = globalConfigMem[ptr];
      uint16_t stateSegmentStartPtr = ptr;
      stateIndex++;
      int16_t actionIndex = -1;
      ptr++;
      server << "[";

      while (ptr < stateSegmentStartPtr + stateSegmentLength + 1) {
        uint8_t actionLength = (globalConfigMem[ptr] & (128 + 64 + 32)) >> 5; // Upper 3 bits
        actionIndex++;
        server << "[";

        uint8_t deviceIndex = globalConfigMem[ptr] & 31; // Bit 0-3 + bit 4 (action glue)
        server << deviceIndex << ",";
        ptr++;
        uint8_t deviceFunctionIndex = globalConfigMem[ptr];
        server << deviceFunctionIndex << ",";
        ptr++;

        for (uint8_t a = 0; a < actionLength; a++) {
          server << globalConfigMem[ptr] << ",";
          ptr++;
        }
        server << "],";
      }
      server << "],";
    }
    server << "],";
  }
  server << F("];var online=0;var SK_MAXACTIONS=") << SK_MAXACTIONS << ";";

  server << F("var ctrlCfg=[[");
  ptr = getConfigMemIPIndex();
  for (uint8_t a = 0; a < 8; a++) {
    server << globalConfigMem[ptr + a] << (a == 3 ? "],[" : (a < 7 ? "," : ""));
  }

  server << F("]];var HWdis=[");
  for (uint8_t a = 0; a < SK_HWCCOUNT; a++) {
    server << HWdis[a] << (a < SK_HWCCOUNT - 1 ? "," : "");
  }
  server << F("];var MODdis=[");
  for (uint8_t a = 0; a < SK_MODCOUNT; a++) {
    server << MODdis[a] << ",";
  }

  server << F("0];var devCfg=[[");
  for (uint8_t a = 0; a < (SK_DEVICES>0?SK_DEVICES:1); a++) {
    ptr = getConfigMemDevIndex(a);
    for (uint8_t b = 0; b < 5; b++) {
      server << globalConfigMem[ptr + b] << (b < 4 ? "," : (a < SK_DEVICES - 1 ? "],[" : ""));
    }
  }
  server << F("]];var states=[[\"");
  ptr = getConfigMemStateIndex();
  uint8_t nStates = globalConfigMem[ptr + 1];
  // Serial << nStates << "\n";
  // Serial << ptr << "\n";
  // Serial << globalConfigMem[ptr] << "\n";
  /*
                for(uint8_t a=0; a<20; a++)	{
                    Serial << ":" << globalConfigMem[ptr+a] << "\n";
                }
        */

  for (uint8_t a = 0; a < nStates; a++) {
    //	Serial << F("'") << ((char *)&globalConfigMem[ptr + 2]) << F("'");
    server << (char *)&globalConfigMem[ptr + 2] << (a + 1 < nStates ? F("\"],[\"") : F(""));
    ptr += strlen((char *)&globalConfigMem[ptr + 2]) + 1;
  }
  server << F("\"]];</script>");

  server << F("<script src=\"script.js\"></script>");
  server << F("<input type=\"submit\" value=\"Save\" class=\"sBut\">");

  server << F("<h2>Presets</h2><input type=\"submit\" name=\"prS\" value=\"Load\">");
  server << F("<input type=\"submit\" name=\"prS\" value=\"Save To\">");
  server << F("<input type=\"submit\" name=\"prS\" value=\"Delete\">");
  server << F("<select name=\"prN\">");
  server << F("<option value=\"0\">(Default)</option>");
  for (uint8_t a = 1; a <= getNumberOfPresets(); a++) {
    getPresetName(_strCache, a);
    server << F("<option value=\"") << a << F("\"") << (a == EEPROM.read(EEPROM_PRESET_START + 1) ? F(" selected") : F("")) << F(">") << _strCache << F("</option>");
  }
  server << F("<option value=\"") << (getNumberOfPresets() + 1) << F("\">(New)</option>");
  server << F("</select>");
  getPresetName(_strCache, EEPROM.read(EEPROM_PRESET_START + 1));
  server << F("<input name=\"prTl\" value=\"") << _strCache << F("\">");

  server << F(" - Memory used: ") << ((float)getPresetStoreLength() / (4096 - EEPROM_PRESET_START) * 100) << F("% <hr/><br/>");

  server << F("</form><span class=\"fp\">MAC Address: ");
  for (uint8_t a = 0; a < 6; a++) {
    server << _HEXPADL(mac[a], 2, "0") << (a != 5 ? F(":") : F("\n")) << F("");
  }

  server << F("<br/>Copyright 2016 SKAARHOJ K/S - All code is Open Source GNU/GPL, see http://skaarhoj.com/about/licenses/. Do not remove this message.</span></body></html>");
}
void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) { webDefaultView(server, type); }
