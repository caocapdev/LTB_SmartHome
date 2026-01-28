#include "MCPHandler.h"
#include "RelayHandler.h"
#include "AlarmHandler.h"
#include <WebSocketMCP.h>
#include <ArduinoJson.h>

// MCP Server Configuration
const char* mcpEndpoint = "wss://api.xiaozhi.me/mcp/?token=0c6e9a90-3d5b-4199-9880-02c9598f978e";
WebSocketMCP mcpClient;

// Internal callback for connection
void onConnectionStatus(bool connected) {
    if (connected) {
        Serial.println("[MCP] ✅ Đã kết nối tới máy chủ");
        registerMcpTools();
    } else {
        Serial.println("[MCP] ⚠️ Mất kết nối với máy chủ MCP");
    }
}

void registerMcpTools() {
    auto controlRelay = [](const String& name, int index, const String& args) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, args);
        String state = doc["state"].as<String>();
        
        // In our system: 1 is OFF, 0 is ON
        bool relayValue = (state == "off") ? 1 : 0;
        setRelay(index, relayValue);
        
        return WebSocketMCP::ToolResponse("{\"success\":true,\"device\":\"" + name + "\",\"state\":\"" + state + "\"}");
    };

    // Device Mapping
    // toilet -> r1 (index 0)
    mcpClient.registerTool("toilet_lights", "Điều khiển đèn toilet", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("toilet", 0, args); });

    // sân thượng -> r2 (index 1)
    mcpClient.registerTool("rooftop_lights", "Điều khiển đèn sân thượng", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("sân thượng", 1, args); });

    // phòng ngủ 1 -> r3 (index 2)
    mcpClient.registerTool("bedroom_lights_1", "Điều khiển đèn phòng ngủ 1", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("phòng ngủ 1", 2, args); });

    // quạt -> r6 (index 5)
    mcpClient.registerTool("fan_control", "Điều khiển quạt", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("quạt", 5, args); });

    // bếp -> r7 (index 6)
    mcpClient.registerTool("kitchen_lights", "Điều khiển đèn bếp", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("bếp", 6, args); });

    // garage -> r8 (index 7)
    mcpClient.registerTool("garage_lights", "Điều khiển đèn garage", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("garage", 7, args); });

    // mặt trước nhà -> r10 (index 9)
    mcpClient.registerTool("front_lights", "Điều khiển đèn mặt trước nhà", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("mặt trước nhà", 9, args); });

    // phòng khách 2 -> r11 (index 10)
    mcpClient.registerTool("living_room_lights_2", "Điều khiển đèn phòng khách 2", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("phòng khách 2", 10, args); });

    // phòng khách 1 -> r12 (index 11)
    mcpClient.registerTool("living_room_lights_1", "Điều khiển đèn phòng khách 1", R"({"type":"object","properties":{"state":{"type":"string","enum":["on","off"]}},"required":["state"]})", 
        [controlRelay](const String& args){ return controlRelay("phòng khách 1", 11, args); });

    // Away Mode: Turn off all lights and devices, and arm the system
    mcpClient.registerTool("away_mode", "Chế độ vắng nhà: Tắt tất cả thiết bị và kích hoạt báo động", R"({})", 
        [](const String& args){
            turnOffAllRelays();
            setArmedState(true);
            return WebSocketMCP::ToolResponse("{\"success\":true,\"mode\":\"away\",\"message\":\"All devices OFF and system ARMED\"}");
        });

    // Home Mode: Disarm the system and turn on essential lights
    mcpClient.registerTool("home_mode", "Chế độ về nhà: Tắt báo động và bật đèn cơ bản", R"({})", 
        [](const String& args){
            setArmedState(false);
            setRelay(0, 0); // Toilet ON
            setRelay(11, 0); // Living Room 1 ON
            return WebSocketMCP::ToolResponse("{\"success\":true,\"mode\":\"home\",\"message\":\"System DISARMED and Welcome Home lights ON\"}");
        });
}

void initMCP() {
    mcpClient.begin(mcpEndpoint, onConnectionStatus);
}

void handleMCP() {
    mcpClient.loop();
}
