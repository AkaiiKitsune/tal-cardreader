#include "cardio.h"
char module[] = "TAL HID";

static bool CARDIO_RUNNER_INITIALIZED = false;

static HANDLE CARDIO_POLL_THREAD;
static bool CARDIO_POLL_STOP_FLAG;
static bool waitingForTouch = false;
static bool HasCard = false;

typedef void (*callbackTouch)(i32, i32, u8[168], u64);
callbackTouch touchCallback;
u64 touchData;
static char AccessID[21] = "00000000000000000001";
static u8 cardData[168] = {0x01, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x2E, 0x58, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x5C, 0x97, 0x44, 0xF0, 0x88, 0x04, 0x00, 0x43, 0x26, 0x2C, 0x33, 0x00, 0x04,
                           0x06, 0x10, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
                           0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
                           0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4E, 0x42, 0x47, 0x49, 0x43, 0x36,
                           0x00, 0x00, 0xFA, 0xE9, 0x69, 0x00, 0xF6, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void PollReader()
{
    // update HID devices
    EnterCriticalSection(&CARDIO_HID_CRIT_SECTION);
    for (size_t device_no = 0; device_no < CARDIO_HID_CONTEXTS_LENGTH; device_no++)
    {
        struct cardio_hid_device *device = &CARDIO_HID_CONTEXTS[device_no];

        // get status
        cardio_hid_poll_value_t status = cardio_hid_device_poll(device);
        if (status == HID_POLL_CARD_READY)
        {

            // read card
            if (cardio_hid_device_read(device) == HID_CARD_NONE)
                continue;

            // if card not empty
            if (*((uint64_t *)&device->u.usage_value[0]) > 0)
            {
                printWarning("%s (%s): Read card %02X%02X%02X%02X%02X%02X%02X%02X\n", __func__, module, device->u.usage_value[0], device->u.usage_value[1], device->u.usage_value[2], device->u.usage_value[3], device->u.usage_value[4], device->u.usage_value[5], device->u.usage_value[6], device->u.usage_value[7]);

                if (waitingForTouch)
                {
                    // Convert card to proper format
                    sprintf(AccessID, "%020llu", device->u.usage64[0]);
                    // Format the AccessID in a way that doesn't crash the game.
                    // memset(AccessID, '0', 10);

                    HasCard = true;
                }
            }
        }
    }
    LeaveCriticalSection(&CARDIO_HID_CRIT_SECTION);
    Sleep(15);
}

static unsigned int __stdcall cardio_poll_thread_proc(void *ctx)
{
    while (!CARDIO_POLL_STOP_FLAG)
    {
        PollReader();
    }
    return 0;
}

void cardio_runner_start()
{
    // initialize
    if (!CARDIO_RUNNER_INITIALIZED)
    {
        CARDIO_RUNNER_INITIALIZED = true;
        printWarning("%s (%s): Initializing CARDIO\n", __func__, module);

        // initialize
        if (!cardio_hid_init())
        {
            printWarning("%s (%s): Couldn't init CARDIO HID\n", __func__, module);
            return;
        }

        // scan HID devices
        if (!cardio_hid_scan())
        {
            printWarning("%s (%s): Couldn't scan for CARDIO devices\n", __func__, module);
            return;
        }
    }
}

void cardio_runner_stop()
{
    // shutdown HID
    cardio_hid_close();

    // set initialized to false
    CARDIO_RUNNER_INITIALIZED = false;
}

void Init()
{
    printWarning("%s (%s): Starting HID Service...\n", __func__, module);
    // Find and initialize reader(s)
    cardio_runner_start();

    // Start reader thread
    CARDIO_POLL_STOP_FLAG = false;
    CARDIO_POLL_THREAD = (HANDLE)_beginthreadex(
        NULL,
        0,
        cardio_poll_thread_proc,
        NULL,
        0,
        NULL);
}

void Update()
{
    if (HasCard)
    {
        HasCard = false;

        // Generating a ChipID Derived from the AccessID
        char ChipID[33] = "000000000000";
        strcat(ChipID, AccessID);

        // Insert card in game
        printWarning("%s (%s): Inserting Card %s, with ChipID %s\n", __func__, module, AccessID, ChipID);
        memcpy(cardData + 0x2C, ChipID, 33);
        memcpy(cardData + 0x50, AccessID, 21);
        touchCallback(0, 0, cardData, touchData);
    }
}

void WaitTouch(i32 (*callback)(i32, i32, u8[168], u64), u64 data)
{
    printWarning("%s (%s): Waiting for touch\n", __func__, module);
    waitingForTouch = true;
    touchCallback = callback;
    touchData = data;
}

void Exit()
{
    cardio_runner_stop();
    CARDIO_POLL_STOP_FLAG = true;
    WaitForSingleObject(CARDIO_POLL_THREAD, INFINITE);
    CloseHandle(CARDIO_POLL_THREAD);
    CARDIO_POLL_THREAD = NULL;
    CARDIO_POLL_STOP_FLAG = false;
}
