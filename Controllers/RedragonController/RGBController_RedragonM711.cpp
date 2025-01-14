/*-----------------------------------------*\
|  RGBController_RedragonM711.cpp           |
|                                           |
|  Generic RGB Interface for Redragon M711  |
|  Cobra RGB Mouse                          |
|                                           |
|  Adam Honse (CalcProgrammer1) 3/25/2020   |
\*-----------------------------------------*/

#include "RGBController_RedragonM711.h"

/**------------------------------------------------------------------*\
    @name Redragon Mice
    @category Mouse
    @type USB
    @save :robot:
    @direct :x:
    @effects :white_check_mark:
    @detectors DetectRedragonMice
    @comment
\*-------------------------------------------------------------------*/

RGBController_RedragonM711::RGBController_RedragonM711(RedragonM711Controller* controller_ptr)
{
    controller  = controller_ptr;

    name        = "Redragon Mouse Device";
    vendor      = "Redragon";
    type        = DEVICE_TYPE_MOUSE;
    description = "Redragon Mouse Device";
    location    = controller->GetDeviceLocation();
    serial      = controller->GetSerialString();

    mode Static;
    Static.name       = "Static";
    Static.value      = REDRAGON_M711_MODE_STATIC;
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_AUTOMATIC_SAVE;
    Static.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    mode Wave;
    Wave.name       = "Wave";
    Wave.value      = REDRAGON_M711_MODE_WAVE;
    Wave.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_AUTOMATIC_SAVE;
    Wave.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Wave);

    mode Breathing;
    Breathing.name       = "Breathing";
    Breathing.value      = REDRAGON_M711_MODE_BREATHING;
    Breathing.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_AUTOMATIC_SAVE;
    Breathing.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Breathing);

    mode Rainbow;
    Rainbow.name       = "Rainbow";
    Rainbow.value      = REDRAGON_M711_MODE_RAINBOW;
    Rainbow.flags      = MODE_FLAG_AUTOMATIC_SAVE;
    Rainbow.color_mode = MODE_COLORS_NONE;
    modes.push_back(Rainbow);

    mode Flashing;
    Flashing.name       = "Flashing";
    Flashing.value      = REDRAGON_M711_MODE_FLASHING;
    Flashing.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_AUTOMATIC_SAVE;
    Flashing.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Flashing);

    SetupZones();
}

RGBController_RedragonM711::~RGBController_RedragonM711()
{
    delete controller;
}

void RGBController_RedragonM711::SetupZones()
{
    zone m711_zone;
    m711_zone.name           = "Mouse";
    m711_zone.type           = ZONE_TYPE_SINGLE;
    m711_zone.leds_min       = 1;
    m711_zone.leds_max       = 1;
    m711_zone.leds_count     = 1;
    m711_zone.matrix_map     = NULL;
    zones.push_back(m711_zone);

    led m711_led;
    m711_led.name = "Mouse";
    leds.push_back(m711_led);

    SetupColors();
}

void RGBController_RedragonM711::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_RedragonM711::DeviceUpdateLEDs()
{
    unsigned char red = RGBGetRValue(colors[0]);
    unsigned char grn = RGBGetGValue(colors[0]);
    unsigned char blu = RGBGetBValue(colors[0]);

    controller->SendMouseColor(red, grn, blu);
    controller->SendMouseApply();
}

void RGBController_RedragonM711::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_RedragonM711::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_RedragonM711::DeviceUpdateMode()
{
    bool random       = (modes[active_mode].color_mode == MODE_COLORS_RANDOM);
    unsigned char red = RGBGetRValue(colors[0]);
    unsigned char grn = RGBGetGValue(colors[0]);
    unsigned char blu = RGBGetBValue(colors[0]);

    if((modes[active_mode].value == REDRAGON_M711_MODE_BREATHING) && random)
    {
        controller->SendMouseMode(REDRAGON_M711_MODE_RANDOM_BREATHING, 0, red, grn, blu);
    }
    else
    {
        controller->SendMouseMode(modes[active_mode].value, 0, red, grn, blu);
    }

    controller->SendMouseApply();
}
