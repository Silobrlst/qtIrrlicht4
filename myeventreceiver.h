#include <QDebug>
#include <irrlicht.h>

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;
using namespace gui;

class MyEventReceiver : public IEventReceiver
{
public:
    IrrlichtDevice *device;

    virtual bool OnEvent(const SEvent& event)
    {
        // Escape swaps Camera Input
        if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown == false)
        {
            if ( OnKeyUp(event.KeyInput.Key) )
                return true;
        }

        if (event.EventType == EET_GUI_EVENT)
        {
            switch(event.GUIEvent.EventType)
            {
            case EGET_MENU_ITEM_SELECTED:
                // a menu item was clicked
                OnMenuItemSelected( (IGUIContextMenu*)event.GUIEvent.Caller );
                break;
            default:
                break;
            }
        }

        return false;
    }

    bool OnKeyUp(irr::EKEY_CODE keyCode)
    {
        if (keyCode == irr::KEY_ESCAPE)
        {
            if (device)
            {
                scene::ICameraSceneNode * camera = device->getSceneManager()->getActiveCamera();
                if (camera)
                {
                    camera->setInputReceiverEnabled( !camera->isInputReceiverEnabled() );
                }
                return true;
            }
        }
        return false;
    }

    void OnMenuItemSelected( IGUIContextMenu* menu )
    {
        s32 id = menu->getItemCommandId(menu->getSelectedItem());
        IGUIEnvironment* env = device->getGUIEnvironment();
    }

    void setDevice(IrrlichtDevice *deviceIn){
        device = deviceIn;
    }
};
