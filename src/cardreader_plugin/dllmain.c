#include "CardIO/cardio.h"
#include "SmartCard/scard.h"
char module[] = "CardReader";

// Reader Thread
static bool READER_RUNNER_INITIALIZED = false;
static HANDLE READER_POLL_THREAD;
static bool READER_POLL_STOP_FLAG;

// SmartCard Specific

// Game specific
static bool HasCard = false;

// Misc
bool usingSmartCard = false;
int readCooldown = 200;

char AccessID[21] = "00000000000000000001";

static unsigned int __stdcall reader_poll_thread_proc(void *ctx)
{
    while (!READER_POLL_STOP_FLAG)
    {
        if (HasCard && !usingSmartCard) // A Cardio scan is already in progress, SmartCard doesn't need any extra cooldown.
        {
            Sleep(500);
        }
        else
        {

            uint8_t UID[8] = {0};

            // update devices
            if (!usingSmartCard) // CardIO
            {
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
                            for (int i = 0; i < 8; ++i)
                                UID[i] = device->u.usage_value[i];
                    }
                }
                LeaveCriticalSection(&CARDIO_HID_CRIT_SECTION);
                Sleep(readCooldown);
            }
            else // SmartCard
            {
                scard_update(UID);
            }

            if (UID[0] > 0) // If a card was read, format it properly and set HasCard to true so the game can insert it on next frame.
            {
                printWarning("%s (%s): Read card %02X%02X%02X%02X%02X%02X%02X%02X\n", __func__, module, UID[0], UID[1], UID[2], UID[3], UID[4], UID[5], UID[6], UID[7]);

                // Properly format the AccessID
                u64 ReversedAccessID;
                for (int i = 0; i < 8; i++)
                    ReversedAccessID = (ReversedAccessID << 8) | UID[i];
                sprintf(AccessID, "%020llu", ReversedAccessID);

                HasCard = true;
            }
        }
    }

    return 0;
}

bool reader_runner_start()
{
    // initialize
    if (!READER_RUNNER_INITIALIZED)
    {
        READER_RUNNER_INITIALIZED = true;

        if (!usingSmartCard)
        { // CardIO INIT
            printWarning("%s (%s): Initializing CardIO\n", __func__, module);

            // Initialize
            if (!cardio_hid_init())
            {
                printError("%s (%s): Couldn't init CardIO\n", __func__, module);
                return FALSE;
            }

            // Scan HID devices
            if (!cardio_hid_scan())
            {
                printError("%s (%s): Couldn't scan for CardIO devices\n", __func__, module);
                return FALSE;
            }
        }
        else
        { // SmartCard INIT
            printWarning("%s (%s): Initializing SmartCard\n", __func__, module);

            if (!scard_init())
            {
                //  Initialize
                printError("%s (%s): Couldn't init SmartCard\n", __func__, module);
                return FALSE;
            }
        }

        return TRUE; // Init success
    }

    return TRUE;
}

void reader_runner_stop()
{
    // shutdown HID
    if (!usingSmartCard)
    {
        cardio_hid_close();
    }

    // set initialized to false
    READER_RUNNER_INITIALIZED = false;
}

void Init()
{
    printWarning("%s (%s): Starting HID Service...\n", __func__, module);

    // Read config
    toml_table_t *config = openConfig(configPath("plugins/cardreader.toml"));
    if (config)
    {
        usingSmartCard = readConfigBool(config, "using_smartcard", false);
        readCooldown = readConfigInt(config, "read_cooldown", usingSmartCard ? 500 : 50);
        toml_free(config);
    }
    else
    {
        printError("%s (%s): Config file not found\n", __func__, module);
        return;
    }

    printInfo("%s (%s): Using %s as reader type\n", __func__, module, usingSmartCard ? "Smart Card" : "cardIO");
    printInfo("%s (%s): Card read cooldown set to %d ms\n", __func__, module, readCooldown);

    //  Find and initialize reader(s)
    if (!reader_runner_start())
        return;

    printWarning("%s (%s): Starting reader thread.\n", __func__, module);

    // Start reader thread
    READER_POLL_STOP_FLAG = false;
    READER_POLL_THREAD = (HANDLE)_beginthreadex(
        NULL,
        0,
        reader_poll_thread_proc,
        NULL,
        0,
        NULL);
}

void UsingQr()
{
}

bool CheckQr()
{
    if (HasCard)
    {
        // Insert card in game
        printInfo("%s (%s): Inserting QR Code %s\n", __func__, module, AccessID);
        return TRUE;
    }
    return FALSE;
}

int GetQr(int len_limit, uint8_t *buffer)
{
    HasCard = FALSE;
    char tempAccessID[30];
    strcpy(tempAccessID, "BNTTCNID"); // Appending "BNTTCNID" so the qrcode gets used as a login code
    strcat(tempAccessID, AccessID);
    memcpy(buffer, tempAccessID, 30);
    return 30;
}

void Exit()
{
    reader_runner_stop();
    READER_POLL_STOP_FLAG = true;
    WaitForSingleObject(READER_POLL_THREAD, INFINITE);
    CloseHandle(READER_POLL_THREAD);
    READER_POLL_THREAD = NULL;
    READER_POLL_STOP_FLAG = false;
}
