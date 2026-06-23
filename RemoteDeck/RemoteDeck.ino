/*
 * RemoteDeck firmware
 *
 * Serves an embedded web control surface from an ESP32-S3 and forwards
 * browser actions as USB HID keyboard and mouse events.
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DEBUG_ON 0
#if DEBUG_ON
#define DBG_begin(...)    Serial.begin(__VA_ARGS__)
#define DBG_print(...)    Serial.print(__VA_ARGS__)
#define DBG_println(...)  Serial.println(__VA_ARGS__)
#define DBG_printf(...)   Serial.printf(__VA_ARGS__)
#else
#define DBG_begin(...)
#define DBG_print(...)
#define DBG_println(...)
#define DBG_printf(...)
#endif

USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;
WiFiUDP Udp;
DNSServer dnsServer;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

#include "index_html.h"

const char *DEVICE_NAME = "RemoteDeck";
const char *MDNS_NAME = "remotedeck";
const char *FIRMWARE_VERSION = "1.0.2";
const char *BUILD_TARGET = "ESP32-S3 USB HID";
const byte DNS_PORT = 53;
const uint8_t WIFI_AP_CHANNEL = 6;
const uint8_t WIFI_AP_MAX_CLIENTS = 4;
const unsigned long WIFI_HEALTH_INTERVAL_MS = 10000;
const unsigned long WIFI_AP_STARTUP_TIMEOUT_MS = 3500;
const uint8_t WIFI_AP_STARTUP_RETRIES = 4;
const uint8_t MAX_AP_RECOVERY_ATTEMPTS = 6;

const int MAX_ROWS = 6;
const int MAX_COLS = 17;

const uint8_t Keycodes[MAX_ROWS][MAX_COLS] = {
  {KEY_TAB, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12},
  {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '+', '\b', KEY_INSERT, KEY_HOME, KEY_PAGE_UP},
  {KEY_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\', KEY_DELETE, KEY_END, KEY_PAGE_DOWN},
  {KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', KEY_RETURN},
  {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_RIGHT_SHIFT, 0, KEY_UP_ARROW},
  {KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', KEY_RIGHT_ALT, KEY_RIGHT_GUI, 0, KEY_RIGHT_CTRL, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW},
};

unsigned long lastWifiHealthCheck = 0;
uint8_t apRecoveryAttempts = 0;

bool wifiModeHasAp()
{
  wifi_mode_t mode = WiFi.getMode();
  return mode == WIFI_AP || mode == WIFI_AP_STA;
}

bool accessPointLooksReady()
{
  return wifiModeHasAp() && WiFi.softAPIP() != IPAddress(0, 0, 0, 0);
}

bool startRemoteDeckAccessPoint(bool resetFirst)
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.setSleep(false);

  if (resetFirst) {
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    delay(250);
  }

  WiFi.softAPConfig(
    IPAddress(192, 168, 4, 1),
    IPAddress(192, 168, 4, 1),
    IPAddress(255, 255, 255, 0)
  );

  bool started = WiFi.softAP(
    DEVICE_NAME,
    nullptr,
    WIFI_AP_CHANNEL,
    false,
    WIFI_AP_MAX_CLIENTS
  );

  if (started && accessPointLooksReady()) {
    dnsServer.stop();
    delay(20);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    return true;
  }

  return false;
}

bool ensureRemoteDeckAccessPoint()
{
  for (uint8_t attempt = 0; attempt < WIFI_AP_STARTUP_RETRIES; attempt++) {
    bool resetFirst = attempt > 0;

    if (startRemoteDeckAccessPoint(resetFirst)) {
      unsigned long startedAt = millis();

      while (millis() - startedAt < WIFI_AP_STARTUP_TIMEOUT_MS) {
        if (accessPointLooksReady()) {
          apRecoveryAttempts = 0;
          return true;
        }

        delay(75);
      }
    }
  }

  return false;
}

void maintainWifi()
{
  unsigned long now = millis();

  if (now - lastWifiHealthCheck < WIFI_HEALTH_INTERVAL_MS) {
    return;
  }

  lastWifiHealthCheck = now;

  if (!accessPointLooksReady()) {
    bool recovered = ensureRemoteDeckAccessPoint();
    apRecoveryAttempts = recovered ? 0 : apRecoveryAttempts + 1;

    if (!recovered && apRecoveryAttempts >= MAX_AP_RECOVERY_ATTEMPTS) {
      ESP.restart();
    }

    return;
  }

  apRecoveryAttempts = 0;

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }
}

int8_t clampMouseDelta(int value)
{
  if (value > 127) {
    return 127;
  }
  if (value < -127) {
    return -127;
  }
  return (int8_t)value;
}

void sendJson(uint8_t client, JsonDocument &doc)
{
  String response;
  serializeJson(doc, response);
  webSocket.sendTXT(client, response);
}

void sendError(uint8_t client, const char *action, const char *message)
{
  JsonDocument response;
  response["action"] = action;
  response["error"] = message;
  sendJson(client, response);
}

void tapShortcut(uint8_t first, uint8_t second = 0, uint8_t third = 0, uint16_t holdMs = 40)
{
  if (first != 0) {
    Keyboard.press(first);
  }
  if (second != 0) {
    Keyboard.press(second);
  }
  if (third != 0) {
    Keyboard.press(third);
  }

  delay(holdMs);
  Keyboard.releaseAll();
}

void openRunBox()
{
  tapShortcut(KEY_LEFT_GUI, 'r', 0, 60);
  delay(350);
}

void openRunCommand(const char *command)
{
  openRunBox();
  Keyboard.print(command);
  delay(100);
  Keyboard.write(KEY_RETURN);
}

void openCmdCommand(const char *command)
{
  String runCommand = "cmd /k ";
  runCommand += command;
  openRunCommand(runCommand.c_str());
}

void openPowerShellCommand(const char *command)
{
  openRunCommand("powershell -NoLogo -NoProfile -ExecutionPolicy Bypass");
  delay(2200);
  Keyboard.print(command);
  delay(100);
  Keyboard.write(KEY_RETURN);
}

void openElevatedCmdCommand(const char *command)
{
  String runCommand = "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Start-Process cmd -ArgumentList '/k ";
  runCommand += command;
  runCommand += "' -Verb RunAs\"";
  openRunCommand(runCommand.c_str());
}

void openElevatedPowerShell()
{
  openRunCommand("powershell -NoProfile -ExecutionPolicy Bypass -Command \"Start-Process powershell -ArgumentList '-NoExit -NoProfile -ExecutionPolicy Bypass' -Verb RunAs\"");
}

const char *normalizeProfile(const char *profile)
{
  if (profile != nullptr && strcmp(profile, "linux") == 0) {
    return "linux";
  }

  if (profile != nullptr && strcmp(profile, "macos") == 0) {
    return "macos";
  }

  return "windows";
}

bool profileIs(const char *profile, const char *expected)
{
  return strcmp(profile, expected) == 0;
}

void typeCommandAndEnter(const char *command)
{
  Keyboard.print(command);
  delay(100);
  Keyboard.write(KEY_RETURN);
}

void openLinuxTerminal()
{
  tapShortcut(KEY_LEFT_CTRL, KEY_LEFT_ALT, 't', 60);
  delay(1000);
}

void showMacSpotlight()
{
  tapShortcut(KEY_LEFT_GUI, ' ', 0, 60);
  delay(450);
}

void openMacSpotlight(const char *query)
{
  showMacSpotlight();

  if (query == nullptr || query[0] == '\0') {
    return;
  }

  Keyboard.print(query);
  delay(120);
  Keyboard.write(KEY_RETURN);
}

void openMacTerminal()
{
  openMacSpotlight("terminal");
  delay(1400);
}

void openLinuxCommand(const char *command)
{
  openLinuxTerminal();
  typeCommandAndEnter(command);
}

void openMacTerminalCommand(const char *command)
{
  openMacTerminal();
  typeCommandAndEnter(command);
}

void runProfileCommand(const char *profile, const char *windowsCommand, const char *linuxCommand, const char *macCommand)
{
  if (profileIs(profile, "linux")) {
    openLinuxCommand(linuxCommand);
    return;
  }

  if (profileIs(profile, "macos")) {
    openMacTerminalCommand(macCommand);
    return;
  }

  openPowerShellCommand(windowsCommand);
}

bool handleShortcut(const char *action, const char *profile)
{
  profile = normalizeProfile(profile);

  if (strcmp(action, "run") == 0) {
    if (profileIs(profile, "windows")) {
      openRunBox();
    } else if (profileIs(profile, "linux")) {
      openLinuxTerminal();
    } else {
      openMacSpotlight("");
    }
    return true;
  }

  if (strcmp(action, "copy") == 0) {
    if (profileIs(profile, "macos")) {
      tapShortcut(KEY_LEFT_GUI, 'c');
    } else {
      tapShortcut(KEY_LEFT_CTRL, 'c');
    }
    return true;
  }

  if (strcmp(action, "paste") == 0) {
    if (profileIs(profile, "macos")) {
      tapShortcut(KEY_LEFT_GUI, 'v');
    } else {
      tapShortcut(KEY_LEFT_CTRL, 'v');
    }
    return true;
  }

  if (strcmp(action, "selectall") == 0) {
    if (profileIs(profile, "macos")) {
      tapShortcut(KEY_LEFT_GUI, 'a');
    } else {
      tapShortcut(KEY_LEFT_CTRL, 'a');
    }
    return true;
  }

  if (strcmp(action, "terminal") == 0) {
    if (profileIs(profile, "linux")) {
      openLinuxTerminal();
    } else if (profileIs(profile, "macos")) {
      openMacTerminal();
    } else {
      openRunCommand("cmd");
    }
    return true;
  }

  if (strcmp(action, "spotlight") == 0) {
    openMacSpotlight("");
    return true;
  }

  if (strcmp(action, "activity") == 0) {
    if (profileIs(profile, "macos")) {
      openMacSpotlight("activity monitor");
    } else if (profileIs(profile, "linux")) {
      openLinuxCommand("top");
    } else {
      tapShortcut(KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_ESC, 60);
    }
    return true;
  }

  if (strcmp(action, "forcequit") == 0) {
    tapShortcut(KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_ESC, 60);
    return true;
  }

  if (strcmp(action, "cad") == 0) {
    tapShortcut(KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_DELETE, 60);
    return true;
  }

  if (strcmp(action, "winx") == 0) {
    tapShortcut(KEY_LEFT_GUI, 'x');
    return true;
  }

  if (strcmp(action, "lock") == 0) {
    if (profileIs(profile, "linux")) {
      tapShortcut(KEY_LEFT_CTRL, KEY_LEFT_ALT, 'l', 60);
    } else if (profileIs(profile, "macos")) {
      tapShortcut(KEY_LEFT_CTRL, KEY_LEFT_GUI, 'q', 60);
    } else {
      tapShortcut(KEY_LEFT_GUI, 'l');
    }
    return true;
  }

  if (strcmp(action, "taskmgr") == 0) {
    tapShortcut(KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_ESC, 60);
    return true;
  }

  if (strcmp(action, "cmd") == 0) {
    openRunCommand("cmd");
    return true;
  }

  if (strcmp(action, "cmdadmin") == 0) {
    openElevatedCmdCommand("");
    return true;
  }

  if (strcmp(action, "powershell") == 0) {
    openRunCommand("powershell -NoLogo -NoProfile -ExecutionPolicy Bypass");
    return true;
  }

  if (strcmp(action, "powershelladmin") == 0) {
    openElevatedPowerShell();
    return true;
  }

  if (strcmp(action, "devmgmt") == 0) {
    openRunCommand("devmgmt.msc");
    return true;
  }

  if (strcmp(action, "services") == 0) {
    if (profileIs(profile, "windows")) {
      openRunCommand("services.msc");
      return true;
    }

    runProfileCommand(
      profile,
      "sc query",
      "systemctl --type=service --state=running --no-pager | head -40",
      "launchctl list | head -40"
    );
    return true;
  }

  if (strcmp(action, "eventvwr") == 0) {
    openRunCommand("eventvwr.msc");
    return true;
  }

  if (strcmp(action, "ncpa") == 0) {
    openRunCommand("ncpa.cpl");
    return true;
  }

  if (strcmp(action, "sysinfo") == 0) {
    runProfileCommand(
      profile,
      "Get-ComputerInfo | Select-Object CsName,OsName,OsVersion,OsBuildNumber,CsManufacturer,CsModel",
      "hostnamectl; uname -a",
      "scutil --get ComputerName; sw_vers; uname -a"
    );
    return true;
  }

  if (strcmp(action, "ipconfig") == 0 || strcmp(action, "netinfo") == 0) {
    runProfileCommand(
      profile,
      "Get-NetIPConfiguration; Get-NetAdapter | Select-Object Name,Status,LinkSpeed,MacAddress",
      "ip addr; echo; ip route",
      "ifconfig; echo; netstat -rn"
    );
    return true;
  }

  if (strcmp(action, "routes") == 0) {
    runProfileCommand(
      profile,
      "Get-NetRoute -AddressFamily IPv4 | Sort-Object RouteMetric | Select-Object -First 30 DestinationPrefix,NextHop,RouteMetric,InterfaceAlias",
      "ip route",
      "netstat -rn"
    );
    return true;
  }

  if (strcmp(action, "diskinfo") == 0) {
    runProfileCommand(
      profile,
      "Get-PSDrive -PSProvider FileSystem | Select-Object Name,Used,Free,Root",
      "df -h",
      "df -h"
    );
    return true;
  }

  if (strcmp(action, "memoryinfo") == 0) {
    runProfileCommand(
      profile,
      "Get-CimInstance Win32_OperatingSystem | Select-Object FreePhysicalMemory,TotalVisibleMemorySize",
      "free -h",
      "vm_stat"
    );
    return true;
  }

  if (strcmp(action, "cpuinfo") == 0) {
    runProfileCommand(
      profile,
      "Get-CimInstance Win32_Processor | Select-Object Name,NumberOfCores,NumberOfLogicalProcessors",
      "lscpu",
      "sysctl -n machdep.cpu.brand_string; sysctl hw.physicalcpu hw.logicalcpu"
    );
    return true;
  }

  if (strcmp(action, "uptimeinfo") == 0) {
    runProfileCommand(
      profile,
      "Get-CimInstance Win32_OperatingSystem | Select-Object LastBootUpTime",
      "uptime",
      "uptime"
    );
    return true;
  }

  if (strcmp(action, "processes") == 0) {
    runProfileCommand(
      profile,
      "Get-Process | Sort-Object WorkingSet -Descending | Select-Object -First 25 Name,Id,CPU,WorkingSet",
      "ps aux --sort=-%mem | head -25",
      "ps aux -m | head -25"
    );
    return true;
  }

  if (strcmp(action, "logs") == 0) {
    runProfileCommand(
      profile,
      "Get-WinEvent -LogName System -MaxEvents 25 | Select-Object TimeCreated,Id,ProviderName,LevelDisplayName,Message",
      "journalctl -n 40 --no-pager",
      "log show --last 10m --style compact | tail -40"
    );
    return true;
  }

  if (strcmp(action, "dmesg") == 0) {
    runProfileCommand(
      profile,
      "Get-WinEvent -LogName System -MaxEvents 25 | Select-Object TimeCreated,Id,ProviderName,LevelDisplayName,Message",
      "dmesg | tail -40",
      "dmesg | tail -40"
    );
    return true;
  }

  if (strcmp(action, "dnscheck") == 0) {
    runProfileCommand(
      profile,
      "Resolve-DnsName google.com",
      "getent hosts google.com || nslookup google.com",
      "dscacheutil -q host -a name google.com; nslookup google.com"
    );
    return true;
  }

  if (strcmp(action, "ping8") == 0) {
    runProfileCommand(
      profile,
      "ping 8.8.8.8",
      "ping -c 4 8.8.8.8",
      "ping -c 4 8.8.8.8"
    );
    return true;
  }

  if (strcmp(action, "gpupdate") == 0) {
    openElevatedCmdCommand("gpupdate /force");
    return true;
  }

  if (strcmp(action, "sfcscan") == 0) {
    openElevatedCmdCommand("sfc /scannow");
    return true;
  }

  if (strcmp(action, "dismrepair") == 0) {
    openElevatedCmdCommand("DISM /Online /Cleanup-Image /RestoreHealth");
    return true;
  }

  if (strcmp(action, "flushdns") == 0) {
    openElevatedCmdCommand("ipconfig /flushdns");
    return true;
  }

  if (strcmp(action, "winsockreset") == 0) {
    openElevatedCmdCommand("netsh winsock reset");
    return true;
  }

  return false;
}

void sendToolOutput(uint8_t client, const char *action, const String &output)
{
  JsonDocument response;
  response["action"] = action;
  response["output"] = output;
  sendJson(client, response);
}

void populateDeviceStatus(JsonDocument &doc)
{
  const char *hostname = WiFi.getHostname();

  doc["device"] = DEVICE_NAME;
  doc["version"] = FIRMWARE_VERSION;
  doc["target"] = BUILD_TARGET;
  doc["mdns"] = MDNS_NAME;
  doc["uptimeMs"] = millis();
  doc["stationConnected"] = WiFi.status() == WL_CONNECTED;
  doc["ssid"] = WiFi.SSID();
  doc["ip"] = WiFi.localIP().toString();
  doc["apip"] = WiFi.softAPIP().toString();
  doc["host"] = hostname != nullptr ? hostname : MDNS_NAME;
  doc["gateway"] = WiFi.gatewayIP().toString();
  doc["rssi"] = WiFi.RSSI();
  doc["apClients"] = WiFi.softAPgetStationNum();
  doc["freeHeap"] = ESP.getFreeHeap();
}

void sendDeviceStatus(uint8_t client)
{
  JsonDocument response;
  populateDeviceStatus(response);
  response["action"] = "status";
  sendJson(client, response);
}

String formatUptime(unsigned long uptimeMs)
{
  unsigned long seconds = uptimeMs / 1000;
  unsigned int days = seconds / 86400;
  seconds %= 86400;
  unsigned int hours = seconds / 3600;
  seconds %= 3600;
  unsigned int minutes = seconds / 60;
  seconds %= 60;

  String output;

  if (days > 0) {
    output += String(days);
    output += "d ";
  }

  output += String(hours);
  output += "h ";
  output += String(minutes);
  output += "m ";
  output += String(seconds);
  output += "s";

  return output;
}

void sendDeviceHealth(uint8_t client)
{
  JsonDocument status;
  populateDeviceStatus(status);

  String output;
  output += "RemoteDeck Device Health\n";
  output += "Version: ";
  output += FIRMWARE_VERSION;
  output += "\nTarget: ";
  output += BUILD_TARGET;
  output += "\nUptime: ";
  output += formatUptime(millis());
  output += "\nStation: ";
  output += (WiFi.status() == WL_CONNECTED ? "Connected" : "Offline");
  output += "\nSSID: ";
  output += WiFi.SSID();
  output += "\nStation IP: ";
  output += WiFi.localIP().toString();
  output += "\nAccess Point IP: ";
  output += WiFi.softAPIP().toString();
  output += "\nAP Clients: ";
  output += String(WiFi.softAPgetStationNum());
  output += "\nRSSI: ";
  output += String(WiFi.RSSI());
  output += " dBm";
  output += "\nFree Heap: ";
  output += String(ESP.getFreeHeap());
  output += " bytes";

  status["action"] = "devicehealth";
  status["output"] = output;
  sendJson(client, status);
}

void sendLocalNetworkInfo(uint8_t client)
{
  JsonDocument response;
  const char *hostname = WiFi.getHostname();

  response["action"] = "ipinfo";
  response["wifi"] = WiFi.SSID();
  response["ip"] = WiFi.localIP().toString();
  response["apip"] = WiFi.softAPIP().toString();
  response["host"] = hostname != nullptr ? hostname : MDNS_NAME;
  response["gateway"] = WiFi.gatewayIP().toString();
  response["rssi"] = WiFi.RSSI();

  sendJson(client, response);
}

void sendPublicIpInfo(uint8_t client)
{
  HTTPClient http;

  if (!http.begin("http://ip-api.com/json?fields=status,message,query,isp,city,regionName,country")) {
    sendError(client, "publicip", "Unable to start HTTP request");
    return;
  }

  http.setTimeout(5000);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    http.end();
    sendError(client, "publicip", "Public IP lookup failed");
    return;
  }

  JsonDocument apiDoc;
  DeserializationError error = deserializeJson(apiDoc, http.getString());
  http.end();

  if (error) {
    sendError(client, "publicip", "Public IP response was not valid JSON");
    return;
  }

  if (strcmp(apiDoc["status"] | "", "fail") == 0) {
    sendError(client, "publicip", apiDoc["message"] | "Public IP lookup failed");
    return;
  }

  JsonDocument response;
  response["action"] = "publicip";
  response["publicip"] = apiDoc["query"] | "";
  response["isp"] = apiDoc["isp"] | "";
  response["city"] = apiDoc["city"] | "";
  response["region"] = apiDoc["regionName"] | "";
  response["country"] = apiDoc["country"] | "";

  sendJson(client, response);
}

void sendNetworkSnapshot(uint8_t client)
{
  String scanResults;
  scanResults += "Device IP: ";
  scanResults += WiFi.localIP().toString();
  scanResults += "\nAccess Point: ";
  scanResults += WiFi.softAPIP().toString();
  scanResults += "\nAP Clients: ";
  scanResults += String(WiFi.softAPgetStationNum());
  scanResults += "\nGateway: ";
  scanResults += WiFi.gatewayIP().toString();
  scanResults += "\nSSID: ";
  scanResults += WiFi.SSID();

  JsonDocument response;
  response["action"] = "networkscan";
  response["scanresults"] = scanResults;

  sendJson(client, response);
}

void sendTicketInfo(uint8_t client)
{
  String output;
  const char *hostname = WiFi.getHostname();

  output += "RemoteDeck Ticket Info\n";
  output += "SSID: ";
  output += WiFi.SSID();
  output += "\nStation IP: ";
  output += WiFi.localIP().toString();
  output += "\nAccess Point IP: ";
  output += WiFi.softAPIP().toString();
  output += "\nAP Clients: ";
  output += String(WiFi.softAPgetStationNum());
  output += "\nGateway: ";
  output += WiFi.gatewayIP().toString();
  output += "\nHostname: ";
  output += (hostname != nullptr ? hostname : MDNS_NAME);
  output += "\nRSSI: ";
  output += String(WiFi.RSSI());
  output += " dBm";

  sendToolOutput(client, "ticketinfo", output);
}

void sendPcSpecs(uint8_t client, const char *profile)
{
  profile = normalizeProfile(profile);

  if (profileIs(profile, "linux")) {
    openLinuxCommand("printf 'RemoteDeck System Specs\\n'; hostnamectl; echo; uname -a; echo; lscpu | head -25; echo; free -h");
    sendToolOutput(client, "pcspecs", "System Specs command sent to the Linux target.\nA terminal or shell will show hostname, OS/kernel, CPU, and memory.");
    return;
  }

  if (profileIs(profile, "macos")) {
    openMacTerminalCommand("printf 'RemoteDeck System Specs\\n'; scutil --get ComputerName; sw_vers; sysctl -n machdep.cpu.brand_string; sysctl hw.memsize");
    sendToolOutput(client, "pcspecs", "System Specs command sent to the macOS target.\nTerminal will show computer name, macOS version, CPU, and memory bytes.");
    return;
  }

  const char *command = "$cs=Get-CimInstance Win32_ComputerSystem;$os=Get-CimInstance Win32_OperatingSystem;$cpu=Get-CimInstance Win32_Processor|Select-Object -First 1;[pscustomobject]@{PC=$env:COMPUTERNAME;OS=$os.Caption;Version=$os.Version;Build=$os.BuildNumber;CPU=$cpu.Name;MemoryGB=[math]::Round($cs.TotalPhysicalMemory/1GB,1)}|Format-List";

  openPowerShellCommand(command);
  sendToolOutput(client, "pcspecs", "System Specs command sent to the Windows target.\nPowerShell will show PC name, OS, version, build, CPU, and memory.");
}

void sendDnsLookup(uint8_t client, const char *host)
{
  if (host == nullptr || host[0] == '\0') {
    sendError(client, "dnslookup", "Host is required");
    return;
  }

  IPAddress resolvedIp;
  int result = WiFi.hostByName(host, resolvedIp);

  if (result != 1) {
    sendError(client, "dnslookup", "DNS lookup failed");
    return;
  }

  String output;
  output += "Host: ";
  output += host;
  output += "\nAddress: ";
  output += resolvedIp.toString();

  sendToolOutput(client, "dnslookup", output);
}

void sendPortCheck(uint8_t client, const char *host, int port)
{
  if (host == nullptr || host[0] == '\0') {
    sendError(client, "portcheck", "Host is required");
    return;
  }

  if (port <= 0 || port > 65535) {
    sendError(client, "portcheck", "Port must be 1-65535");
    return;
  }

  WiFiClient tcpClient;
  tcpClient.setTimeout(3000);
  bool open = tcpClient.connect(host, (uint16_t)port);
  tcpClient.stop();

  String output;
  output += "Target: ";
  output += host;
  output += ":";
  output += String(port);
  output += "\nStatus: ";
  output += (open ? "Open" : "Closed or filtered");

  sendToolOutput(client, "portcheck", output);
}

bool hexPairToByte(char high, char low, uint8_t &value)
{
  if (!isxdigit((unsigned char)high) || !isxdigit((unsigned char)low)) {
    return false;
  }

  char pair[3] = {high, low, '\0'};
  value = (uint8_t)strtoul(pair, nullptr, 16);
  return true;
}

bool parseMacAddress(const char *mac, uint8_t bytes[6])
{
  if (mac == nullptr) {
    return false;
  }

  char clean[13];
  uint8_t cleanIndex = 0;

  for (uint8_t i = 0; mac[i] != '\0' && cleanIndex < sizeof(clean) - 1; i++) {
    if (isxdigit((unsigned char)mac[i])) {
      clean[cleanIndex++] = mac[i];
    }
  }

  clean[cleanIndex] = '\0';

  if (cleanIndex != 12) {
    return false;
  }

  for (uint8_t i = 0; i < 6; i++) {
    if (!hexPairToByte(clean[i * 2], clean[i * 2 + 1], bytes[i])) {
      return false;
    }
  }

  return true;
}

void sendWakeOnLan(uint8_t client, const char *mac)
{
  uint8_t macBytes[6];

  if (!parseMacAddress(mac, macBytes)) {
    sendError(client, "wol", "MAC address must be 12 hex digits");
    return;
  }

  uint8_t packet[102];
  memset(packet, 0xFF, 6);

  for (uint8_t i = 1; i <= 16; i++) {
    memcpy(&packet[i * 6], macBytes, 6);
  }

  IPAddress broadcastIp(255, 255, 255, 255);
  IPAddress localIp = WiFi.localIP();
  IPAddress subnetMask = WiFi.subnetMask();

  if (localIp != IPAddress(0, 0, 0, 0) && subnetMask != IPAddress(0, 0, 0, 0)) {
    uint8_t inverseSubnet[4] = {
      (uint8_t)~subnetMask[0],
      (uint8_t)~subnetMask[1],
      (uint8_t)~subnetMask[2],
      (uint8_t)~subnetMask[3]
    };

    broadcastIp = IPAddress(
      (uint8_t)((localIp[0] & subnetMask[0]) | inverseSubnet[0]),
      (uint8_t)((localIp[1] & subnetMask[1]) | inverseSubnet[1]),
      (uint8_t)((localIp[2] & subnetMask[2]) | inverseSubnet[2]),
      (uint8_t)((localIp[3] & subnetMask[3]) | inverseSubnet[3])
    );
  }

  int result = 0;

  for (uint8_t i = 0; i < 3; i++) {
    Udp.beginPacket(broadcastIp, 9);
    Udp.write(packet, sizeof(packet));
    result += Udp.endPacket();
    delay(20);
  }

  String output;
  output += (result > 0 ? "Wake packet sent" : "Wake packet failed");
  output += "\nMAC: ";
  output += mac;
  output += "\nBroadcast: ";
  output += broadcastIp.toString();

  sendToolOutput(client, "wol", output);
}

bool handleTool(uint8_t client, const char *action, JsonDocument &doc)
{
  const char *profile = normalizeProfile(doc["profile"] | "windows");

  if (strcmp(action, "devicehealth") == 0) {
    sendDeviceHealth(client);
    return true;
  }

  if (strcmp(action, "ipinfo") == 0) {
    sendLocalNetworkInfo(client);
    return true;
  }

  if (strcmp(action, "publicip") == 0) {
    sendPublicIpInfo(client);
    return true;
  }

  if (strcmp(action, "networkscan") == 0) {
    sendNetworkSnapshot(client);
    return true;
  }

  if (strcmp(action, "ticketinfo") == 0) {
    sendTicketInfo(client);
    return true;
  }

  if (strcmp(action, "pcspecs") == 0) {
    sendPcSpecs(client, profile);
    return true;
  }

  if (strcmp(action, "dnslookup") == 0) {
    sendDnsLookup(client, doc["host"] | "");
    return true;
  }

  if (strcmp(action, "portcheck") == 0) {
    sendPortCheck(client, doc["host"] | "", doc["port"] | 0);
    return true;
  }

  if (strcmp(action, "wol") == 0) {
    sendWakeOnLan(client, doc["mac"] | "");
    return true;
  }

  return false;
}

bool handleKeyEvent(const char *event, JsonDocument &doc)
{
  const char *key = doc["key"] | nullptr;

  if (key == nullptr) {
    return false;
  }

  bool pressed = strcmp(event, "touch start") == 0;
  bool released = strcmp(event, "touch end") == 0;

  if (!pressed && !released) {
    return false;
  }

  if (strlen(key) == 1) {
    char keyChar = key[0];

    if (isalpha((unsigned char)keyChar)) {
      bool shifted = doc["shift"] | false;
      keyChar = shifted ? toupper((unsigned char)keyChar) : tolower((unsigned char)keyChar);
    }

    if (pressed) {
      Keyboard.press(keyChar);
    } else {
      Keyboard.release(keyChar);
    }

    return true;
  }

  uint8_t keycode = 0;

  if (strcmp(key, "SPACE") == 0) {
    keycode = ' ';
  } else if (strcmp(key, "ENTER") == 0) {
    keycode = KEY_RETURN;
  } else if (strcmp(key, "BKSP") == 0) {
    keycode = '\b';
  } else if (strcmp(key, "TAB") == 0) {
    keycode = KEY_TAB;
  } else if (strcmp(key, "ESC") == 0) {
    keycode = KEY_ESC;
  }

  if (keycode == 0) {
    return true;
  }

  if (pressed) {
    Keyboard.press(keycode);
  } else {
    Keyboard.release(keycode);
  }

  return true;
}

bool handleMouseEvent(const char *event, JsonDocument &doc)
{
  if (strcmp(event, "mouse_move") == 0) {
    Mouse.move(
      clampMouseDelta(doc["dx"] | 0),
      clampMouseDelta(doc["dy"] | 0)
    );
    return true;
  }

  if (strcmp(event, "mouse_left") == 0) {
    Mouse.click(MOUSE_LEFT);
    return true;
  }

  if (strcmp(event, "mouse_right") == 0) {
    Mouse.click(MOUSE_RIGHT);
    return true;
  }

  if (strcmp(event, "mouse_scroll") == 0) {
    Mouse.move(0, 0, clampMouseDelta(doc["delta"] | 0));
    return true;
  }

  return false;
}

bool handleLegacyGridEvent(const char *event, JsonDocument &doc)
{
  bool pressed = strcmp(event, "touch start") == 0;
  bool released = strcmp(event, "touch end") == 0;

  if (!pressed && !released) {
    return false;
  }

  if (!doc["row"].is<int>() || !doc["col"].is<int>()) {
    return false;
  }

  int row = doc["row"];
  int col = doc["col"];

  if (row < 0) {
    DBG_printf("row negative %d\n", row);
    row = 0;
  } else if (row >= MAX_ROWS) {
    DBG_printf("row too high %d\n", row);
    row = MAX_ROWS - 1;
  }

  if (col < 0) {
    DBG_printf("col negative %d\n", col);
    col = 0;
  } else if (col >= MAX_COLS) {
    DBG_printf("col too high %d\n", col);
    col = MAX_COLS - 1;
  }

  uint8_t keycode = Keycodes[row][col];

  if (keycode == 0) {
    return true;
  }

  if (pressed) {
    Keyboard.press(keycode);
  } else {
    Keyboard.release(keycode);
  }

  return true;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  DBG_printf("webSocketEvent(%d, %d, ...)\r\n", num, type);

  switch (type) {
    case WStype_DISCONNECTED:
      DBG_printf("[%u] Disconnected!\r\n", num);
      break;

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        DBG_printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        sendDeviceStatus(num);
      }
      break;

    case WStype_TEXT:
      {
        DBG_printf("[%u] get Text: [%d] %s \r\n", num, length, payload);

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          DBG_print(F("deserializeJson() failed: "));
          DBG_println(error.f_str());
          return;
        }

        const char *event = doc["event"] | "";
        const char *action = doc["action"] | "";

        if (event[0] == '\0') {
          return;
        }

        if (strcmp(event, "tool") == 0) {
          if (!handleTool(num, action, doc)) {
            sendError(num, action, "Unknown tool action");
          }
          return;
        }

        if (strcmp(event, "shortcut") == 0) {
          const char *profile = doc["profile"] | "windows";

          if (!handleShortcut(action, profile)) {
            sendError(num, action, "Unknown shortcut action");
          }
          return;
        }

        if (handleKeyEvent(event, doc)) {
          return;
        }

        if (handleMouseEvent(event, doc)) {
          return;
        }

        handleLegacyGridEvent(event, doc);
      }
      break;

    case WStype_BIN:
      DBG_printf("[%u] get binary length: %u\r\n", num, length);
      webSocket.sendBIN(num, payload, length);
      break;

    default:
      DBG_printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}

void handleRoot()
{
  server.send(200, "text/html", INDEX_HTML);
}

void handleStatusJson()
{
  JsonDocument response;
  populateDeviceStatus(response);

  String output;
  serializeJson(response, output);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", output);
}

String captivePortalUrl()
{
  return String("http://") + WiFi.softAPIP().toString() + "/";
}

void redirectToCaptivePortal()
{
  server.sendHeader("Location", captivePortalUrl(), true);
  server.send(302, "text/plain", "");
}

void handleNotFound()
{
  if (server.method() == HTTP_GET) {
    redirectToCaptivePortal();
    return;
  }

  server.send(404, "text/plain", "File Not Found");
}

void setup()
{
  DBG_begin(115200);
  delay(300);

  WiFi.mode(WIFI_AP_STA);
  WiFi.persistent(false);
  WiFi.setSleep(false);
  WiFi.setHostname(MDNS_NAME);

  USB.usbClass(0);
  USB.usbSubClass(0);
  USB.usbProtocol(0);
  Keyboard.begin();
  Mouse.begin();
  USB.begin();

  if (!ensureRemoteDeckAccessPoint()) {
    delay(1000);
    ESP.restart();
  }

  WiFiManager wm;
  wm.setHostname(MDNS_NAME);
  wm.setEnableConfigPortal(false);
  wm.setConnectTimeout(8);

  bool res = wm.autoConnect(DEVICE_NAME);

  if (!ensureRemoteDeckAccessPoint()) {
    delay(1000);
    ESP.restart();
  }

  if (!res) {
    DBG_println(F("Station WiFi unavailable; continuing with RemoteDeck AP only"));
  }

  if (MDNS.begin(MDNS_NAME)) {
    DBG_println(F("mDNS responder started"));
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
  } else {
    DBG_println(F("Error setting up mDNS responder"));
  }

  DBG_print(F("Station IP: "));
  DBG_println(WiFi.localIP());

  DBG_print(F("Access Point IP: "));
  DBG_println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/status.json", handleStatusJson);
  server.on("/health", handleStatusJson);
  server.on("/generate_204", redirectToCaptivePortal);
  server.on("/gen_204", redirectToCaptivePortal);
  server.on("/hotspot-detect.html", redirectToCaptivePortal);
  server.on("/library/test/success.html", redirectToCaptivePortal);
  server.on("/ncsi.txt", redirectToCaptivePortal);
  server.on("/connecttest.txt", redirectToCaptivePortal);
  server.on("/redirect", redirectToCaptivePortal);
  server.on("/fwlink", redirectToCaptivePortal);
  server.onNotFound(handleNotFound);

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Udp.begin(9);
}

void loop()
{
  maintainWifi();
  dnsServer.processNextRequest();
  webSocket.loop();
  server.handleClient();
}
