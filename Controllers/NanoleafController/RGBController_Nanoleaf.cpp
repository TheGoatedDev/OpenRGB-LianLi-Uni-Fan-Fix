/*-----------------------------------------*\
|  RGBController_Nanoleaf.cpp               |
|                                           |
|  Generic RGB Interface for Nanoleaf       |
|                                           |
|  Nikita Rushmanov 01/13/2022              |
\*-----------------------------------------*/

#include "RGBController_Nanoleaf.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include <curl/curl.h>
#include "json.hpp"
using json = nlohmann::json;

RGBController_Nanoleaf::RGBController_Nanoleaf(std::string a_address, int a_port, std::string a_auth_token) :
    controller(a_address, a_port, a_auth_token)
{
    location    = a_address+":"+std::to_string(a_port);
    name        = controller.GetName();
    serial      = controller.GetSerial();
    vendor      = controller.GetManufacturer();
    version     = controller.GetFirmwareVersion();
    description = controller.GetModel();
    type        = DEVICE_TYPE_LIGHT;

    /*-------------------------------------------------------------*\
    | Direct mode currently only supported for Nanoleaf Panels.     |
    \*-------------------------------------------------------------*/
    if(controller.GetModel() == NANOLEAF_LIGHT_PANELS_MODEL)
    {
        mode Direct;
        Direct.name       = "Direct";
        Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
        Direct.color_mode = MODE_COLORS_PER_LED;
        modes.push_back(Direct);

        /*---------------------------------------------------------*\
        | Set this effect as current if the name is selected.       |
        \*---------------------------------------------------------*/
        if(controller.GetSelectedEffect() == NANOLEAF_DIRECT_MODE_EFFECT_NAME)
        {
            /*-----------------------------------------------------*\
            | If the direct mode is active, we need to call this    |
            | method to open the socket.                            |
            \*-----------------------------------------------------*/
            controller.StartExternalControl();
            active_mode = 0;
        }
    }

    /*-------------------------------------------------------------*\
    | Create additional modes from device effects list              |
    \*-------------------------------------------------------------*/
    for(std::vector<std::string>::const_iterator it = controller.GetEffects().begin(); it != controller.GetEffects().end(); ++it)
    {
        mode effect;
        effect.name             = *it;
        effect.flags            = MODE_FLAG_HAS_BRIGHTNESS;
        effect.color_mode       = MODE_COLORS_NONE;
        effect.brightness_max   = 100;
        effect.brightness_min   = 0;
        effect.brightness       = 100;

        modes.push_back(effect);

        /*---------------------------------------------------------*\
        | Set this effect as current if the name is selected.       |
        \*---------------------------------------------------------*/
        if(controller.GetSelectedEffect() == effect.name)
        {
            active_mode         = modes.size() - 1;
        }
    }

    SetupZones();
}

void RGBController_Nanoleaf::SetupZones()
{
    zone led_zone;
    led_zone.name           = "Nanoleaf Layout";
    led_zone.type           = ZONE_TYPE_LINEAR;
    led_zone.leds_count     = controller.GetPanelIds().size();
    led_zone.leds_min       = led_zone.leds_count;
    led_zone.leds_max       = led_zone.leds_count;
    led_zone.matrix_map     = NULL;

    for(std::vector<int>::const_iterator it = controller.GetPanelIds().begin(); it != controller.GetPanelIds().end(); ++it)
    {
        led new_led;
        new_led.name = std::to_string(*it);
        leds.push_back(new_led);
    }

    zones.push_back(led_zone);

    SetupColors();
}

void RGBController_Nanoleaf::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_Nanoleaf::DeviceUpdateLEDs()
{
    if(controller.GetModel() == NANOLEAF_LIGHT_PANELS_MODEL)
    {
        controller.UpdateLEDs(colors);
    }
}

void RGBController_Nanoleaf::UpdateZoneLEDs(int /*zone*/)
{
	DeviceUpdateLEDs();
}

void RGBController_Nanoleaf::UpdateSingleLED(int /*led*/)
{
	DeviceUpdateLEDs();
}

void RGBController_Nanoleaf::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_Nanoleaf::DeviceUpdateMode()
{
    /*---------------------------------------------------------*\
    | Mode 0 is reserved for Direct mode                        |
    \*---------------------------------------------------------*/
    if(active_mode == 0 && controller.GetModel() == NANOLEAF_LIGHT_PANELS_MODEL)
    {
        controller.StartExternalControl();
    }
    /*---------------------------------------------------------*\
    | Update normal effects.                                    |
    \*---------------------------------------------------------*/
    else
    {
        controller.SelectEffect(modes[active_mode].name);
        controller.SetBrightness(modes[active_mode].brightness);
    }
}
