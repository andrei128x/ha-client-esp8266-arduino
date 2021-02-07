#include "../system/system.h"
#if defined(ENABLE_MODULE_HTTP) && (ENABLE_MODULE_HTTP == true)
#include "stdio.h"
#include "../init/global.h"

/* FUNCTIONS unit */
#include "../system/storage.h"
#include "./http_module.h"
#include "../devices/sensors.h"
#include "../devices/gate_controlller.h"

// #include <WiFiUdp.h>
// #include <ESP8266WiFi.h>

/* ----------- DEFINES ------------- */
#define HTTP_PORT 80

/*------------ VARIABLES -------------- */
WebServer server(HTTP_PORT);

/* ----------- FUNCTIONS -------------- */
void __dummyFunc__(){}
#define DUMMYFUNC __dummyFunc__

/* mandatory cycle function for http service */
void cyclicHandleWebRequests()
{
    server.handleClient();
}

void serverHandleInfoRequest()
{
    String response;

    response = "<!doctype html>\n\
<html lang=\"en\">\n\
<head>\n\
	<meta charset=\"utf-8\">\n\
	<title>Home IOT Project</title>\n\
	<meta name=\"description\" content=\"Home IOT Project\">\n\
	<meta name=\"author\" content=\"SitePoint\">\n\
</head>\n\
<body>\n\
	<h3>\n";

    response += "		<p>Temperature: ";
    response += temperatureCString;
    response += "&#176;C</p>\n";

    response += "		<p>Uptime: ";
    getSystemUptime(&response);
    response += "</p>\n";

    response += "		<p>Device IP: " + server.hostHeader() + "</p>\n";
    //u32AsciiToString(&response, server.global_hostHeader());

    response += "		<p>Last reset type: ";
    u32AsciiToString(&response, u32ResetType);

    response += "</p>\n		<p>Signal strength:" + String((int)WiFi.RSSI()) + "</p>\n	</h3>\n</body>\n</html>";

    server.send(200, "text/html", response);
    setActivityStateLED(ACTIVITY_START);
    // Serial.print("HTTP INFO requested \n ");
}

void serverHandleInfoRequestJson()
{
    String response;

    response = "{\"build_date\": \"" + String(__DATE__  __TIME__) + "\",";
    response += "\"temperature\":\"";
    response += temperatureCString;
    response += "\", \"uptime\":\"";

    getSystemUptime(&response);

    response += "\", \"reset\":\"";
    response.concat(u32ResetType);
    response += "\"}";

    server.send(200, "application/json", response);
    setActivityStateLED(ACTIVITY_START);
}

void serverHandleGetWiFiCredentials()
{
    char sTemplate[] = "SSID:\t%s\npassword:\t%s\n";
    char sResult[162];

    sprintf(sResult, sTemplate, storedDataEEPROM.SSID, storedDataEEPROM.password);
    server.send(200, "text/html", sResult);
    setActivityStateLED(ACTIVITY_START);
}

void serverHandleSetWiFiCredentials()
{
    String message = "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (unsigned char i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    // server.send(404, "text/plain", message);

    if ((server.arg("ssid") == NULL) || (server.arg("pass") == NULL))
        message = "<!doctype html>\
                  <html lang = \"en\" >\
                                <head><meta charset = \"utf-8\"><title> The HTML5 Herald</title>\
                                <meta name = \"description\" content = \"The HTML5 Herald\"></head>\
                                <body><form>\
                                    <label for=\"ssid\">SSID</label><br><input type=\"text\" id=\"ssid\" name=\"ssid\"><br>\
                                    <label for=\"pass\">Password</label><br><input type=\"password\" id=\"pass\" name=\"pass\"><br>\
                                    <input type=\"submit\" value=\"Connect\">\
                                </form></body>\
                    </html>";

    strncpy(storedDataEEPROM.SSID, server.arg("ssid").c_str(), 32);
    strncpy(storedDataEEPROM.password, server.arg("pass").c_str(), 64);

    putDataToEEPROM();

/* FIXME disabled for ESP32
    ETS_UART_INTR_DISABLE();
    SpiFlashOpResult SpiResult = spi_flash_erase_sector(0x7E); // erash WiFi config's flash sector
    ETS_UART_INTR_ENABLE();
    */

    Serial.print("Password configuration result: ");
    // Serial.println(SpiResult);   //FIXME disabled for ESP32

    server.send(201, "text/html", message);
}

void serverHandleReboot()
{
    server.send(201, "text/html", "Rebooting...");

    //TODO implement some form of Timeout for Reboot (restart after some X seconds in order to receive the HTTP confirmation)
    ESP.restart();
}

void serverHandleSetForwardingIP()
{
    //TODO implement API for saving Forwarding UDP IP
}

#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
void serverHandleServoClickRequest()
{
    doClickRelay();

    //doClickRelay(15); // no. of cycles the button is kept pressed; 10ms or 100ms cycles
    char responseTemplate[] = "{ \"response\":\"[%s]\" }";
    char responseResult[32];

        sprintf(responseResult, responseTemplate, "OK");

    server.send(200, "application/json", responseResult);
}
#endif

/* init function for http microservice */
void initWebServer()
{
    /* logging API */
    server.on("/toggle-debug", {});

    /* information APIs */
    server.on("/info", serverHandleInfoRequest);
    server.on("/info.json", serverHandleInfoRequestJson);

    /* WiFi client SSID and password storage in EEPROM */
    server.on("/get-ssid", serverHandleGetWiFiCredentials);
    server.on("/set-ssid", serverHandleSetWiFiCredentials);
    server.on("/reboot", serverHandleReboot);

#if defined(ENABLE_MODULE_UDP_ADC_FORWARD) && (ENABLE_MODULE_UDP_ADC_FORWARD == true)
    server.on("/set-forward-ip", serverHandleSetForwardingIP);
#endif

#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
    /* servo position APIs */
    server.on("/servo/click", serverHandleServoClickRequest);
#endif

    server.begin();
    Serial.println("HTTP server started");
}

/* ---END OF FILE --- */
#endif
