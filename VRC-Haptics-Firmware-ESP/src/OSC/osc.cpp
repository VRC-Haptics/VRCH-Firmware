#include "OSC/callbacks.h"
#include "logging/Logger.h"
#include "OSC/osc.h"

namespace Haptics {
namespace Wireless
{
    void printRawPacket() {
        printRaw();
    }
    
    /// @brief start mDNS and OSC
    void Start(Config *conf) {

        WiFi.mode(WIFI_STA);
        WiFi.setSleep(false); // thanks Daky
    
        // Start WiFi connection
        WiFi.begin(conf->wifi_ssid, conf->wifi_password);
        logger.debug("Connecting to: %s", conf->wifi_ssid);
        
        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
            delay(1); // needed for esp8266
        }
    
        // Print the IP address
        const String selfIP = WiFi.localIP().toString();
        logger.debug("Connected @ %s", selfIP);
    
        // Start listening for OSC server
        OscWiFi.subscribe(RECIEVE_PORT, PING_ADDRESS, &handlePing);
        logger.debug("Server started on port: %d", RECIEVE_PORT);
        
        String mac = WiFi.macAddress();
        String ip = WiFi.localIP().toString();
        String name = conf->mdns_name;

        broadcastMessage = "{";
        broadcastMessage += "\"mac\":\"" + mac + "\",";
        broadcastMessage += "\"ip\":\"" + ip + "\",";
        broadcastMessage += "\"name\":\"" + name + "\",";
        broadcastMessage += "\"port\":" + String(recvPort);
        broadcastMessage += "}";
        

        udpClient.beginMulticast(IPAddress(MULTICAST_GROUP), MULTICAST_PORT);
        Broadcast(); //broadcast first time
    }
    
    void Broadcast() {
        // Send broadcast
        logger.debug("broadcast message: %s", broadcastMessage.c_str());
        udpClient.beginPacket(IPAddress(MULTICAST_GROUP), MULTICAST_PORT);
        udpClient.write((uint8_t*)broadcastMessage.c_str(), broadcastMessage.length());  // Send data
        udpClient.endPacket();  // Ensure packet is sent
    }
    
    bool WiFiConnected(){
        return WiFi.status() == WL_CONNECTED;
    }
    
    void StartHeartBeat( String hostIP, uint16_t sendPort) {
        // Publish heart beat on one second intervals
        OscWiFi.publish(hostIP, sendPort, HEARTBEAT_ADDRESS) 
            -> setFrameRate(1.);
        heartbeatPublisher = OscWiFi.getPublishElementRef(hostIP, sendPort, HEARTBEAT_ADDRESS);
    
        if (!heartbeatPublisher) {
            logger.warn("Heartbeat wasn't established");
        }
    }
    
    void handlePing(const OscMessage& message){
        //if we recieve a ping and we were already setup, it is likely a server restart. 
        //In that case the port, ip, and other values should be reinited
    
        sendPort = message.arg<uint16_t>(0);  // Get the host's port from the message
        hostIP = message.remoteIP();  // Get the host's IP address
    
        //create our own recieving server
        OscWiFi.subscribe(RECIEVE_PORT, MOTOR_ADDRESS, &motorMessage_callback);
        OscWiFi.subscribe(RECIEVE_PORT, COMMAND_ADDRESS, &commandMessageCallback);
    
        logger.debug("Received ping from: %s", hostIP);
    
        //sending client
        oscClient = OscWiFi.getClient();
    
        // Respond to ping
        OscMessage pingResponse(PING_ADDRESS);
        pingResponse.pushInt32(RECIEVE_PORT);
        pingResponse.pushString(WiFi.macAddress());
        oscClient.send(hostIP, sendPort, pingResponse);
        logger.debug("Sending hrtbt to %s:%d", hostIP, sendPort);
    
        if (!heartbeatPublisher){
           StartHeartBeat(hostIP, sendPort);
        }
        globals.beenPinged = true;
    
    }
    
    /// @brief Push and pull OSC updates
    void Tick() {
        OscWiFi.update(); // should be called to subscribe + publish osc
    }



} // namespace Wireless
} // namespace Haptics
