#ifndef INPUT_H
#define INPUT_H

#include "game_systems.h"

class CInput : public CGameSystem
{
public:
    CInput();

    static CInput* GetInstance()
    {
        static CInput input;
        return &input;
    }

    bool Init() { return true; }
    void Destroy() {}

    void Update() {}
    void PostRender();

    void SetKeyUp(int keycode);
    void SetKeyDown(int keycode);

    bool KeyUp(int keycode);
    bool KeyPressed(int keycode);
    bool KeyHeld(int keycode);
    bool KeyReleased(int keycode);

protected:
private:

    char *m_pCurrentState;
    char *m_pPreviousState;

    char m_szKeyBuffer1[255];
    char m_szKeyBuffer2[255];
};

extern CInput* g_pInput;

#define VK_0			0x30 	// 0 key
#define VK_1			0x31 	// 1 key
#define VK_2			0x32 	// 2 key
#define VK_3			0x33 	// 3 key
#define VK_4			0x34 	// 4 key
#define VK_5			0x35 	// 5 key
#define VK_6			0x36 	// 6 key
#define VK_7			0x37 	// 7 key
#define VK_8			0x38 	// 8 key
#define VK_9			0x39 	// 9 key
#define VK_A			0x41 	// A key
#define VK_B			0x42 	// B key
#define VK_C			0x43 	// C key
#define VK_D			0x44 	// D key
#define VK_E			0x45 	// E key
#define VK_F			0x46 	// F key
#define VK_G			0x47 	// G key
#define VK_H			0x48 	// H key
#define VK_I			0x49 	// I key
#define VK_J			0x4A 	// J key
#define VK_K			0x4B 	// K key
#define VK_L			0x4C 	// L key
#define VK_M			0x4D 	// M key
#define VK_N			0x4E 	// N key
#define VK_O			0x4F 	// O key
#define VK_P			0x50 	// P key
#define VK_Q			0x51 	// Q key
#define VK_R			0x52 	// R key
#define VK_S			0x53 	// S key
#define VK_T			0x54 	// T key
#define VK_U			0x55 	// U key
#define VK_V			0x56 	// V key
#define VK_W			0x57 	// W key
#define VK_X			0x58 	// X key
#define VK_Y			0x59 	// Y key
#define VK_Z			0x5A 	// Z key

#endif // INPUT_H