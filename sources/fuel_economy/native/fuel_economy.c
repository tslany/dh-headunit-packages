/*
 * Passive fuel-economy backend for the Genesis DH AppInfo process.
 *
 * The DSO is loaded by an exact-preimage AppInfo DT_NEEDED patch.  Its
 * constructor removes the stale tmpfs snapshot and installs two guarded
 * detours.  Collection starts later, after AppInfo publishes its normal QCAN
 * receive filters.  The CAN hooks invoke the OEM implementation first.  No
 * SocketCAN socket, local launch IPC, or CAN transmit API exists in this
 * library.
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
#if __SIZEOF_POINTER__ == 8
typedef unsigned long usize;
typedef signed long ssize;
typedef unsigned long uptr;
#else
typedef unsigned int usize;
typedef signed int ssize;
typedef unsigned int uptr;
#endif

struct timespec_native { signed long tv_sec; signed long tv_nsec; };

extern void *memcpy(void *, const void *, usize);
extern void *memset(void *, int, usize);
extern int memcmp(const void *, const void *, usize);
extern int snprintf(char *, usize, const char *, ...);
#if !defined(__i386__)
extern int clock_gettime(int, struct timespec_native *);
extern int nanosleep(const struct timespec_native *, struct timespec_native *);
#endif
extern int open(const char *, int, ...);
extern ssize write(int, const void *, usize);
extern int close(int);
extern int rename(const char *, const char *);
extern int unlink(const char *);
extern int getpid(void);
extern void *mmap(void *, usize, int, int, int, signed long);
extern int munmap(void *, usize);
extern int mprotect(void *, usize, int);
extern int pthread_create(unsigned long *, const void *,
                          void *(*)(void *), void *);
extern int pthread_detach(unsigned long);
extern void syslog(int, const char *, ...);

extern void app_can_received(void *, const void *)
    __asm__("_ZN11CanDataBind37CQCANControllerTest_Can_data_receivedERK14SQCanFrameList")
    __attribute__((weak, visibility("default")));
extern void app_filter_ids(void *, const void *)
    __asm__("_ZN11CanDataBind17Can_filter_msg_idERK11SQCanIdList")
    __attribute__((weak, visibility("default")));
extern void qt_uint_list_append(void *, const u32 *)
    __asm__("_ZN5QListIjE6appendERKj")
    __attribute__((weak, visibility("default")));
extern void qt_uint_list_destroy(void *)
    __asm__("_ZN5QListIjED1Ev")
    __attribute__((weak, visibility("default")));

#define EXPORT __attribute__((visibility("default")))
#define CONSTRUCTOR __attribute__((constructor))

#define BUILD_ID "fuel-economy-appinfo-v1"
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define MAP_PRIVATE 2
#define MAP_ANONYMOUS 0x20
#define O_WRONLY 1
#define O_CREAT 0100
#define O_EXCL 0200
#define O_TRUNC 01000
#define O_NOFOLLOW 0400000
#define O_CLOEXEC 02000000

#define CAN_ID_SPEED 0x56eu
#define CAN_ID_FUEL 0x5d7u
#define INVALID_10_BIT 0x3ffu
#define INVALID_12_BIT 0xfffu
#define HISTORY_COUNT 12u
#define BUCKET_MS 150000u
#define INTEGRATION_FUEL_FRESH_MS 5000u
#define INTEGRATION_SPEED_FRESH_MS 3000u
#define DISPLAY_FRESH_MS 15000u
#define SUSPEND_GAP_MS 10000u

#ifndef FE_STATE_PATH
#define FE_STATE_PATH "/tmp/dh_fuel_economy_state.xml"
#endif
#ifndef FE_STATE_TEMP_PREFIX
#define FE_STATE_TEMP_PREFIX "/tmp/.dh_fuel_economy_state.xml."
#endif
#define STATE_PATH FE_STATE_PATH
#define STATE_TEMP_PREFIX FE_STATE_TEMP_PREFIX

struct fuel_fields {
    u16 dte_raw;
    u16 average_raw;
    u16 instant_raw;
    u8 status_high;
    u8 status_low;
    u8 tail_status;
    u8 dte_valid;
    u8 average_valid;
    u8 instant_valid;
    u8 dlc;
    u8 raw[8];
};

struct fuel_model {
    u64 started_ms;
    u64 last_advanced_ms;
    u64 next_boundary_ms;
    u64 last_fuel_ms;
    u64 last_speed_ms;
    u32 fuel_frames;
    u32 speed_frames;
    u32 discontinuities;
    u16 speed_raw;
    u8 speed_dlc;
    u8 speed_data[8];
    struct fuel_fields fuel;
    double bucket_numerator;
    double bucket_weight;
    double session_numerator;
    double session_weight;
    double displayed_session_raw;
    double history[HISTORY_COUNT];
    u16 history_valid_mask;
    u8 fuel_seen;
    u8 speed_seen;
};

struct qt_list_data_layout {
    volatile s32 ref;
    u32 alloc;
    u32 begin;
    u32 end;
    u32 reserved;
    void *nodes[1];
};

struct sq_can_frame_layout {
    u32 identifier;
    u8 dlc;
    u8 data[8];
};

struct sq_can_frame_list_layout {
    s32 count;
    struct qt_list_data_layout *list_data;
};

struct sq_can_id_list_layout {
    s32 count;
    struct qt_list_data_layout *list_data;
};

typedef void (*app_receive_fn)(void *, const void *);
typedef void (*app_filter_fn)(void *, const void *);

struct hook_record {
    u8 *target;
    void *replacement;
    u32 stolen;
    u8 original[16];
    u8 *trampoline;
    int installed;
};

static struct hook_record g_receive_hook;
static struct hook_record g_filter_hook;
static app_receive_fn g_original_receive;
static app_filter_fn g_original_filter;
static struct fuel_model g_model;
static volatile int g_model_lock;
static volatile int g_backend_ready;
static volatile int g_worker_started;
static volatile int g_filter_error_logged;
static volatile int g_first_speed_logged;
static volatile int g_first_fuel_logged;
static char g_last_published[4096];
static int g_last_published_length;

#if defined(__i386__)
static signed long syscall_two(signed long number, signed long first,
                               signed long second)
{
    signed long result;
    __asm__ volatile(
        "pushl %%ebx\n\t"
        "movl %%esi, %%ebx\n\t"
        "int $0x80\n\t"
        "popl %%ebx"
        : "=a"(result)
        : "0"(number), "S"(first), "c"(second)
        : "memory", "cc");
    return result;
}

static int native_clock_gettime(int clock_id, struct timespec_native *value)
{
    return (int)syscall_two(265, (signed long)clock_id,
                            (signed long)(uptr)value);
}

static int native_nanosleep(const struct timespec_native *request,
                            struct timespec_native *remaining)
{
    return (int)syscall_two(162, (signed long)(uptr)request,
                            (signed long)(uptr)remaining);
}
#else
static int native_clock_gettime(int clock_id, struct timespec_native *value)
{
    return clock_gettime(clock_id, value);
}

static int native_nanosleep(const struct timespec_native *request,
                            struct timespec_native *remaining)
{
    return nanosleep(request, remaining);
}
#endif

static u64 monotonic_ms(void)
{
    struct timespec_native value;
    if (native_clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        return 0u;
    if (value.tv_sec < 0 || value.tv_nsec < 0)
        return 0u;
    return (u64)(unsigned long)value.tv_sec * 1000u
         + (u64)(unsigned long)value.tv_nsec / 1000000u;
}

static u64 realtime_ms(void)
{
    struct timespec_native value;
    if (native_clock_gettime(CLOCK_REALTIME, &value) != 0)
        return 0u;
    if (value.tv_sec < 0 || value.tv_nsec < 0)
        return 0u;
    return (u64)(unsigned long)value.tv_sec * 1000u
         + (u64)(unsigned long)value.tv_nsec / 1000000u;
}

static void lock_model(void)
{
    while (__sync_lock_test_and_set(&g_model_lock, 1) != 0) {
        struct timespec_native delay;
        delay.tv_sec = 0;
        delay.tv_nsec = 1000000;
        native_nanosleep(&delay, 0);
    }
}

static void unlock_model(void)
{
    __sync_lock_release(&g_model_lock);
}

static int decode_fuel(const u8 *data, u32 length, struct fuel_fields *out)
{
    u32 copy_length;
    if (!data || !out || length < 5u)
        return -1;
    memset(out, 0, sizeof(*out));
    copy_length = length < 8u ? length : 8u;
    memcpy(out->raw, data, copy_length);
    out->dlc = (u8)copy_length;
    out->dte_raw = (u16)(((u16)data[0] << 4) | (data[1] >> 4));
    out->status_high = (u8)((data[1] >> 2) & 0x03u);
    out->status_low = (u8)(data[1] & 0x03u);
    out->average_raw = (u16)(((u16)data[2] << 2) | (data[3] >> 6));
    out->instant_raw = (u16)((((u16)data[3] & 0x3fu) << 4)
                             | (data[4] >> 4));
    out->tail_status = (u8)(data[4] & 0x0fu);
    out->dte_valid = (u8)(out->dte_raw != INVALID_12_BIT);
    out->average_valid = (u8)(out->average_raw != INVALID_10_BIT);
    out->instant_valid = (u8)(out->instant_raw != INVALID_10_BIT);
    return 0;
}

static void model_reset(struct fuel_model *model, u64 now)
{
    memset(model, 0, sizeof(*model));
    model->started_ms = now;
    model->last_advanced_ms = now;
    model->next_boundary_ms = now + BUCKET_MS;
    model->displayed_session_raw = -1.0;
}

static int integration_active(const struct fuel_model *model, u64 at)
{
    return model->fuel_seen && model->speed_seen
        && model->fuel.instant_valid && model->speed_raw > 0u
        && at <= model->last_fuel_ms + INTEGRATION_FUEL_FRESH_MS
        && at <= model->last_speed_ms + INTEGRATION_SPEED_FRESH_MS;
}

static u64 minimum_u64(u64 first, u64 second)
{
    return first < second ? first : second;
}

static void integrate_interval(struct fuel_model *model, u64 start, u64 end)
{
    u64 active_end;
    double weight;
    int first_session_sample;
    if (end <= start || !integration_active(model, start))
        return;
    active_end = minimum_u64(end,
                             model->last_fuel_ms
                             + INTEGRATION_FUEL_FRESH_MS);
    active_end = minimum_u64(active_end,
                             model->last_speed_ms
                             + INTEGRATION_SPEED_FRESH_MS);
    if (active_end <= start)
        return;
    weight = (double)model->speed_raw * (double)(active_end - start);
    first_session_sample = model->session_weight <= 0.0;
    model->bucket_numerator += (double)model->fuel.instant_raw * weight;
    model->bucket_weight += weight;
    model->session_numerator += (double)model->fuel.instant_raw * weight;
    model->session_weight += weight;
    if (first_session_sample)
        model->displayed_session_raw =
            model->session_numerator / model->session_weight;
}

static void finish_bucket(struct fuel_model *model)
{
    u32 index;
    for (index = HISTORY_COUNT - 1u; index > 0u; --index)
        model->history[index] = model->history[index - 1u];
    model->history_valid_mask = (u16)(model->history_valid_mask << 1);
    if (model->bucket_weight > 0.0) {
        model->history[0] = model->bucket_numerator / model->bucket_weight;
        model->history_valid_mask |= 1u;
    } else {
        model->history[0] = 0.0;
    }
    model->history_valid_mask &= (u16)((1u << HISTORY_COUNT) - 1u);
    if (model->session_weight > 0.0)
        model->displayed_session_raw =
            model->session_numerator / model->session_weight;
    model->bucket_numerator = 0.0;
    model->bucket_weight = 0.0;
    model->next_boundary_ms += BUCKET_MS;
}

static void model_discontinuity(struct fuel_model *model, u64 now)
{
    model->last_advanced_ms = now;
    model->next_boundary_ms = now + BUCKET_MS;
    model->bucket_numerator = 0.0;
    model->bucket_weight = 0.0;
    model->discontinuities += 1u;
}

static void model_advance(struct fuel_model *model, u64 now)
{
    u64 cursor;
    if (now < model->last_advanced_ms)
        now = model->last_advanced_ms;
    if (now - model->last_advanced_ms > SUSPEND_GAP_MS) {
        model_discontinuity(model, now);
        return;
    }
    cursor = model->last_advanced_ms;
    while (cursor < now) {
        u64 end = minimum_u64(now, model->next_boundary_ms);
        integrate_interval(model, cursor, end);
        cursor = end;
        if (cursor >= model->next_boundary_ms)
            finish_bucket(model);
    }
    model->last_advanced_ms = now;
}

static void model_update_speed(struct fuel_model *model, const u8 *data,
                               u32 length, u64 now)
{
    u32 copy_length;
    if (!data || length == 0u)
        return;
    model_advance(model, now);
    copy_length = length < 8u ? length : 8u;
    memset(model->speed_data, 0, sizeof(model->speed_data));
    memcpy(model->speed_data, data, copy_length);
    model->speed_dlc = (u8)copy_length;
    model->speed_raw = data[0];
    model->last_speed_ms = now;
    model->speed_frames += 1u;
    model->speed_seen = 1u;
}

static void model_update_fuel(struct fuel_model *model,
                              const struct fuel_fields *fuel, u64 now)
{
    struct fuel_fields merged;
    model_advance(model, now);
    merged = *fuel;
    if (!merged.dte_valid && model->fuel.dte_valid) {
        merged.dte_raw = model->fuel.dte_raw;
        merged.dte_valid = 1u;
    }
    if (!merged.average_valid && model->fuel.average_valid) {
        merged.average_raw = model->fuel.average_raw;
        merged.average_valid = 1u;
    }
    model->fuel = merged;
    model->last_fuel_ms = now;
    model->fuel_frames += 1u;
    model->fuel_seen = 1u;
}

static int extract_frame(const struct sq_can_frame_list_layout *list,
                         u32 index, const struct sq_can_frame_layout **out)
{
    struct qt_list_data_layout *data;
    u32 available;
    u32 node_index;
    u32 alloc;
    if (!list || !out || list->count <= 0 || list->count > 64)
        return -1;
    data = list->list_data;
    if (!data || data->end < data->begin)
        return -1;
    available = data->end - data->begin;
    if (available == 0u || available > 64u || index >= available)
        return -1;
    alloc = data->alloc & 0x7fffffffu;
    node_index = data->begin + index;
    if (alloc == 0u || node_index >= alloc)
        return -1;
    *out = (const struct sq_can_frame_layout *)data->nodes[node_index];
    if (!*out || (*out)->dlc > 8u)
        return -1;
    return 0;
}

static void process_received_list(const void *opaque_list)
{
    const struct sq_can_frame_list_layout *list =
        (const struct sq_can_frame_list_layout *)opaque_list;
    u32 available;
    u32 index;
    u64 now;
    if (!g_backend_ready || !list || !list->list_data
            || list->count <= 0 || list->count > 64)
        return;
    if (list->list_data->end < list->list_data->begin)
        return;
    available = list->list_data->end - list->list_data->begin;
    if (available < (u32)list->count || available > 64u)
        return;
    available = (u32)list->count;
    now = monotonic_ms();
    if (now == 0u)
        return;
    for (index = 0u; index < available; ++index) {
        const struct sq_can_frame_layout *frame = 0;
        if (extract_frame(list, index, &frame) != 0)
            return;
        if (frame->identifier == CAN_ID_SPEED && frame->dlc >= 1u) {
            lock_model();
            model_update_speed(&g_model, frame->data, frame->dlc, now);
            unlock_model();
            if (__sync_bool_compare_and_swap(&g_first_speed_logged, 0, 1))
                syslog(6, "fuel-economy: first 0x56e speed frame received");
        } else if (frame->identifier == CAN_ID_FUEL && frame->dlc >= 5u) {
            struct fuel_fields decoded;
            if (decode_fuel(frame->data, frame->dlc, &decoded) == 0) {
                lock_model();
                model_update_fuel(&g_model, &decoded, now);
                unlock_model();
                if (__sync_bool_compare_and_swap(&g_first_fuel_logged,
                                                 0, 1))
                    syslog(6, "fuel-economy: first 0x5d7 fuel frame received");
            }
        }
    }
}

static int write_all(int descriptor, const char *data, usize length)
{
    usize done = 0u;
    while (done < length) {
        ssize result = write(descriptor, data + done, length - done);
        if (result <= 0)
            return -1;
        done += (usize)result;
    }
    return 0;
}

static int serialize_state(const struct fuel_model *model, u64 now,
                           u64 published_wall_ms,
                           char *buffer, usize capacity)
{
    double session = model->displayed_session_raw;
    u64 fuel_age = 0u;
    u64 speed_age = 0u;
    int fuel_fresh = 0;
    int speed_fresh = 0;
    int result;
    if (model->fuel_seen) {
        fuel_age = now >= model->last_fuel_ms
                 ? now - model->last_fuel_ms : 0u;
        fuel_fresh = fuel_age <= DISPLAY_FRESH_MS;
    }
    if (model->speed_seen) {
        speed_age = now >= model->last_speed_ms
                  ? now - model->last_speed_ms : 0u;
        speed_fresh = speed_age <= INTEGRATION_SPEED_FRESH_MS;
    }
    result = snprintf(
        buffer, capacity,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<fuel><state schema=\"1\" build=\"%s\" "
        "layout=\"dh-5d7-shared-v1\" unit_profile=\"caller-gated\" "
        "published_wall_ms=\"%llu\" "
        "backend_ready=\"%d\" fuel_seen=\"%d\" fuel_fresh=\"%d\" "
        "speed_seen=\"%d\" speed_fresh=\"%d\" "
        "fuel_frames=\"%u\" speed_frames=\"%u\" "
        "discontinuities=\"%u\" speed_raw=\"%u\" "
        "speed_dlc=\"%u\" speed_frame=\"%02x%02x%02x%02x%02x%02x%02x%02x\" "
        "dte_raw=\"%u\" dte_valid=\"%u\" "
        "average_raw=\"%u\" average_valid=\"%u\" "
        "instant_raw=\"%u\" instant_valid=\"%u\" "
        "status_high=\"%u\" status_low=\"%u\" tail_status=\"%u\" "
        "fuel_dlc=\"%u\" fuel_frame=\"%02x%02x%02x%02x%02x%02x%02x%02x\" "
        "session_raw_tenths=\"%.3f\" history_mask=\"%u\" "
        "h0=\"%.3f\" h1=\"%.3f\" h2=\"%.3f\" h3=\"%.3f\" "
        "h4=\"%.3f\" h5=\"%.3f\" h6=\"%.3f\" h7=\"%.3f\" "
        "h8=\"%.3f\" h9=\"%.3f\" h10=\"%.3f\" h11=\"%.3f\"/>"
        "</fuel>\n",
        BUILD_ID, (unsigned long long)published_wall_ms,
        g_backend_ready ? 1 : 0,
        model->fuel_seen ? 1 : 0, fuel_fresh,
        model->speed_seen ? 1 : 0, speed_fresh,
        model->fuel_frames, model->speed_frames, model->discontinuities,
        (u32)model->speed_raw,
        (u32)model->speed_dlc,
        (u32)model->speed_data[0], (u32)model->speed_data[1],
        (u32)model->speed_data[2], (u32)model->speed_data[3],
        (u32)model->speed_data[4], (u32)model->speed_data[5],
        (u32)model->speed_data[6], (u32)model->speed_data[7],
        (u32)model->fuel.dte_raw, (u32)model->fuel.dte_valid,
        (u32)model->fuel.average_raw, (u32)model->fuel.average_valid,
        (u32)model->fuel.instant_raw, (u32)model->fuel.instant_valid,
        (u32)model->fuel.status_high, (u32)model->fuel.status_low,
        (u32)model->fuel.tail_status, (u32)model->fuel.dlc,
        (u32)model->fuel.raw[0], (u32)model->fuel.raw[1],
        (u32)model->fuel.raw[2], (u32)model->fuel.raw[3],
        (u32)model->fuel.raw[4], (u32)model->fuel.raw[5],
        (u32)model->fuel.raw[6], (u32)model->fuel.raw[7], session,
        (u32)model->history_valid_mask,
        model->history[0], model->history[1], model->history[2],
        model->history[3], model->history[4], model->history[5],
        model->history[6], model->history[7], model->history[8],
        model->history[9], model->history[10], model->history[11]);
    if (result < 0 || (usize)result >= capacity)
        return -1;
    return result;
}

static void publish_state(const struct fuel_model *snapshot, u64 now,
                          u64 published_wall_ms)
{
    char xml[4096];
    char temporary[96];
    int length;
    int path_length;
    int descriptor;
    int write_result;
    int close_result;
    length = serialize_state(snapshot, now, published_wall_ms,
                             xml, sizeof(xml));
    if (length <= 0)
        return;
    if (length == g_last_published_length
            && memcmp(xml, g_last_published, (usize)length) == 0)
        return;
    path_length = snprintf(temporary, sizeof(temporary), "%s%d.%llu",
                           STATE_TEMP_PREFIX, getpid(),
                           (unsigned long long)now);
    if (path_length <= 0 || (usize)path_length >= sizeof(temporary))
        return;
    descriptor = open(temporary, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC
                      | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (descriptor < 0)
        return;
    write_result = write_all(descriptor, xml, (usize)length);
    close_result = close(descriptor);
    if (write_result == 0 && close_result == 0
            && rename(temporary, STATE_PATH) == 0) {
        memcpy(g_last_published, xml, (usize)length);
        g_last_published_length = length;
        return;
    }
    unlink(temporary);
}

static void clear_stale_state_on_load(void)
{
    /* The fixed name is never opened; unlink cannot follow a symlink. */
    unlink(STATE_PATH);
    g_last_published_length = 0;
}

static void *state_worker(void *unused)
{
    struct timespec_native delay;
    (void)unused;
    delay.tv_sec = 1;
    delay.tv_nsec = 0;
    for (;;) {
        struct fuel_model snapshot;
        u64 now = monotonic_ms();
        u64 wall_now = realtime_ms();
        if (now != 0u) {
            lock_model();
            model_advance(&g_model, now);
            snapshot = g_model;
            unlock_model();
            publish_state(&snapshot, now, wall_now);
        }
        native_nanosleep(&delay, 0);
    }
    return 0;
}

static void start_backend_after_qcan(void)
{
    unsigned long thread;
    u64 now;
    if (__sync_lock_test_and_set(&g_worker_started, 1) != 0)
        return;
    now = monotonic_ms();
    if (now == 0u) {
        g_worker_started = 0;
        return;
    }
    lock_model();
    model_reset(&g_model, now);
    g_backend_ready = 1;
    unlock_model();
    if (pthread_create(&thread, 0, state_worker, 0) != 0) {
        g_backend_ready = 0;
        g_worker_started = 0;
        return;
    }
    pthread_detach(thread);
    syslog(6, "fuel-economy: QCAN filter active; collector started");
}

static int qt_list_count(const struct qt_list_data_layout *data)
{
    if (!data || data->end < data->begin || data->end - data->begin > 128u)
        return -1;
    return (int)(data->end - data->begin);
}

static void filter_hook(void *self, const void *opaque_ids)
{
    const struct sq_can_id_list_layout *source =
        (const struct sq_can_id_list_layout *)opaque_ids;
    struct sq_can_id_list_layout expanded;
    u32 speed = CAN_ID_SPEED;
    u32 fuel = CAN_ID_FUEL;
    int count;

    g_original_filter(self, opaque_ids);
    if (!source || !source->list_data || !qt_uint_list_append
            || !qt_uint_list_destroy) {
        if (__sync_bool_compare_and_swap(&g_filter_error_logged, 0, 1))
            syslog(3, "fuel-economy: QCAN filter extension unavailable");
        return;
    }
    count = qt_list_count(source->list_data);
    if (count < 0) {
        if (__sync_bool_compare_and_swap(&g_filter_error_logged, 0, 1))
            syslog(3, "fuel-economy: invalid OEM QCAN filter list");
        return;
    }
    expanded.count = source->count;
    expanded.list_data = source->list_data;
    __sync_add_and_fetch(&expanded.list_data->ref, 1);
    qt_uint_list_append(&expanded.list_data, &speed);
    qt_uint_list_append(&expanded.list_data, &fuel);
    count = qt_list_count(expanded.list_data);
    if (count >= 0) {
        expanded.count = count;
        g_original_filter(self, &expanded);
        start_backend_after_qcan();
    } else if (__sync_bool_compare_and_swap(&g_filter_error_logged, 0, 1)) {
        syslog(3, "fuel-economy: expanded QCAN filter list is invalid");
    }
    qt_uint_list_destroy(&expanded.list_data);
}

static void receive_hook(void *self, const void *frames)
{
    g_original_receive(self, frames);
    process_received_list(frames);
}

static int prepare_hook(struct hook_record *record, void *target,
                        void *replacement, const u8 *expected, u32 stolen)
{
    u8 *site = (u8 *)target;
    u8 *trampoline;
    s32 relative;
    if (!record || !site || !replacement || !expected
            || stolen < 5u || stolen > sizeof(record->original))
        return -1;
    if (memcmp(site, expected, stolen) != 0)
        return -2;
    trampoline = (u8 *)mmap(0, 4096u, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (trampoline == (void *)(uptr)-1)
        return -3;
    memcpy(trampoline, site, stolen);
    trampoline[stolen] = 0xe9;
    relative = (s32)(site + stolen - (trampoline + stolen + 5u));
    memcpy(trampoline + stolen + 1u, &relative, 4u);
    if (mprotect(trampoline, 4096u, PROT_READ | PROT_EXEC) != 0) {
        munmap(trampoline, 4096u);
        return -4;
    }
    memset(record, 0, sizeof(*record));
    record->target = site;
    record->replacement = replacement;
    record->stolen = stolen;
    memcpy(record->original, site, stolen);
    record->trampoline = trampoline;
    return 0;
}

static int activate_hook(struct hook_record *record)
{
    uptr page;
    s32 relative;
    u32 index;
    if (!record || !record->target || !record->trampoline)
        return -1;
    page = (uptr)record->target & ~(uptr)4095u;
    if (mprotect((void *)page, 4096u,
                 PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
        return -2;
    relative = (s32)((u8 *)record->replacement - (record->target + 5u));
    record->target[0] = 0xe9;
    memcpy(record->target + 1u, &relative, 4u);
    for (index = 5u; index < record->stolen; ++index)
        record->target[index] = 0x90u;
    if (mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC) != 0) {
        /* The page is still writable here: remove the partial jump first. */
        memcpy(record->target, record->original, record->stolen);
        mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC);
        return -3;
    }
    record->installed = 1;
    return 0;
}

static int deactivate_hook(struct hook_record *record)
{
    uptr page;
    if (!record || !record->installed)
        return 0;
    page = (uptr)record->target & ~(uptr)4095u;
    if (mprotect((void *)page, 4096u,
                 PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
        return -1;
    memcpy(record->target, record->original, record->stolen);
    record->installed = 0;
    if (mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC) != 0) {
        /* Retry once; original bytes are already restored either way. */
        if (mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC) != 0)
            return -2;
    }
    return 0;
}

static void discard_hook(struct hook_record *record)
{
    if (!record)
        return;
    if (record->installed && deactivate_hook(record) != 0
            && record->installed)
        return;
    if (record->trampoline)
        munmap(record->trampoline, 4096u);
    memset(record, 0, sizeof(*record));
}

static int receive_prologue(void *target, const u8 **pattern, u32 *length)
{
    static const u8 eu_na[] = {0x55, 0x57, 0x56, 0x53, 0x83, 0xec, 0x6c};
    static const u8 kdm[] = {
        0x55, 0x57, 0x56, 0x53, 0x81, 0xec, 0x6c, 0x03, 0x00, 0x00
    };
    if (memcmp(target, eu_na, sizeof(eu_na)) == 0) {
        *pattern = eu_na;
        *length = sizeof(eu_na);
        return 0;
    }
    if (memcmp(target, kdm, sizeof(kdm)) == 0) {
        *pattern = kdm;
        *length = sizeof(kdm);
        return 0;
    }
    return -1;
}

#ifndef FE_SELF_TEST
static void CONSTRUCTOR install_fuel_backend(void)
{
    static const u8 filter_expected[] = {
        0x83, 0xec, 0x2c, 0x8b, 0x44, 0x24, 0x34
    };
    const u8 *receive_expected = 0;
    u32 receive_stolen = 0u;
    int result;
    if (!app_can_received || !app_filter_ids) {
        syslog(3, "fuel-economy: AppInfo QCAN symbols unavailable");
        return;
    }
    clear_stale_state_on_load();
    result = receive_prologue((void *)app_can_received,
                              &receive_expected, &receive_stolen);
    if (result != 0) {
        syslog(3, "fuel-economy: receive hook prologue refused");
        return;
    }
    result = prepare_hook(&g_filter_hook, (void *)app_filter_ids,
                          (void *)filter_hook, filter_expected,
                          sizeof(filter_expected));
    if (result != 0) {
        syslog(3, "fuel-economy: filter hook preparation refused (%d)",
               result);
        return;
    }
    result = prepare_hook(&g_receive_hook, (void *)app_can_received,
                          (void *)receive_hook, receive_expected,
                          receive_stolen);
    if (result != 0) {
        syslog(3, "fuel-economy: receive hook preparation refused (%d)",
               result);
        goto fail;
    }
    g_original_filter = (app_filter_fn)g_filter_hook.trampoline;
    g_original_receive = (app_receive_fn)g_receive_hook.trampoline;
    /* Receive must be active before Filter can start the worker. */
    result = activate_hook(&g_receive_hook);
    if (result != 0) {
        syslog(3, "fuel-economy: receive hook activation refused (%d)",
               result);
        goto fail;
    }
    result = activate_hook(&g_filter_hook);
    if (result != 0) {
        syslog(3, "fuel-economy: filter hook activation refused (%d)",
               result);
        goto fail;
    }
    syslog(6, "fuel-economy: AppInfo hooks active");
    return;

fail:
    discard_hook(&g_filter_hook);
    discard_hook(&g_receive_hook);
}
#endif

#ifdef FE_SELF_TEST
extern int printf(const char *, ...);

static int close_enough(double first, double second)
{
    double difference = first - second;
    if (difference < 0.0)
        difference = -difference;
    return difference < 0.0001;
}

int main(void)
{
    struct fuel_fields decoded;
    struct fuel_model model;
    struct sq_can_frame_layout frame;
    struct {
        volatile s32 ref;
        u32 alloc;
        u32 begin;
        u32 end;
        u32 reserved;
        void *nodes[1];
    } list_data;
    struct sq_can_frame_list_layout list;
    static const u8 final_frame[8] = {
        0x1b, 0xa0, 0x42, 0xbf, 0xf0, 0, 0, 0
    };
    static const u8 cruise_frame[8] = {
        0x1c, 0x20, 0x61, 0x44, 0x40, 0, 0, 0
    };
    static const u8 unavailable_frame[8] = {
        0xff, 0xff, 0xff, 0xff, 0xf0, 0, 0, 0
    };
    static const u8 dte_unavailable_frame[8] = {
        0xff, 0xf0, 0x61, 0x44, 0x40, 0, 0, 0
    };
    static const u8 average_unavailable_frame[8] = {
        0x1c, 0x20, 0xff, 0xc4, 0x40, 0, 0, 0
    };
    const u8 *pattern;
    u32 length;
    u8 eu_prologue[10] = {0x55,0x57,0x56,0x53,0x83,0xec,0x6c,0,0,0};
    u8 kdm_prologue[10] = {0x55,0x57,0x56,0x53,0x81,0xec,0x6c,3,0,0};
    u8 speed_frame[8] = {20, 0, 0, 0, 0, 0, 0, 0};

    if (decode_fuel(final_frame, 8u, &decoded) != 0
            || decoded.dte_raw != 442u || decoded.average_raw != 266u
            || decoded.instant_valid)
        return 1;
    if (decode_fuel(cruise_frame, 8u, &decoded) != 0
            || decoded.dte_raw != 450u || decoded.average_raw != 389u
            || decoded.instant_raw != 68u)
        return 2;
    if (decode_fuel(unavailable_frame, 8u, &decoded) != 0
            || decoded.dte_valid || decoded.average_valid
            || decoded.instant_valid)
        return 3;
    model_reset(&model, 0u);
    model_update_fuel(&model, &decoded, 10u);
    if (model.fuel.dte_valid || model.fuel.average_valid)
        return 4;
    if (decode_fuel(final_frame, 8u, &decoded) != 0)
        return 5;
    model_update_fuel(&model, &decoded, 20u);
    if (!model.fuel.dte_valid || model.fuel.dte_raw != 442u
            || !model.fuel.average_valid || model.fuel.average_raw != 266u)
        return 6;
    if (decode_fuel(unavailable_frame, 8u, &decoded) != 0)
        return 7;
    model_update_fuel(&model, &decoded, 30u);
    if (!model.fuel.dte_valid || model.fuel.dte_raw != 442u
            || !model.fuel.average_valid || model.fuel.average_raw != 266u
            || model.fuel.instant_valid || model.fuel.raw[0] != 0xffu)
        return 8;
    if (decode_fuel(dte_unavailable_frame, 8u, &decoded) != 0)
        return 9;
    model_update_fuel(&model, &decoded, 40u);
    if (!model.fuel.dte_valid || model.fuel.dte_raw != 442u
            || !model.fuel.average_valid || model.fuel.average_raw != 389u
            || model.fuel.raw[0] != 0xffu)
        return 10;
    if (decode_fuel(average_unavailable_frame, 8u, &decoded) != 0)
        return 11;
    model_update_fuel(&model, &decoded, 50u);
    if (!model.fuel.dte_valid || model.fuel.dte_raw != 450u
            || !model.fuel.average_valid || model.fuel.average_raw != 389u
            || model.fuel.raw[2] != 0xffu)
        return 12;
    model_reset(&model, 60u);
    if (model.fuel.dte_valid || model.fuel.average_valid
            || model.fuel.dte_raw != 0u || model.fuel.average_raw != 0u)
        return 13;
    if (decode_fuel(cruise_frame, 8u, &decoded) != 0)
        return 14;
    model_reset(&model, 0u);
    model_update_speed(&model, speed_frame, 8u, 0u);
    decoded.instant_raw = 100u;
    decoded.instant_valid = 1u;
    model_update_fuel(&model, &decoded, 0u);
    model.next_boundary_ms = 5000u;
    model_advance(&model, 5000u);
    model_update_speed(&model, speed_frame, 8u, 5000u);
    decoded.instant_raw = 200u;
    model_update_fuel(&model, &decoded, 5000u);
    model.next_boundary_ms = 10000u;
    model_advance(&model, 7500u);
    if (!close_enough(model.displayed_session_raw, 100.0))
        return 30;
    model_advance(&model, 10000u);
    if ((model.history_valid_mask & 3u) != 3u
            || !close_enough(model.history[0], 200.0)
            || !close_enough(model.history[1], 100.0)
            || !close_enough(model.session_numerator / model.session_weight,
                             150.0)
            || !close_enough(model.displayed_session_raw, 150.0))
        return 15;
    model_advance(&model, 30000u);
    if (model.discontinuities != 1u || !model.fuel_seen || !model.speed_seen
            || model.bucket_weight != 0.0
            || !close_enough(model.displayed_session_raw, 150.0))
        return 16;

    memset(&frame, 0, sizeof(frame));
    frame.identifier = CAN_ID_FUEL;
    frame.dlc = 8u;
    memcpy(frame.data, final_frame, 8u);
    list_data.ref = 1;
    list_data.alloc = 1u;
    list_data.begin = 0u;
    list_data.end = 1u;
    list_data.reserved = 0u;
    list_data.nodes[0] = &frame;
    list.count = 1;
    list.list_data = (struct qt_list_data_layout *)&list_data;
    {
        const struct sq_can_frame_layout *found = 0;
        if (extract_frame(&list, 0u, &found) != 0 || found != &frame)
            return 17;
    }
    if (receive_prologue(eu_prologue, &pattern, &length) != 0 || length != 7u)
        return 18;
    if (receive_prologue(kdm_prologue, &pattern, &length) != 0 || length != 10u)
        return 19;
    printf("fuel-economy-native-self-test=PASS\n");
    return 0;
}
#endif
