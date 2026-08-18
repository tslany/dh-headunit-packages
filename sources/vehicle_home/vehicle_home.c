/*
 * AppHomeMenu half of the Vehicle Information feature.
 *
 * All three pinned regional executables load this DSO. It preserves the OEM
 * model and UI paths:
 *
 *   - EU gets one native Vehicle IconItem after its six stock front items.
 *   - viewId 4 gets native Fuel Economy and Climate rows before the stock
 *     regional children. The OEM CurvedList retains focus, touch, rotary,
 *     paging, DRS, and Back behavior.
 *   - Climate emits the same UISH event as the physical CLIMATE button.
 *   - Fuel uses (58, 4) only as a QML-local row marker. The Vehicle overlay
 *     consumes it before LaunchApplication, so no OEM application is routed.
 */

typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;
#if __SIZEOF_POINTER__ == 8
typedef unsigned long usize;
typedef signed long ssize;
typedef unsigned long uptr;
#else
typedef unsigned int usize;
typedef signed int ssize;
typedef unsigned int uptr;
#endif

#define EXPORT __attribute__((visibility("default")))
#define CONSTRUCTOR __attribute__((constructor))

#define BUILD_ID "vehicle-home-v1"
#define VEHICLE_APP_ID 120
#define VEHICLE_VIEW_ID 4
#define HOME_INFO_APP_ID 58
#define FUEL_SENTINEL_VIEW_ID 4
#define CLIMATE_SENTINEL_APP_ID 121
#define CLIMATE_VIEW_ID 12
#define UISH_EVENT_SOFT_HVAC 71
#define UISH_APP_INFO 9
#define UISH_APP_UI_MANAGER 1
#define UISH_EVENT_IGNITION_CHANGED 3209
#define IGNITION_PAYLOAD_BYTES 24
#define SYS_LAST_STATUS_BYTES 3
#define SYS_LAST_STATUS_IGNITION_BYTE 1
#define SYS_LAST_STATUS_IGNITION_MASK 0x02u

static int should_append_main(u32 country, int screen, u32 view_id, int rows)
{
    return country == 0x20u && screen == 1 && view_id == 0u && rows == 6;
}

static int should_prepend_rows(int screen, u32 view_id, int rows)
{
    return screen == 1 && view_id == VEHICLE_VIEW_ID && rows == 0;
}

static int is_fuel_row(int app_id, int view_id)
{
    return app_id == HOME_INFO_APP_ID && view_id == FUEL_SENTINEL_VIEW_ID;
}

static int is_climate_row(int app_id, int view_id)
{
    return app_id == CLIMATE_SENTINEL_APP_ID && view_id == CLIMATE_VIEW_ID;
}

static int display_for_screen(int screen)
{
    if (screen == 1) return 1;
    if (screen == 2) return 2;
    return 4;
}

static int ignition_value(int event_id, int payload_bytes, int value)
{
    if (event_id != UISH_EVENT_IGNITION_CHANGED ||
            payload_bytes != IGNITION_PAYLOAD_BYTES ||
            (value != 0 && value != 1))
        return -1;
    return value;
}

static int ignition_from_last_status(const u8 *status, int status_bytes)
{
    if (!status || status_bytes != SYS_LAST_STATUS_BYTES) return -1;
    return (status[SYS_LAST_STATUS_IGNITION_BYTE] &
            SYS_LAST_STATUS_IGNITION_MASK) ? 1 : 0;
}

static int removes_injected_rows(int row, int count)
{
    return row >= 0 && row < 2 && count > 0;
}

EXPORT const char *vh_build_id(void) { return BUILD_ID; }
EXPORT int vh_should_append_for_test(u32 country, int screen,
                                     u32 view_id, int rows)
{
    return should_append_main(country, screen, view_id, rows);
}
EXPORT int vh_should_prepend_for_test(int screen, u32 view_id, int rows)
{
    return should_prepend_rows(screen, view_id, rows);
}
EXPORT int vh_is_fuel_route_for_test(int app_id, int view_id)
{
    return is_fuel_row(app_id, view_id);
}
EXPORT int vh_is_climate_route_for_test(int app_id, int view_id)
{
    return is_climate_row(app_id, view_id);
}
EXPORT int vh_display_for_screen_for_test(int screen)
{
    return display_for_screen(screen);
}
EXPORT int vh_ignition_value_for_test(int event_id, int payload_bytes,
                                      int value)
{
    return ignition_value(event_id, payload_bytes, value);
}
EXPORT int vh_ignition_from_last_status_for_test(const u8 *status,
                                                  int status_bytes)
{
    return ignition_from_last_status(status, status_bytes);
}
EXPORT int vh_removes_injected_for_test(int row, int count)
{
    return removes_injected_rows(row, count);
}
EXPORT int vh_vehicle_app_id(void) { return VEHICLE_APP_ID; }
EXPORT int vh_vehicle_view_id(void) { return VEHICLE_VIEW_ID; }
EXPORT int vh_home_info_app_id(void) { return HOME_INFO_APP_ID; }
EXPORT int vh_fuel_view_id(void) { return FUEL_SENTINEL_VIEW_ID; }
EXPORT int vh_climate_app_id(void) { return CLIMATE_SENTINEL_APP_ID; }
EXPORT int vh_climate_view_id(void) { return CLIMATE_VIEW_ID; }

#ifndef VH_HOST_TEST

extern void *memcpy(void *, const void *, usize);
extern void *memset(void *, int, usize);
extern int memcmp(const void *, const void *, usize);
extern ssize readlink(const char *, char *, usize);
extern int mprotect(void *, usize, int);
extern void *mmap(void *, usize, int, int, int, signed long);
extern int munmap(void *, usize);
extern void syslog(int, const char *, ...);
extern void *cpp_new(u32) __asm__("_Znwj");
extern void qstring_construct(void *, const char *)
    __asm__("_ZN7QStringC1EPKc");
extern void qstring_destroy(void *) __asm__("_ZN7QStringD1Ev");
extern void qbytearray_construct(void *, const char *, int)
    __asm__("_ZN10QByteArrayC1EPKci");
extern void qbytearray_destroy(void *) __asm__("_ZN10QByteArrayD1Ev");
extern void event_construct(void *, int, int, int, const void *)
    __asm__("_ZN5EventC1Ei8APP_ID_TS0_RK10QByteArray");
extern void event_destroy(void *) __asm__("_ZN5EventD1Ev");
extern int event_get_id(const void *)
    __asm__("_ZNK5Event10GetEventIdEv");
extern const void *event_data(const void *)
    __asm__("_ZNK5Event4DataEv");
extern void notify_uish(void *, void *, int)
    __asm__("_ZN13AppEngineBase10NotifyUISHER5Event9DISPLAY_T");
extern void *qlist_detach(void *) __asm__("_ZN9QListData6detachEv");
extern void **qlist_append(void *) __asm__("_ZN9QListData6appendEv");
extern int qobject_connect(const void *, const char *, const void *,
                           const char *, int)
    __asm__("_ZN7QObject7connectEPKS_PKcS1_S3_N2Qt14ConnectionTypeE");
struct dbus_connection;
struct dbus_message;
extern struct dbus_connection *dbus_bus_get_private(int, void *);
extern void dbus_connection_set_exit_on_disconnect(
    struct dbus_connection *, int);
extern void dbus_connection_close(struct dbus_connection *);
extern void dbus_connection_unref(struct dbus_connection *);
extern struct dbus_message *dbus_message_new_method_call(
    const char *, const char *, const char *, const char *);
extern struct dbus_message *dbus_connection_send_with_reply_and_block(
    struct dbus_connection *, struct dbus_message *, int, void *);
extern int dbus_message_get_args(struct dbus_message *, void *, int, ...);
extern void dbus_message_unref(struct dbus_message *);

#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define MAP_PRIVATE 2
#define MAP_ANONYMOUS 0x20
#define DBUS_BUS_SESSION 0
#define DBUS_TYPE_INVALID 0
#define DBUS_TYPE_BYTE ((int)'y')
#define DBUS_TYPE_ARRAY ((int)'a')
#define MICOM_QUERY_TIMEOUT_MS 1000

struct qstring_native { void *data; };
struct qbytearray_native { void *data; };
struct qbytearray_data_native {
    int ref;
    int alloc;
    int size;
    const u8 *bytes;
};
struct event_native { u32 words[4]; };
struct qlist_native { void *data; };
struct model_index_native {
    int row;
    int column;
    void *internal;
    const void *model;
};
typedef void (*fill_app_list_fn)(void *, void *, int, u32);
typedef void (*launch_application_fn)(void *, int, int, int, int,
                                      const void *);
typedef void (*notify_common_fn)(void *, const void *);
typedef void (*item_data_changed_fn)(void *);
typedef int (*remove_rows_fn)(void *, int, int, const void *);
typedef int (*row_count_fn)(void *, const struct model_index_native *);
typedef void (*icon_item_constructor_fn)(
    void *, const int *, const int *, const struct qstring_native *,
    const struct qstring_native *, int, int, int,
    const struct qlist_native *);
typedef void (*append_items_fn)(void *, const struct qlist_native *);
typedef void (*qlist_destructor_fn)(struct qlist_native *);

struct home_profile {
    const char *name;
    u32 qlist_shared_null;
    u32 launch;
    u32 notify_common;
    u32 remove_rows;
    u32 fill;
    u32 icon_constructor;
    u32 append_items;
    u32 qlist_destructor;
    u32 item_data_changed;
    int append_eu_main;
};

struct hook_record {
    u8 *target;
    u32 stolen;
    u8 original[16];
    u8 *trampoline;
};

static const struct home_profile k_profiles[] = {
    {"eu",  0x080b82f8u, 0x08057b40u, 0x08064070u, 0x0806e350u,
     0x0808f240u,
     0x0806a7e0u, 0x0806e7b0u, 0x0806ec30u, 0x080990f0u, 1},
    {"na",  0x080ae958u, 0x08055790u, 0x08061c10u, 0x0806be80u,
     0x08087d40u,
     0x08068310u, 0x0806c2e0u, 0x0806c760u, 0x08091a40u, 0},
    {"kdm", 0x080bb998u, 0x08059630u, 0x08065970u, 0x0806fc10u,
     0x08091920u,
     0x0806c0a0u, 0x08070070u, 0x080704f0u, 0x0809bd40u, 0},
};

static const u8 k_fill_signature[16] = {
    0x55, 0x89, 0xe5, 0x57, 0x56, 0x53, 0x8d, 0xa4,
    0x24, 0x74, 0xff, 0xff, 0xff, 0x8b, 0x55, 0x0c,
};
static const u8 k_hook_prologue[5] = {0x55, 0x89, 0xe5, 0x57, 0x56};
static const u8 k_remove_rows_prologue[5] = {
    0x55, 0x31, 0xc0, 0x89, 0xe5,
};
static const u8 k_icon_signature[16] = {
    0x55, 0x89, 0xe5, 0x8d, 0x64, 0x24, 0xd8, 0xc7,
    0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, 0x89,
};
static const u8 k_append_signature[16] = {
    0x55, 0x89, 0xe5, 0x57, 0x56, 0x8d, 0x55, 0xc8,
    0x53, 0x8d, 0xa4, 0x24, 0x64, 0xff, 0xff, 0xff,
};
static const u8 k_list_destructor_signature[16] = {
    0x55, 0x8d, 0x2c, 0x24, 0x8b, 0x55, 0x08, 0x8b,
    0x02, 0xf0, 0xff, 0x08, 0x0f, 0x95, 0xc1, 0x84,
};
static const u8 k_item_data_changed_signature[16] = {
    0x55, 0x89, 0xe5, 0x8d, 0x64, 0x24, 0xe8, 0xc7,
    0x44, 0x24, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xc7,
};

static const char k_process[] = "/app/bin/AppHomeMenu";
static const char k_vehicle_marker[] = "APP_ID_VEHICLE_DIAGNOSTICS";
static const char k_vehicle_image[] =
    "/app/share/images/AppHome/main/ico_home_234_vehicle";
static const char k_fuel_text[] = "Fuel Economy";
static const char k_fuel_image[] =
    "/app/share/images/AppHome/2dep/icon_2dep_driving_info";
static const char k_climate_text[] = "STR_ASSISTANT_WINDOW_CLIMATE";
static const char k_climate_image[] =
    "/app/share/images/AppHome/2dep/ico_2dep_climate";
static const char k_value_signal[] =
    "2ValueChanged( HSDefP::APP_DATA_T, QVariant )";
static const char k_update_slot[] =
    "1UpdateDataField( HSDefP::APP_DATA_T, QVariant )";
static const char k_micom_service[] = "com.lge.car.micom";
static const char k_micom_path[] = "/SysInfo";
static const char k_micom_interface[] = "com.lge.car.micom.sysinfo";
static const char k_micom_last_status[] = "LgeDBus_AskSysLastStatus";

static const struct home_profile *g_profile;
static struct hook_record g_launch_hook;
static struct hook_record g_common_hook;
static struct hook_record g_remove_hook;
static struct hook_record g_fill_hook;
static launch_application_fn g_original_launch;
static notify_common_fn g_original_common;
static remove_rows_fn g_original_remove;
static fill_app_list_fn g_original_fill;
static volatile int g_ignition_on;
static volatile int g_ignition_known;
static int g_ignition_query_done;
static void *g_fuel_item;
static void *g_climate_item;
static void *g_vehicle_model;

static void clear_vehicle_row_references(void)
{
    g_fuel_item = 0;
    g_climate_item = 0;
    g_vehicle_model = 0;
    __sync_synchronize();
}

static void update_vehicle_row_state(int ignition_on)
{
    void *fuel;
    void *climate;
    int changed = ignition_on != g_ignition_on;
    g_ignition_on = ignition_on;
    __sync_synchronize();
    g_ignition_known = 1;
    if (!changed) return;
    fuel = g_fuel_item;
    climate = g_climate_item;
    if (fuel) {
        *((u8 *)fuel + 0x24u) = (u8)ignition_on;
        ((item_data_changed_fn)(uptr)g_profile->item_data_changed)(fuel);
    }
    if (climate) {
        *((u8 *)climate + 0x24u) = (u8)ignition_on;
        ((item_data_changed_fn)(uptr)g_profile->item_data_changed)(climate);
    }
    syslog(6, "vehicle-home: Vehicle rows %s for ignition",
           ignition_on ? "enabled" : "disabled");
}

static int query_current_ignition(void)
{
    struct dbus_connection *connection;
    struct dbus_message *request;
    struct dbus_message *reply;
    u8 *status = 0;
    int status_bytes = 0;
    int ignition_on = -1;
    connection = dbus_bus_get_private(DBUS_BUS_SESSION, 0);
    if (!connection) return -1;
    dbus_connection_set_exit_on_disconnect(connection, 0);
    request = dbus_message_new_method_call(
        k_micom_service, k_micom_path, k_micom_interface,
        k_micom_last_status);
    if (!request) {
        dbus_connection_close(connection);
        dbus_connection_unref(connection);
        return -1;
    }
    reply = dbus_connection_send_with_reply_and_block(
        connection, request, MICOM_QUERY_TIMEOUT_MS, 0);
    dbus_message_unref(request);
    if (reply) {
        if (dbus_message_get_args(
                reply, 0, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE,
                &status, &status_bytes, DBUS_TYPE_INVALID))
            ignition_on = ignition_from_last_status(status, status_bytes);
        dbus_message_unref(reply);
    }
    dbus_connection_close(connection);
    dbus_connection_unref(connection);
    return ignition_on;
}

static void initialize_ignition_state(void)
{
    int ignition_on;
    if (g_ignition_query_done) return;
    g_ignition_query_done = 1;
    ignition_on = query_current_ignition();
    if (ignition_on < 0) {
        if (!g_ignition_known) g_ignition_on = 0;
        syslog(3, "vehicle-home: current ignition query failed; state %s",
               g_ignition_known ? "retained" : "failed closed");
        return;
    }
    update_vehicle_row_state(ignition_on);
    syslog(6, "vehicle-home: initial ignition state %s",
           ignition_on ? "on" : "off");
}

static int read_ignition_event(const void *event)
{
    const struct qbytearray_native *array;
    const struct qbytearray_data_native *data;
    int value;
    int event_id;
    if (!event) return -1;
    event_id = event_get_id(event);
    if (event_id != UISH_EVENT_IGNITION_CHANGED) return -1;
    array = (const struct qbytearray_native *)event_data(event);
    data = array ? (const struct qbytearray_data_native *)array->data : 0;
    if (!data || data->size != IGNITION_PAYLOAD_BYTES || !data->bytes)
        return -1;
    memcpy(&value, data->bytes + 0x14u, sizeof(value));
    return ignition_value(event_id, data->size, value);
}

static void hook_notify_common(void *context, const void *event)
{
    int ignition_on;
    g_original_common(context, event);
    ignition_on = read_ignition_event(event);
    if (ignition_on >= 0) update_vehicle_row_state(ignition_on);
}

static int hook_remove_rows(void *model, int row, int count,
                            const void *parent)
{
    if (model == g_vehicle_model && removes_injected_rows(row, count))
        clear_vehicle_row_references();
    return g_original_remove(model, row, count, parent);
}

static void send_climate_overview(void *context, int screen)
{
    static const char empty[] = "";
    struct qbytearray_native data;
    struct event_native event;
    qbytearray_construct(&data, empty, 0);
    event_construct(&event, UISH_EVENT_SOFT_HVAC, UISH_APP_INFO,
                    UISH_APP_UI_MANAGER, &data);
    notify_uish(context, &event, display_for_screen(screen));
    event_destroy(&event);
    qbytearray_destroy(&data);
}

static void hook_launch_application(void *context, int app_id, int view_id,
                                    int screen, int content, const void *text)
{
    if (context && is_climate_row(app_id, view_id)) {
        if (g_ignition_on) send_climate_overview(context, screen);
        return;
    }
    g_original_launch(context, app_id, view_id, screen, content, text);
}

static usize string_length(const char *value)
{
    usize length = 0;
    while (value[length]) ++length;
    return length;
}

static int exact_process(const char *expected)
{
    char path[96];
    usize expected_length = string_length(expected);
    ssize actual_length = readlink("/proc/self/exe", path, sizeof(path));
    return actual_length == (ssize)expected_length &&
           !memcmp(path, expected, expected_length);
}

static int model_row_count(void *model)
{
    struct model_index_native invalid = {-1, -1, 0, 0};
    void **vtable;
    row_count_fn row_count;
    if (!model) return -1;
    vtable = *(void ***)model;
    if (!vtable) return -1;
    row_count = (row_count_fn)vtable[14]; /* QAbstractItemModel + 0x38. */
    return row_count ? row_count(model, &invalid) : -1;
}

static void empty_list(struct qlist_native *list)
{
    list->data = (void *)(uptr)g_profile->qlist_shared_null;
    (void)__sync_add_and_fetch((volatile int *)list->data, 1);
}

static void *make_item(int view_id, int app_id, const char *text_value,
                       const char *image_value, int enabled)
{
    icon_item_constructor_fn construct_icon =
        (icon_item_constructor_fn)(uptr)g_profile->icon_constructor;
    struct qstring_native text;
    struct qstring_native image;
    struct qlist_native dependencies;
    void *item = cpp_new(0x30u);
    if (!item) return 0;
    qstring_construct(&text, text_value);
    qstring_construct(&image, image_value);
    empty_list(&dependencies);
    construct_icon(item, &view_id, &app_id, &text, &image,
                   enabled, 0, 1, &dependencies);
    ((qlist_destructor_fn)(uptr)g_profile->qlist_destructor)(&dependencies);
    qstring_destroy(&image);
    qstring_destroy(&text);
    return item;
}

static int append_item_list(void *model, void **items, u32 count)
{
    struct qlist_native list;
    u32 index;
    void **slot;
    empty_list(&list);
    /* QListData::append() assumes unique storage, as the OEM inline does. */
    (void)qlist_detach(&list);
    for (index = 0; index < count; ++index) {
        slot = qlist_append(&list);
        if (!slot) {
            ((qlist_destructor_fn)(uptr)g_profile->qlist_destructor)(&list);
            return -1;
        }
        *slot = items[index];
    }
    ((append_items_fn)(uptr)g_profile->append_items)(model, &list);
    ((qlist_destructor_fn)(uptr)g_profile->qlist_destructor)(&list);
    return 0;
}

static int connect_item(void *context, void *item)
{
    void *engine = *(void **)((u8 *)context + 8u);
    return !engine || qobject_connect(engine, k_value_signal, item,
                                      k_update_slot, 0x80);
}

static int prepend_vehicle_rows(void *context, void *model)
{
    void *items[2];
    int first_connected;
    int second_connected;
    clear_vehicle_row_references();
    initialize_ignition_state();
    items[0] = make_item(FUEL_SENTINEL_VIEW_ID, HOME_INFO_APP_ID,
                         k_fuel_text, k_fuel_image, g_ignition_on);
    items[1] = make_item(CLIMATE_VIEW_ID, CLIMATE_SENTINEL_APP_ID,
                         k_climate_text, k_climate_image, g_ignition_on);
    if (!items[0] || !items[1]) return -1;
    first_connected = connect_item(context, items[0]);
    second_connected = connect_item(context, items[1]);
    if (!first_connected || !second_connected)
        syslog(3, "vehicle-home: Vehicle row data connection failed");
    if (append_item_list(model, items, 2u)) return -2;
    g_fuel_item = items[0];
    g_climate_item = items[1];
    g_vehicle_model = model;
    __sync_synchronize();
    return 0;
}

static int append_vehicle_tile(void *context, void *model)
{
    void *item = make_item(VEHICLE_VIEW_ID, VEHICLE_APP_ID,
                           k_vehicle_marker, k_vehicle_image, 1);
    if (!item) return -1;
    if (!connect_item(context, item))
        syslog(3, "vehicle-home: Vehicle tile data connection failed");
    if (append_item_list(model, &item, 1u)) return -2;
    return model_row_count(model) == 7 ? 0 : -3;
}

static void hook_fill_app_list(void *context, void *model,
                               int screen, u32 view_id)
{
    int rows;
    u32 country;
    if (context && model) {
        rows = model_row_count(model);
        if (should_prepend_rows(screen, view_id, rows) &&
                prepend_vehicle_rows(context, model))
            syslog(3, "vehicle-home: guarded Vehicle rows prepend failed");
    }
    g_original_fill(context, model, screen, view_id);
    if (!g_profile->append_eu_main || !context || !model) return;
    country = *(const u32 *)((const u8 *)context + 0x10u);
    rows = model_row_count(model);
    if (should_append_main(country, screen, view_id, rows) &&
            append_vehicle_tile(context, model))
        syslog(3, "vehicle-home: guarded EU Vehicle append failed");
}

static int restore_hook(struct hook_record *record)
{
    uptr page;
    if (!record || !record->target || !record->stolen) return 0;
    page = ((uptr)record->target) & ~(uptr)4095u;
    if (memcmp(record->target, record->original, record->stolen)) {
        if (mprotect((void *)page, 4096u,
                     PROT_READ | PROT_WRITE | PROT_EXEC)) return -1;
        memcpy(record->target, record->original, record->stolen);
        __sync_synchronize();
        if (memcmp(record->target, record->original, record->stolen)) return -2;
    }
    if (mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC)) return -3;
    if (record->trampoline) munmap(record->trampoline, 4096u);
    memset(record, 0, sizeof(*record));
    return 0;
}

static int install_hook(struct hook_record *record, void *target,
                        void *replacement, const u8 *expected, u32 stolen,
                        void **original_out)
{
    u8 patch[16];
    u8 *site = (u8 *)target;
    u8 *trampoline;
    s32 relative;
    uptr page;
    int result = 0;
    if (!record || !original_out || stolen < 5u ||
            stolen > sizeof(record->original) || memcmp(site, expected, stolen))
        return -1;
    trampoline = (u8 *)mmap(0, 4096u, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (trampoline == (void *)(uptr)-1) return -2;
    memcpy(trampoline, site, stolen);
    trampoline[stolen] = 0xe9;
    relative = (s32)(site + stolen - (trampoline + stolen + 5u));
    memcpy(trampoline + stolen + 1u, &relative, 4u);
    if (mprotect(trampoline, 4096u, PROT_READ | PROT_EXEC)) {
        munmap(trampoline, 4096u);
        return -3;
    }
    memset(record, 0, sizeof(*record));
    record->target = site;
    record->stolen = stolen;
    record->trampoline = trampoline;
    memcpy(record->original, site, stolen);

    /* Publish the callable original before another thread can enter the jump. */
    *original_out = trampoline;
    __sync_synchronize();
    patch[0] = 0xe9;
    relative = (s32)((u8 *)replacement - (site + 5u));
    memcpy(patch + 1u, &relative, 4u);
    if (stolen > 5u) memset(patch + 5u, 0x90, stolen - 5u);
    page = ((uptr)site) & ~(uptr)4095u;
    if (mprotect((void *)page, 4096u,
                 PROT_READ | PROT_WRITE | PROT_EXEC)) {
        result = -4;
    } else {
        memcpy(site, patch, stolen);
        __sync_synchronize();
        if (memcmp(site, patch, stolen)) result = -5;
        else if (mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC))
            result = -6;
    }
    if (result) {
        int rollback = restore_hook(record);
        if (rollback)
            syslog(3, "vehicle-home: fatal hook rollback protection failure");
        /* A live or indeterminate detour must retain its callable trampoline. */
        if (!record->target ||
                !memcmp(site, record->original, stolen))
            *original_out = 0;
        __sync_synchronize();
    }
    return result;
}

static int valid_profile(const struct home_profile *profile)
{
    return !memcmp((const void *)(uptr)profile->launch,
                   k_hook_prologue, sizeof(k_hook_prologue)) &&
           !memcmp((const void *)(uptr)profile->fill,
                   k_fill_signature, sizeof(k_fill_signature)) &&
           !memcmp((const void *)(uptr)profile->notify_common,
                   k_hook_prologue, sizeof(k_hook_prologue)) &&
           !memcmp((const void *)(uptr)profile->remove_rows,
                   k_remove_rows_prologue,
                   sizeof(k_remove_rows_prologue)) &&
           !memcmp((const void *)(uptr)profile->icon_constructor,
                   k_icon_signature, sizeof(k_icon_signature)) &&
           !memcmp((const void *)(uptr)profile->append_items,
                   k_append_signature, sizeof(k_append_signature)) &&
           !memcmp((const void *)(uptr)profile->qlist_destructor,
                   k_list_destructor_signature,
                   sizeof(k_list_destructor_signature)) &&
           !memcmp((const void *)(uptr)profile->item_data_changed,
                   k_item_data_changed_signature,
                   sizeof(k_item_data_changed_signature));
}

static const struct home_profile *select_profile(void)
{
    const struct home_profile *selected = 0;
    u32 index;
    for (index = 0; index < sizeof(k_profiles) / sizeof(k_profiles[0]); ++index) {
        if (!valid_profile(&k_profiles[index])) continue;
        if (selected) return 0;
        selected = &k_profiles[index];
    }
    return selected;
}

CONSTRUCTOR static void vehicle_home_init(void)
{
    int result;
    if (!exact_process(k_process)) return;
    g_profile = select_profile();
    if (!g_profile) {
        syslog(3, "vehicle-home: no exact regional profile");
        return;
    }
    result = install_hook(&g_launch_hook,
                          (void *)(uptr)g_profile->launch,
                          (void *)hook_launch_application,
                          k_hook_prologue, sizeof(k_hook_prologue),
                          (void **)&g_original_launch);
    if (result) {
        syslog(3, "vehicle-home: %s launch hook refused (%d)",
               g_profile->name, result);
        return;
    }
    result = install_hook(&g_remove_hook,
                          (void *)(uptr)g_profile->remove_rows,
                          (void *)hook_remove_rows,
                          k_remove_rows_prologue,
                          sizeof(k_remove_rows_prologue),
                          (void **)&g_original_remove);
    if (result) {
        if (!g_remove_hook.target && restore_hook(&g_launch_hook))
            syslog(3, "vehicle-home: launch hook rollback failed");
        syslog(3, "vehicle-home: %s model lifetime hook refused (%d)",
               g_profile->name, result);
        return;
    }
    result = install_hook(&g_common_hook,
                          (void *)(uptr)g_profile->notify_common,
                          (void *)hook_notify_common,
                          k_hook_prologue, sizeof(k_hook_prologue),
                          (void **)&g_original_common);
    if (result) {
        if (!g_common_hook.target) {
            if (restore_hook(&g_remove_hook))
                syslog(3, "vehicle-home: lifetime hook rollback failed");
            if (restore_hook(&g_launch_hook))
                syslog(3, "vehicle-home: launch hook rollback failed");
        }
        syslog(3, "vehicle-home: %s ignition hook refused (%d)",
               g_profile->name, result);
        return;
    }
    result = install_hook(&g_fill_hook,
                          (void *)(uptr)g_profile->fill,
                          (void *)hook_fill_app_list,
                          k_hook_prologue, sizeof(k_hook_prologue),
                          (void **)&g_original_fill);
    if (result) {
        if (!g_fill_hook.target) {
            if (restore_hook(&g_common_hook))
                syslog(3, "vehicle-home: ignition hook rollback failed");
            if (restore_hook(&g_remove_hook))
                syslog(3, "vehicle-home: lifetime hook rollback failed");
            if (restore_hook(&g_launch_hook))
                syslog(3, "vehicle-home: launch hook rollback failed");
        }
        syslog(3, "vehicle-home: %s model hook refused (%d)",
               g_profile->name, result);
        return;
    }
    syslog(6, "vehicle-home: %s Vehicle model hook active",
           g_profile->name);
}

#endif /* !VH_HOST_TEST */
