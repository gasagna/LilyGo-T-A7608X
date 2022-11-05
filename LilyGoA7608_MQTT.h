#ifndef LILYGOA7608_MQTT_H_
#define LILYGOA7608_MQTT_H_

enum MQTT_CONNECT_ERROR_CODE {
    MQTT_CONNECT_SSLVERSION_ERROR = -1,
    MQTT_CONNECT_AUTHMODE_ERROR   = -2,
    MQTT_CONNECT_CACERT_ERROR     = -3,
    MQTT_CONNECT_START_ERROR      = -4,
    MQTT_CONNECT_CLIENTACQ_ERROR  = -5,
    MQTT_CONNECT_SSLCFG_ERROR     = -6,
    MQTT_CONNECT_CONNECT_ERROR    = -7
};

template <typename MODEM>
class NativeMQTTClient {
  private:
    MODEM& _modem;

  public:
    // Initialise from modem
    NativeMQTTClient(MODEM& modem) 
        : _modem(modem) {}

    // Set server certificate. Return true if successful.
    bool setCACert(const String& cert) {
        // delete default server certificate
        _modem.sendAT("+CCERTDELE=\"__server_cert__.pem\"");
        _modem.waitResponse();

        // download certificate to the module and wait for `>`
        _modem.sendAT("+CCERTDOWN=\"__server_cert__.pem\",", cert.length());
        if (_modem.waitResponse(">") != 1) {
            return false;
        }

        // write certificate and check we received an OK after the write
        _modem.stream.write(cert.c_str(), cert.length());
        _modem.stream.flush();
        if (_modem.waitResponse("OK") != 1) {
            return false;
        }

        return true;
    }

    // Connect to MQTT server
    int16_t connect(const char* clientID,  const char* server, int port, const char* username, const char* password, int keepalive) {
        _modem.sendAT("+CSSLCFG=\"sslversion\",0,3");
        if (_modem.waitResponse() != 1)
            return MQTT_CONNECT_SSLVERSION_ERROR;

        _modem.sendAT("+CSSLCFG=\"authmode\",0,1");
        if (_modem.waitResponse() != 1)
            return MQTT_CONNECT_AUTHMODE_ERROR;

        _modem.sendAT("+CSSLCFG=\"cacert\",0,\"__server_cert__.pem\"");
        if (_modem.waitResponse() != 1)
            return MQTT_CONNECT_CACERT_ERROR;

        _modem.sendAT("+CMQTTSTART");
        if (_modem.waitResponse("\r\n+CMQTTSTART: ") != 1) 
            return MQTT_CONNECT_START_ERROR;
        if (_modem.stream.parseInt() == 0) {
            // consume output if all OK
            _modem.waitResponse();
        } else {
            return MQTT_CONNECT_START_ERROR;
        }

        _modem.sendAT("+CMQTTACCQ=0,\"", clientID, "\",1");
        if (_modem.waitResponse() != 1)
            return MQTT_CONNECT_CLIENTACQ_ERROR;

        _modem.sendAT("+CMQTTSSLCFG=0,0");
        if (_modem.waitResponse() != 1)
            return MQTT_CONNECT_SSLCFG_ERROR;

        _modem.sendAT("+CMQTTCONNECT=0,\"tcp://", server, ":", port, "\",", keepalive, ",1,\"", username, "\",\"", password, "\"");
        _modem.waitResponse("+CMQTTCONNECT: 0,");
        // if (_modem.waitResponse("+CMQTTCONNECT: 0,") != 1) 
            // return MQTT_CONNECT_CONNECT_ERROR;
        
        // read int then consume output and return
        int retcode = _modem.stream.parseInt();
        _modem.waitResponse();
        return retcode;
    }


    // disconnect MQTT
    bool disconnect() {

        _modem.sendAT("+CMQTTDISC?");
        _modem.waitResponse(20000);

        // disconnect
        _modem.sendAT("+CMQTTDISC=0,120");
        if (_modem.waitResponse(120000, "+CMQTTDISC: 0,") != 1)
            return false;

        if (_modem.stream.parseInt() != 0)
            return false;

        _modem.sendAT("+CMQTTREL=0");
        if (_modem.waitResponse() != 1)
            return false;

        // example
        // AT+CMQTTSTOP
        // OK 
        //
        // +CMQTTSTOP: 0
        _modem.sendAT("+CMQTTSTOP");
        if (_modem.waitResponse("\r\n+CMQTTSTOP: ") != 1)
            return false;

        if (_modem.stream.parseInt() != 0) 
            return false;

        return true;
    }
};


// void publish(const String& topic, const String& payload) {
//     int8_t out;

//     _modem.sendAT("+CMQTTTOPIC=0,", topic.length());
//     _modem.waitResponse(GF(">"));
//     _modem.stream.write(topic.c_str(), topic.length());
//     _modem.stream.flush();
//     out = _modem.waitResponse();

//     _modem.sendAT("+CMQTTPAYLOAD=0,", payload.length());
//     _modem.waitResponse(GF(">"));
//     _modem.stream.write(payload.c_str(), payload.length());
//     _modem.stream.flush();
//     out = _modem.waitResponse();

//     _modem.sendAT("+CMQTTPUB=0,1,60");
//     out = _modem.waitResponse();
// }


#endif LILYGOA7608_MQTT_H_