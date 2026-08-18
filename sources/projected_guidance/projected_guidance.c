/*
 * Persistent projected-guidance ingress for the late-EU/NA Genesis DH build.
 *
 * The same DSO is loaded by iAP2Service, AppProjection, and (through the root
 * QCAN dependency) AppNavi.  The first adds and decodes CarPlay Route
 * Guidance; AppProjection owns normalized ingress and the projected writer;
 * AppNavi suppresses only conflicting native guidance while that writer is
 * proven live.
 *
 * There are no runtime files and no SD-card dependency.  All protocol hooks
 * call the fitted OEM implementation first (inbound) or forward its exact
 * frame (outbound), and transfer bounded copies to worker threads.
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
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
struct tm_native {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    signed long tm_gmtoff;
    const char *tm_zone;
};
struct pollfd_native { int fd; short events; short revents; };
struct sockaddr_un_native {
    unsigned short family;
    char path[108];
};
struct ucred_native { int pid; u32 uid; u32 gid; };

extern void *malloc(usize);
extern void free(void *);
extern void *memcpy(void *, const void *, usize);
extern void *memset(void *, int, usize);
extern int memcmp(const void *, const void *, usize);
extern int snprintf(char *, usize, const char *, ...);
extern ssize readlink(const char *, char *, usize);
extern int getpid(void);
extern u32 geteuid(void);
extern int mprotect(void *, usize, int);
extern void *mmap(void *, usize, int, int, int, signed long);
extern int munmap(void *, usize);
extern int clock_gettime(int, struct timespec_native *);
extern struct tm_native *localtime_r(const signed long *, struct tm_native *);
extern int nanosleep(const struct timespec_native *, struct timespec_native *);
extern int socket(int, int, int);
extern int socketpair(int, int, int, int *);
extern int bind(int, const void *, u32);
extern int listen(int, int);
extern int accept(int, void *, u32 *);
extern int connect(int, const void *, u32);
extern ssize send(int, const void *, usize, int);
extern ssize recv(int, void *, usize, int);
extern int getsockopt(int, int, int, void *, u32 *);
extern int fcntl(int, int, ...);
extern int close(int);
extern int poll(struct pollfd_native *, u32, int);
extern int pthread_create(unsigned long *, const void *,
                          void *(*)(void *), void *);
extern int pthread_detach(unsigned long);
extern void syslog(int, const char *, ...);
extern void logger_system_logs(int, const char *)
    __asm__("_ZN6Logger16HandleSystemLogsE9QtMsgTypePKc")
    __attribute__((weak, visibility("default")));
extern void *logger_existing_instance
    __asm__("_ZN6Logger9m_pLoggerE")
    __attribute__((weak, visibility("default")));
extern int qt_metatype_type(const char *)
    __asm__("_ZN9QMetaType4typeEPKc")
    __attribute__((weak, visibility("default")));
extern int qt_metatype_register(const char *, void (*)(void *),
                                void *(*)(const void *))
    __asm__("_ZN9QMetaType12registerTypeEPKcPFvPvEPFS2_PKvE")
    __attribute__((weak, visibility("default")));

#define EXPORT __attribute__((visibility("default")))
#define CONSTRUCTOR __attribute__((constructor))

#define BUILD_ID "projected-guidance-v1"
#define PACKET_MAGIC 0x31494750u /* PGI1 on little-endian IA-32. */
#define PACKET_VERSION 1u
#define SOURCE_CARPLAY 2u
#define SOURCE_ANDROID_AUTO 1u
#define MAX_FRAME 4096u
#define MAX_TLVS 64u
#define MAX_TLV_VALUE 1024u
#define MAX_COMPONENT_IDS 8u
#define MAX_MANEUVER_ITEMS 16u
#define MAX_JUNCTION_ANGLES 16u
#define MAX_LANES 8u
#define MAX_LANE_ANGLES 16u
#define FRAME_SLOTS 64u
#define TEXT_BYTES 96u
#define MANEUVER_CACHE 16u
#define HEARTBEAT_MS 1000u
#define WATCHDOG_MS 3500u
#define QT_CRITICAL_MSG 2

#define MSG_IDENT_INFO 0x1d01u
#define MSG_IDENT_ACCEPTED 0x1d02u
#define MSG_IDENT_REJECTED 0x1d03u
#define MSG_AUTH_COMPLETE 0xaa05u
#define MSG_ROUTE_START 0x5200u
#define MSG_ROUTE_UPDATE 0x5201u
#define MSG_MANEUVER_UPDATE 0x5202u
#define MSG_ROUTE_STOP 0x5203u
#define MSG_LANE_UPDATE 0x5204u
#define IDENT_SENT 0x0006u
#define IDENT_RECEIVED 0x0007u
#define IDENT_ROUTE_COMPONENT 0x001eu

#define EVENT_SNAPSHOT 1u
#define EVENT_CLEAR 2u
#define EVENT_HEARTBEAT 3u
#define EVENT_FAULT 4u

#define CLEAR_SESSION_REPLACED 1u
#define CLEAR_ROUTE_INACTIVE 2u
#define CLEAR_TBT_OFF 3u
#define CLEAR_IPC_LOST 4u
#define CLEAR_WATCHDOG 5u
#define CLEAR_DECODE_FAULT 6u
#define CLEAR_DISCONNECTED 7u

#define CP_DECISION_ACTIVE 1u
#define CP_DECISION_ROUTE_INACTIVE 2u
#define CP_DECISION_SOURCE_UNSUPPORTED 4u
#define CP_DECISION_GATE_UNKNOWN 5u
#define CP_DECISION_TBT_OFF 6u
#define CP_DECISION_DISCONNECTED 7u

#define PRES_ROUTE_STATE       (1u << 0)
#define PRES_MANEUVER_STATE    (1u << 1)
#define PRES_CURRENT_ROAD      (1u << 2)
#define PRES_DESTINATION       (1u << 3)
#define PRES_ETA               (1u << 4)
#define PRES_TIME_REMAINING    (1u << 5)
#define PRES_DISTANCE_REMAIN   (1u << 6)
#define PRES_NEXT_TURN_METRES  (1u << 7)
#define PRES_CURRENT_INDEX     (1u << 8)
#define PRES_VISIBLE           (1u << 9)
#define PRES_SOURCE_SUPPORTS   (1u << 10)
#define PRES_SOURCE_TYPE       (1u << 11)
#define PRES_SEMANTIC          (1u << 12)
#define PRES_DESCRIPTION       (1u << 13)
#define PRES_AFTER_ROAD        (1u << 14)
#define PRES_DRIVING_SIDE      (1u << 15)
#define PRES_JUNCTION_TYPE     (1u << 16)
#define PRES_EXIT_ANGLE        (1u << 17)
#define PRES_LINKED_LANE       (1u << 18)
#define PRES_LANE_INDEX        (1u << 19)
#define PRES_LANE_COUNT        (1u << 20)
#define PRES_EXIT_INFO         (1u << 21)
#define PRES_TURN_SIDE         (1u << 22)

#define SEM_UNKNOWN 0u
#define SEM_CONTEXT 1u
#define SEM_TURN_LEFT 2u
#define SEM_TURN_RIGHT 3u
#define SEM_STRAIGHT 4u
#define SEM_U_TURN 5u
#define SEM_CONTINUE 6u
#define SEM_ROUNDABOUT_ENTER 7u
#define SEM_ROUNDABOUT_EXIT 8u
#define SEM_OFF_RAMP 9u
#define SEM_ON_RAMP 10u
#define SEM_ARRIVE 11u
#define SEM_PROCEED_TO_ROUTE 12u
#define SEM_KEEP_LEFT 13u
#define SEM_KEEP_RIGHT 14u
#define SEM_FERRY_ENTER 15u
#define SEM_FERRY_EXIT 16u
#define SEM_FERRY_CHANGE 17u
#define SEM_U_TURN_TO_ROUTE 18u
#define SEM_ROUNDABOUT_U_TURN 19u
#define SEM_OFF_RAMP_LEFT 20u
#define SEM_OFF_RAMP_RIGHT 21u
#define SEM_ARRIVE_LEFT 22u
#define SEM_ARRIVE_RIGHT 23u
#define SEM_SHARP_LEFT 24u
#define SEM_SHARP_RIGHT 25u
#define SEM_SLIGHT_LEFT 26u
#define SEM_SLIGHT_RIGHT 27u
#define SEM_CHANGE_HIGHWAY 28u
#define SEM_CHANGE_HIGHWAY_LEFT 29u
#define SEM_CHANGE_HIGHWAY_RIGHT 30u

#define GENESIS_NO_INFO 0x01u
#define GENESIS_EMPTY_NAV_FIELD 0x0fu

#define ITEM_ROUTE 1u
#define ITEM_SESSION_BEGIN 2u
#define ITEM_NEGOTIATION_FAULT 3u

#define AF_UNIX 1
#define SOCK_SEQPACKET 5
#define SOCK_DGRAM 2
#define SOCK_CLOEXEC 02000000
#define SOL_SOCKET 1
#define SO_PEERCRED 17
#define MSG_DONTWAIT 0x40
#define MSG_NOSIGNAL 0x4000
#define MSG_TRUNC 0x20
#define POLLIN 0x0001
#define POLLERR 0x0008
#define POLLHUP 0x0010
#define POLLNVAL 0x0020
#define F_SETFD 2
#define F_SETFL 4
#define FD_CLOEXEC 1
#define O_NONBLOCK 04000
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define MAP_PRIVATE 2
#define MAP_ANONYMOUS 0x20
#define CLOCK_MONOTONIC 1

#define LEASE_MAGIC 0x314c4750u /* PGL1 on little-endian IA-32. */
#define LEASE_VERSION 1u
#define LEASE_HEARTBEAT_MS 250u
#define LEASE_EXPIRY_MS 1000u
#define LEASE_REPLAY_FALLBACK_MS 1250u

/* Exact stock executable addresses, validated against the pinned hashes. */
#define IAP_SEND_ADDRESS ((void *)0x08056112u)
#define IAP_DISPATCH_ADDRESS ((void *)0x080551f0u)
#define IAP_RAW_SEND_ADDRESS ((void *)0x08056180u)
#define APP_TBT_ADDRESS ((void *)0x080764f0u)
#define APP_AA_STATUS_ADDRESS ((void *)0x080d9880u)
#define APP_AA_DISTANCE_ADDRESS ((void *)0x080dbb50u)
#define APP_AA_NEXT_ADDRESS ((void *)0x080dbe50u)
#define APP_USB_CONNECTION_ADDRESS ((void *)0x080907b0u)
#define APP_OWNER_EVENT_ADDRESS ((void *)0x08061a00u)
#define APP_AA_FOCUS_ADDRESS ((void *)0x0805b750u)
#define APP_AOAP_CONNECTION_ADDRESS ((void *)0x080d0280u)
#define APP_NOTIFIER_CTOR_ADDRESS ((void *)0x080dee40u)
#define APP_NOTIFIER_DTOR_ADDRESS ((void *)0x080df280u)
#define APP_QCAN_SIGNAL_ADDRESS ((void *)0x0812de10u)
#define APP_EVENT_ID_ADDRESS ((void *)0x080521b4u)
#define APP_EVENT_COMMAND_ADDRESS ((void *)0x080529b4u)
#define NAVI_BUILD_ID_ADDRESS ((const void *)0x08048178u)
#define NAVI_OWNER_EVENT_ADDRESS ((void *)0x0806ca90u)
#define NAVI_SENDER_DTOR_ADDRESS ((void *)0x0807e180u)
#define NAVI_CAN_SEND_ADDRESS ((void *)0x0807e200u)
#define NAVI_EVENT_ID_ADDRESS ((void *)0x0805c384u)
#define NAVI_EVENT_COMMAND_ADDRESS ((void *)0x0805cdc4u)

#define ROUTE_OWNER_EVENT 0x0afdu
#define ROUTE_OWNER_OFF 0u
#define ROUTE_OWNER_NATIVE 1u
#define ROUTE_OWNER_PROJECTED 2u
#define COUNTRY_VARIANT_EUROPE 5u
#define WRITER_GRACE_MS 500u
#define CP_GAP_GRACE_MS 1500u
#define WRITER_RATE_MS 100u
#define WRITER_CLEAR_RETRY_MS 1000u
#define WRITER_FAILSAFE_MS 1000u
#define QCAN_FRAME_ID 0x115u
#define QCAN_ROUTE_FRAME_ID 0x506u
#define NAVI_ROUTE_STATE_FRAME_ID 0x123u
#define NAVI_COMPASS_FRAME_ID 0x1e5u
#define QCAN_FRAME_DLC 8u
#define WRITER_FRAME_115 0x01u
#define WRITER_FRAME_506 0x02u
#define WRITER_SOURCE_AA 1u
#define WRITER_SOURCE_CP 2u
#define WRITER_DESIRED_IDLE 0u
#define WRITER_DESIRED_ACTIVE 1u
#define WRITER_DESIRED_CLEAR 2u
#define WRITER_CANDIDATE_HOLD 2
#define WRITER_CANDIDATE_GAP_EXPIRED 3
#define PROVENANCE_SLOTS 16u
#define NAVI_CACHE_123 0x01u
#define NAVI_CACHE_115 0x02u
#define NAVI_CACHE_506 0x04u

struct byte_array { u8 *ptr; u32 capacity; u32 used; };
typedef void (*iap_send_fn)(void *, void *, unsigned int, struct byte_array *);
typedef void (*iap_dispatch_fn)(void *, void *, const u8 *, u32, unsigned int);
typedef void (*iap_raw_send_fn)(void *, void *, unsigned int, const u8 *, u32);
typedef void (*app_tbt_fn)(void *, int);
typedef int (*app_aa_status_fn)(void *, int);
typedef int (*app_aa_distance_fn)(void *, int, int);
typedef int (*app_aa_next_fn)(void *, const void *);
typedef void (*app_usb_connection_fn)(void *, int, int);
typedef void (*app_owner_event_fn)(void *, void *);
typedef void (*app_aa_focus_fn)(void *, int);
typedef void (*app_aoap_connection_fn)(void *, int);
typedef void (*app_notifier_ctor_fn)(void *, void *);
typedef void (*app_notifier_dtor_fn)(void *);
typedef u32 (*app_event_get_fn)(const void *);
typedef void (*navi_owner_event_fn)(void *, void *);
typedef void (*navi_can_send_fn)(void *, u8 *, s16);
typedef void (*navi_sender_dtor_fn)(void *);
struct sqcan_frame;
typedef void (*app_qcan_signal_fn)(void *, const struct sqcan_frame *);
typedef void (*app_qcan_raw_fn)(void *, const struct sqcan_frame *);
typedef int (*thread_create_fn)(unsigned long *, const void *,
                                void *(*)(void *), void *);
typedef int (*thread_detach_fn)(unsigned long);

struct hook_record {
    u8 *target;
    void *replacement;
    u32 stolen;
    u8 original[16];
    u8 *trampoline;
    int installed;
};

struct sqcan_frame {
    u32 id;
    u8 dlc;
    u8 data[8];
    u8 padding[3];
};

typedef char sqcan_frame_must_be_16[
    sizeof(struct sqcan_frame) == 16u ? 1 : -1];

struct writer_state {
    volatile int lock;
    void *notifier;
    void *qcan;
    u32 generation;
    u32 input_epoch;
    u32 owner_since_ms;
    u32 last_tx_ms;
    u32 submitted_generation;
    u32 submitted_at_ms;
    u32 cp_gap_since_ms;
    u32 inflight;
    u8 country_variant;
    u8 owner_source;
    u8 pending_sources;
    u8 desired;
    u8 desired_source;
    u8 desired_frames;
    u8 clear_reason;
    u8 cp_gap_hold;
    u8 gap_clear_cancellable;
    u8 published;
    u8 published_source;
    u8 published_frames;
    u8 notifier_closing;
    u8 transport_ready;
    u8 emissions_started;
    u8 stop;
    u8 stopped;
    u8 desired_payload[8];
    u8 desired_route_payload[8];
    u8 published_payload[8];
    u8 published_route_payload[8];
};

struct writer_candidate {
    u8 frames;
    u8 cp_gap_hold;
    u32 cp_gap_since_ms;
    u8 payload_115[8];
    u8 payload_506[8];
};

struct frame_provenance {
    const struct sqcan_frame *pointer;
    void *qcan;
    u32 generation;
    u8 kind;
    u8 queued;
};

struct pg_packet {
    u32 magic;
    u16 version;
    u16 size;
    u8 source;
    u8 event_kind;
    u8 clear_reason;
    u8 route_active;
    u32 session_epoch;
    u32 sequence;
    u32 monotonic_ms;
    u32 presence;
    u32 maneuver_revision;
    u16 maneuver_index;
    u16 linked_lane_index;
    u16 lane_index;
    u16 lane_count;
    u8 route_state;
    u8 maneuver_state;
    u8 source_type;
    u8 semantic;
    u8 driving_side;
    u8 junction_type;
    u8 visible;
    u8 source_supports;
    s16 exit_angle_deg;
    u8 turn_side;
    u8 reserved;
    u32 next_turn_metres;
    u32 distance_remaining_metres;
    u64 eta_unix_seconds;
    u64 time_remaining_seconds;
    char current_road[TEXT_BYTES];
    char destination[TEXT_BYTES];
    char description[TEXT_BYTES];
    char after_road[TEXT_BYTES];
    char exit_info[TEXT_BYTES];
} __attribute__((packed));

struct lease_packet {
    u32 magic;
    u16 version;
    u16 size;
    u32 sequence;
    u32 generation;
    u32 monotonic_ms;
    u8 active;
    u8 frames;
    u8 source;
    u8 reserved;
} __attribute__((packed));

typedef char lease_packet_must_be_24[
    sizeof(struct lease_packet) == 24u ? 1 : -1];

struct navi_gate_state {
    volatile int lock;
    volatile int send_lock;
    void *sender;
    void *destroyed_sender;
    u32 lease_connection;
    u32 lease_sequence;
    u32 lease_rx_ms;
    u32 owner_connection;
    u32 owner_sequence;
    u32 replay_deadline_ms;
    u32 sender_epoch;
    u8 lease_valid;
    u8 lease_active;
    u8 lease_frames;
    u8 lease_source;
    u8 lease_sequence_valid;
    u8 owner_sequence_valid;
    u8 local_projected;
    u8 owner_needs_fresh;
    u8 gate_engaged;
    u8 replay_wait_inactive;
    u8 replay_pending;
    u8 cache_valid;
    u8 closing;
    u8 suppress_logged;
    u8 awaiting_inactive;
    u8 cache_123[8];
    u8 cache_115[8];
    u8 cache_506[8];
};

struct ingress_item {
    volatile int ready;
    u32 order;
    u32 epoch;
    u32 kind;
    u16 message_id;
    u16 reserved;
    u32 length;
    u8 data[MAX_FRAME];
};

struct ident_scan { int sent_seen; int received_seen; int route_present; };
struct session_state {
    volatile int lock;
    void *self;
    void *link;
    unsigned int session;
    u32 epoch;
    int advertised;
    int accepted;
    int rejected;
    int auth;
    int start_sent;
    volatile int failed;
    void *early_self;
    void *early_link;
    unsigned int early_session;
    int early_auth;
};

struct route_fields {
    u32 presence;
    u8 route_state;
    u8 maneuver_state;
    u8 visible;
    u8 source_supports;
    u16 current_index;
    u16 lane_index;
    u16 lane_count;
    u32 next_turn_metres;
    u32 distance_remaining_metres;
    u64 eta_unix_seconds;
    u64 time_remaining_seconds;
    char current_road[TEXT_BYTES];
    char destination[TEXT_BYTES];
};

struct maneuver_fields {
    int valid;
    u16 index;
    u32 presence;
    u8 source_type;
    u8 semantic;
    u8 driving_side;
    u8 junction_type;
    s16 exit_angle_deg;
    u16 linked_lane_index;
    char description[TEXT_BYTES];
    char after_road[TEXT_BYTES];
    char exit_info[TEXT_BYTES];
};

struct decoder_state {
    u32 epoch;
    u32 sequence;
    u32 revision;
    int current_valid;
    u16 current_index;
    struct route_fields route;
    struct maneuver_fields maneuvers[MANEUVER_CACHE];
    struct pg_packet latest;
    int latest_valid;
    int failed;
};

static const u8 k_iap_send_prologue[5] = {0x55,0x89,0xe5,0x56,0x53};
static const u8 k_iap_dispatch_prologue[11] = {
    0x55,0x89,0xe5,0x53,0x8d,0xa4,0x24,0x7c,0xfe,0xff,0xff
};
static const u8 k_iap_raw_send_prologue[8] = {
    0x55,0x8d,0x2c,0x24,0x8d,0x64,0x24,0xc8
};
static const u8 k_app_tbt_prologue[7] = {0x8d,0xa4,0x24,0x64,0xff,0xff,0xff};
static const u8 k_app_aa_status_prologue[8] = {
    0x55,0x57,0x56,0x53,0x8d,0x64,0x24,0xa4
};
static const u8 k_app_aa_guidance_prologue[8] = {
    0x55,0x57,0x56,0x53,0x8d,0x64,0x24,0x94
};
static const u8 k_app_usb_connection_prologue[11] = {
    0x55,0x57,0x56,0x53,0x8d,0xa4,0x24,0x44,0xff,0xff,0xff
};
static const u8 k_app_owner_event_prologue[8] = {
    0x55,0x57,0x56,0x53,0x8d,0x64,0x24,0x84
};
static const u8 k_app_aa_focus_prologue[8] = {
    0x8d,0x64,0x24,0x84,0x89,0x5c,0x24,0x6c
};
static const u8 k_app_aoap_connection_prologue[8] = {
    0x55,0x57,0x56,0x53,0x8d,0x64,0x24,0xa4
};
static const u8 k_app_notifier_ctor_prologue[8] = {
    0x55,0x57,0x56,0x53,0x8d,0x64,0x24,0x94
};
static const u8 k_app_notifier_dtor_prologue[8] = {
    0x8d,0x64,0x24,0xe4,0x89,0x5c,0x24,0x14
};
static const u8 k_app_qcan_raw_prologue[6] = {
    0x55,0x89,0xe5,0x57,0x56,0x53
};
static const u8 k_navi_owner_event_prologue[10] = {
    0x55,0x89,0xe5,0x8d,0xa4,0x24,0xa8,0xfc,0xff,0xff
};
static const u8 k_navi_sender_dtor_prologue[8] = {
    0x55,0x89,0xe5,0x53,0x8d,0x64,0x24,0xdc
};
static const u8 k_navi_can_send_prologue[7] = {
    0x55,0x89,0xe5,0x8d,0x64,0x24,0xc8
};
static const u8 k_navi_build_id[20] = {
    0x91,0x83,0xe1,0x2e,0xfc,0x5b,0xc7,0x4e,0x6a,0xf6,
    0x09,0x4b,0x28,0x4b,0xa2,0xc9,0xc3,0xfc,0x98,0x4c
};
static const u8 k_route_component[86] = {
    0x00,0x56,0x00,0x1e,0x00,0x06,0x00,0x00,0x00,0x10,
    0x00,0x22,0x00,0x01,
    0x52,0x6f,0x75,0x74,0x65,0x47,0x75,0x69,0x64,0x61,0x6e,0x63,0x65,
    0x44,0x69,0x73,0x70,0x6c,0x61,0x79,0x43,0x6f,0x6d,0x70,0x6f,0x6e,
    0x65,0x6e,0x74,0x00,
    0x00,0x06,0x00,0x02,0x01,0x00,0x00,0x06,0x00,0x03,0x01,0x00,
    0x00,0x06,0x00,0x04,0x01,0x00,0x00,0x06,0x00,0x05,0x01,0x00,
    0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x07,0x01,0x00,
    0x00,0x06,0x00,0x08,0x00,0x06
};
static const u8 k_start_frame[24] = {
    0x40,0x40,0x00,0x18,0x52,0x00,0x00,0x06,0x00,0x00,0x00,0x10,
    0x00,0x04,0x00,0x01,0x00,0x04,0x00,0x02,0x00,0x04,0x00,0x03
};
static const char k_socket_name[] = "dh.projected-guidance.v1";
static const char k_lease_socket_name[] = "dh.projected-guidance.lease.v1";

static struct hook_record g_iap_send_hook, g_iap_dispatch_hook;
static struct hook_record g_app_tbt_hook, g_app_aa_status_hook;
static struct hook_record g_app_aa_distance_hook, g_app_aa_next_hook;
static struct hook_record g_app_usb_connection_hook;
static struct hook_record g_app_owner_event_hook, g_app_aa_focus_hook;
static struct hook_record g_app_aoap_connection_hook;
static struct hook_record g_app_notifier_ctor_hook, g_app_notifier_dtor_hook;
static struct hook_record g_app_qcan_raw_hook;
static struct hook_record g_navi_owner_event_hook, g_navi_sender_dtor_hook;
static struct hook_record g_navi_can_send_hook;
static iap_send_fn g_iap_original_send;
static iap_dispatch_fn g_iap_original_dispatch;
static iap_raw_send_fn g_iap_raw_send = (iap_raw_send_fn)IAP_RAW_SEND_ADDRESS;
static app_tbt_fn g_app_original_tbt;
static app_aa_status_fn g_app_original_aa_status;
static app_aa_distance_fn g_app_original_aa_distance;
static app_aa_next_fn g_app_original_aa_next;
static app_usb_connection_fn g_app_original_usb_connection;
static app_owner_event_fn g_app_original_owner_event;
static app_aa_focus_fn g_app_original_aa_focus;
static app_aoap_connection_fn g_app_original_aoap_connection;
static app_notifier_ctor_fn g_app_original_notifier_ctor;
static app_notifier_dtor_fn g_app_original_notifier_dtor;
static app_qcan_raw_fn g_app_original_qcan_raw;
static navi_owner_event_fn g_navi_original_owner_event;
static navi_sender_dtor_fn g_navi_original_sender_dtor;
static navi_can_send_fn g_navi_original_can_send;
static app_event_get_fn g_navi_event_id =
    (app_event_get_fn)NAVI_EVENT_ID_ADDRESS;
static app_event_get_fn g_navi_event_command =
    (app_event_get_fn)NAVI_EVENT_COMMAND_ADDRESS;
static app_qcan_signal_fn g_app_qcan_signal =
    (app_qcan_signal_fn)APP_QCAN_SIGNAL_ADDRESS;
static app_event_get_fn g_app_event_id =
    (app_event_get_fn)APP_EVENT_ID_ADDRESS;
static app_event_get_fn g_app_event_command =
    (app_event_get_fn)APP_EVENT_COMMAND_ADDRESS;
static struct session_state g_session;
static struct ingress_item g_ingress[FRAME_SLOTS];
static volatile u32 g_ingress_order;
static volatile u32 g_ingress_dropped;
static volatile u32 g_ingress_drop_epoch;
static volatile int g_ingress_producer_lock;
static volatile int g_iap_ready;
static volatile int g_cp_tbt = -1;
static volatile int g_cp_usb_connected = -1;
static volatile int g_cp_clear_pending;
static volatile int g_core_lock;
static volatile u32 g_aa_revision;
static volatile int g_aa_active;
static volatile int g_aa_clear_pending;
static volatile int g_aa_pair_pending;
static struct pg_packet g_core_aa;
static struct pg_packet g_core_cp;
static int g_core_cp_valid;
static int g_core_cp_gap_pending;
static u32 g_core_cp_gap_since_ms;
static int g_core_cp_sequence_valid;
static u32 g_core_cp_epoch;
static u32 g_core_cp_sequence;
static struct pg_packet g_core_cp_logged;
static int g_core_cp_logged_valid;
static u8 g_core_cp_logged_decision;
static volatile int g_core_aa_valid;
static struct writer_state g_writer;
static volatile int g_provenance_lock;
static struct frame_provenance g_provenance[PROVENANCE_SLOTS];
static volatile u32 g_qcan_failure_events;
static struct sqcan_frame g_qcan_failure_sentinel = {0xffffffffu, 0u,
    {0u,0u,0u,0u,0u,0u,0u,0u}, {0u,0u,0u}};
static int g_writer_wake[2] = {-1, -1};
static int g_lease_wake[2] = {-1, -1};
static int g_navi_server_fd = -1;
static int g_qcan_metatype_irreversible;
static volatile int g_aa_focus_request = -1;
static volatile int g_aa_aoap_connected = -1;
static volatile int g_navi_ready;
static struct navi_gate_state g_navi_gate;

extern void qcan_send_raw_export(void *, const struct sqcan_frame *)
    __asm__("_ZN25CQCANController_ApplnTxRx20Send_data_to_can_rawERK10SQCanFrame")
    __attribute__((weak, visibility("default")));

static u16 be16(const u8 *p) { return (u16)(((u16)p[0] << 8) | p[1]); }
static u32 be32(const u8 *p) {
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) |
           ((u32)p[2] << 8) | (u32)p[3];
}
static u64 be64(const u8 *p) {
    u64 value = 0;
    u32 i;
    for (i = 0; i < 8; ++i) value = (value << 8) | p[i];
    return value;
}
static s16 bes16(const u8 *p) { return (s16)be16(p); }
static void put16(u8 *p, u16 value) {
    p[0] = (u8)(value >> 8); p[1] = (u8)value;
}
static usize cstrlen(const char *p) {
    usize n = 0; if (!p) return 0; while (p[n]) ++n; return n;
}
static u32 mono_ms(void) {
    struct timespec_native ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts)) return 0;
    return (u32)ts.tv_sec * 1000u + (u32)(ts.tv_nsec / 1000000l);
}
static void pause_ms(u32 milliseconds) {
    struct timespec_native ts;
    ts.tv_sec = (signed long)(milliseconds / 1000u);
    ts.tv_nsec = (signed long)(milliseconds % 1000u) * 1000000l;
    nanosleep(&ts, 0);
}
static int elapsed(u32 now, u32 before, u32 limit) {
    return (u32)(now - before) >= limit;
}

static int exact_process(const char *expected) {
    char path[96];
    usize length = cstrlen(expected);
    ssize got = readlink("/proc/self/exe", path, sizeof(path));
    return got == (ssize)length && !memcmp(path, expected, length);
}

static int exact_peer(int fd, const char *expected) {
    struct ucred_native credential;
    u32 size = (u32)sizeof(credential);
    char proc_path[48], actual[96];
    int count;
    ssize got;
    usize expected_len = cstrlen(expected);
    if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &credential, &size) ||
            size != sizeof(credential) || credential.uid != geteuid() ||
            credential.pid <= 1) return 0;
    count = snprintf(proc_path, sizeof(proc_path), "/proc/%d/exe", credential.pid);
    if (count <= 0 || (usize)count >= sizeof(proc_path)) return 0;
    got = readlink(proc_path, actual, sizeof(actual));
    return got == (ssize)expected_len && !memcmp(actual, expected, expected_len);
}

static void native_log(const char *line) {
    if (!line) return;
    if (logger_system_logs && &logger_existing_instance &&
            logger_existing_instance)
        logger_system_logs(QT_CRITICAL_MSG, line);
    else syslog(6, "%s", line);
}

static void log_escape(const char source[TEXT_BYTES],
                       char output[TEXT_BYTES * 4u]) {
    static const char digits[] = "0123456789ABCDEF";
    u32 input = 0, used = 0;
    while (input < TEXT_BYTES && source[input]) {
        u8 value = (u8)source[input++];
        if (value >= 0x20u && value < 0x7fu &&
                value != '\\' && value != '"') {
            output[used++] = (char)value;
        } else {
            output[used++] = '\\'; output[used++] = 'x';
            output[used++] = digits[value >> 4];
            output[used++] = digits[value & 0x0fu];
        }
    }
    output[used] = 0;
}

static const char *semantic_name(u8 semantic) {
    static const char *const names[] = {
        "UNKNOWN", "CONTEXT_NO_ACTION", "TURN_LEFT", "TURN_RIGHT",
        "STRAIGHT", "U_TURN", "CONTINUE", "ROUNDABOUT_ENTER",
        "ROUNDABOUT_EXIT", "OFF_RAMP", "ON_RAMP", "ARRIVE",
        "PROCEED_TO_ROUTE", "KEEP_LEFT", "KEEP_RIGHT", "FERRY_ENTER",
        "FERRY_EXIT", "FERRY_CHANGE", "U_TURN_TO_ROUTE",
        "ROUNDABOUT_U_TURN", "OFF_RAMP_LEFT", "OFF_RAMP_RIGHT",
        "ARRIVE_LEFT", "ARRIVE_RIGHT", "SHARP_LEFT", "SHARP_RIGHT",
        "SLIGHT_LEFT", "SLIGHT_RIGHT", "CHANGE_HIGHWAY",
        "CHANGE_HIGHWAY_LEFT", "CHANGE_HIGHWAY_RIGHT"
    };
    return semantic < sizeof(names) / sizeof(names[0]) ? names[semantic] : names[0];
}

static int genesis_eu_symbol_allowed(u8 symbol) {
    return !((symbol >= 0xa0u && symbol <= 0xa5u) || symbol == 0xb2u);
}

static u8 maneuver_semantic(u8 source_type) {
    if (source_type >= 28u && source_type <= 46u) return SEM_ROUNDABOUT_EXIT;
    switch (source_type) {
    case 0: return SEM_CONTEXT;
    case 1: case 20: return SEM_TURN_LEFT;
    case 2: case 21: return SEM_TURN_RIGHT;
    case 3: return SEM_STRAIGHT;
    case 4: case 26: return SEM_U_TURN;
    case 5: return SEM_CONTINUE;
    case 6: return SEM_ROUNDABOUT_ENTER;
    case 7: return SEM_ROUNDABOUT_EXIT;
    case 8: return SEM_OFF_RAMP;
    case 9: return SEM_ON_RAMP;
    case 10: case 12: case 27: return SEM_ARRIVE;
    case 11: return SEM_PROCEED_TO_ROUTE;
    case 13: return SEM_KEEP_LEFT;
    case 14: return SEM_KEEP_RIGHT;
    case 15: return SEM_FERRY_ENTER;
    case 16: return SEM_FERRY_EXIT;
    case 17: return SEM_FERRY_CHANGE;
    case 18: return SEM_U_TURN_TO_ROUTE;
    case 19: return SEM_ROUNDABOUT_U_TURN;
    case 22: return SEM_OFF_RAMP_LEFT;
    case 23: return SEM_OFF_RAMP_RIGHT;
    case 24: return SEM_ARRIVE_LEFT;
    case 25: return SEM_ARRIVE_RIGHT;
    case 47: return SEM_SHARP_LEFT;
    case 48: return SEM_SHARP_RIGHT;
    case 49: return SEM_SLIGHT_LEFT;
    case 50: return SEM_SLIGHT_RIGHT;
    case 51: return SEM_CHANGE_HIGHWAY;
    case 52: return SEM_CHANGE_HIGHWAY_LEFT;
    case 53: return SEM_CHANGE_HIGHWAY_RIGHT;
    default: return SEM_UNKNOWN;
    }
}

static int frame_id(const u8 *data, u32 length, u16 *message_id) {
    if (!data || !message_id || length < 6u || length > MAX_FRAME) return -1;
    if (data[0] != 0x40 || data[1] != 0x40 || be16(data + 2) != length)
        return -2;
    *message_id = be16(data + 4);
    return 0;
}

struct genesis_115_distance {
    u16 whole;
    u8 unit;
    u8 decimal;
};

/*
 * AppNavi first rounds the signed metre count to binary32, widens it to
 * binary64, then performs each imperial operation in binary64.  The volatile
 * stores preserve those exact stages even in an IA-32 build using x87.
 */
static double genesis_115_imperial_product(u32 metres, double multiplier) {
    volatile float binary32_metres = (float)(s32)metres;
    volatile double binary64_metres = (double)binary32_metres;
    volatile double product = binary64_metres * multiplier;
    return product;
}

static int genesis_115_encode_distance(u32 metres, int imperial,
                                       struct genesis_115_distance *output) {
    struct genesis_115_distance encoded;
    u32 converted;
    if (!output || (imperial != 0 && imperial != 1) ||
            metres > 0x7fffffffu) return -1;
    encoded.decimal = 0u;
    if (!imperial) {
        encoded.unit = 0u;
        if (metres > 99949u) {
            converted = (metres + 500u) / 1000u;
            encoded.unit = 1u;
        } else if (metres > 949u) {
            converted = (metres + 50u) / 100u;
            encoded.whole = (u16)(converted / 10u);
            encoded.unit = 1u;
            encoded.decimal = (u8)(converted % 10u);
            *output = encoded;
            return 0;
        } else if (metres > 299u) {
            converted = ((metres + 50u) / 100u) * 100u;
        } else if (metres > 9u) {
            converted = (metres / 10u) * 10u;
        } else if (metres) {
            converted = metres > 7u ? 10u : 5u;
        } else converted = 0u;
    } else {
        if (metres > 160949u) {
            volatile double rounded =
                genesis_115_imperial_product(metres, 0.000621) + 0.5;
            converted = (u32)rounded;
            encoded.unit = 2u;
        } else if (metres > 320u) {
            volatile double rounded_tenths =
                genesis_115_imperial_product(metres, 0.000621) + 0.05;
            volatile double tenths_product = rounded_tenths * 10.0;
            converted = (u32)tenths_product;
            encoded.whole = (u16)(converted / 10u);
            encoded.unit = 2u;
            encoded.decimal = (u8)(converted % 10u);
            *output = encoded;
            return 0;
        } else if (metres > 9u) {
            volatile double tens =
                genesis_115_imperial_product(metres, 1.093613) / 10.0;
            converted = (u32)tens * 10u;
            encoded.unit = 4u;
        } else if (metres) {
            converted =
                (u32)genesis_115_imperial_product(metres, 1.093613);
            converted = converted > 7u ? 10u : 5u;
            encoded.unit = 4u;
        } else {
            converted = 0u;
            encoded.unit = 4u;
        }
    }
    if (converted > 0xffffu) return -2;
    encoded.whole = (u16)converted;
    *output = encoded;
    return 0;
}

static int genesis_115_encode_no_info(u8 output[8]) {
    static const u8 no_info[8] = {
        0x01u, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu
    };
    if (!output) return -1;
    memcpy(output, no_info, sizeof(no_info));
    return 0;
}

/*
 * Vehicle-captured AppNavi 0x506 route-summary layout.  A live projected
 * heading is not available in AppProjection, so B3 uses the stock invalid
 * value rather than reusing a captured vehicle heading.  B6=09 is the
 * captured active arrival-time/status combination; the OEM name of bits 2-3
 * is not recovered.  Stock clears retain their live heading byte.  This
 * writer therefore retains its published B3 (currently FF) and applies the
 * recovered 0F clear status; that FF-heading clear still needs vehicle rating.
 */
static int genesis_506_encode_arrival(u32 metres, u8 hour, u8 minute,
                                      u8 output[8]) {
    struct genesis_115_distance distance;
    u8 encoded[8];
    int result;
    if (!output || hour > 23u || minute > 59u) return -1;
    result = genesis_115_encode_distance(metres, 0, &distance);
    if (result) return result;
    encoded[0] = (u8)((distance.unit << 4) | distance.decimal);
    encoded[1] = hour;
    encoded[2] = minute;
    encoded[3] = 0xffu;
    encoded[4] = (u8)(distance.whole >> 8);
    encoded[5] = (u8)(distance.whole & 0xffu);
    encoded[6] = 0x09u;
    encoded[7] = 0x00u;
    memcpy(output, encoded, sizeof(encoded));
    return 0;
}

static int genesis_506_encode_no_info(u8 output[8]) {
    static const u8 no_info[8] = {
        0xffu,0xffu,0xffu,0xffu,0xffu,0xffu,0x0fu,0x00u
    };
    if (!output) return -1;
    memcpy(output, no_info, sizeof(no_info));
    return 0;
}

static int genesis_506_encode_packet(const struct pg_packet *packet,
                                     u8 output[8]) {
    struct tm_native local;
    signed long eta;
    if (!packet || !output || packet->source != SOURCE_CARPLAY ||
            packet->event_kind != EVENT_SNAPSHOT || !packet->route_active ||
            (packet->presence & (PRES_DISTANCE_REMAIN | PRES_ETA)) !=
                (PRES_DISTANCE_REMAIN | PRES_ETA) ||
            !packet->eta_unix_seconds ||
            packet->eta_unix_seconds > 0x7fffffffu) return -1;
    eta = (signed long)packet->eta_unix_seconds;
    memset(&local, 0, sizeof(local));
    if (!localtime_r(&eta, &local) || local.tm_hour < 0 ||
            local.tm_hour > 23 || local.tm_min < 0 || local.tm_min > 59)
        return -2;
    return genesis_506_encode_arrival(packet->distance_remaining_metres,
                                      (u8)local.tm_hour, (u8)local.tm_min,
                                      output);
}

static int genesis_115_encode_symbol_frame(u8 symbol, u32 metres,
                                           int imperial, u8 output[8]) {
    struct genesis_115_distance distance;
    u8 encoded[8];
    int result;
    if (!output || symbol == GENESIS_NO_INFO ||
            !genesis_eu_symbol_allowed(symbol)) return -1;
    result = genesis_115_encode_distance(metres, imperial, &distance);
    if (result) return result;
    encoded[0] = symbol;
    encoded[1] = 0u;
    encoded[2] = 0u;
    encoded[3] = 0x03u;
    encoded[4] = (u8)(distance.whole >> 8);
    encoded[5] = (u8)(distance.whole & 0xffu);
    encoded[6] = (u8)(distance.unit << 4);
    encoded[7] = (u8)(0xf0u | distance.decimal);
    memcpy(output, encoded, sizeof(encoded));
    return 0;
}

static int genesis_roundabout_symbol(s32 bearing_degrees, int left_hand_traffic,
                                     u8 *output) {
    s32 normalized;
    u32 sector;
    if (!output || (left_hand_traffic != 0 && left_hand_traffic != 1))
        return -1;
    normalized = bearing_degrees % 360;
    if (normalized < 0) normalized += 360;
    sector = ((u32)normalized + 15u) / 30u;
    sector %= 12u;
    *output = (u8)((left_hand_traffic ? 0xc0u : 0x60u) + sector);
    return 0;
}

static int packet_turn_side(const struct pg_packet *packet, u8 *side) {
    if (!(packet->presence & PRES_TURN_SIDE) || packet->turn_side < 1u ||
            packet->turn_side > 3u) return -1;
    *side = packet->turn_side;
    return 0;
}

static int aa_roundabout_symbol(const struct pg_packet *packet, u8 *output) {
    u32 quantized;
    u32 sector;
    u8 side;
    if (packet_turn_side(packet, &side) || side == 3u ||
            !(packet->presence & PRES_EXIT_ANGLE) ||
            packet->exit_angle_deg < 1 || packet->exit_angle_deg > 360)
        return -1;
    quantized = ((u32)packet->exit_angle_deg + 15u) / 30u;
    quantized %= 12u;
    if (side == 1u) {
        /* AA LEFT is the clockwise/LHT family; quantize its sweep first. */
        sector = (6u + quantized) % 12u;
        *output = (u8)(0xc0u + sector);
    } else {
        /* AA RIGHT is the counterclockwise/RHT family. */
        sector = (18u - quantized) % 12u;
        *output = (u8)(0x60u + sector);
    }
    return 0;
}

static int cp_roundabout_type(u8 source_type) {
    return source_type == 7u ||
           (source_type >= 28u && source_type <= 46u);
}

static int cp_roundabout_symbol(const struct pg_packet *packet, u8 *output) {
    if (!(packet->presence & PRES_EXIT_ANGLE) ||
            !(packet->presence & PRES_DRIVING_SIDE) ||
            !(packet->presence & PRES_JUNCTION_TYPE) ||
            packet->junction_type != 1u || packet->driving_side > 1u ||
            packet->exit_angle_deg < -360 || packet->exit_angle_deg > 360)
        return -1;
    return genesis_roundabout_symbol((s32)packet->exit_angle_deg,
                                     packet->driving_side == 1u, output);
}

static int resolve_aa_maneuver(const struct pg_packet *packet, u8 *output) {
    u8 side;
    switch (packet->source_type) {
    case 1u:
        *output = 0x02u; return 0;
    case 3u:
        if (packet_turn_side(packet, &side)) return -1;
        *output = side == 1u ? 0x47u : (side == 2u ? 0x42u : 0x1au);
        return 0;
    case 4u:
        if (packet_turn_side(packet, &side)) return -1;
        *output = side == 1u ? 0x46u : (side == 2u ? 0x43u : 0x1au);
        return 0;
    case 5u:
        if (packet_turn_side(packet, &side)) return -1;
        *output = side == 1u ? 0x45u : (side == 2u ? 0x44u : 0x1au);
        return 0;
    case 6u:
        if (packet_turn_side(packet, &side) || side == 3u) return -1;
        *output = side == 1u ? 0x48u : 0x49u;
        return 0;
    case 7u:
        if (packet_turn_side(packet, &side)) return -1;
        *output = side == 1u ? 0x58u : (side == 2u ? 0x57u : 0x56u);
        return 0;
    case 8u:
        if (packet_turn_side(packet, &side)) return -1;
        *output = side == 1u ? 0xa8u : 0xa7u;
        return 0;
    case 9u:
        if (packet_turn_side(packet, &side)) return -1;
        *output = side == 1u ? 0x91u : (side == 2u ? 0x90u : 0x1au);
        return 0;
    case 10u:
        if (packet_turn_side(packet, &side) || side == 3u) return -1;
        *output = side == 1u ? 0x55u : 0x54u;
        return 0;
    case 13u:
        return aa_roundabout_symbol(packet, output);
    case 14u:
        *output = 0x41u; return 0;
    case 16u: case 17u:
        *output = 0xb0u; return 0;
    case 19u:
        if (packet_turn_side(packet, &side)) return -1;
        *output = side == 1u ? 0x72u : (side == 2u ? 0x71u : 0x70u);
        return 0;
    default:
        return -1;
    }
}

static int resolve_cp_maneuver(const struct pg_packet *packet, u8 *output) {
    u8 source_type = packet->source_type;
    if (cp_roundabout_type(source_type))
        return cp_roundabout_symbol(packet, output);
    switch (source_type) {
    case 0u: case 5u:
        *output = 0x0bu; return 0;
    case 1u: case 20u:
        *output = 0x46u; return 0;
    case 2u: case 21u:
        *output = 0x43u; return 0;
    case 3u:
        *output = 0x41u; return 0;
    case 4u: case 26u:
        if (!(packet->presence & PRES_DRIVING_SIDE) ||
                packet->driving_side > 1u) return -1;
        *output = packet->driving_side == 1u ? 0x49u : 0x48u;
        return 0;
    case 8u:
        *output = 0xa7u; return 0;
    case 9u:
        *output = 0x56u; return 0;
    case 10u: case 12u: case 27u:
        *output = 0x70u; return 0;
    case 11u: case 18u:
        *output = 0x02u; return 0;
    case 13u:
        *output = 0x91u; return 0;
    case 14u:
        *output = 0x90u; return 0;
    case 15u: case 16u: case 17u:
        *output = 0xb0u; return 0;
    case 19u:
        if (!(packet->presence & PRES_DRIVING_SIDE) ||
                packet->driving_side > 1u) return -1;
        *output = packet->driving_side == 1u ? 0xc6u : 0x66u;
        return 0;
    case 22u:
        *output = 0xa8u; return 0;
    case 23u:
        *output = 0xa7u; return 0;
    case 24u:
        *output = 0x72u; return 0;
    case 25u:
        *output = 0x71u; return 0;
    case 47u:
        *output = 0x45u; return 0;
    case 48u:
        *output = 0x44u; return 0;
    case 49u:
        *output = 0x47u; return 0;
    case 50u:
        *output = 0x42u; return 0;
    case 52u:
        *output = 0x55u; return 0;
    case 53u:
        *output = 0x54u; return 0;
    default:
        return -1;
    }
}

static int resolve_maneuver(const struct pg_packet *packet, u8 *output) {
    int result;
    if (!packet || !output || !(packet->presence & PRES_SOURCE_TYPE)) return -1;
    if (packet->source == SOURCE_ANDROID_AUTO)
        result = resolve_aa_maneuver(packet, output);
    else if (packet->source == SOURCE_CARPLAY)
        result = resolve_cp_maneuver(packet, output);
    else return -1;
    if (result || !genesis_eu_symbol_allowed(*output) ||
            *output == GENESIS_NO_INFO) return -1;
    return 0;
}

static int route_id(u16 value) { return (value & 0xfff0u) == 0x5200u; }

static int has_route_id(const u8 *data, u32 length) {
    u32 offset;
    if (length & 1u) return 0;
    for (offset = 0; offset < length; offset += 2u)
        if (route_id(be16(data + offset))) return 1;
    return 0;
}

static int scan_ident(const u8 *data, u32 length, struct ident_scan *scan) {
    u32 offset = 6u, count = 0;
    u16 message_id;
    if (!scan) return -1;
    memset(scan, 0, sizeof(*scan));
    if (frame_id(data, length, &message_id) || message_id != MSG_IDENT_INFO)
        return -2;
    while (offset < length) {
        u16 total, type;
        u32 value_length;
        if (++count > MAX_TLVS || length - offset < 4u) return -3;
        total = be16(data + offset);
        type = be16(data + offset + 2u);
        if (total < 4u || total > length - offset) return -4;
        value_length = (u32)total - 4u;
        if (value_length > MAX_TLV_VALUE) return -5;
        if (type == IDENT_ROUTE_COMPONENT) scan->route_present = 1;
        if (type == IDENT_SENT) {
            if (++scan->sent_seen != 1 || (value_length & 1u)) return -6;
            if (has_route_id(data + offset + 4u, value_length))
                scan->route_present = 1;
        } else if (type == IDENT_RECEIVED) {
            if (++scan->received_seen != 1 || (value_length & 1u)) return -7;
            if (has_route_id(data + offset + 4u, value_length))
                scan->route_present = 1;
        }
        offset += total;
    }
    if (offset != length || scan->sent_seen != 1 || scan->received_seen != 1)
        return -8;
    return scan->route_present ? 1 : 0;
}

static int mutate_ident(const u8 *source, u32 source_length,
                        u8 **output, u32 *output_length) {
    struct ident_scan scan;
    u32 source_offset = 6u, destination_offset = 6u, new_length;
    u8 *destination;
    if (!output || !output_length || scan_ident(source, source_length, &scan) != 0)
        return -1;
    if (source_length > MAX_FRAME - 10u - sizeof(k_route_component)) return -2;
    new_length = source_length + 10u + sizeof(k_route_component);
    destination = (u8 *)malloc(new_length + 1u);
    if (!destination) return -3;
    memcpy(destination, source, 6u);
    put16(destination + 2u, (u16)new_length);
    while (source_offset < source_length) {
        u16 total = be16(source + source_offset);
        u16 type = be16(source + source_offset + 2u);
        u32 value_length = (u32)total - 4u;
        u32 added = type == IDENT_SENT ? 4u :
                    (type == IDENT_RECEIVED ? 6u : 0u);
        if (value_length + added > MAX_TLV_VALUE) {
            free(destination); return -4;
        }
        put16(destination + destination_offset, (u16)((u32)total + added));
        put16(destination + destination_offset + 2u, type);
        memcpy(destination + destination_offset + 4u,
               source + source_offset + 4u, value_length);
        destination_offset += 4u + value_length;
        if (type == IDENT_SENT) {
            put16(destination + destination_offset, MSG_ROUTE_START);
            put16(destination + destination_offset + 2u, MSG_ROUTE_STOP);
            destination_offset += 4u;
        } else if (type == IDENT_RECEIVED) {
            put16(destination + destination_offset, MSG_ROUTE_UPDATE);
            put16(destination + destination_offset + 2u, MSG_MANEUVER_UPDATE);
            put16(destination + destination_offset + 4u, MSG_LANE_UPDATE);
            destination_offset += 6u;
        }
        source_offset += total;
    }
    memcpy(destination + destination_offset, k_route_component,
           sizeof(k_route_component));
    destination_offset += sizeof(k_route_component);
    if (destination_offset != new_length) {
        free(destination); return -5;
    }
    destination[new_length] = 0;
    *output = destination;
    *output_length = new_length;
    return 0;
}

static int prepare_hook(struct hook_record *record, void *target,
                        void *replacement, const u8 *expected, u32 stolen) {
    u8 *site = (u8 *)target, *trampoline;
    s32 relative;
    if (!record || stolen < 5u || stolen > sizeof(record->original) ||
            memcmp(site, expected, stolen)) return -1;
    trampoline = (u8 *)mmap(0, 4096u, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (trampoline == (void *)(uptr)-1) return -2;
    memcpy(trampoline, site, stolen);
    trampoline[stolen] = 0xe9;
    relative = (s32)(site + stolen - (trampoline + stolen + 5u));
    memcpy(trampoline + stolen + 1u, &relative, 4u);
    if (mprotect(trampoline, 4096u, PROT_READ | PROT_EXEC)) {
        munmap(trampoline, 4096u); return -3;
    }
    memset(record, 0, sizeof(*record));
    record->target = site;
    record->replacement = replacement;
    record->stolen = stolen;
    record->trampoline = trampoline;
    memcpy(record->original, site, stolen);
    return 0;
}

static void discard_hook(struct hook_record *record) {
    if (record->trampoline) munmap(record->trampoline, 4096u);
    memset(record, 0, sizeof(*record));
}

static int hook_recovery_verified(struct hook_record *record) {
    if (!memcmp(record->target, record->original, record->stolen)) {
        record->installed = 0;
        return 0;
    }
    record->installed = 1;
    return -1;
}

static int commit_hook(struct hook_record *record) {
    u8 patch[16];
    s32 relative;
    uptr page;
    if (!record->target || !record->trampoline || record->stolen < 5u ||
            memcmp(record->target, record->original, record->stolen)) return -1;
    patch[0] = 0xe9;
    relative = (s32)((u8 *)record->replacement - (record->target + 5u));
    memcpy(patch + 1u, &relative, 4u);
    if (record->stolen > 5u) memset(patch + 5u, 0x90, record->stolen - 5u);
    page = ((uptr)record->target) & ~(uptr)4095u;
    if (mprotect((void *)page, 4096u, PROT_READ | PROT_WRITE | PROT_EXEC))
        return -2;
    memcpy(record->target, patch, record->stolen);
    __sync_synchronize();
    record->installed = 1;
    if (memcmp(record->target, patch, record->stolen)) {
        memcpy(record->target, record->original, record->stolen);
        __sync_synchronize();
        if (hook_recovery_verified(record)) {
            (void)mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC);
            return -3;
        }
        (void)mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC);
        return -4;
    }
    if (mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC)) return -5;
    return 0;
}

static int try_lock(volatile int *lock) {
    u32 attempt;
    for (attempt = 0; attempt < 16u; ++attempt)
        if (!__sync_lock_test_and_set(lock, 1)) return 1;
    return 0;
}

static void unlock(volatile int *lock) { __sync_lock_release(lock); }
static const char *clear_name(u8 reason);

static void blocking_lock(volatile int *lock) {
    while (__sync_lock_test_and_set(lock, 1)) __sync_synchronize();
}

static void writer_wake(void) {
    u8 byte = 1u;
    if (g_writer_wake[1] >= 0)
        (void)send(g_writer_wake[1], &byte, 1u,
                   MSG_DONTWAIT | MSG_NOSIGNAL);
}

static void writer_lease_wake(void) {
    u8 byte = 1u;
    if (g_lease_wake[1] >= 0)
        (void)send(g_lease_wake[1], &byte, 1u,
                   MSG_DONTWAIT | MSG_NOSIGNAL);
}

static void writer_input_mutated_internal(int cp_input, int preserve_gap,
                                          int cancel_gap_clear,
                                          int preserve_gap_clear, int wake) {
    int keep_gap_active;
    blocking_lock(&g_writer.lock);
    g_writer.input_epoch++;
    keep_gap_active = cp_input && preserve_gap && g_writer.cp_gap_hold &&
        g_writer.desired == WRITER_DESIRED_ACTIVE &&
        g_writer.desired_source == WRITER_SOURCE_CP;
    if (cp_input && !preserve_gap) {
        g_writer.cp_gap_hold = 0u;
        g_writer.cp_gap_since_ms = 0u;
        if (g_writer.desired == WRITER_DESIRED_CLEAR &&
                g_writer.gap_clear_cancellable) {
            if (cancel_gap_clear) {
                g_writer.desired = WRITER_DESIRED_IDLE;
                g_writer.desired_source = 0u;
                g_writer.desired_frames = 0u;
                g_writer.clear_reason = 0u;
            }
            if (cancel_gap_clear || !preserve_gap_clear)
                g_writer.gap_clear_cancellable = 0u;
        }
    }
    if (g_writer.desired != WRITER_DESIRED_CLEAR && !keep_gap_active) {
        g_writer.generation++;
        g_writer.submitted_generation = 0;
        g_writer.submitted_at_ms = 0u;
        if (g_writer.desired == WRITER_DESIRED_ACTIVE) {
            g_writer.desired = WRITER_DESIRED_IDLE;
            g_writer.desired_source = 0u;
            g_writer.desired_frames = 0u;
        }
    }
    unlock(&g_writer.lock);
    if (wake) writer_wake();
}

static void writer_input_mutated(void) {
    writer_input_mutated_internal(0, 0, 0, 0, 1);
}

static void writer_cp_input_mutated(int preserve_gap) {
    writer_input_mutated_internal(1, preserve_gap, 0, 0, 1);
}

static void writer_cp_input_committed(int preserve_gap, int publishable,
                                      int preserve_gap_clear) {
    writer_input_mutated_internal(1, preserve_gap, publishable,
                                  preserve_gap_clear, 0);
}

static int provenance_add(const struct sqcan_frame *pointer, void *qcan,
                          u32 generation, u8 kind) {
    u32 index;
    int result = -1;
    if (!pointer || !qcan || (kind != WRITER_DESIRED_ACTIVE &&
                              kind != WRITER_DESIRED_CLEAR)) return -1;
    blocking_lock(&g_provenance_lock);
    for (index = 0; index < PROVENANCE_SLOTS; ++index) {
        if (!g_provenance[index].pointer) {
            g_provenance[index].pointer = pointer;
            g_provenance[index].qcan = qcan;
            g_provenance[index].generation = generation;
            g_provenance[index].kind = kind;
            g_provenance[index].queued = 0u;
            result = 0;
            break;
        }
    }
    unlock(&g_provenance_lock);
    return result;
}

static int provenance_get(const struct sqcan_frame *pointer,
                          struct frame_provenance *output) {
    u32 index;
    int result = 0;
    if (!pointer) return 0;
    blocking_lock(&g_provenance_lock);
    for (index = 0; index < PROVENANCE_SLOTS; ++index) {
        if (g_provenance[index].pointer == pointer) {
            if (output) *output = g_provenance[index];
            result = 1;
            break;
        }
    }
    unlock(&g_provenance_lock);
    return result;
}

static int provenance_remove(const struct sqcan_frame *pointer) {
    u32 index;
    int removed = 0;
    blocking_lock(&g_provenance_lock);
    for (index = 0; index < PROVENANCE_SLOTS; ++index) {
        if (g_provenance[index].pointer == pointer) {
            memset(&g_provenance[index], 0, sizeof(g_provenance[index]));
            removed = 1;
            break;
        }
    }
    unlock(&g_provenance_lock);
    return removed;
}

static int provenance_has_queued(void) {
    u32 index;
    int result = 0;
    blocking_lock(&g_provenance_lock);
    for (index = 0; index < PROVENANCE_SLOTS; ++index) {
        if (g_provenance[index].pointer && g_provenance[index].queued) {
            result = 1;
            break;
        }
    }
    unlock(&g_provenance_lock);
    return result;
}

static int provenance_transfer(const struct sqcan_frame *source,
                               const struct sqcan_frame *destination) {
    struct frame_provenance metadata;
    u32 index, empty = PROVENANCE_SLOTS;
    int result = 0;
    blocking_lock(&g_provenance_lock);
    for (index = 0; index < PROVENANCE_SLOTS; ++index) {
        if (g_provenance[index].pointer == source) {
            metadata = g_provenance[index];
            result = 1;
        }
        if (!g_provenance[index].pointer && empty == PROVENANCE_SLOTS)
            empty = index;
    }
    if (result && empty < PROVENANCE_SLOTS) {
        metadata.pointer = destination;
        metadata.queued = 1u;
        g_provenance[empty] = metadata;
        result = 2;
    }
    unlock(&g_provenance_lock);
    return result;
}

static void qcan_frame_destroy(void *pointer) {
    if (pointer == &g_qcan_failure_sentinel) {
        u32 pending = g_qcan_failure_events;
        while (pending) {
            u32 observed = __sync_val_compare_and_swap(
                &g_qcan_failure_events, pending, pending - 1u);
            if (observed == pending) break;
            pending = observed;
        }
        writer_wake();
        return;
    }
    (void)provenance_remove((const struct sqcan_frame *)pointer);
    free(pointer);
}

static void *qcan_frame_copy(const void *source) {
    struct sqcan_frame *copy;
    int owned = provenance_get((const struct sqcan_frame *)source, 0);
    if (!source) return 0;
    copy = (struct sqcan_frame *)malloc(sizeof(*copy));
    if (!copy) {
        (void)__sync_add_and_fetch(&g_qcan_failure_events, 1u);
        return &g_qcan_failure_sentinel;
    }
    memcpy(copy, source, sizeof(*copy));
    if (owned &&
            provenance_transfer((const struct sqcan_frame *)source, copy) != 2) {
        free(copy);
        (void)__sync_add_and_fetch(&g_qcan_failure_events, 1u);
        return &g_qcan_failure_sentinel;
    }
    return copy;
}

static int register_qcan_metatype(void) {
    static const char name[] = "SQCanFrame";
    int type_id;
    if (!qt_metatype_type || !qt_metatype_register) return -1;
    if (qt_metatype_type(name) != 0) return -2;
    type_id = qt_metatype_register(name, qcan_frame_destroy, qcan_frame_copy);
    g_qcan_metatype_irreversible = 1;
    if (type_id <= 0 || qt_metatype_type(name) != type_id) return -3;
    return 0;
}

static int writer_country_allowed(u8 country) {
    return country == COUNTRY_VARIANT_EUROPE;
}

static void core_reset_cp_gap_state(void) {
    blocking_lock(&g_core_lock);
    g_core_cp_gap_pending = 0;
    g_core_cp_gap_since_ms = 0u;
    unlock(&g_core_lock);
}

static int writer_cp_gap_eligible(void) {
    int eligible;
    blocking_lock(&g_writer.lock);
    eligible = !g_writer.notifier_closing &&
        g_writer.owner_source == WRITER_SOURCE_CP &&
        writer_country_allowed(g_writer.country_variant) &&
        g_writer.published &&
        g_writer.published_source == WRITER_SOURCE_CP &&
        g_writer.desired != WRITER_DESIRED_CLEAR;
    unlock(&g_writer.lock);
    return eligible;
}

static int writer_known_frames(u8 frames) {
    return frames && !(frames & (u8)~(WRITER_FRAME_115 | WRITER_FRAME_506));
}

static int writer_active_anchor_locked(const struct sqcan_frame *frame) {
    if (!writer_known_frames(g_writer.desired_frames)) return 0;
    if (g_writer.desired_frames & WRITER_FRAME_115)
        return frame->id == QCAN_FRAME_ID &&
            !memcmp(frame->data, g_writer.desired_payload, 8u);
    return frame->id == QCAN_ROUTE_FRAME_ID &&
        !memcmp(frame->data, g_writer.desired_route_payload, 8u);
}

static void writer_506_clear_locked(u8 output[8]) {
    (void)genesis_506_encode_no_info(output);
    if (g_writer.published_frames & WRITER_FRAME_506)
        output[3] = g_writer.published_route_payload[3];
}

static int writer_clear_anchor_locked(const struct sqcan_frame *frame) {
    u8 clear_payload[8];
    if (!writer_known_frames(g_writer.published_frames)) return 0;
    if (g_writer.published_frames & WRITER_FRAME_115) {
        (void)genesis_115_encode_no_info(clear_payload);
        return frame->id == QCAN_FRAME_ID &&
            !memcmp(frame->data, clear_payload, 8u);
    }
    writer_506_clear_locked(clear_payload);
    return frame->id == QCAN_ROUTE_FRAME_ID &&
        !memcmp(frame->data, clear_payload, 8u);
}

static void writer_raw_frame(void *qcan, u32 id, const u8 payload[8],
                             const struct sqcan_frame *anchor) {
    struct sqcan_frame synthetic;
    if (anchor && anchor->id == id && anchor->dlc == QCAN_FRAME_DLC &&
            !memcmp(anchor->data, payload, 8u)) {
        g_app_original_qcan_raw(qcan, anchor);
        return;
    }
    memset(&synthetic, 0, sizeof(synthetic));
    synthetic.id = id;
    synthetic.dlc = QCAN_FRAME_DLC;
    memcpy(synthetic.data, payload, 8u);
    g_app_original_qcan_raw(qcan, &synthetic);
}

static void writer_raw_clear_set(void *qcan, u8 frames,
                                 const struct sqcan_frame *anchor) {
    u8 payload[8];
    if (frames & WRITER_FRAME_506) {
        writer_506_clear_locked(payload);
        writer_raw_frame(qcan, QCAN_ROUTE_FRAME_ID, payload, anchor);
    }
    if (frames & WRITER_FRAME_115) {
        (void)genesis_115_encode_no_info(payload);
        writer_raw_frame(qcan, QCAN_FRAME_ID, payload, anchor);
    }
}

static void writer_log_frame(const char *action, u8 source, u32 generation,
                             u32 id, const u8 payload[8]) {
    char line[240];
    snprintf(line, sizeof(line),
        "PGI:writer action=%s src=%s gen=%u id=%03X "
        "data=%02X%02X%02X%02X%02X%02X%02X%02X",
        action, source == WRITER_SOURCE_CP ? "CP" : "AA", generation, id,
        payload[0], payload[1], payload[2], payload[3], payload[4],
        payload[5], payload[6], payload[7]);
    native_log(line);
}

static int writer_active_valid_locked(const struct frame_provenance *meta,
                                      const struct sqcan_frame *frame,
                                      void *qcan, u32 now) {
    return meta->kind == WRITER_DESIRED_ACTIVE &&
        frame->dlc == QCAN_FRAME_DLC &&
        qcan == meta->qcan && qcan == g_writer.qcan &&
        g_writer.desired == WRITER_DESIRED_ACTIVE &&
        meta->generation == g_writer.generation &&
        g_writer.owner_source == g_writer.desired_source &&
        writer_country_allowed(g_writer.country_variant) &&
        elapsed(now, g_writer.owner_since_ms, WRITER_GRACE_MS) &&
        writer_active_anchor_locked(frame);
}

static int writer_clear_valid_locked(const struct frame_provenance *meta,
                                     const struct sqcan_frame *frame,
                                     void *qcan) {
    return meta->kind == WRITER_DESIRED_CLEAR &&
        frame->dlc == QCAN_FRAME_DLC &&
        qcan == meta->qcan && qcan == g_writer.qcan &&
        g_writer.desired == WRITER_DESIRED_CLEAR &&
        meta->generation == g_writer.generation &&
        writer_clear_anchor_locked(frame);
}

static void hook_app_qcan_raw(void *qcan, const struct sqcan_frame *frame) {
    struct frame_provenance meta;
    char line[224];
    u8 logged_payload[8], logged_route_payload[8], logged_route_clear[8];
    u8 logged_frames = 0u, logged_removed = 0u;
    u8 logged_source = 0u, logged_reason = 0u, logged_kind = 0u;
    u32 logged_generation = 0u;
    u32 now;
    if (!frame || frame == &g_qcan_failure_sentinel) return;
    if (!provenance_get(frame, &meta)) {
        g_app_original_qcan_raw(qcan, frame);
        return;
    }
    if (!frame || !g_app_original_qcan_raw) return;
    now = mono_ms();
    blocking_lock(&g_writer.lock);
    if (writer_active_valid_locked(&meta, frame, qcan, now)) {
        logged_removed = (u8)(g_writer.published_frames &
                              (u8)~g_writer.desired_frames);
        if (logged_removed & WRITER_FRAME_506)
            writer_506_clear_locked(logged_route_clear);
        writer_raw_clear_set(qcan, logged_removed, 0);
        if (g_writer.desired_frames & WRITER_FRAME_115)
            writer_raw_frame(qcan, QCAN_FRAME_ID, g_writer.desired_payload,
                             frame);
        if (g_writer.desired_frames & WRITER_FRAME_506)
            writer_raw_frame(qcan, QCAN_ROUTE_FRAME_ID,
                             g_writer.desired_route_payload, frame);
        g_writer.published = 1u;
        g_writer.published_source = g_writer.desired_source;
        g_writer.published_frames = g_writer.desired_frames;
        if (g_writer.desired_frames & WRITER_FRAME_115)
            memcpy(g_writer.published_payload, g_writer.desired_payload, 8u);
        else memset(g_writer.published_payload, 0, 8u);
        if (g_writer.desired_frames & WRITER_FRAME_506)
            memcpy(g_writer.published_route_payload,
                   g_writer.desired_route_payload, 8u);
        else memset(g_writer.published_route_payload, 0, 8u);
        g_writer.last_tx_ms = now;
        logged_kind = WRITER_DESIRED_ACTIVE;
        logged_source = g_writer.published_source;
        logged_generation = g_writer.generation;
        logged_frames = g_writer.published_frames;
        memcpy(logged_payload, g_writer.published_payload, 8u);
        memcpy(logged_route_payload, g_writer.published_route_payload, 8u);
    } else if (writer_clear_valid_locked(&meta, frame, qcan)) {
        logged_kind = WRITER_DESIRED_CLEAR;
        logged_source = g_writer.published_source;
        logged_reason = g_writer.clear_reason;
        logged_generation = g_writer.generation;
        logged_frames = g_writer.published_frames;
        writer_raw_clear_set(qcan, logged_frames, frame);
        g_writer.published = 0u;
        g_writer.published_source = 0u;
        g_writer.published_frames = 0u;
        memset(g_writer.published_payload, 0, 8u);
        memset(g_writer.published_route_payload, 0, 8u);
        g_writer.desired = WRITER_DESIRED_IDLE;
        g_writer.desired_source = 0u;
        g_writer.desired_frames = 0u;
        g_writer.cp_gap_hold = 0u;
        g_writer.cp_gap_since_ms = 0u;
        g_writer.gap_clear_cancellable = 0u;
        g_writer.submitted_generation = 0u;
        g_writer.submitted_at_ms = 0u;
        g_writer.generation++;
    }
    unlock(&g_writer.lock);
    if (logged_kind) writer_lease_wake();
    if (logged_kind == WRITER_DESIRED_ACTIVE) {
        if (logged_removed & WRITER_FRAME_506) {
            writer_log_frame("TX_REMOVE", logged_source, logged_generation,
                             QCAN_ROUTE_FRAME_ID, logged_route_clear);
        }
        if (logged_removed & WRITER_FRAME_115) {
            (void)genesis_115_encode_no_info(logged_payload);
            writer_log_frame("TX_REMOVE", logged_source, logged_generation,
                             QCAN_FRAME_ID, logged_payload);
        }
        if (logged_frames & WRITER_FRAME_115)
            writer_log_frame("TX_ACTIVE", logged_source, logged_generation,
                             QCAN_FRAME_ID, logged_payload);
        if (logged_frames & WRITER_FRAME_506)
            writer_log_frame("TX_ACTIVE", logged_source, logged_generation,
                             QCAN_ROUTE_FRAME_ID, logged_route_payload);
    } else if (logged_kind == WRITER_DESIRED_CLEAR) {
        snprintf(line, sizeof(line),
            "PGI:writer action=TX_CLEAR src=%s gen=%u frames=%02X reason=%s",
            logged_source == WRITER_SOURCE_CP ? "CP" : "AA",
            logged_generation, logged_frames, clear_name(logged_reason));
        native_log(line);
    }
}

static void writer_release_inflight(void) {
    blocking_lock(&g_writer.lock);
    if (g_writer.inflight) g_writer.inflight--;
    unlock(&g_writer.lock);
    writer_wake();
}

static void writer_wait_inflight(void) {
    for (;;) {
        u32 inflight;
        blocking_lock(&g_writer.lock);
        inflight = g_writer.inflight;
        unlock(&g_writer.lock);
        if (!inflight) return;
        pause_ms(1u);
    }
}

static int writer_emit_desired(u32 generation) {
    struct sqcan_frame frame;
    void *notifier;
    void *qcan;
    u8 kind;
    int queued = provenance_has_queued();
    int failed_copy_queued = g_qcan_failure_events != 0u;
    int retry_clear;
    u32 now = mono_ms();
    blocking_lock(&g_writer.lock);
    kind = g_writer.desired;
    retry_clear = kind == WRITER_DESIRED_CLEAR &&
        generation == g_writer.submitted_generation &&
        elapsed(now, g_writer.submitted_at_ms, WRITER_CLEAR_RETRY_MS);
    if (!g_writer.transport_ready || !g_writer.notifier || !g_writer.qcan ||
            g_writer.inflight || g_writer.notifier_closing ||
            generation != g_writer.generation ||
            (generation == g_writer.submitted_generation && !retry_clear) ||
            (kind != WRITER_DESIRED_ACTIVE &&
             kind != WRITER_DESIRED_CLEAR)) {
        unlock(&g_writer.lock);
        return 0;
    }
    if (kind == WRITER_DESIRED_ACTIVE) {
        if (queued || !writer_known_frames(g_writer.desired_frames) ||
                !g_writer.owner_source ||
                !writer_country_allowed(g_writer.country_variant) ||
                !elapsed(now, g_writer.owner_since_ms, WRITER_GRACE_MS) ||
                (g_writer.published &&
                 !elapsed(now, g_writer.last_tx_ms, WRITER_RATE_MS))) {
            unlock(&g_writer.lock);
            return 0;
        }
    } else {
        if (!g_writer.published ||
                !writer_known_frames(g_writer.published_frames)) {
            g_writer.desired = WRITER_DESIRED_IDLE;
            g_writer.desired_source = 0u;
            g_writer.desired_frames = 0u;
            g_writer.generation++;
            unlock(&g_writer.lock);
            return 0;
        }
        if (queued || failed_copy_queued) {
            unlock(&g_writer.lock);
            return 0;
        }
    }
    notifier = g_writer.notifier;
    qcan = g_writer.qcan;
    memset(&frame, 0, sizeof(frame));
    frame.dlc = QCAN_FRAME_DLC;
    if (kind == WRITER_DESIRED_ACTIVE) {
        if (g_writer.desired_frames & WRITER_FRAME_115) {
            frame.id = QCAN_FRAME_ID;
            memcpy(frame.data, g_writer.desired_payload, 8u);
        } else {
            frame.id = QCAN_ROUTE_FRAME_ID;
            memcpy(frame.data, g_writer.desired_route_payload, 8u);
        }
    } else if (g_writer.published_frames & WRITER_FRAME_115) {
        frame.id = QCAN_FRAME_ID;
        (void)genesis_115_encode_no_info(frame.data);
    } else {
        frame.id = QCAN_ROUTE_FRAME_ID;
        writer_506_clear_locked(frame.data);
    }
    g_writer.submitted_generation = generation;
    g_writer.submitted_at_ms = now;
    g_writer.inflight++;
    unlock(&g_writer.lock);
    if (provenance_add(&frame, qcan, generation, kind)) {
        writer_release_inflight();
        return -1;
    }
    blocking_lock(&g_writer.lock);
    g_writer.emissions_started = 1u;
    unlock(&g_writer.lock);
    g_app_qcan_signal(notifier, &frame);
    (void)provenance_remove(&frame);
    writer_release_inflight();
    return 1;
}

static int writer_emit_direct_clear(u32 generation) {
    struct sqcan_frame frame;
    char line[192];
    void *qcan;
    u8 source, frames;
    u8 reason;
    if (!g_app_original_qcan_raw) return -1;
    memset(&frame, 0, sizeof(frame));
    frame.dlc = QCAN_FRAME_DLC;
    blocking_lock(&g_writer.lock);
    if (!g_writer.qcan || generation != g_writer.generation ||
            g_writer.desired != WRITER_DESIRED_CLEAR ||
            !g_writer.published) {
        unlock(&g_writer.lock);
        return 0;
    }
    qcan = g_writer.qcan;
    source = g_writer.published_source;
    frames = g_writer.published_frames;
    reason = g_writer.clear_reason;
    if (!writer_known_frames(frames)) {
        unlock(&g_writer.lock);
        return 0;
    }
    if (frames & WRITER_FRAME_115) {
        frame.id = QCAN_FRAME_ID;
        (void)genesis_115_encode_no_info(frame.data);
    } else {
        frame.id = QCAN_ROUTE_FRAME_ID;
        writer_506_clear_locked(frame.data);
    }
    g_writer.submitted_generation = generation;
    g_writer.submitted_at_ms = mono_ms();
    g_writer.emissions_started = 1u;
    writer_raw_clear_set(qcan, frames, &frame);
    g_writer.published = 0u;
    g_writer.published_source = 0u;
    g_writer.published_frames = 0u;
    memset(g_writer.published_payload, 0, sizeof(g_writer.published_payload));
    memset(g_writer.published_route_payload, 0,
           sizeof(g_writer.published_route_payload));
    g_writer.desired = WRITER_DESIRED_IDLE;
    g_writer.desired_source = 0u;
    g_writer.desired_frames = 0u;
    g_writer.cp_gap_hold = 0u;
    g_writer.cp_gap_since_ms = 0u;
    g_writer.gap_clear_cancellable = 0u;
    g_writer.submitted_generation = 0u;
    g_writer.submitted_at_ms = 0u;
    g_writer.generation++;
    unlock(&g_writer.lock);
    writer_lease_wake();
    snprintf(line, sizeof(line),
        "PGI:writer action=TX_CLEAR src=%s gen=%u frames=%02X reason=%s",
        source == WRITER_SOURCE_CP ? "CP" : "AA",
        generation, frames, clear_name(reason));
    native_log(line);
    return 1;
}

static void writer_set_desired_clear_locked(u8 source_filter, u8 reason) {
    int published_matches = g_writer.published &&
        (!source_filter || g_writer.published_source == source_filter);
    int desired_matches = g_writer.desired == WRITER_DESIRED_ACTIVE &&
        (!source_filter || g_writer.desired_source == source_filter);
    if (!published_matches && !desired_matches) return;
    g_writer.generation++;
    g_writer.submitted_generation = 0u;
    g_writer.submitted_at_ms = 0u;
    g_writer.cp_gap_hold = 0u;
    g_writer.cp_gap_since_ms = 0u;
    g_writer.gap_clear_cancellable = 0u;
    if (published_matches) {
        g_writer.desired = WRITER_DESIRED_CLEAR;
        g_writer.desired_source = g_writer.published_source;
        g_writer.desired_frames = 0u;
        g_writer.clear_reason = reason;
    } else {
        g_writer.desired = WRITER_DESIRED_IDLE;
        g_writer.desired_source = 0u;
        g_writer.desired_frames = 0u;
    }
}

static void writer_release_source(u8 source, u8 reason) {
    u32 generation;
    u8 clear_filter;
    blocking_lock(&g_writer.lock);
    if (!(g_writer.pending_sources & source) &&
            g_writer.owner_source != source &&
            g_writer.desired_source != source &&
            g_writer.published_source != source) {
        unlock(&g_writer.lock);
        return;
    }
    clear_filter = g_writer.owner_source == source ? 0u : source;
    g_writer.pending_sources &= (u8)~source;
    if (!clear_filter) {
        g_writer.owner_source = 0u;
        g_writer.country_variant = 0u;
    }
    g_writer.input_epoch++;
    g_writer.generation++;
    g_writer.submitted_generation = 0u;
    g_writer.submitted_at_ms = 0u;
    writer_set_desired_clear_locked(clear_filter, reason);
    generation = g_writer.generation;
    unlock(&g_writer.lock);
    (void)writer_emit_direct_clear(generation);
    writer_lease_wake();
    writer_wake();
}

static void writer_set_pending_source(u8 source, int enabled) {
    blocking_lock(&g_writer.lock);
    if ((enabled && ((g_writer.pending_sources & source) ||
                     (g_writer.owner_source == source &&
                      !g_writer.pending_sources))) ||
            (!enabled && !(g_writer.pending_sources & source))) {
        unlock(&g_writer.lock);
        return;
    }
    if (enabled) g_writer.pending_sources |= source;
    else g_writer.pending_sources &= (u8)~source;
    g_writer.input_epoch++;
    g_writer.generation++;
    g_writer.submitted_generation = 0u;
    g_writer.submitted_at_ms = 0u;
    if (g_writer.desired == WRITER_DESIRED_ACTIVE) {
        g_writer.desired = WRITER_DESIRED_IDLE;
        g_writer.desired_source = 0u;
        g_writer.desired_frames = 0u;
    }
    unlock(&g_writer.lock);
    writer_wake();
}

static int writer_encode_packet_candidate(
        u8 owner_source, const struct pg_packet *packet,
        struct writer_candidate *candidate) {
    u8 symbol;
    if (!packet || !candidate || packet->event_kind != EVENT_SNAPSHOT ||
            !packet->route_active) return 0;
    if ((packet->presence & PRES_NEXT_TURN_METRES) &&
            !resolve_maneuver(packet, &symbol) &&
            !genesis_115_encode_symbol_frame(
                symbol, packet->next_turn_metres, 0,
                candidate->payload_115))
        candidate->frames |= WRITER_FRAME_115;
    if (owner_source == WRITER_SOURCE_CP &&
            !genesis_506_encode_packet(packet, candidate->payload_506))
        candidate->frames |= WRITER_FRAME_506;
    return writer_known_frames(candidate->frames);
}

static int writer_build_candidate(u8 owner_source, u32 input_epoch,
                                  struct writer_candidate *candidate) {
    struct pg_packet packet;
    int valid = 0;
    int hold = 0;
    int gap_active = 0;
    int gap_expired = 0;
    int blank_eligible = 0;
    u32 now = mono_ms();
    if (!candidate) return -1;
    memset(candidate, 0, sizeof(*candidate));
    if (!try_lock(&g_core_lock)) return -1;
    if (owner_source == WRITER_SOURCE_CP) {
        if (g_cp_tbt == 1 && g_cp_usb_connected == 1 &&
                g_core_cp_gap_pending) {
            candidate->cp_gap_hold = 1u;
            candidate->cp_gap_since_ms = g_core_cp_gap_since_ms;
            gap_active = 1;
            if (elapsed(now, g_core_cp_gap_since_ms, CP_GAP_GRACE_MS))
                gap_expired = 1;
        } else if (g_cp_tbt == 1 && g_cp_usb_connected == 1 &&
                g_core_cp_valid) {
            packet = g_core_cp;
            valid = 1;
        }
    } else if (owner_source == WRITER_SOURCE_AA) {
        /* NextTurn clears distance before the matching Distance callback. */
        if (g_aa_focus_request == 1 && g_aa_aoap_connected == 1 &&
                g_aa_active && g_aa_pair_pending) {
            hold = 1;
        } else if (g_aa_focus_request == 1 && g_aa_aoap_connected == 1 &&
                g_aa_active && g_core_aa_valid) {
            packet = g_core_aa;
            valid = 1;
        }
    }
    unlock(&g_core_lock);
    if (valid)
        valid = writer_encode_packet_candidate(owner_source, &packet,
                                                candidate);
    if (valid && owner_source == WRITER_SOURCE_CP &&
            !(candidate->frames & WRITER_FRAME_115) &&
            (candidate->frames & WRITER_FRAME_506) &&
            !(packet.presence & PRES_NEXT_TURN_METRES))
        blank_eligible = 1;
    blocking_lock(&g_writer.lock);
    if (input_epoch != g_writer.input_epoch ||
            owner_source != g_writer.owner_source ||
            !writer_country_allowed(g_writer.country_variant)) {
        unlock(&g_writer.lock);
        return -1;
    }
    if (gap_active && !gap_expired) {
        valid = 0;
        if (g_writer.published &&
                g_writer.published_source == WRITER_SOURCE_CP &&
                (g_writer.published_frames & WRITER_FRAME_115) &&
                !genesis_115_encode_symbol_frame(
                    GENESIS_EMPTY_NAV_FIELD, 0u, 0,
                    candidate->payload_115)) {
            candidate->frames = WRITER_FRAME_115;
            if (g_writer.published_frames & WRITER_FRAME_506) {
                candidate->frames |= WRITER_FRAME_506;
                memcpy(candidate->payload_506,
                       g_writer.published_route_payload, 8u);
            }
            valid = writer_known_frames(candidate->frames);
        }
        if (!valid) hold = 1;
    } else if (blank_eligible && g_writer.published &&
            g_writer.published_source == WRITER_SOURCE_CP &&
            (g_writer.published_frames & WRITER_FRAME_115) &&
            !genesis_115_encode_symbol_frame(
                GENESIS_EMPTY_NAV_FIELD, 0u, 0,
                candidate->payload_115)) {
        /* Only an existing visual 0x115 may transition to active blank. */
        candidate->frames |= WRITER_FRAME_115;
        valid = writer_known_frames(candidate->frames);
    }
    unlock(&g_writer.lock);
    if (gap_expired) return WRITER_CANDIDATE_GAP_EXPIRED;
    return hold ? WRITER_CANDIDATE_HOLD : valid;
}

static void writer_update_desired(void) {
    u8 owner_source;
    struct writer_candidate candidate_payload;
    u32 input_epoch;
    u32 generation;
    int candidate;
    int cp_gap_lock_held = 0;
    blocking_lock(&g_writer.lock);
    if (g_writer.desired == WRITER_DESIRED_CLEAR) {
        generation = g_writer.generation;
        unlock(&g_writer.lock);
        (void)writer_emit_desired(generation);
        return;
    }
    owner_source = g_writer.owner_source;
    input_epoch = g_writer.input_epoch;
    if (!owner_source || !writer_country_allowed(g_writer.country_variant)) {
        unlock(&g_writer.lock);
        return;
    }
    unlock(&g_writer.lock);
    candidate = writer_build_candidate(owner_source, input_epoch,
                                        &candidate_payload);
    if (candidate < 0) return;
    if (candidate == WRITER_CANDIDATE_HOLD) {
        if (candidate_payload.cp_gap_hold) {
            blocking_lock(&g_writer.lock);
            if (input_epoch == g_writer.input_epoch &&
                    owner_source == WRITER_SOURCE_CP &&
                    owner_source == g_writer.owner_source &&
                    g_writer.published &&
                    g_writer.published_source == WRITER_SOURCE_CP) {
                g_writer.cp_gap_hold = 1u;
                g_writer.cp_gap_since_ms =
                    candidate_payload.cp_gap_since_ms;
            }
            unlock(&g_writer.lock);
        }
        return;
    }
    if (candidate == WRITER_CANDIDATE_GAP_EXPIRED) {
        if (!try_lock(&g_core_lock)) return;
        cp_gap_lock_held = 1;
        if (!g_core_cp_gap_pending ||
                g_core_cp_gap_since_ms !=
                    candidate_payload.cp_gap_since_ms ||
                !elapsed(mono_ms(), candidate_payload.cp_gap_since_ms,
                         CP_GAP_GRACE_MS)) {
            unlock(&g_core_lock);
            return;
        }
        candidate = 0;
    }
    blocking_lock(&g_writer.lock);
    if (input_epoch != g_writer.input_epoch ||
            owner_source != g_writer.owner_source ||
            !writer_country_allowed(g_writer.country_variant) ||
            (cp_gap_lock_held &&
             (!g_core_cp_gap_pending ||
              g_core_cp_gap_since_ms !=
                candidate_payload.cp_gap_since_ms))) {
        unlock(&g_writer.lock);
        if (cp_gap_lock_held) unlock(&g_core_lock);
        return;
    }
    if (cp_gap_lock_held) {
        g_core_cp_gap_pending = 0;
        g_core_cp_gap_since_ms = 0u;
    }
    if (candidate) {
        int same_desired = g_writer.desired == WRITER_DESIRED_ACTIVE &&
            g_writer.desired_source == owner_source &&
            g_writer.desired_frames == candidate_payload.frames &&
            (!(candidate_payload.frames & WRITER_FRAME_115) ||
             !memcmp(g_writer.desired_payload,
                     candidate_payload.payload_115, 8u)) &&
            (!(candidate_payload.frames & WRITER_FRAME_506) ||
             !memcmp(g_writer.desired_route_payload,
                     candidate_payload.payload_506, 8u));
        int same_published = g_writer.published &&
            g_writer.published_source == owner_source &&
            g_writer.published_frames == candidate_payload.frames &&
            (!(candidate_payload.frames & WRITER_FRAME_115) ||
             !memcmp(g_writer.published_payload,
                     candidate_payload.payload_115, 8u)) &&
            (!(candidate_payload.frames & WRITER_FRAME_506) ||
             !memcmp(g_writer.published_route_payload,
                     candidate_payload.payload_506, 8u));
        if (!same_desired) {
            g_writer.generation++;
            g_writer.desired = WRITER_DESIRED_ACTIVE;
            g_writer.desired_source = owner_source;
            g_writer.desired_frames = candidate_payload.frames;
            memcpy(g_writer.desired_payload, candidate_payload.payload_115,
                   8u);
            memcpy(g_writer.desired_route_payload,
                   candidate_payload.payload_506, 8u);
            g_writer.submitted_generation = 0u;
            g_writer.submitted_at_ms = 0u;
        }
        if (candidate_payload.cp_gap_hold &&
                owner_source == WRITER_SOURCE_CP) {
            g_writer.cp_gap_hold = 1u;
            g_writer.cp_gap_since_ms = candidate_payload.cp_gap_since_ms;
            g_writer.gap_clear_cancellable = 0u;
        } else {
            g_writer.cp_gap_hold = 0u;
            g_writer.cp_gap_since_ms = 0u;
            g_writer.gap_clear_cancellable = 0u;
        }
        if (same_published)
            g_writer.submitted_generation = g_writer.generation;
    } else if (g_writer.published) {
        writer_set_desired_clear_locked(0u, CLEAR_ROUTE_INACTIVE);
        if (candidate_payload.cp_gap_hold &&
                g_writer.desired == WRITER_DESIRED_CLEAR &&
                g_writer.desired_source == WRITER_SOURCE_CP)
            g_writer.gap_clear_cancellable = 1u;
    } else if (g_writer.desired == WRITER_DESIRED_ACTIVE) {
        g_writer.generation++;
        g_writer.desired = WRITER_DESIRED_IDLE;
        g_writer.desired_source = 0u;
        g_writer.desired_frames = 0u;
        g_writer.submitted_generation = 0u;
        g_writer.submitted_at_ms = 0u;
    }
    generation = g_writer.generation;
    unlock(&g_writer.lock);
    if (cp_gap_lock_held) unlock(&g_core_lock);
    (void)writer_emit_desired(generation);
}

static int writer_timeout(u32 now) {
    u32 wait = WRITER_FAILSAFE_MS;
    blocking_lock(&g_writer.lock);
    if (g_writer.stop) wait = 0u;
    else if (g_writer.desired == WRITER_DESIRED_CLEAR) {
        if (g_writer.submitted_generation != g_writer.generation) wait = 0u;
        else {
            u32 age = now - g_writer.submitted_at_ms;
            wait = age < WRITER_CLEAR_RETRY_MS ?
                WRITER_CLEAR_RETRY_MS - age : 0u;
        }
    }
    else if (g_writer.desired == WRITER_DESIRED_ACTIVE &&
             g_writer.submitted_generation != g_writer.generation) {
        u32 age = now - g_writer.owner_since_ms;
        if (age < WRITER_GRACE_MS)
            wait = WRITER_GRACE_MS - age;
        else if (g_writer.published) {
            age = now - g_writer.last_tx_ms;
            wait = age < WRITER_RATE_MS ? WRITER_RATE_MS - age : 0u;
        } else wait = 0u;
    }
    else if (g_writer.cp_gap_hold && g_writer.published &&
             g_writer.published_source == WRITER_SOURCE_CP) {
        u32 age = now - g_writer.cp_gap_since_ms;
        wait = age < CP_GAP_GRACE_MS ? CP_GAP_GRACE_MS - age : 0u;
    }
    unlock(&g_writer.lock);
    if (!wait && (provenance_has_queued() || g_qcan_failure_events != 0u))
        wait = WRITER_FAILSAFE_MS;
    return (int)wait;
}

static void writer_drain_wake(void) {
    u8 bytes[32];
    if (g_writer_wake[0] < 0) return;
    while (recv(g_writer_wake[0], bytes, sizeof(bytes), MSG_DONTWAIT) > 0) { }
}

static void *writer_worker(void *unused) {
    (void)unused;
    native_log("PGI:writer state=READY transport=stock-qcan scope=EU");
    for (;;) {
        struct pollfd_native descriptor;
        int timeout;
        blocking_lock(&g_writer.lock);
        if (g_writer.stop) {
            g_writer.stopped = 1u;
            unlock(&g_writer.lock);
            return 0;
        }
        unlock(&g_writer.lock);
        writer_update_desired();
        timeout = writer_timeout(mono_ms());
        descriptor.fd = g_writer_wake[0];
        descriptor.events = POLLIN;
        descriptor.revents = 0;
        if (descriptor.fd >= 0)
            (void)poll(&descriptor, 1u, timeout);
        else pause_ms((u32)timeout);
        writer_drain_wake();
    }
}

static void writer_owner_release(u32 command) {
    char line[160];
    u8 previous_source;
    u32 generation;
    int clear_projected = command != ROUTE_OWNER_NATIVE;
    g_aa_clear_pending = 1;
    g_aa_pair_pending = 0;
    g_core_aa_valid = 0;
    __sync_synchronize();
    blocking_lock(&g_writer.lock);
    previous_source = g_writer.owner_source;
    g_writer.pending_sources = 0u;
    g_writer.owner_source = 0u;
    g_writer.country_variant = 0u;
    g_writer.input_epoch++;
    g_writer.generation++;
    g_writer.submitted_generation = 0u;
    g_writer.submitted_at_ms = 0u;
    g_writer.cp_gap_hold = 0u;
    g_writer.cp_gap_since_ms = 0u;
    g_writer.gap_clear_cancellable = 0u;
    if (clear_projected && g_writer.published) {
        g_writer.desired = WRITER_DESIRED_CLEAR;
        g_writer.desired_source = g_writer.published_source;
        g_writer.desired_frames = 0u;
        g_writer.clear_reason = 0u;
    } else {
        g_writer.desired = WRITER_DESIRED_IDLE;
        g_writer.desired_source = 0u;
        g_writer.desired_frames = 0u;
        if (!clear_projected) {
            g_writer.published = 0u;
            g_writer.published_source = 0u;
            g_writer.published_frames = 0u;
            memset(g_writer.published_payload, 0,
                   sizeof(g_writer.published_payload));
            memset(g_writer.published_route_payload, 0,
                   sizeof(g_writer.published_route_payload));
        }
    }
    generation = g_writer.generation;
    unlock(&g_writer.lock);
    core_reset_cp_gap_state();
    if (clear_projected) (void)writer_emit_direct_clear(generation);
    if (previous_source) {
        snprintf(line, sizeof(line),
            "PGI:writer action=OWNER_RELEASE src=%s command=%u gen=%u",
            previous_source == WRITER_SOURCE_CP ? "CP" : "AA",
            command, generation);
        native_log(line);
    }
    writer_lease_wake();
    writer_wake();
}

static void writer_owner_acquire(void *engine) {
    char line[176];
    u32 country = *(const u32 *)((const u8 *)engine + 0x24u);
    u32 generation;
    u8 pending;
    int clear_projected = 0;
    blocking_lock(&g_writer.lock);
    g_writer.cp_gap_hold = 0u;
    g_writer.cp_gap_since_ms = 0u;
    g_writer.gap_clear_cancellable = 0u;
    pending = g_writer.pending_sources;
    if (!pending && g_writer.owner_source) {
        g_writer.country_variant = country <= 0xffu ? (u8)country : 0u;
        unlock(&g_writer.lock);
        return;
    }
    if (pending != WRITER_SOURCE_AA && pending != WRITER_SOURCE_CP) {
        g_writer.pending_sources = 0u;
        g_writer.owner_source = 0u;
        g_writer.country_variant = 0u;
        g_writer.input_epoch++;
        g_writer.generation++;
        g_writer.submitted_generation = 0u;
        g_writer.submitted_at_ms = 0u;
        if (g_writer.published) {
            g_writer.desired = WRITER_DESIRED_CLEAR;
            g_writer.desired_source = g_writer.published_source;
            g_writer.desired_frames = 0u;
            g_writer.clear_reason = CLEAR_SESSION_REPLACED;
            clear_projected = 1;
        } else {
            g_writer.desired = WRITER_DESIRED_IDLE;
            g_writer.desired_source = 0u;
            g_writer.desired_frames = 0u;
        }
        generation = g_writer.generation;
        unlock(&g_writer.lock);
        if (clear_projected) (void)writer_emit_direct_clear(generation);
        snprintf(line, sizeof(line),
            "PGI:writer action=OWNER_REJECT reason=AMBIGUOUS gen=%u",
            generation);
        native_log(line);
        writer_wake();
        return;
    }
    if (pending == g_writer.owner_source) {
        g_writer.pending_sources = 0u;
        g_writer.country_variant = country <= 0xffu ? (u8)country : 0u;
        if (g_writer.published_source &&
                g_writer.published_source != g_writer.owner_source) {
            g_writer.input_epoch++;
            g_writer.generation++;
            g_writer.submitted_generation = 0u;
            g_writer.submitted_at_ms = 0u;
            g_writer.desired = WRITER_DESIRED_CLEAR;
            g_writer.desired_source = g_writer.published_source;
            g_writer.desired_frames = 0u;
            g_writer.clear_reason = CLEAR_SESSION_REPLACED;
            clear_projected = 1;
        }
        generation = g_writer.generation;
        unlock(&g_writer.lock);
        if (clear_projected) {
            (void)writer_emit_direct_clear(generation);
            writer_wake();
        }
        return;
    }
    g_writer.owner_source = pending;
    g_writer.pending_sources = 0u;
    g_writer.country_variant = country <= 0xffu ? (u8)country : 0u;
    g_writer.owner_since_ms = mono_ms();
    g_writer.input_epoch++;
    g_writer.generation++;
    if (g_writer.published) {
        g_writer.desired = WRITER_DESIRED_CLEAR;
        g_writer.desired_source = g_writer.published_source;
        g_writer.desired_frames = 0u;
        g_writer.clear_reason = CLEAR_SESSION_REPLACED;
        clear_projected = 1;
    } else {
        g_writer.desired = WRITER_DESIRED_IDLE;
        g_writer.desired_source = 0u;
        g_writer.desired_frames = 0u;
    }
    g_writer.submitted_generation = 0u;
    g_writer.submitted_at_ms = 0u;
    pending = g_writer.owner_source;
    country = g_writer.country_variant;
    generation = g_writer.generation;
    unlock(&g_writer.lock);
    if (clear_projected) (void)writer_emit_direct_clear(generation);
    snprintf(line, sizeof(line),
        "PGI:writer action=OWNER_ACQUIRE src=%s country=%u gen=%u",
        pending == WRITER_SOURCE_CP ? "CP" : "AA", country, generation);
    native_log(line);
    writer_wake();
}

static void hook_app_owner_event(void *self, void *event) {
    u32 event_id = event ? g_app_event_id(event) : 0u;
    u32 command = event ? g_app_event_command(event) : 0xffffffffu;
    if (event_id == ROUTE_OWNER_EVENT &&
            command != ROUTE_OWNER_PROJECTED)
        writer_owner_release(command);
    g_app_original_owner_event(self, event);
    if (event_id == ROUTE_OWNER_EVENT &&
            command == ROUTE_OWNER_PROJECTED)
        writer_owner_acquire(self);
}

static void invalidate_aa_cache(void) {
    g_aa_clear_pending = 1;
    g_aa_pair_pending = 0;
    __sync_synchronize();
    g_core_aa_valid = 0;
    __sync_synchronize();
}

static void hook_app_aa_focus(void *self, int enabled) {
    int previous = g_aa_focus_request;
    g_aa_focus_request = enabled ? 1 : 0;
    if (enabled) {
        if (previous != 1) invalidate_aa_cache();
        writer_set_pending_source(WRITER_SOURCE_AA, 1);
    }
    else {
        invalidate_aa_cache();
        writer_release_source(WRITER_SOURCE_AA, CLEAR_ROUTE_INACTIVE);
    }
    g_app_original_aa_focus(self, enabled);
}

static void hook_app_aoap_connection(void *self, int connected) {
    int previous = g_aa_aoap_connected;
    g_aa_aoap_connected = connected ? 1 : 0;
    if (!connected) {
        invalidate_aa_cache();
        writer_release_source(WRITER_SOURCE_AA, CLEAR_DISCONNECTED);
    } else {
        if (previous != 1) invalidate_aa_cache();
        writer_input_mutated();
    }
    g_app_original_aoap_connection(self, connected);
}

static void hook_app_notifier_ctor(void *self, void *ctrl) {
    void *qcan;
    g_app_original_notifier_ctor(self, ctrl);
    qcan = *(void **)((u8 *)self + 0x10u);
    g_aa_clear_pending = 1;
    g_aa_pair_pending = 0;
    g_core_aa_valid = 0;
    __sync_synchronize();
    blocking_lock(&g_writer.lock);
    g_writer.input_epoch++;
    g_writer.generation++;
    g_writer.notifier = self;
    g_writer.qcan = qcan;
    g_writer.notifier_closing = 1u;
    g_writer.owner_since_ms = 0u;
    g_writer.country_variant = 0u;
    g_writer.owner_source = 0u;
    g_writer.pending_sources = 0u;
    g_writer.desired = WRITER_DESIRED_IDLE;
    g_writer.desired_source = 0u;
    g_writer.desired_frames = 0u;
    g_writer.published = 0u;
    g_writer.published_source = 0u;
    g_writer.published_frames = 0u;
    memset(g_writer.desired_payload, 0, sizeof(g_writer.desired_payload));
    memset(g_writer.desired_route_payload, 0,
           sizeof(g_writer.desired_route_payload));
    memset(g_writer.published_payload, 0,
           sizeof(g_writer.published_payload));
    memset(g_writer.published_route_payload, 0,
           sizeof(g_writer.published_route_payload));
    g_writer.submitted_generation = 0u;
    g_writer.submitted_at_ms = 0u;
    g_writer.cp_gap_hold = 0u;
    g_writer.cp_gap_since_ms = 0u;
    g_writer.gap_clear_cancellable = 0u;
    unlock(&g_writer.lock);
    core_reset_cp_gap_state();
    blocking_lock(&g_writer.lock);
    if (g_writer.notifier == self) g_writer.notifier_closing = 0u;
    unlock(&g_writer.lock);
    writer_lease_wake();
    writer_wake();
}

static void writer_reset_session_locked(void) {
    g_aa_pair_pending = 0;
    g_writer.owner_since_ms = 0u;
    g_writer.last_tx_ms = 0u;
    g_writer.submitted_generation = 0u;
    g_writer.submitted_at_ms = 0u;
    g_writer.cp_gap_since_ms = 0u;
    g_writer.country_variant = 0u;
    g_writer.owner_source = 0u;
    g_writer.pending_sources = 0u;
    g_writer.desired = WRITER_DESIRED_IDLE;
    g_writer.desired_source = 0u;
    g_writer.desired_frames = 0u;
    g_writer.clear_reason = 0u;
    g_writer.cp_gap_hold = 0u;
    g_writer.gap_clear_cancellable = 0u;
    g_writer.published = 0u;
    g_writer.published_source = 0u;
    g_writer.published_frames = 0u;
    memset(g_writer.desired_payload, 0, sizeof(g_writer.desired_payload));
    memset(g_writer.desired_route_payload, 0,
           sizeof(g_writer.desired_route_payload));
    memset(g_writer.published_payload, 0, sizeof(g_writer.published_payload));
    memset(g_writer.published_route_payload, 0,
           sizeof(g_writer.published_route_payload));
}

static void hook_app_notifier_dtor(void *self) {
    u32 generation;
    int ours;
    blocking_lock(&g_writer.lock);
    ours = g_writer.notifier == self;
    if (ours) {
        g_aa_clear_pending = 1;
        g_aa_pair_pending = 0;
        g_core_aa_valid = 0;
        __sync_synchronize();
        g_writer.notifier_closing = 1u;
        g_writer.pending_sources = 0u;
        g_writer.owner_source = 0u;
        g_writer.country_variant = 0u;
        g_writer.input_epoch++;
        g_writer.generation++;
        g_writer.submitted_generation = 0u;
        g_writer.submitted_at_ms = 0u;
        g_writer.cp_gap_hold = 0u;
        g_writer.cp_gap_since_ms = 0u;
        g_writer.gap_clear_cancellable = 0u;
        if (g_writer.published) {
            g_writer.desired = WRITER_DESIRED_CLEAR;
            g_writer.desired_source = g_writer.published_source;
            g_writer.desired_frames = 0u;
            g_writer.clear_reason = CLEAR_DISCONNECTED;
        } else {
            g_writer.desired = WRITER_DESIRED_IDLE;
            g_writer.desired_source = 0u;
            g_writer.desired_frames = 0u;
        }
    }
    generation = g_writer.generation;
    unlock(&g_writer.lock);
    if (ours) {
        core_reset_cp_gap_state();
        (void)writer_emit_direct_clear(generation);
        writer_wait_inflight();
        blocking_lock(&g_writer.lock);
        g_writer.notifier = 0;
        g_writer.qcan = 0;
        g_writer.notifier_closing = 0u;
        writer_reset_session_locked();
        unlock(&g_writer.lock);
        writer_lease_wake();
    }
    g_app_original_notifier_dtor(self);
}

static void note_ingress_drop(u32 epoch, u32 kind) {
    u32 current = g_ingress_drop_epoch;
    (void)kind;
    while ((s32)(epoch - current) > 0) {
        u32 observed = __sync_val_compare_and_swap(
            &g_ingress_drop_epoch, current, epoch);
        if (observed == current) break;
        current = observed;
    }
    __sync_synchronize();
    __sync_add_and_fetch(&g_ingress_dropped, 1u);
}

static int queue_item(u32 kind, u32 epoch, u16 message_id,
                      const u8 *data, u32 length) {
    u32 order, index;
    struct ingress_item *slot;
    if (length > MAX_FRAME || (length && !data)) return -1;
    if (!try_lock(&g_ingress_producer_lock)) {
        note_ingress_drop(epoch, kind);
        return -2;
    }
    order = __sync_add_and_fetch(&g_ingress_order, 1u);
    index = (order - 1u) % FRAME_SLOTS;
    slot = &g_ingress[index];
    if (!__sync_bool_compare_and_swap(&slot->ready, 0, -1)) {
        note_ingress_drop(epoch, kind);
        unlock(&g_ingress_producer_lock);
        return -3;
    }
    slot->order = order;
    slot->epoch = epoch;
    slot->kind = kind;
    slot->message_id = message_id;
    slot->length = length;
    if (length) memcpy(slot->data, data, length);
    __sync_synchronize();
    slot->ready = 1;
    unlock(&g_ingress_producer_lock);
    return 0;
}

static int take_item(struct ingress_item *output) {
    u32 index, chosen = FRAME_SLOTS, chosen_order = 0;
    if (!output) return 0;
    for (index = 0; index < FRAME_SLOTS; ++index) {
        u32 order;
        if (g_ingress[index].ready != 1) continue;
        __sync_synchronize();
        order = g_ingress[index].order;
        if (chosen == FRAME_SLOTS || (s32)(order - chosen_order) < 0) {
            chosen = index; chosen_order = order;
        }
    }
    if (chosen == FRAME_SLOTS ||
            !__sync_bool_compare_and_swap(&g_ingress[chosen].ready, 1, -1))
        return 0;
    output->order = g_ingress[chosen].order;
    output->epoch = g_ingress[chosen].epoch;
    output->kind = g_ingress[chosen].kind;
    output->message_id = g_ingress[chosen].message_id;
    output->length = g_ingress[chosen].length;
    if (output->length) memcpy(output->data, g_ingress[chosen].data,
                               output->length);
    __sync_synchronize();
    g_ingress[chosen].ready = 0;
    return 1;
}

static int route_message(u16 message_id) {
    return message_id == MSG_ROUTE_UPDATE ||
           message_id == MSG_MANEUVER_UPDATE ||
           message_id == MSG_LANE_UPDATE;
}

static int session_matches(void *self, void *link, unsigned int session) {
    return !g_session.failed && g_session.advertised && self == g_session.self &&
           link == g_session.link && session == g_session.session;
}

static int session_can_cache_early_auth(const struct session_state *state,
                                        void *self, void *link,
                                        unsigned int session) {
    int same_tuple = self == state->self && link == state->link &&
                     session == state->session;
    if ((state->failed || state->rejected) && same_tuple) return 0;
    return !state->advertised || !same_tuple;
}

static int message_requires_fail_inactive(u16 message_id) {
    return route_message(message_id) || message_id == MSG_IDENT_ACCEPTED ||
           message_id == MSG_IDENT_REJECTED || message_id == MSG_AUTH_COMPLETE;
}

static int corrupt_route_candidate(const u8 *data, u32 length) {
    return data && length >= 6u && route_message(be16(data + 4u));
}

static int session_note_message(struct session_state *state, u16 message_id) {
    if (message_id == MSG_IDENT_ACCEPTED) {
        if (!state->rejected && !state->failed) state->accepted = 1;
    } else if (message_id == MSG_IDENT_REJECTED) {
        state->accepted = 0;
        state->rejected = 1;
        state->start_sent = 0;
        state->failed = 1;
        return -1;
    }
    else if (message_id == MSG_AUTH_COMPLETE) state->auth = 1;
    if (!state->start_sent && state->accepted && state->auth &&
            !state->rejected) {
        state->start_sent = 1;
        return 1;
    }
    return 0;
}

static int identify_candidate(const struct byte_array *array) {
    return array && array->ptr && array->capacity >= 6u && array->used >= 6u &&
           array->ptr[0] == 0x40u && array->ptr[1] == 0x40u &&
           be16(array->ptr + 4u) == MSG_IDENT_INFO;
}

static int identify_bounds_valid(const struct byte_array *array) {
    return array && array->used <= array->capacity && array->used <= MAX_FRAME;
}

static void session_fail_inactive(u16 message_id) {
    u32 epoch = g_session.epoch;
    g_session.failed = 1;
    g_session.early_auth = 0;
    __sync_synchronize();
    if (try_lock(&g_session.lock)) {
        epoch = ++g_session.epoch;
        if (!epoch) epoch = ++g_session.epoch;
        g_session.advertised = 0;
        g_session.accepted = 0;
        g_session.rejected = 1;
        g_session.auth = 0;
        g_session.start_sent = 0;
        g_session.early_auth = 0;
        unlock(&g_session.lock);
    }
    queue_item(ITEM_NEGOTIATION_FAULT, epoch, message_id, 0, 0);
}

static void hook_iap_send(void *self, void *link, unsigned int session,
                          struct byte_array *array) {
    u16 message_id = 0;
    struct ident_scan scan;
    int scan_result;
    u8 *changed = 0;
    u32 changed_length = 0;
    struct byte_array replacement;
    u32 epoch;
    if (!g_iap_ready || !identify_candidate(array)) {
        g_iap_original_send(self, link, session, array); return;
    }
    g_session.failed = 1;
    __sync_synchronize();
    if (!identify_bounds_valid(array) ||
            frame_id(array->ptr, array->used, &message_id) ||
            message_id != MSG_IDENT_INFO) {
        session_fail_inactive(MSG_IDENT_INFO);
        g_iap_original_send(self, link, session, array); return;
    }
    scan_result = scan_ident(array->ptr, array->used, &scan);
    if (scan_result < 0 || scan_result > 0 ||
            mutate_ident(array->ptr, array->used, &changed, &changed_length)) {
        session_fail_inactive(message_id);
        g_iap_original_send(self, link, session, array);
        return;
    }
    if (!try_lock(&g_session.lock)) {
        free(changed);
        session_fail_inactive(message_id);
        g_iap_original_send(self, link, session, array);
        return;
    }
    epoch = ++g_session.epoch;
    if (!epoch) epoch = ++g_session.epoch;
    g_session.self = self;
    g_session.link = link;
    g_session.session = session;
    g_session.advertised = 1;
    g_session.accepted = 0;
    g_session.rejected = 0;
    g_session.start_sent = 0;
    g_session.auth = g_session.early_auth &&
        g_session.early_self == self && g_session.early_link == link &&
        g_session.early_session == session;
    g_session.early_auth = 0;
    g_session.failed = 0;
    unlock(&g_session.lock);
    queue_item(ITEM_SESSION_BEGIN, epoch, message_id, 0, 0);
    replacement.ptr = changed;
    replacement.capacity = changed_length;
    replacement.used = changed_length;
    g_iap_original_send(self, link, session, &replacement);
    free(changed);
}

static void hook_iap_dispatch(void *self, void *link, const u8 *data,
                              u32 length, unsigned int session) {
    u16 message_id = 0;
    int valid = !frame_id(data, length, &message_id);
    int corrupt_route = !valid && corrupt_route_candidate(data, length);
    int emit_start = 0, capture = 0, negotiation_fault = 0;
    u32 epoch = 0;
    /* OEM handling is always complete before the adapter observes the frame. */
    g_iap_original_dispatch(self, link, data, length, session);
    if (!g_iap_ready) return;
    if (!try_lock(&g_session.lock)) {
        if (corrupt_route ||
                (valid && message_requires_fail_inactive(message_id)))
            session_fail_inactive(valid ? message_id : be16(data + 4u));
        return;
    }
    if (!valid) {
        if (corrupt_route && session_matches(self, link, session)) {
            epoch = g_session.epoch;
            negotiation_fault = 1;
        }
        unlock(&g_session.lock);
        if (negotiation_fault)
            queue_item(ITEM_NEGOTIATION_FAULT, epoch,
                       be16(data + 4u), 0, 0);
        return;
    }
    if (session_matches(self, link, session)) {
        int transition;
        epoch = g_session.epoch;
        transition = session_note_message(&g_session, message_id);
        emit_start = transition > 0;
        negotiation_fault = transition < 0;
        if (g_session.start_sent && route_message(message_id)) capture = 1;
    } else if (message_id == MSG_AUTH_COMPLETE &&
               session_can_cache_early_auth(&g_session, self, link, session)) {
        g_session.early_self = self;
        g_session.early_link = link;
        g_session.early_session = session;
        g_session.early_auth = 1;
    }
    unlock(&g_session.lock);
    if (negotiation_fault)
        queue_item(ITEM_NEGOTIATION_FAULT, epoch, message_id, 0, 0);
    if (emit_start) g_iap_raw_send(self, link, session,
                                   k_start_frame, sizeof(k_start_frame));
    if (capture && queue_item(ITEM_ROUTE, epoch, message_id, data, length)) {
        /* The worker turns the monotonic drop counter into one bounded fault. */
    }
}

struct tlv_cursor {
    const u8 *data;
    u32 length;
    u32 offset;
    u32 count;
};
struct seen_ids { u16 ids[MAX_TLVS]; u32 count; };

static int next_tlv(struct tlv_cursor *cursor, u16 *field_id,
                    const u8 **value, u32 *value_length) {
    u16 total;
    if (cursor->offset == cursor->length) return 0;
    if (++cursor->count > MAX_TLVS ||
            cursor->length - cursor->offset < 4u) return -1;
    total = be16(cursor->data + cursor->offset);
    if (total < 4u || total > cursor->length - cursor->offset) return -2;
    *field_id = be16(cursor->data + cursor->offset + 2u);
    *value = cursor->data + cursor->offset + 4u;
    *value_length = (u32)total - 4u;
    if (*value_length > MAX_TLV_VALUE) return -3;
    cursor->offset += total;
    return 1;
}

static int remember_id(struct seen_ids *seen, u16 field_id) {
    u32 index;
    for (index = 0; index < seen->count; ++index)
        if (seen->ids[index] == field_id) return -1;
    if (seen->count >= MAX_TLVS) return -2;
    seen->ids[seen->count++] = field_id;
    return 0;
}

static int scalar_u8(const u8 *value, u32 length, u8 *output) {
    if (length != 1u) return -1;
    *output = value[0]; return 0;
}
static int scalar_bool(const u8 *value, u32 length, u8 *output) {
    if (scalar_u8(value, length, output) || *output > 1u) return -1;
    return 0;
}
static int scalar_u16(const u8 *value, u32 length, u16 *output) {
    if (length != 2u) return -1;
    *output = be16(value); return 0;
}
static int scalar_s16(const u8 *value, u32 length, s16 *output) {
    if (length != 2u) return -1;
    *output = bes16(value); return 0;
}
static int scalar_u32(const u8 *value, u32 length, u32 *output) {
    if (length != 4u) return -1;
    *output = be32(value); return 0;
}
static int scalar_u64(const u8 *value, u32 length, u64 *output) {
    if (length != 8u) return -1;
    *output = be64(value); return 0;
}

static int number_list(const u8 *value, u32 length, u32 maximum,
                       int allow_empty) {
    u32 count;
    (void)value;
    if (length & 1u) return -1;
    count = length / 2u;
    return ((allow_empty || count) && count <= maximum) ? (int)count : -1;
}

static int utf8_sequence(const u8 *source, u32 remaining) {
    u8 first;
    if (!remaining) return -1;
    first = source[0];
    if (first < 0x80u) return 1;
    if (first >= 0xc2u && first <= 0xdfu) {
        if (remaining < 2u || (source[1] & 0xc0u) != 0x80u) return -1;
        return 2;
    }
    if (first >= 0xe0u && first <= 0xefu) {
        if (remaining < 3u || (source[1] & 0xc0u) != 0x80u ||
                (source[2] & 0xc0u) != 0x80u) return -1;
        if ((first == 0xe0u && source[1] < 0xa0u) ||
                (first == 0xedu && source[1] >= 0xa0u)) return -1;
        return 3;
    }
    if (first >= 0xf0u && first <= 0xf4u) {
        if (remaining < 4u || (source[1] & 0xc0u) != 0x80u ||
                (source[2] & 0xc0u) != 0x80u ||
                (source[3] & 0xc0u) != 0x80u) return -1;
        if ((first == 0xf0u && source[1] < 0x90u) ||
                (first == 0xf4u && source[1] >= 0x90u)) return -1;
        return 4;
    }
    return -1;
}

static int decode_text(const u8 *value, u32 length, char output[TEXT_BYTES]) {
    u32 offset = 0, used = 0, content_length = length;
    int copying = 1;
    memset(output, 0, TEXT_BYTES);
    while (content_length && value[content_length - 1u] == 0) --content_length;
    while (offset < content_length) {
        int sequence;
        u32 index;
        if (!value[offset]) return -1;
        sequence = utf8_sequence(value + offset, content_length - offset);
        if (sequence < 0) return -2;
        if (copying && used + (u32)sequence < TEXT_BYTES) {
            if (sequence == 1 && (value[offset] < 0x20u || value[offset] == 0x7fu))
                output[used++] = ' ';
            else for (index = 0; index < (u32)sequence; ++index)
                output[used++] = (char)value[offset + index];
        } else if (copying) copying = 0;
        offset += (u32)sequence;
    }
    output[used] = 0;
    return 0;
}

static int validate_component_ids(const u8 *value, u32 length) {
    return number_list(value, length, MAX_COMPONENT_IDS, 0) < 0 ? -1 : 0;
}

static int parse_5201(const u8 *frame, u32 length,
                      struct route_fields *update,
                      int *list_seen, int *list_empty) {
    struct tlv_cursor cursor;
    struct seen_ids seen;
    u16 field_id;
    const u8 *value;
    u32 value_length;
    int result, item_count = 0;
    u8 temporary_u8;
    u16 temporary_u16;
    s16 temporary_s16;
    char temporary_text[TEXT_BYTES];
    memset(update, 0, sizeof(*update));
    memset(&seen, 0, sizeof(seen));
    *list_seen = 0; *list_empty = 0;
    cursor.data = frame; cursor.length = length; cursor.offset = 6u; cursor.count = 0;
    while ((result = next_tlv(&cursor, &field_id, &value, &value_length)) > 0) {
        ++item_count;
        if (field_id <= 0x0016u && remember_id(&seen, field_id)) return -10;
        switch (field_id) {
        case 0x0000:
            if (validate_component_ids(value, value_length)) return -11;
            break;
        case 0x0001:
            if (scalar_u8(value, value_length, &update->route_state)) return -12;
            update->presence |= PRES_ROUTE_STATE; break;
        case 0x0002:
            if (scalar_u8(value, value_length, &update->maneuver_state)) return -13;
            update->presence |= PRES_MANEUVER_STATE; break;
        case 0x0003:
            if (decode_text(value, value_length, update->current_road)) return -14;
            update->presence |= PRES_CURRENT_ROAD; break;
        case 0x0004:
            if (decode_text(value, value_length, update->destination)) return -15;
            update->presence |= PRES_DESTINATION; break;
        case 0x0005:
            if (scalar_u64(value, value_length, &update->eta_unix_seconds)) return -16;
            update->presence |= PRES_ETA; break;
        case 0x0006:
            if (scalar_u64(value, value_length, &update->time_remaining_seconds)) return -17;
            update->presence |= PRES_TIME_REMAINING; break;
        case 0x0007:
            if (scalar_u32(value, value_length,
                           &update->distance_remaining_metres)) return -18;
            update->presence |= PRES_DISTANCE_REMAIN; break;
        case 0x0008: case 0x000b: case 0x0013:
            if (decode_text(value, value_length, temporary_text)) return -19;
            break;
        case 0x0009: case 0x000c: case 0x0016:
            if (scalar_u8(value, value_length, &temporary_u8)) return -20;
            break;
        case 0x000a:
            if (scalar_u32(value, value_length, &update->next_turn_metres)) return -21;
            update->presence |= PRES_NEXT_TURN_METRES; break;
        case 0x000d: {
            int count = number_list(value, value_length, MAX_MANEUVER_ITEMS, 1);
            if (count < 0) return -22;
            *list_seen = 1; *list_empty = count == 0;
            if (count) {
                update->current_index = be16(value);
                update->presence |= PRES_CURRENT_INDEX;
            }
            break;
        }
        case 0x000e:
            if (scalar_u16(value, value_length, &temporary_u16)) return -23;
            break;
        case 0x000f:
            if (scalar_bool(value, value_length, &update->visible)) return -24;
            update->presence |= PRES_VISIBLE; break;
        case 0x0010:
            if (scalar_u16(value, value_length, &update->lane_index)) return -25;
            update->presence |= PRES_LANE_INDEX; break;
        case 0x0011:
            if (scalar_u16(value, value_length, &update->lane_count)) return -26;
            update->presence |= PRES_LANE_COUNT; break;
        case 0x0012:
            if (scalar_bool(value, value_length, &temporary_u8)) return -27;
            break;
        case 0x0014:
            if (scalar_bool(value, value_length, &update->source_supports)) return -28;
            update->presence |= PRES_SOURCE_SUPPORTS; break;
        case 0x0015:
            if (scalar_s16(value, value_length, &temporary_s16)) return -29;
            break;
        default:
            break;
        }
    }
    return result < 0 || !item_count ? -30 : 0;
}

static int parse_5202(const u8 *frame, u32 length,
                      struct maneuver_fields *update) {
    struct tlv_cursor cursor;
    struct seen_ids seen;
    u16 field_id;
    const u8 *value;
    u32 value_length, angle_count = 0;
    int result, item_count = 0;
    u8 temporary_u8;
    u32 temporary_u32;
    char temporary_text[TEXT_BYTES];
    memset(update, 0, sizeof(*update));
    memset(&seen, 0, sizeof(seen));
    cursor.data = frame; cursor.length = length; cursor.offset = 6u; cursor.count = 0;
    while ((result = next_tlv(&cursor, &field_id, &value, &value_length)) > 0) {
        ++item_count;
        if (field_id <= 0x000du && field_id != 0x000au &&
                remember_id(&seen, field_id)) return -10;
        switch (field_id) {
        case 0x0000:
            if (validate_component_ids(value, value_length)) return -11;
            break;
        case 0x0001:
            if (scalar_u16(value, value_length, &update->index)) return -12;
            update->valid = 1; break;
        case 0x0002:
            if (decode_text(value, value_length, update->description)) return -13;
            update->presence |= PRES_DESCRIPTION; break;
        case 0x0003:
            if (scalar_u8(value, value_length, &update->source_type)) return -14;
            update->semantic = maneuver_semantic(update->source_type);
            update->presence |= PRES_SOURCE_TYPE | PRES_SEMANTIC; break;
        case 0x0004:
            if (decode_text(value, value_length, update->after_road)) return -15;
            update->presence |= PRES_AFTER_ROAD; break;
        case 0x0005:
            if (scalar_u32(value, value_length, &temporary_u32)) return -16;
            break;
        case 0x0006:
            if (decode_text(value, value_length, temporary_text)) return -17;
            break;
        case 0x0007:
            if (scalar_u8(value, value_length, &temporary_u8)) return -18;
            break;
        case 0x0008:
            if (scalar_u8(value, value_length, &update->driving_side)) return -19;
            update->presence |= PRES_DRIVING_SIDE; break;
        case 0x0009:
            if (scalar_u8(value, value_length, &update->junction_type)) return -20;
            update->presence |= PRES_JUNCTION_TYPE; break;
        case 0x000a: {
            int count = number_list(value, value_length,
                                    MAX_JUNCTION_ANGLES - angle_count, 0);
            if (count < 0) return -21;
            angle_count += (u32)count; break;
        }
        case 0x000b:
            if (scalar_s16(value, value_length, &update->exit_angle_deg)) return -22;
            update->presence |= PRES_EXIT_ANGLE; break;
        case 0x000c:
            if (scalar_u16(value, value_length,
                           &update->linked_lane_index)) return -23;
            update->presence |= PRES_LINKED_LANE; break;
        case 0x000d:
            if (decode_text(value, value_length, update->exit_info)) return -24;
            update->presence |= PRES_EXIT_INFO; break;
        default:
            break;
        }
    }
    return result < 0 || !item_count || !update->valid ? -30 : 0;
}

static int parse_lane_group(const u8 *data, u32 length, u16 *positions,
                            u32 *position_count, u32 *lane_count) {
    struct tlv_cursor cursor;
    u16 field_id, current_position = 0;
    const u8 *value;
    u32 value_length, angle_count = 0, index;
    int result, have_lane = 0, status_seen = 0, highlight_seen = 0;
    u8 temporary_u8;
    s16 temporary_s16;
    cursor.data = data; cursor.length = length; cursor.offset = 0; cursor.count = 0;
    while ((result = next_tlv(&cursor, &field_id, &value, &value_length)) > 0) {
        if (field_id == 0x0000) {
            if (scalar_u16(value, value_length, &current_position)) return -1;
            for (index = 0; index < *position_count; ++index)
                if (positions[index] == current_position) return -2;
            if (*position_count >= MAX_LANES) return -3;
            positions[(*position_count)++] = current_position;
            ++*lane_count;
            have_lane = 1; status_seen = 0; highlight_seen = 0; angle_count = 0;
        } else if (field_id == 0x0001) {
            if (!have_lane || status_seen++ ||
                    scalar_u8(value, value_length, &temporary_u8)) return -4;
        } else if (field_id == 0x0002) {
            int count;
            if (!have_lane) return -5;
            count = number_list(value, value_length,
                                MAX_LANE_ANGLES - angle_count, 0);
            if (count < 0) return -6;
            angle_count += (u32)count;
        } else if (field_id == 0x0003) {
            if (!have_lane || highlight_seen++ ||
                    scalar_s16(value, value_length, &temporary_s16)) return -7;
        }
    }
    return result < 0 ? -8 : 0;
}

static int parse_5204(const u8 *frame, u32 length,
                      u16 *lane_index, u16 *lane_count,
                      char description[TEXT_BYTES], u32 *presence) {
    struct tlv_cursor cursor;
    struct seen_ids seen;
    u16 field_id, positions[MAX_LANES];
    const u8 *value;
    u32 value_length, position_count = 0, parsed_count = 0;
    int result, item_count = 0;
    memset(&seen, 0, sizeof(seen));
    *presence = 0; *lane_count = 0; description[0] = 0;
    cursor.data = frame; cursor.length = length; cursor.offset = 6u; cursor.count = 0;
    while ((result = next_tlv(&cursor, &field_id, &value, &value_length)) > 0) {
        ++item_count;
        if ((field_id == 0x0000u || field_id == 0x0001u ||
             field_id == 0x0003u) && remember_id(&seen, field_id)) return -10;
        if (field_id == 0x0000) {
            if (validate_component_ids(value, value_length)) return -11;
        } else if (field_id == 0x0001) {
            if (scalar_u16(value, value_length, lane_index)) return -12;
            *presence |= PRES_LANE_INDEX;
        } else if (field_id == 0x0002) {
            if (parse_lane_group(value, value_length, positions,
                                 &position_count, &parsed_count)) return -13;
        } else if (field_id == 0x0003) {
            if (decode_text(value, value_length, description)) return -14;
        }
    }
    if (parsed_count) {
        *lane_count = (u16)parsed_count;
        *presence |= PRES_LANE_COUNT;
    }
    return result < 0 || !item_count ? -20 : 0;
}

static struct maneuver_fields *maneuver_slot(struct decoder_state *state,
                                               u16 index) {
    u32 slot;
    for (slot = 0; slot < MANEUVER_CACHE; ++slot)
        if (state->maneuvers[slot].valid &&
                state->maneuvers[slot].index == index)
            return &state->maneuvers[slot];
    for (slot = 0; slot < MANEUVER_CACHE; ++slot)
        if (!state->maneuvers[slot].valid) {
            state->maneuvers[slot].valid = 1;
            state->maneuvers[slot].index = index;
            return &state->maneuvers[slot];
        }
    slot = (u32)index % MANEUVER_CACHE;
    memset(&state->maneuvers[slot], 0, sizeof(state->maneuvers[slot]));
    state->maneuvers[slot].valid = 1;
    state->maneuvers[slot].index = index;
    return &state->maneuvers[slot];
}

static struct maneuver_fields *find_maneuver(struct decoder_state *state,
                                              u16 index) {
    u32 slot;
    for (slot = 0; slot < MANEUVER_CACHE; ++slot)
        if (state->maneuvers[slot].valid &&
                state->maneuvers[slot].index == index)
            return &state->maneuvers[slot];
    return 0;
}

static void merge_route(struct route_fields *target,
                        const struct route_fields *source) {
    u32 presence = source->presence;
    if (presence & PRES_ROUTE_STATE) target->route_state = source->route_state;
    if (presence & PRES_MANEUVER_STATE) target->maneuver_state = source->maneuver_state;
    if (presence & PRES_CURRENT_ROAD)
        memcpy(target->current_road, source->current_road, TEXT_BYTES);
    if (presence & PRES_DESTINATION)
        memcpy(target->destination, source->destination, TEXT_BYTES);
    if (presence & PRES_ETA) target->eta_unix_seconds = source->eta_unix_seconds;
    if (presence & PRES_TIME_REMAINING)
        target->time_remaining_seconds = source->time_remaining_seconds;
    if (presence & PRES_DISTANCE_REMAIN)
        target->distance_remaining_metres = source->distance_remaining_metres;
    if (presence & PRES_NEXT_TURN_METRES)
        target->next_turn_metres = source->next_turn_metres;
    if (presence & PRES_CURRENT_INDEX) target->current_index = source->current_index;
    if (presence & PRES_VISIBLE) target->visible = source->visible;
    if (presence & PRES_SOURCE_SUPPORTS)
        target->source_supports = source->source_supports;
    if (presence & PRES_LANE_INDEX) target->lane_index = source->lane_index;
    if (presence & PRES_LANE_COUNT) target->lane_count = source->lane_count;
    target->presence |= presence;
}

static int merge_maneuver(struct maneuver_fields *target,
                          const struct maneuver_fields *source) {
    struct maneuver_fields before;
    u32 presence = source->presence;
    memcpy(&before, target, sizeof(before));
    if (presence & PRES_SOURCE_TYPE) target->source_type = source->source_type;
    if (presence & PRES_SEMANTIC) target->semantic = source->semantic;
    if (presence & PRES_DESCRIPTION)
        memcpy(target->description, source->description, TEXT_BYTES);
    if (presence & PRES_AFTER_ROAD)
        memcpy(target->after_road, source->after_road, TEXT_BYTES);
    if (presence & PRES_DRIVING_SIDE) target->driving_side = source->driving_side;
    if (presence & PRES_JUNCTION_TYPE) target->junction_type = source->junction_type;
    if (presence & PRES_EXIT_ANGLE) target->exit_angle_deg = source->exit_angle_deg;
    if (presence & PRES_LINKED_LANE)
        target->linked_lane_index = source->linked_lane_index;
    if (presence & PRES_EXIT_INFO)
        memcpy(target->exit_info, source->exit_info, TEXT_BYTES);
    target->presence |= presence;
    return memcmp(&before, target, sizeof(before)) != 0;
}

static int route_active(const struct route_fields *route) {
    u8 state;
    if (!(route->presence & PRES_ROUTE_STATE)) return 0;
    state = route->route_state;
    if (state == 0u || state == 2u) return 0;
    /* 0x000f can be false while coherent active-route updates continue. */
    if ((route->presence & PRES_SOURCE_SUPPORTS) && !route->source_supports)
        return 0;
    return state <= 6u;
}

static void build_packet(struct decoder_state *state, struct pg_packet *packet,
                         u8 event_kind, u8 clear_reason) {
    struct maneuver_fields *maneuver = 0;
    struct route_fields *route = &state->route;
    memset(packet, 0, sizeof(*packet));
    packet->magic = PACKET_MAGIC;
    packet->version = PACKET_VERSION;
    packet->size = (u16)sizeof(*packet);
    packet->source = SOURCE_CARPLAY;
    packet->event_kind = event_kind;
    packet->clear_reason = clear_reason;
    packet->route_active = (u8)route_active(route);
    packet->session_epoch = state->epoch;
    packet->sequence = ++state->sequence;
    packet->monotonic_ms = mono_ms();
    packet->presence = route->presence;
    packet->maneuver_revision = state->revision;
    packet->route_state = route->route_state;
    packet->maneuver_state = route->maneuver_state;
    packet->visible = route->visible;
    packet->source_supports = route->source_supports;
    packet->next_turn_metres = route->next_turn_metres;
    packet->distance_remaining_metres = route->distance_remaining_metres;
    packet->eta_unix_seconds = route->eta_unix_seconds;
    packet->time_remaining_seconds = route->time_remaining_seconds;
    packet->lane_index = route->lane_index;
    packet->lane_count = route->lane_count;
    memcpy(packet->current_road, route->current_road, TEXT_BYTES);
    memcpy(packet->destination, route->destination, TEXT_BYTES);
    if (state->current_valid) {
        packet->maneuver_index = state->current_index;
        packet->presence |= PRES_CURRENT_INDEX;
        maneuver = find_maneuver(state, state->current_index);
    }
    if (maneuver) {
        packet->presence |= maneuver->presence;
        packet->source_type = maneuver->source_type;
        packet->semantic = maneuver->semantic;
        packet->driving_side = maneuver->driving_side;
        packet->junction_type = maneuver->junction_type;
        packet->exit_angle_deg = maneuver->exit_angle_deg;
        packet->linked_lane_index = maneuver->linked_lane_index;
        memcpy(packet->description, maneuver->description, TEXT_BYTES);
        memcpy(packet->after_road, maneuver->after_road, TEXT_BYTES);
        memcpy(packet->exit_info, maneuver->exit_info, TEXT_BYTES);
    }
}

static void decoder_reset(struct decoder_state *state, u32 epoch,
                          struct pg_packet *output, u8 reason) {
    u32 previous_epoch = state->epoch;
    u32 previous_sequence = state->sequence;
    memset(state, 0, sizeof(*state));
    state->epoch = epoch;
    if (previous_epoch == epoch) state->sequence = previous_sequence;
    state->revision = 1u;
    build_packet(state, output, EVENT_CLEAR, reason);
    state->latest = *output;
    state->latest_valid = 1;
}

static int decode_item(struct decoder_state *state,
                       const struct ingress_item *item,
                       struct pg_packet *output) {
    u16 message_id;
    if (item->kind == ITEM_SESSION_BEGIN) {
        if (state->latest_valid &&
                (s32)(item->epoch - state->epoch) <= 0) return 0;
        decoder_reset(state, item->epoch, output, CLEAR_SESSION_REPLACED);
        return 1;
    }
    if (item->kind == ITEM_NEGOTIATION_FAULT) {
        if (state->latest_valid &&
                (s32)(item->epoch - state->epoch) < 0) return 0;
        decoder_reset(state, item->epoch, output, CLEAR_DECODE_FAULT);
        state->failed = 1;
        output->event_kind = EVENT_FAULT;
        state->latest = *output;
        return 1;
    }
    if (state->failed) return 0;
    if (item->kind != ITEM_ROUTE || item->epoch != state->epoch ||
            frame_id(item->data, item->length, &message_id) ||
            message_id != item->message_id || !route_message(message_id))
        return -1;
    if (message_id == MSG_ROUTE_UPDATE) {
        struct route_fields update;
        int list_seen, list_empty;
        int maneuver_changed = 0;
        int previous_valid = state->current_valid;
        u16 previous_index = state->current_index;
        if (parse_5201(item->data, item->length, &update,
                       &list_seen, &list_empty)) goto malformed;
        if (list_seen) {
            if (list_empty) state->current_valid = 0;
            else {
                state->current_valid = 1;
                state->current_index = update.current_index;
            }
            maneuver_changed = previous_valid && (!state->current_valid ||
                               previous_index != state->current_index);
            if (previous_valid != state->current_valid ||
                    maneuver_changed) {
                ++state->revision;
                state->route.presence &= ~PRES_NEXT_TURN_METRES;
                state->route.next_turn_metres = 0;
            }
        }
        if (maneuver_changed) {
            update.presence &= ~PRES_NEXT_TURN_METRES;
            update.next_turn_metres = 0;
        }
        merge_route(&state->route, &update);
        if ((state->route.presence & PRES_ROUTE_STATE) &&
                (state->route.route_state == 0u || state->route.route_state == 2u)) {
            decoder_reset(state, item->epoch, output, CLEAR_ROUTE_INACTIVE);
        } else build_packet(state, output, EVENT_SNAPSHOT, 0);
    } else if (message_id == MSG_MANEUVER_UPDATE) {
        struct maneuver_fields update, *target;
        int changed;
        if (parse_5202(item->data, item->length, &update)) goto malformed;
        target = maneuver_slot(state, update.index);
        changed = merge_maneuver(target, &update);
        if (changed && state->current_valid && state->current_index == update.index) {
            ++state->revision;
            state->route.presence &= ~PRES_NEXT_TURN_METRES;
            state->route.next_turn_metres = 0;
        }
        build_packet(state, output, EVENT_SNAPSHOT, 0);
    } else {
        u16 lane_index = 0, lane_count = 0;
        u32 presence = 0;
        char description[TEXT_BYTES];
        if (parse_5204(item->data, item->length, &lane_index, &lane_count,
                       description, &presence)) goto malformed;
        if (presence & PRES_LANE_INDEX) {
            state->route.lane_index = lane_index;
            state->route.presence |= PRES_LANE_INDEX;
        }
        if (presence & PRES_LANE_COUNT) {
            state->route.lane_count = lane_count;
            state->route.presence |= PRES_LANE_COUNT;
        }
        build_packet(state, output, EVENT_SNAPSHOT, 0);
    }
    state->latest = *output;
    state->latest_valid = 1;
    return 1;
malformed:
    decoder_reset(state, item->epoch, output, CLEAR_DECODE_FAULT);
    state->failed = 1;
    output->event_kind = EVENT_FAULT;
    state->latest = *output;
    return -2;
}

static int decoder_apply_drop(struct decoder_state *state,
                              u32 *observed_drops,
                              struct pg_packet *packet) {
    u32 dropped = g_ingress_dropped;
    u32 drop_epoch;
    if (*observed_drops == dropped) return 0;
    __sync_synchronize();
    drop_epoch = g_ingress_drop_epoch;
    *observed_drops = dropped;
    if ((s32)(drop_epoch - state->epoch) < 0 ||
            (drop_epoch == state->epoch && state->failed)) return 0;
    decoder_reset(state, drop_epoch, packet, CLEAR_DECODE_FAULT);
    state->failed = 1;
    packet->event_kind = EVENT_FAULT;
    state->latest = *packet;
    return 1;
}

static void socket_address_named(struct sockaddr_un_native *address,
                                 u32 *length, const char *name) {
    usize name_length = cstrlen(name);
    memset(address, 0, sizeof(*address));
    address->family = AF_UNIX;
    address->path[0] = 0;
    memcpy(address->path + 1, name, name_length);
    *length = (u32)(sizeof(address->family) + 1u + name_length);
}

static void socket_address(struct sockaddr_un_native *address, u32 *length) {
    socket_address_named(address, length, k_socket_name);
}

static int connect_core(void) {
    struct sockaddr_un_native address;
    u32 length;
    int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
    if (fd < 0) fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (fd < 0) return -1;
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    socket_address(&address, &length);
    if (connect(fd, &address, length) ||
            !exact_peer(fd, "/app/bin/AppProjection")) {
        close(fd); return -2;
    }
    return fd;
}

static int send_packet(int fd, const struct pg_packet *packet) {
    ssize sent;
    if (fd < 0 || !packet) return -1;
    sent = send(fd, packet, sizeof(*packet), MSG_DONTWAIT | MSG_NOSIGNAL);
    return sent == (ssize)sizeof(*packet) ? 0 : -2;
}

static int safe_reconnect_replay(const struct pg_packet *packet) {
    return packet && (packet->event_kind == EVENT_CLEAR ||
                      packet->event_kind == EVENT_FAULT);
}

static void *iap_worker(void *unused) {
    struct decoder_state state;
    struct ingress_item item;
    struct pg_packet packet;
    int fd = -1;
    u32 last_connect = 0, last_heartbeat = 0, observed_drops = 0;
    (void)unused;
    memset(&state, 0, sizeof(state));
    for (;;) {
        u32 now = mono_ms();
        if (fd < 0 && (!last_connect || elapsed(now, last_connect, 1000u))) {
            last_connect = now;
            fd = connect_core();
            if (fd >= 0 && state.latest_valid &&
                    safe_reconnect_replay(&state.latest)) {
                packet = state.latest;
                packet.sequence = ++state.sequence;
                packet.monotonic_ms = now;
                if (send_packet(fd, &packet)) { close(fd); fd = -1; }
            }
        }
        while (take_item(&item)) {
            int decoded = decode_item(&state, &item, &packet);
            if ((decoded == 1 || decoded == -2) && fd >= 0 &&
                    send_packet(fd, &packet)) {
                close(fd); fd = -1;
            }
        }
        if (decoder_apply_drop(&state, &observed_drops, &packet)) {
            if (fd >= 0 && send_packet(fd, &packet)) { close(fd); fd = -1; }
        }
        now = mono_ms();
        if (fd >= 0 && (!last_heartbeat ||
                        elapsed(now, last_heartbeat, HEARTBEAT_MS))) {
            last_heartbeat = now;
            if (state.latest_valid) packet = state.latest;
            else {
                memset(&packet, 0, sizeof(packet));
                packet.magic = PACKET_MAGIC;
                packet.version = PACKET_VERSION;
                packet.size = (u16)sizeof(packet);
                packet.source = SOURCE_CARPLAY;
            }
            packet.event_kind = EVENT_HEARTBEAT;
            packet.sequence = ++state.sequence;
            packet.monotonic_ms = now;
            if (send_packet(fd, &packet)) { close(fd); fd = -1; }
        }
        pause_ms(20u);
    }
    return 0;
}

static int valid_packet(const struct pg_packet *packet, usize length) {
    u32 known_presence = PRES_ROUTE_STATE | PRES_MANEUVER_STATE |
        PRES_CURRENT_ROAD | PRES_DESTINATION | PRES_ETA |
        PRES_TIME_REMAINING | PRES_DISTANCE_REMAIN | PRES_NEXT_TURN_METRES |
        PRES_CURRENT_INDEX | PRES_VISIBLE | PRES_SOURCE_SUPPORTS |
        PRES_SOURCE_TYPE | PRES_SEMANTIC | PRES_DESCRIPTION | PRES_AFTER_ROAD |
        PRES_DRIVING_SIDE | PRES_JUNCTION_TYPE | PRES_EXIT_ANGLE |
        PRES_LINKED_LANE | PRES_LANE_INDEX | PRES_LANE_COUNT | PRES_EXIT_INFO;
    if (!packet || length != sizeof(*packet) || packet->magic != PACKET_MAGIC ||
            packet->version != PACKET_VERSION || packet->size != sizeof(*packet) ||
            packet->source != SOURCE_CARPLAY || packet->reserved ||
            packet->route_active > 1u || (packet->presence & ~known_presence) ||
            packet->turn_side ||
            packet->event_kind < EVENT_SNAPSHOT ||
            packet->event_kind > EVENT_FAULT) return 0;
    if (((packet->presence & PRES_VISIBLE) && packet->visible > 1u) ||
            ((packet->presence & PRES_SOURCE_SUPPORTS) &&
             packet->source_supports > 1u)) return 0;
    if (packet->current_road[TEXT_BYTES - 1u] ||
            packet->destination[TEXT_BYTES - 1u] ||
            packet->description[TEXT_BYTES - 1u] ||
            packet->after_road[TEXT_BYTES - 1u] ||
            packet->exit_info[TEXT_BYTES - 1u]) return 0;
#define ABSENT_NONZERO(bit, member) \
    (!(packet->presence & (bit)) && packet->member)
    if (ABSENT_NONZERO(PRES_ROUTE_STATE, route_state) ||
            ABSENT_NONZERO(PRES_MANEUVER_STATE, maneuver_state) ||
            ABSENT_NONZERO(PRES_ETA, eta_unix_seconds) ||
            ABSENT_NONZERO(PRES_TIME_REMAINING, time_remaining_seconds) ||
            ABSENT_NONZERO(PRES_DISTANCE_REMAIN, distance_remaining_metres) ||
            ABSENT_NONZERO(PRES_NEXT_TURN_METRES, next_turn_metres) ||
            ABSENT_NONZERO(PRES_CURRENT_INDEX, maneuver_index) ||
            ABSENT_NONZERO(PRES_VISIBLE, visible) ||
            ABSENT_NONZERO(PRES_SOURCE_SUPPORTS, source_supports) ||
            ABSENT_NONZERO(PRES_SOURCE_TYPE, source_type) ||
            ABSENT_NONZERO(PRES_SEMANTIC, semantic) ||
            ABSENT_NONZERO(PRES_DRIVING_SIDE, driving_side) ||
            ABSENT_NONZERO(PRES_JUNCTION_TYPE, junction_type) ||
            ABSENT_NONZERO(PRES_EXIT_ANGLE, exit_angle_deg) ||
            ABSENT_NONZERO(PRES_LINKED_LANE, linked_lane_index) ||
            ABSENT_NONZERO(PRES_LANE_INDEX, lane_index) ||
            ABSENT_NONZERO(PRES_LANE_COUNT, lane_count)) return 0;
#undef ABSENT_NONZERO
    return 1;
}

static int same_log_payload(const struct pg_packet *left,
                            const struct pg_packet *right) {
    struct pg_packet a = *left, b = *right;
    a.sequence = b.sequence = 0;
    a.monotonic_ms = b.monotonic_ms = 0;
    a.event_kind = b.event_kind = 0;
    return !memcmp(&a, &b, sizeof(a));
}

static int sequence_is_new(int previous_valid, u32 previous_epoch,
                           u32 previous_sequence,
                           u32 candidate_epoch, u32 candidate_sequence) {
    return !previous_valid || candidate_epoch != previous_epoch ||
           (s32)(candidate_sequence - previous_sequence) > 0;
}

static const char *clear_name(u8 reason) {
    switch (reason) {
    case CLEAR_SESSION_REPLACED: return "SESSION_REPLACED";
    case CLEAR_ROUTE_INACTIVE: return "ROUTE_INACTIVE";
    case CLEAR_TBT_OFF: return "TBT_OFF";
    case CLEAR_IPC_LOST: return "IPC_LOST";
    case CLEAR_WATCHDOG: return "WATCHDOG";
    case CLEAR_DECODE_FAULT: return "DECODE_FAULT";
    case CLEAR_DISCONNECTED: return "DISCONNECTED";
    default: return "UNSPECIFIED";
    }
}

static const char *cp_decision_name(u8 decision) {
    switch (decision) {
    case CP_DECISION_ROUTE_INACTIVE: return "ROUTE_INACTIVE";
    case CP_DECISION_SOURCE_UNSUPPORTED: return "SOURCE_UNSUPPORTED";
    case CP_DECISION_GATE_UNKNOWN: return "GATE_UNKNOWN";
    case CP_DECISION_TBT_OFF: return "TBT_OFF";
    case CP_DECISION_DISCONNECTED: return "DISCONNECTED";
    default: return 0;
    }
}

static u8 cp_packet_decision(const struct pg_packet *packet) {
    if (!packet->route_active) {
        if ((packet->presence & PRES_SOURCE_SUPPORTS) &&
                !packet->source_supports)
            return CP_DECISION_SOURCE_UNSUPPORTED;
        return CP_DECISION_ROUTE_INACTIVE;
    }
    if (g_cp_tbt == 0) return CP_DECISION_TBT_OFF;
    if (g_cp_usb_connected == 0) return CP_DECISION_DISCONNECTED;
    if (g_cp_tbt != 1 || g_cp_usb_connected != 1)
        return CP_DECISION_GATE_UNKNOWN;
    return CP_DECISION_ACTIVE;
}

static void log_cp_packet(const struct pg_packet *packet, u8 decision) {
    const char *suppression = cp_decision_name(decision);
    char line[640];
    if (packet->event_kind == EVENT_CLEAR || packet->event_kind == EVENT_FAULT) {
        char destination[TEXT_BYTES * 4u];
        log_escape(packet->destination, destination);
        snprintf(line, sizeof(line),
            "PGI:src=CP action=%s reason=%s epoch=%u seq=%u destination=\"%s\"",
            packet->event_kind == EVENT_FAULT ? "FAULT" : "CLEAR",
            clear_name(packet->clear_reason), packet->session_epoch,
            packet->sequence, destination);
        line[sizeof(line) - 1u] = 0;
        native_log(line);
    } else {
        if (suppression)
            snprintf(line, sizeof(line),
                "PGI:src=CP action=SUPPRESSED reason=%s epoch=%u seq=%u "
                "state=%u active=%u rev=%u idx=%u type=%u semantic=%s "
                "distanceMeters=%u routeRemainingMeters=%u angle=%d",
                suppression, packet->session_epoch, packet->sequence,
                packet->route_state, packet->route_active,
                packet->maneuver_revision, packet->maneuver_index,
                packet->source_type, semantic_name(packet->semantic),
                packet->next_turn_metres, packet->distance_remaining_metres,
                (int)packet->exit_angle_deg);
        else
            snprintf(line, sizeof(line),
                "PGI:src=CP epoch=%u seq=%u state=%u active=%u rev=%u idx=%u "
                "type=%u semantic=%s distanceMeters=%u routeRemainingMeters=%u "
                "angle=%d",
                packet->session_epoch, packet->sequence, packet->route_state,
                packet->route_active, packet->maneuver_revision,
                packet->maneuver_index, packet->source_type,
                semantic_name(packet->semantic), packet->next_turn_metres,
                packet->distance_remaining_metres, (int)packet->exit_angle_deg);
        line[sizeof(line) - 1u] = 0;
        native_log(line);
        if (suppression)
            snprintf(line, sizeof(line),
                "PGI:src=CP action=SUPPRESSED reason=%s epoch=%u seq=%u "
                "meta presence=0x%08x pManeuverState=%u maneuverState=%u "
                "pEta=%u etaUnix=%llu pTime=%u timeRemainingSeconds=%llu "
                "pLaneIndex=%u laneIndex=%u pLaneCount=%u laneCount=%u "
                "pVisible=%u visible=%u pSourceSupports=%u sourceSupports=%u "
                "pDrivingSide=%u drivingSide=%u pJunctionType=%u junctionType=%u "
                "pLinkedLane=%u linkedLaneIndex=%u tbtGate=%d usbGate=%d",
                suppression, packet->session_epoch, packet->sequence,
                packet->presence,
                !!(packet->presence & PRES_MANEUVER_STATE), packet->maneuver_state,
                !!(packet->presence & PRES_ETA),
                (unsigned long long)packet->eta_unix_seconds,
                !!(packet->presence & PRES_TIME_REMAINING),
                (unsigned long long)packet->time_remaining_seconds,
                !!(packet->presence & PRES_LANE_INDEX), packet->lane_index,
                !!(packet->presence & PRES_LANE_COUNT), packet->lane_count,
                !!(packet->presence & PRES_VISIBLE), packet->visible,
                !!(packet->presence & PRES_SOURCE_SUPPORTS), packet->source_supports,
                !!(packet->presence & PRES_DRIVING_SIDE), packet->driving_side,
                !!(packet->presence & PRES_JUNCTION_TYPE), packet->junction_type,
                !!(packet->presence & PRES_LINKED_LANE), packet->linked_lane_index,
                g_cp_tbt, g_cp_usb_connected);
        else
            snprintf(line, sizeof(line),
                "PGI:src=CP epoch=%u seq=%u meta presence=0x%08x "
                "pManeuverState=%u maneuverState=%u pEta=%u etaUnix=%llu "
                "pTime=%u timeRemainingSeconds=%llu pLaneIndex=%u laneIndex=%u "
                "pLaneCount=%u laneCount=%u pVisible=%u visible=%u "
                "pSourceSupports=%u sourceSupports=%u pDrivingSide=%u "
                "drivingSide=%u pJunctionType=%u junctionType=%u "
                "pLinkedLane=%u linkedLaneIndex=%u tbtGate=%d usbGate=%d",
                packet->session_epoch, packet->sequence, packet->presence,
                !!(packet->presence & PRES_MANEUVER_STATE), packet->maneuver_state,
                !!(packet->presence & PRES_ETA),
                (unsigned long long)packet->eta_unix_seconds,
                !!(packet->presence & PRES_TIME_REMAINING),
                (unsigned long long)packet->time_remaining_seconds,
                !!(packet->presence & PRES_LANE_INDEX), packet->lane_index,
                !!(packet->presence & PRES_LANE_COUNT), packet->lane_count,
                !!(packet->presence & PRES_VISIBLE), packet->visible,
                !!(packet->presence & PRES_SOURCE_SUPPORTS), packet->source_supports,
                !!(packet->presence & PRES_DRIVING_SIDE), packet->driving_side,
                !!(packet->presence & PRES_JUNCTION_TYPE), packet->junction_type,
                !!(packet->presence & PRES_LINKED_LANE), packet->linked_lane_index,
                g_cp_tbt, g_cp_usb_connected);
        line[sizeof(line) - 1u] = 0;
        native_log(line);
#define LOG_CP_TEXT(bit, label, member) do { \
            if (packet->presence & (bit)) { \
                char escaped[TEXT_BYTES * 4u]; \
                log_escape(packet->member, escaped); \
                if (suppression) \
                    snprintf(line, sizeof(line), \
                        "PGI:src=CP action=SUPPRESSED reason=%s epoch=%u " \
                        "seq=%u field=" label " value=\"%s\"", suppression, \
                        packet->session_epoch, packet->sequence, escaped); \
                else snprintf(line, sizeof(line), \
                    "PGI:src=CP epoch=%u seq=%u field=" label " value=\"%s\"", \
                    packet->session_epoch, packet->sequence, escaped); \
                line[sizeof(line) - 1u] = 0; \
                native_log(line); \
            } \
        } while (0)
        LOG_CP_TEXT(PRES_CURRENT_ROAD, "road", current_road);
        LOG_CP_TEXT(PRES_DESTINATION, "destination", destination);
        LOG_CP_TEXT(PRES_DESCRIPTION, "instruction", description);
        LOG_CP_TEXT(PRES_AFTER_ROAD, "afterRoad", after_road);
        LOG_CP_TEXT(PRES_EXIT_INFO, "exitInfo", exit_info);
#undef LOG_CP_TEXT
    }
}

static void core_clear_cp(u8 reason) {
    char line[192];
    int had_state = g_core_cp_valid;
    g_cp_clear_pending = 1;
    g_core_cp_gap_pending = 0;
    g_core_cp_gap_since_ms = 0u;
    __sync_synchronize();
    g_core_cp_valid = 0;
    g_core_cp_logged_valid = 0;
    __sync_synchronize();
    if (try_lock(&g_core_lock)) {
        g_core_cp_logged_valid = 0;
        g_core_cp_gap_pending = 0;
        g_core_cp_gap_since_ms = 0u;
        g_cp_clear_pending = 0;
        unlock(&g_core_lock);
    }
    if (had_state || reason == CLEAR_TBT_OFF || reason == CLEAR_DISCONNECTED) {
        snprintf(line, sizeof(line), "PGI:src=CP action=CLEAR reason=%s",
                 clear_name(reason));
        native_log(line);
    }
    writer_cp_input_mutated(0);
}

static int cp_gate_active(void) {
    return g_cp_tbt == 1 && g_cp_usb_connected == 1;
}

static int core_accept_packet(const struct pg_packet *packet) {
    struct writer_candidate recovery_candidate;
    int should_log = 0;
    int route_gap = 0;
    int publishable = 0;
    int preserve_gap = 0;
    int preserve_gap_clear = 0;
    u8 decision = CP_DECISION_ACTIVE;
    if (!valid_packet(packet, sizeof(*packet))) return -1;
    if (!try_lock(&g_core_lock)) return -2;
    if (g_cp_clear_pending) {
        g_core_cp_valid = 0;
        g_core_cp_logged_valid = 0;
        g_core_cp_gap_pending = 0;
        g_core_cp_gap_since_ms = 0u;
        g_cp_clear_pending = 0;
    }
    if (!sequence_is_new(g_core_cp_sequence_valid, g_core_cp_epoch,
                         g_core_cp_sequence, packet->session_epoch,
                         packet->sequence)) {
        unlock(&g_core_lock); return -3;
    }
    g_core_cp_sequence_valid = 1;
    g_core_cp_epoch = packet->session_epoch;
    g_core_cp_sequence = packet->sequence;
    if (packet->event_kind == EVENT_HEARTBEAT) {
        /* Heartbeats carry the last event; they do not commit new route state. */
        if (g_core_cp_valid && packet->session_epoch == g_core_cp.session_epoch)
            g_core_cp.sequence = packet->sequence;
        unlock(&g_core_lock); return 0;
    }
    route_gap = packet->event_kind == EVENT_CLEAR &&
        packet->clear_reason == CLEAR_ROUTE_INACTIVE && cp_gate_active() &&
        writer_cp_gap_eligible();
    if (packet->event_kind == EVENT_CLEAR || packet->event_kind == EVENT_FAULT) {
        g_core_cp = *packet;
        g_core_cp_valid = 0;
        g_core_cp_logged_valid = 0;
        should_log = 1;
    } else {
        decision = cp_packet_decision(packet);
        g_core_cp = *packet;
        g_core_cp_valid = decision == CP_DECISION_ACTIVE;
        if (g_core_cp_valid) {
            memset(&recovery_candidate, 0, sizeof(recovery_candidate));
            publishable = writer_encode_packet_candidate(
                WRITER_SOURCE_CP, packet, &recovery_candidate);
        }
        if (!g_core_cp_logged_valid ||
                decision != g_core_cp_logged_decision ||
                !same_log_payload(packet, &g_core_cp_logged)) {
            g_core_cp_logged = *packet;
            g_core_cp_logged_valid = 1;
            g_core_cp_logged_decision = decision;
            should_log = 1;
        }
    }
    if (route_gap) {
        if (!g_core_cp_gap_pending) {
            g_core_cp_gap_pending = 1;
            g_core_cp_gap_since_ms = mono_ms();
        }
    } else if (packet->event_kind != EVENT_SNAPSHOT || publishable ||
               !cp_gate_active()) {
        g_core_cp_gap_pending = 0;
        g_core_cp_gap_since_ms = 0u;
    }
    preserve_gap = g_core_cp_gap_pending && (route_gap ||
        (packet->event_kind == EVENT_SNAPSHOT && cp_gate_active() &&
         !publishable));
    preserve_gap_clear = cp_gate_active() &&
        ((packet->event_kind == EVENT_CLEAR &&
          packet->clear_reason == CLEAR_ROUTE_INACTIVE) ||
         (packet->event_kind == EVENT_SNAPSHOT && !publishable));
    if (g_cp_clear_pending) {
        g_core_cp_valid = 0;
        g_core_cp_logged_valid = 0;
        g_core_cp_gap_pending = 0;
        g_core_cp_gap_since_ms = 0u;
        publishable = 0;
        preserve_gap = 0;
        preserve_gap_clear = 0;
        g_cp_clear_pending = 0;
    }
    writer_cp_input_committed(preserve_gap, publishable,
                              preserve_gap_clear);
    unlock(&g_core_lock);
    writer_wake();
    if (packet->event_kind == EVENT_SNAPSHOT && decision == CP_DECISION_ACTIVE &&
            !cp_gate_active()) decision = cp_packet_decision(packet);
    if (should_log) log_cp_packet(packet, decision);
    return 0;
}

static int make_named_server_socket(const char *name) {
    struct sockaddr_un_native address;
    u32 length;
    int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
    if (fd < 0) fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (fd < 0) return -1;
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    socket_address_named(&address, &length, name);
    if (bind(fd, &address, length) || listen(fd, 1)) {
        close(fd); return -2;
    }
    return fd;
}

static int make_server_socket(void) {
    return make_named_server_socket(k_socket_name);
}

static void *app_server_worker(void *unused) {
    int server = -1;
    (void)unused;
    for (;;) {
        int client;
        u32 last_valid;
        if (server < 0) {
            server = make_server_socket();
            if (server < 0) { pause_ms(1000u); continue; }
        }
        client = accept(server, 0, 0);
        if (client < 0) { pause_ms(100u); continue; }
        fcntl(client, F_SETFD, FD_CLOEXEC);
        if (!exact_peer(client, "/app/bin/iAP2Service")) {
            close(client); continue;
        }
        g_core_cp_sequence_valid = 0;
        __sync_synchronize();
        last_valid = mono_ms();
        for (;;) {
            struct pollfd_native descriptor;
            int polled;
            u32 now;
            descriptor.fd = client;
            descriptor.events = POLLIN;
            descriptor.revents = 0;
            polled = poll(&descriptor, 1u, 1000);
            now = mono_ms();
            if (polled < 0 || (descriptor.revents & (POLLERR | POLLHUP))) break;
            if (polled > 0 && (descriptor.revents & POLLIN)) {
                struct pg_packet packet;
                ssize received = recv(client, &packet, sizeof(packet), MSG_TRUNC);
                if (received != (ssize)sizeof(packet) ||
                        core_accept_packet(&packet) < 0) break;
                last_valid = now;
            }
            if (elapsed(now, last_valid, WATCHDOG_MS)) {
                core_clear_cp(CLEAR_WATCHDOG); break;
            }
        }
        close(client);
        core_clear_cp(CLEAR_IPC_LOST);
    }
    return 0;
}

/*
 * AppProjection proves that a projected set is actually published; merely
 * owning 0x0AFD is insufficient.  SO_PEERCRED plus the exact executable path
 * binds this lease to the fitted AppProjection process.  The AppNavi side
 * independently requires its local cmd2 before suppressing anything.
 */
static int writer_lease_snapshot(struct lease_packet *packet, u32 sequence) {
    int active;
    if (!packet) return -1;
    memset(packet, 0, sizeof(*packet));
    packet->magic = LEASE_MAGIC;
    packet->version = LEASE_VERSION;
    packet->size = (u16)sizeof(*packet);
    packet->sequence = sequence;
    packet->monotonic_ms = mono_ms();
    blocking_lock(&g_writer.lock);
    active = g_writer.transport_ready && g_writer.notifier && g_writer.qcan &&
        g_writer.published && writer_known_frames(g_writer.published_frames) &&
        (g_writer.published_source == WRITER_SOURCE_AA ||
         g_writer.published_source == WRITER_SOURCE_CP) &&
        ((!g_writer.notifier_closing && !g_writer.stop &&
          g_writer.owner_source == g_writer.published_source &&
          writer_country_allowed(g_writer.country_variant)) ||
         (g_writer.desired == WRITER_DESIRED_CLEAR &&
          g_writer.desired_source == g_writer.published_source));
    packet->generation = g_writer.generation;
    if (active) {
        packet->active = 1u;
        packet->frames = g_writer.published_frames;
        packet->source = g_writer.published_source;
    }
    unlock(&g_writer.lock);
    return active;
}

static int lease_packet_valid(const struct lease_packet *packet) {
    if (!packet || packet->magic != LEASE_MAGIC ||
            packet->version != LEASE_VERSION ||
            packet->size != sizeof(*packet) || packet->reserved ||
            packet->active > 1u) return 0;
    if (packet->active)
        return writer_known_frames(packet->frames) &&
            (packet->source == WRITER_SOURCE_AA ||
             packet->source == WRITER_SOURCE_CP);
    return packet->frames == 0u && packet->source == 0u;
}

static int connect_lease_server(void) {
    struct sockaddr_un_native address;
    u32 length;
    int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
    if (fd < 0) fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (fd < 0) return -1;
    (void)fcntl(fd, F_SETFD, FD_CLOEXEC);
    socket_address_named(&address, &length, k_lease_socket_name);
    if (connect(fd, &address, length) ||
            !exact_peer(fd, "/navi/Bin/AppNavi")) {
        (void)close(fd);
        return -2;
    }
    return fd;
}

static void writer_lease_drain_wake(void) {
    u8 bytes[32];
    if (g_lease_wake[0] < 0) return;
    while (recv(g_lease_wake[0], bytes, sizeof(bytes), MSG_DONTWAIT) > 0) { }
}

static void *writer_lease_worker(void *unused) {
    int fd = -1;
    u32 sequence = 0u;
    (void)unused;
    for (;;) {
        struct lease_packet packet;
        struct pollfd_native descriptor;
        int polled;
        if (fd < 0) {
            fd = connect_lease_server();
            sequence = 0u;
            if (fd < 0) {
                descriptor.fd = g_lease_wake[0];
                descriptor.events = POLLIN;
                descriptor.revents = 0;
                if (descriptor.fd >= 0)
                    (void)poll(&descriptor, 1u, (int)LEASE_HEARTBEAT_MS);
                else pause_ms(LEASE_HEARTBEAT_MS);
                writer_lease_drain_wake();
                continue;
            }
        }
        (void)writer_lease_snapshot(&packet, ++sequence);
        if (send(fd, &packet, sizeof(packet), MSG_DONTWAIT | MSG_NOSIGNAL) !=
                (ssize)sizeof(packet)) {
            (void)close(fd);
            fd = -1;
            continue;
        }
        descriptor.fd = g_lease_wake[0];
        descriptor.events = POLLIN;
        descriptor.revents = 0;
        if (descriptor.fd >= 0)
            polled = poll(&descriptor, 1u, (int)LEASE_HEARTBEAT_MS);
        else {
            pause_ms(LEASE_HEARTBEAT_MS);
            polled = 0;
        }
        if (polled < 0 ||
                (descriptor.revents & (POLLERR | POLLHUP | POLLNVAL))) {
            (void)close(fd);
            fd = -1;
        }
        writer_lease_drain_wake();
    }
    return 0;
}

static int navi_sequence_after(u32 value, u32 before) {
    return (s32)(value - before) > 0;
}

static int navi_lease_fresh_locked(u32 now) {
    return g_navi_gate.lease_valid &&
        !elapsed(now, g_navi_gate.lease_rx_ms, LEASE_EXPIRY_MS);
}

static int navi_packet_after_owner_locked(void) {
    return !g_navi_gate.owner_sequence_valid ||
        g_navi_gate.lease_connection != g_navi_gate.owner_connection ||
        navi_sequence_after(g_navi_gate.lease_sequence,
                            g_navi_gate.owner_sequence);
}

static int navi_should_gate_locked(u32 now) {
    return g_navi_gate.local_projected &&
        !g_navi_gate.owner_needs_fresh && g_navi_gate.lease_active &&
        navi_lease_fresh_locked(now);
}

static void navi_arm_replay_locked(void) {
    if (g_navi_gate.cache_valid) g_navi_gate.replay_pending = 1u;
    g_navi_gate.replay_wait_inactive = 0u;
    g_navi_gate.replay_deadline_ms = 0u;
}

static void navi_expire_locked(u32 now) {
    if (g_navi_gate.lease_valid &&
            elapsed(now, g_navi_gate.lease_rx_ms, LEASE_EXPIRY_MS)) {
        int was_engaged = g_navi_gate.gate_engaged;
        g_navi_gate.lease_valid = 0u;
        g_navi_gate.lease_active = 0u;
        g_navi_gate.lease_frames = 0u;
        g_navi_gate.lease_source = 0u;
        g_navi_gate.lease_sequence_valid = 0u;
        g_navi_gate.gate_engaged = 0u;
        if (was_engaged) g_navi_gate.awaiting_inactive = 1u;
        if (was_engaged || g_navi_gate.replay_wait_inactive)
            navi_arm_replay_locked();
    }
    if (g_navi_gate.replay_wait_inactive &&
            elapsed(now, g_navi_gate.replay_deadline_ms,
                    LEASE_REPLAY_FALLBACK_MS))
        navi_arm_replay_locked();
}

static void navi_reconcile_gate_locked(u32 now) {
    int should_gate = navi_should_gate_locked(now);
    if (should_gate && !g_navi_gate.gate_engaged) {
        g_navi_gate.gate_engaged = 1u;
        g_navi_gate.replay_wait_inactive = 0u;
        g_navi_gate.replay_pending = 0u;
        g_navi_gate.replay_deadline_ms = 0u;
        g_navi_gate.suppress_logged = 0u;
    } else if (!should_gate && g_navi_gate.gate_engaged) {
        g_navi_gate.gate_engaged = 0u;
        if (g_navi_gate.lease_active)
            g_navi_gate.awaiting_inactive = 1u;
        navi_arm_replay_locked();
    }
}

static void navi_log_gate_transition(const char *action, const char *reason,
                                     u8 source, u8 frames) {
    char line[176];
    snprintf(line, sizeof(line),
        "PGI:navi-gate action=%s reason=%s src=%s frames=%02X",
        action, reason,
        source == WRITER_SOURCE_CP ? "CP" :
            (source == WRITER_SOURCE_AA ? "AA" : "NONE"), frames);
    native_log(line);
}

static void navi_log_suppress(s16 frame_id, u8 source, u8 frames) {
    char line[160];
    snprintf(line, sizeof(line),
        "PGI:navi-gate action=SUPPRESS id=%03X src=%s frames=%02X",
        (u32)(u16)frame_id,
        source == WRITER_SOURCE_CP ? "CP" : "AA", frames);
    native_log(line);
}

static void navi_connection_begin(void) {
    u32 now = mono_ms();
    int was_engaged;
    u8 source, frames;
    blocking_lock(&g_navi_gate.send_lock);
    blocking_lock(&g_navi_gate.lock);
    was_engaged = g_navi_gate.gate_engaged;
    source = g_navi_gate.lease_source;
    frames = g_navi_gate.lease_frames;
    navi_expire_locked(now);
    if (g_navi_gate.gate_engaged)
        g_navi_gate.awaiting_inactive = 1u;
    if (g_navi_gate.gate_engaged || g_navi_gate.replay_wait_inactive)
        navi_arm_replay_locked();
    g_navi_gate.gate_engaged = 0u;
    g_navi_gate.lease_valid = 0u;
    g_navi_gate.lease_active = 0u;
    g_navi_gate.lease_frames = 0u;
    g_navi_gate.lease_source = 0u;
    g_navi_gate.lease_sequence_valid = 0u;
    g_navi_gate.lease_sequence = 0u;
    g_navi_gate.lease_connection++;
    unlock(&g_navi_gate.lock);
    unlock(&g_navi_gate.send_lock);
    if (was_engaged)
        navi_log_gate_transition("RELEASE", "CONNECTION_REPLACED",
                                 source, frames);
}

static void navi_connection_lost(void) {
    int was_engaged;
    u8 source, frames;
    blocking_lock(&g_navi_gate.send_lock);
    blocking_lock(&g_navi_gate.lock);
    was_engaged = g_navi_gate.gate_engaged;
    source = g_navi_gate.lease_source;
    frames = g_navi_gate.lease_frames;
    if (g_navi_gate.gate_engaged)
        g_navi_gate.awaiting_inactive = 1u;
    if (g_navi_gate.gate_engaged || g_navi_gate.replay_wait_inactive)
        navi_arm_replay_locked();
    g_navi_gate.gate_engaged = 0u;
    g_navi_gate.lease_valid = 0u;
    g_navi_gate.lease_active = 0u;
    g_navi_gate.lease_frames = 0u;
    g_navi_gate.lease_source = 0u;
    g_navi_gate.lease_sequence_valid = 0u;
    g_navi_gate.lease_sequence = 0u;
    unlock(&g_navi_gate.lock);
    unlock(&g_navi_gate.send_lock);
    if (was_engaged)
        navi_log_gate_transition("RELEASE", "IPC_LOST", source, frames);
}

static int navi_accept_lease(const struct lease_packet *packet, u32 now) {
    int gate, was_engaged, is_engaged;
    u8 log_source, log_frames;
    blocking_lock(&g_navi_gate.send_lock);
    blocking_lock(&g_navi_gate.lock);
    was_engaged = g_navi_gate.gate_engaged;
    log_source = g_navi_gate.lease_source;
    log_frames = g_navi_gate.lease_frames;
    navi_expire_locked(now);
    if (!lease_packet_valid(packet) ||
            (g_navi_gate.lease_sequence_valid &&
             !navi_sequence_after(packet->sequence,
                                  g_navi_gate.lease_sequence))) {
        unlock(&g_navi_gate.lock);
        unlock(&g_navi_gate.send_lock);
        return -1;
    }
    g_navi_gate.lease_sequence = packet->sequence;
    g_navi_gate.lease_sequence_valid = 1u;
    g_navi_gate.lease_rx_ms = now;
    g_navi_gate.lease_valid = 1u;
    g_navi_gate.lease_active = packet->active;
    g_navi_gate.lease_frames = packet->frames;
    g_navi_gate.lease_source = packet->source;
    if (g_navi_gate.local_projected && g_navi_gate.owner_needs_fresh &&
            navi_packet_after_owner_locked())
        g_navi_gate.owner_needs_fresh = 0u;
    gate = navi_should_gate_locked(now);
    if (gate) {
        if (!g_navi_gate.gate_engaged)
            g_navi_gate.suppress_logged = 0u;
        g_navi_gate.gate_engaged = 1u;
        g_navi_gate.replay_wait_inactive = 0u;
        g_navi_gate.replay_pending = 0u;
        g_navi_gate.replay_deadline_ms = 0u;
    } else {
        g_navi_gate.gate_engaged = 0u;
        if (!packet->active &&
                (was_engaged || g_navi_gate.replay_wait_inactive ||
                 g_navi_gate.awaiting_inactive)) {
            navi_arm_replay_locked();
            g_navi_gate.awaiting_inactive = 0u;
        }
    }
    is_engaged = g_navi_gate.gate_engaged;
    if (is_engaged) {
        log_source = g_navi_gate.lease_source;
        log_frames = g_navi_gate.lease_frames;
    }
    unlock(&g_navi_gate.lock);
    unlock(&g_navi_gate.send_lock);
    if (was_engaged != is_engaged)
        navi_log_gate_transition(is_engaged ? "ENGAGE" : "RELEASE",
                                 packet->active ? "LEASE_ACTIVE" :
                                                  "LEASE_INACTIVE",
                                 log_source, log_frames);
    return 0;
}

static void navi_owner_command(u32 command) {
    u32 now = mono_ms();
    int was_engaged, is_engaged;
    u8 source, frames;
    blocking_lock(&g_navi_gate.send_lock);
    blocking_lock(&g_navi_gate.lock);
    was_engaged = g_navi_gate.gate_engaged;
    source = g_navi_gate.lease_source;
    frames = g_navi_gate.lease_frames;
    navi_expire_locked(now);
    if (command == ROUTE_OWNER_PROJECTED) {
        if (!g_navi_gate.local_projected) {
            g_navi_gate.local_projected = 1u;
            g_navi_gate.owner_needs_fresh = 1u;
            g_navi_gate.owner_connection = g_navi_gate.lease_connection;
            g_navi_gate.owner_sequence = g_navi_gate.lease_sequence;
            g_navi_gate.owner_sequence_valid =
                g_navi_gate.lease_sequence_valid;
            g_navi_gate.gate_engaged = 0u;
            g_navi_gate.replay_wait_inactive = 0u;
            g_navi_gate.replay_pending = 0u;
            g_navi_gate.replay_deadline_ms = 0u;
            g_navi_gate.awaiting_inactive = 0u;
        }
    } else {
        g_navi_gate.local_projected = 0u;
        g_navi_gate.owner_needs_fresh = 0u;
        g_navi_gate.owner_sequence_valid = 0u;
        g_navi_gate.gate_engaged = 0u;
        if (was_engaged) {
            if (g_navi_gate.lease_active && navi_lease_fresh_locked(now)) {
                g_navi_gate.replay_wait_inactive = 1u;
                g_navi_gate.replay_deadline_ms = now;
                g_navi_gate.awaiting_inactive = 1u;
            } else navi_arm_replay_locked();
        }
    }
    is_engaged = g_navi_gate.gate_engaged;
    unlock(&g_navi_gate.lock);
    unlock(&g_navi_gate.send_lock);
    if (was_engaged && !is_engaged)
        navi_log_gate_transition("RELEASE", "OWNER", source, frames);
}

static u8 navi_cache_mask(s16 frame_id) {
    if (frame_id == (s16)NAVI_ROUTE_STATE_FRAME_ID) return NAVI_CACHE_123;
    if (frame_id == (s16)QCAN_FRAME_ID) return NAVI_CACHE_115;
    if (frame_id == (s16)QCAN_ROUTE_FRAME_ID) return NAVI_CACHE_506;
    return 0u;
}

static u8 *navi_cache_payload_locked(u8 mask) {
    if (mask == NAVI_CACHE_123) return g_navi_gate.cache_123;
    if (mask == NAVI_CACHE_115) return g_navi_gate.cache_115;
    return g_navi_gate.cache_506;
}

static int navi_sender_destroyed_locked(u32 entry_epoch, const void *self) {
    return entry_epoch != g_navi_gate.sender_epoch &&
        self == g_navi_gate.destroyed_sender;
}

static void hook_navi_can_send(void *self, u8 *payload, s16 frame_id) {
    u8 replay_123[8], replay_115[8], replay_506[8];
    u8 current_mask, replay_mask = 0u;
    u8 log_source = 0u, log_frames = 0u;
    int current_replayed = 0, gate_transition = 0, log_suppression = 0;
    u32 entry_epoch;
    if (!g_navi_original_can_send) return;
    if (!g_navi_ready) {
        g_navi_original_can_send(self, payload, frame_id);
        return;
    }
    entry_epoch = __sync_add_and_fetch(&g_navi_gate.sender_epoch, 0u);
    blocking_lock(&g_navi_gate.send_lock);
    blocking_lock(&g_navi_gate.lock);
    /* The fitted AppNavi has one global NaviCanClusterComm instance. */
    if (navi_sender_destroyed_locked(entry_epoch, self)) {
        unlock(&g_navi_gate.lock);
        unlock(&g_navi_gate.send_lock);
        return;
    }
    if (g_navi_gate.sender != self) {
        g_navi_gate.sender = self;
        g_navi_gate.destroyed_sender = 0;
        g_navi_gate.closing = 0u;
        g_navi_gate.cache_valid = 0u;
        g_navi_gate.replay_pending = 0u;
    }
    {
        u32 now = mono_ms();
        int was_engaged = g_navi_gate.gate_engaged;
        log_source = g_navi_gate.lease_source;
        log_frames = g_navi_gate.lease_frames;
        navi_expire_locked(now);
        navi_reconcile_gate_locked(now);
        if (was_engaged != g_navi_gate.gate_engaged) {
            gate_transition = g_navi_gate.gate_engaged ? 1 : -1;
            if (gate_transition > 0) {
                log_source = g_navi_gate.lease_source;
                log_frames = g_navi_gate.lease_frames;
            }
        }
    }
    current_mask = navi_cache_mask(frame_id);
    if (current_mask && payload) {
        memcpy(navi_cache_payload_locked(current_mask), payload, 8u);
        g_navi_gate.cache_valid |= current_mask;
    }
    if (current_mask && g_navi_gate.gate_engaged) {
        if (!g_navi_gate.suppress_logged) {
            g_navi_gate.suppress_logged = 1u;
            log_suppression = 1;
            log_source = g_navi_gate.lease_source;
            log_frames = g_navi_gate.lease_frames;
        }
        unlock(&g_navi_gate.lock);
        unlock(&g_navi_gate.send_lock);
        if (gate_transition)
            navi_log_gate_transition(gate_transition > 0 ? "ENGAGE" :
                                                          "RELEASE",
                                     "SEND_FRESHNESS",
                                     log_source, log_frames);
        if (log_suppression)
            navi_log_suppress(frame_id, log_source, log_frames);
        return;
    }
    if (g_navi_gate.replay_pending && !g_navi_gate.gate_engaged) {
        replay_mask = g_navi_gate.cache_valid;
        if (replay_mask & NAVI_CACHE_123)
            memcpy(replay_123, g_navi_gate.cache_123, 8u);
        if (replay_mask & NAVI_CACHE_115)
            memcpy(replay_115, g_navi_gate.cache_115, 8u);
        if (replay_mask & NAVI_CACHE_506)
            memcpy(replay_506, g_navi_gate.cache_506, 8u);
        g_navi_gate.replay_pending = 0u;
        current_replayed = (replay_mask & current_mask) != 0u;
    }
    unlock(&g_navi_gate.lock);
    if (replay_mask & NAVI_CACHE_123)
        g_navi_original_can_send(self, replay_123,
                                (s16)NAVI_ROUTE_STATE_FRAME_ID);
    if (replay_mask & NAVI_CACHE_115)
        g_navi_original_can_send(self, replay_115, (s16)QCAN_FRAME_ID);
    if (replay_mask & NAVI_CACHE_506)
        g_navi_original_can_send(self, replay_506,
                                (s16)QCAN_ROUTE_FRAME_ID);
    if (!current_replayed)
        g_navi_original_can_send(self, payload, frame_id);
    unlock(&g_navi_gate.send_lock);
    if (gate_transition)
        navi_log_gate_transition(gate_transition > 0 ? "ENGAGE" : "RELEASE",
                                 "SEND_FRESHNESS", log_source, log_frames);
}

static void hook_navi_owner_event(void *self, void *event) {
    u32 event_id = 0u, command = 0xffffffffu;
    if (!g_navi_original_owner_event) return;
    if (!g_navi_ready || !event) {
        g_navi_original_owner_event(self, event);
        return;
    }
    event_id = g_navi_event_id(event);
    command = g_navi_event_command(event);
    if (event_id == ROUTE_OWNER_EVENT) {
        navi_owner_command(command);
        if (command != ROUTE_OWNER_OFF && command != ROUTE_OWNER_NATIVE &&
                command != ROUTE_OWNER_PROJECTED)
            native_log("PGI:navi-gate action=UNKNOWN_FAIL_OPEN");
    }
    g_navi_original_owner_event(self, event);
}

static void hook_navi_sender_dtor(void *self) {
    if (!g_navi_original_sender_dtor) return;
    if (!g_navi_ready) {
        g_navi_original_sender_dtor(self);
        return;
    }
    blocking_lock(&g_navi_gate.send_lock);
    blocking_lock(&g_navi_gate.lock);
    (void)__sync_add_and_fetch(&g_navi_gate.sender_epoch, 1u);
    g_navi_gate.destroyed_sender = self;
    if (g_navi_gate.sender == self) {
        g_navi_gate.closing = 1u;
        g_navi_gate.destroyed_sender = self;
        g_navi_gate.sender = 0;
        g_navi_gate.cache_valid = 0u;
        g_navi_gate.replay_pending = 0u;
        g_navi_gate.replay_wait_inactive = 0u;
        g_navi_gate.replay_deadline_ms = 0u;
    }
    unlock(&g_navi_gate.lock);
    g_navi_original_sender_dtor(self);
    unlock(&g_navi_gate.send_lock);
}

static void *navi_lease_server_worker(void *unused) {
    (void)unused;
    for (;;) {
        int client = accept(g_navi_server_fd, 0, 0);
        if (client < 0) {
            pause_ms(100u);
            continue;
        }
        (void)fcntl(client, F_SETFD, FD_CLOEXEC);
        if (!exact_peer(client, "/app/bin/AppProjection")) {
            (void)close(client);
            continue;
        }
        navi_connection_begin();
        for (;;) {
            struct pollfd_native descriptor;
            struct lease_packet packet;
            int polled;
            u32 now;
            descriptor.fd = client;
            descriptor.events = POLLIN;
            descriptor.revents = 0;
            polled = poll(&descriptor, 1u, 100);
            now = mono_ms();
            if (polled < 0 ||
                    (descriptor.revents &
                     (POLLERR | POLLHUP | POLLNVAL))) break;
            if (polled > 0 && (descriptor.revents & POLLIN)) {
                ssize received = recv(client, &packet, sizeof(packet),
                                      MSG_TRUNC);
                if (received != (ssize)sizeof(packet) ||
                        navi_accept_lease(&packet, now)) break;
            } else {
                int was_engaged, is_engaged;
                u8 source, frames;
                blocking_lock(&g_navi_gate.send_lock);
                blocking_lock(&g_navi_gate.lock);
                was_engaged = g_navi_gate.gate_engaged;
                source = g_navi_gate.lease_source;
                frames = g_navi_gate.lease_frames;
                navi_expire_locked(now);
                is_engaged = g_navi_gate.gate_engaged;
                unlock(&g_navi_gate.lock);
                unlock(&g_navi_gate.send_lock);
                if (was_engaged && !is_engaged)
                    navi_log_gate_transition("RELEASE", "LEASE_EXPIRED",
                                             source, frames);
            }
        }
        (void)close(client);
        navi_connection_lost();
    }
    return 0;
}

struct aa_next_record {
    const char *road;
    s32 side;
    u32 magic;
    s32 event;
    s32 maneuver_number;
    s32 turn_angle;
};
#if __SIZEOF_POINTER__ == 4
typedef char aa_side_offset_must_be_4[
    __builtin_offsetof(struct aa_next_record, side) == 4 ? 1 : -1];
typedef char aa_magic_offset_must_be_8[
    __builtin_offsetof(struct aa_next_record, magic) == 8 ? 1 : -1];
#endif

static u8 aa_semantic(s32 event, s32 side) {
    switch (event) {
    case 1: return SEM_PROCEED_TO_ROUTE;
    case 2: return SEM_CONTINUE;
    case 3: return side == 1 ? SEM_SLIGHT_LEFT :
                   (side == 2 ? SEM_SLIGHT_RIGHT : SEM_STRAIGHT);
    case 4: return side == 1 ? SEM_TURN_LEFT :
                   (side == 2 ? SEM_TURN_RIGHT : SEM_STRAIGHT);
    case 5: return side == 1 ? SEM_SHARP_LEFT :
                   (side == 2 ? SEM_SHARP_RIGHT : SEM_STRAIGHT);
    case 6: return SEM_U_TURN;
    case 7: return SEM_ON_RAMP;
    case 8: return side == 1 ? SEM_OFF_RAMP_LEFT :
                   (side == 2 ? SEM_OFF_RAMP_RIGHT : SEM_OFF_RAMP);
    case 9: return side == 1 ? SEM_KEEP_LEFT :
                   (side == 2 ? SEM_KEEP_RIGHT : SEM_CONTINUE);
    case 10: return SEM_CONTINUE;
    case 11: return SEM_ROUNDABOUT_ENTER;
    case 12: case 13: return SEM_ROUNDABOUT_EXIT;
    case 14: return SEM_STRAIGHT;
    case 16: return SEM_FERRY_ENTER;
    case 17: return SEM_FERRY_CHANGE;
    case 19: return side == 1 ? SEM_ARRIVE_LEFT :
                    (side == 2 ? SEM_ARRIVE_RIGHT : SEM_ARRIVE);
    default: return SEM_UNKNOWN;
    }
}

static int bounded_c_string(const char *source, char output[TEXT_BYTES]) {
    u32 length = 0;
    if (!source) { output[0] = 0; return -1; }
    while (length < MAX_TLV_VALUE && source[length]) ++length;
    if (length == MAX_TLV_VALUE) { output[0] = 0; return -2; }
    return decode_text((const u8 *)source, length, output);
}

static void aa_fail_inactive(void) {
    g_aa_clear_pending = 1;
    g_aa_pair_pending = 0;
    __sync_synchronize();
    g_core_aa_valid = 0;
    __sync_synchronize();
}

static int aa_publish_allowed(void) {
    return g_aa_active && g_aa_focus_request == 1 &&
           g_aa_aoap_connected == 1;
}

static int hook_app_aa_status(void *self, int status) {
    int result = g_app_original_aa_status(self, status);
    int was_active = g_aa_active;
    char line[160];
    /* Factory enum: 0 UNAVAILABLE, 1 ACTIVE, 2 INACTIVE. */
    g_aa_active = status == 1;
    if (!g_aa_active || !was_active) {
        g_aa_revision = 0;
        g_aa_clear_pending = 1;
        g_aa_pair_pending = 0;
        g_core_aa_valid = 0;
        __sync_synchronize();
    }
    if (try_lock(&g_core_lock)) {
        if (g_aa_clear_pending) {
            memset(&g_core_aa, 0, sizeof(g_core_aa));
            g_aa_pair_pending = 0;
            g_aa_clear_pending = 0;
        }
        if (g_core_aa.magic != PACKET_MAGIC) {
            memset(&g_core_aa, 0, sizeof(g_core_aa));
            g_core_aa.magic = PACKET_MAGIC;
            g_core_aa.version = PACKET_VERSION;
            g_core_aa.size = (u16)sizeof(g_core_aa);
            g_core_aa.source = SOURCE_ANDROID_AUTO;
        }
        g_core_aa.sequence++;
        g_core_aa.monotonic_ms = mono_ms();
        g_core_aa.event_kind = g_aa_active ? EVENT_SNAPSHOT : EVENT_CLEAR;
        g_core_aa.clear_reason = g_aa_active ? 0 : CLEAR_ROUTE_INACTIVE;
        g_core_aa.route_state = 0;
        g_core_aa.route_active = (u8)g_aa_active;
        g_core_aa.presence &= ~PRES_ROUTE_STATE;
        if (status >= 0 && status <= 2) {
            g_core_aa.route_state = (u8)status;
            g_core_aa.presence |= PRES_ROUTE_STATE;
        }
        if (g_aa_clear_pending) {
            memset(&g_core_aa, 0, sizeof(g_core_aa));
            g_aa_pair_pending = 0;
            g_core_aa_valid = 0;
        } else g_core_aa_valid = aa_publish_allowed();
        unlock(&g_core_lock);
    } else aa_fail_inactive();
    snprintf(line, sizeof(line), "PGI:src=AA state=%d active=%d",
             status, g_aa_active);
    native_log(line);
    writer_input_mutated();
    return result;
}

static int hook_app_aa_next(void *self, const void *opaque) {
    const struct aa_next_record *record =
        (const struct aa_next_record *)opaque;
    char road[TEXT_BYTES], logged_road[TEXT_BYTES * 4u], line[640];
    int road_valid;
    int result;
    u32 revision;
    u8 semantic;
    result = g_app_original_aa_next(self, opaque);
    if (!record || record->magic != 0x314e4141u) return result;
    road_valid = bounded_c_string(record->road, road) == 0;
    if (!road_valid) road[0] = 0;
    semantic = aa_semantic(record->event, record->side);
    revision = __sync_add_and_fetch(&g_aa_revision, 1u);
    if (try_lock(&g_core_lock)) {
        if (g_aa_clear_pending) {
            memset(&g_core_aa, 0, sizeof(g_core_aa));
            g_aa_pair_pending = 0;
            g_aa_clear_pending = 0;
        }
        if (g_core_aa.magic != PACKET_MAGIC) {
            memset(&g_core_aa, 0, sizeof(g_core_aa));
            g_core_aa.magic = PACKET_MAGIC;
            g_core_aa.version = PACKET_VERSION;
            g_core_aa.size = (u16)sizeof(g_core_aa);
            g_core_aa.source = SOURCE_ANDROID_AUTO;
        }
        g_core_aa.sequence++;
        g_core_aa.monotonic_ms = mono_ms();
        g_core_aa.event_kind = EVENT_SNAPSHOT;
        g_core_aa.route_active = (u8)g_aa_active;
        g_core_aa.maneuver_revision = revision;
        g_core_aa.maneuver_index = 0;
        g_core_aa.source_type = 0;
        g_core_aa.semantic = semantic;
        g_core_aa.exit_angle_deg = 0;
        g_core_aa.turn_side = 0;
        g_core_aa.next_turn_metres = 0;
        memset(g_core_aa.after_road, 0, TEXT_BYTES);
        g_core_aa.presence &= ~(PRES_NEXT_TURN_METRES | PRES_AFTER_ROAD |
            PRES_CURRENT_INDEX | PRES_SOURCE_TYPE | PRES_EXIT_ANGLE |
            PRES_TURN_SIDE);
        if (record->maneuver_number >= 0 && record->maneuver_number <= 65535) {
            g_core_aa.maneuver_index = (u16)record->maneuver_number;
            g_core_aa.presence |= PRES_CURRENT_INDEX;
        }
        if (record->event >= 0 && record->event <= 255) {
            g_core_aa.source_type = (u8)record->event;
            g_core_aa.presence |= PRES_SOURCE_TYPE;
        }
        if (record->turn_angle >= -32768 && record->turn_angle <= 32767) {
            g_core_aa.exit_angle_deg = (s16)record->turn_angle;
            g_core_aa.presence |= PRES_EXIT_ANGLE;
        }
        if (record->side >= 1 && record->side <= 3) {
            g_core_aa.turn_side = (u8)record->side;
            g_core_aa.presence |= PRES_TURN_SIDE;
        }
        if (road_valid) {
            memcpy(g_core_aa.after_road, road, TEXT_BYTES);
            g_core_aa.presence |= PRES_AFTER_ROAD;
        }
        g_core_aa.presence |= PRES_SEMANTIC;
        if (g_aa_clear_pending) {
            memset(&g_core_aa, 0, sizeof(g_core_aa));
            g_aa_pair_pending = 0;
            g_core_aa_valid = 0;
        } else {
            g_core_aa_valid = aa_publish_allowed();
            g_aa_pair_pending = g_core_aa_valid;
        }
        unlock(&g_core_lock);
    } else aa_fail_inactive();
    log_escape(road, logged_road);
    snprintf(line, sizeof(line),
        "PGI:src=AA rev=%u type=%d semantic=%s side=%d number=%d angle=%d "
        "roadValid=%d road=\"%s\"",
        revision, record->event, semantic_name(semantic), record->side,
        record->maneuver_number, record->turn_angle, road_valid, logged_road);
    native_log(line);
    writer_input_mutated();
    return result;
}

static int hook_app_aa_distance(void *self, int distance_metres,
                                int time_seconds) {
    char line[224];
    int result = g_app_original_aa_distance(self, distance_metres, time_seconds);
    if (try_lock(&g_core_lock)) {
        if (g_aa_clear_pending) {
            memset(&g_core_aa, 0, sizeof(g_core_aa));
            g_aa_pair_pending = 0;
            g_aa_clear_pending = 0;
        }
        if (g_core_aa.magic == PACKET_MAGIC) {
            g_core_aa.sequence++;
            g_core_aa.monotonic_ms = mono_ms();
            if (distance_metres < 0) {
                g_core_aa.next_turn_metres = 0;
                g_core_aa.presence &= ~PRES_NEXT_TURN_METRES;
            } else {
                g_core_aa.next_turn_metres = (u32)distance_metres;
                g_core_aa.presence |= PRES_NEXT_TURN_METRES;
            }
        }
        if (g_aa_clear_pending) {
            memset(&g_core_aa, 0, sizeof(g_core_aa));
            g_aa_pair_pending = 0;
            g_core_aa_valid = 0;
        } else if (g_core_aa.magic == PACKET_MAGIC) {
            g_core_aa_valid = aa_publish_allowed();
            g_aa_pair_pending = 0;
        } else g_aa_pair_pending = 0;
        unlock(&g_core_lock);
    } else aa_fail_inactive();
    snprintf(line, sizeof(line),
             "PGI:src=AA rev=%u distanceMeters=%d timeSeconds=%d active=%d",
             g_aa_revision, distance_metres, time_seconds, g_aa_active);
    native_log(line);
    writer_input_mutated();
    return result;
}

static void hook_app_tbt(void *self, int enabled) {
    char line[128];
    g_cp_tbt = enabled ? 1 : 0;
    __sync_synchronize();
    if (!enabled) writer_release_source(WRITER_SOURCE_CP, CLEAR_TBT_OFF);
    else writer_set_pending_source(WRITER_SOURCE_CP, 1);
    g_app_original_tbt(self, enabled);
    if (!enabled) core_clear_cp(CLEAR_TBT_OFF);
    else {
        snprintf(line, sizeof(line), "PGI:src=CP action=AVAILABLE reason=TBT_ON");
        native_log(line);
    }
}

static void hook_app_usb_connection(void *self, int connected, int auxiliary) {
    g_cp_usb_connected = connected ? 1 : 0;
    __sync_synchronize();
    if (!connected)
        writer_release_source(WRITER_SOURCE_CP, CLEAR_DISCONNECTED);
    else {
        writer_input_mutated();
        if (g_cp_tbt == 1)
            writer_set_pending_source(WRITER_SOURCE_CP, 1);
    }
    g_app_original_usb_connection(self, connected, auxiliary);
    if (!connected) core_clear_cp(CLEAR_DISCONNECTED);
}

static int restore_hook(struct hook_record *record) {
    uptr page;
    if (!record->installed) return 0;
    page = ((uptr)record->target) & ~(uptr)4095u;
    if (mprotect((void *)page, 4096u, PROT_READ | PROT_WRITE | PROT_EXEC))
        return -1;
    memcpy(record->target, record->original, record->stolen);
    __sync_synchronize();
    if (memcmp(record->target, record->original, record->stolen)) return -2;
    record->installed = 0;
    return mprotect((void *)page, 4096u, PROT_READ | PROT_EXEC) ? -3 : 0;
}

static int start_thread_using(void *(*entry)(void *),
                              thread_create_fn create_function,
                              thread_detach_fn detach_function) {
    unsigned long thread;
    if (create_function(&thread, 0, entry, 0)) return -1;
    (void)detach_function(thread);
    return 0;
}

static int start_thread(void *(*entry)(void *)) {
    return start_thread_using(entry, pthread_create, pthread_detach);
}

static void init_iap_role(void) {
    if (memcmp(IAP_RAW_SEND_ADDRESS, k_iap_raw_send_prologue,
               sizeof(k_iap_raw_send_prologue))) return;
    if (prepare_hook(&g_iap_send_hook, IAP_SEND_ADDRESS, (void *)hook_iap_send,
                     k_iap_send_prologue, sizeof(k_iap_send_prologue))) return;
    g_iap_original_send = (iap_send_fn)g_iap_send_hook.trampoline;
    if (prepare_hook(&g_iap_dispatch_hook, IAP_DISPATCH_ADDRESS,
                     (void *)hook_iap_dispatch, k_iap_dispatch_prologue,
                     sizeof(k_iap_dispatch_prologue))) {
        discard_hook(&g_iap_send_hook); return;
    }
    g_iap_original_dispatch = (iap_dispatch_fn)g_iap_dispatch_hook.trampoline;
    if (commit_hook(&g_iap_send_hook) ||
            commit_hook(&g_iap_dispatch_hook)) {
        restore_hook(&g_iap_dispatch_hook);
        restore_hook(&g_iap_send_hook);
        return;
    }
    __sync_synchronize();
    g_iap_ready = 1;
    if (start_thread(iap_worker)) {
        g_iap_ready = 0;
        __sync_synchronize();
        restore_hook(&g_iap_dispatch_hook);
        restore_hook(&g_iap_send_hook);
    }
}

static void discard_app_hook_if_safe(struct hook_record *record) {
    if (!record->installed) discard_hook(record);
}

static void discard_app_hooks(int keep_qcan) {
    discard_app_hook_if_safe(&g_app_aoap_connection_hook);
    discard_app_hook_if_safe(&g_app_aa_focus_hook);
    discard_app_hook_if_safe(&g_app_owner_event_hook);
    discard_app_hook_if_safe(&g_app_notifier_dtor_hook);
    discard_app_hook_if_safe(&g_app_notifier_ctor_hook);
    discard_app_hook_if_safe(&g_app_usb_connection_hook);
    discard_app_hook_if_safe(&g_app_aa_next_hook);
    discard_app_hook_if_safe(&g_app_aa_distance_hook);
    discard_app_hook_if_safe(&g_app_aa_status_hook);
    discard_app_hook_if_safe(&g_app_tbt_hook);
    if (!keep_qcan) discard_app_hook_if_safe(&g_app_qcan_raw_hook);
}

static int restore_app_hooks(int keep_qcan) {
    int result = 0;
    result |= restore_hook(&g_app_aoap_connection_hook);
    result |= restore_hook(&g_app_aa_focus_hook);
    result |= restore_hook(&g_app_owner_event_hook);
    result |= restore_hook(&g_app_notifier_dtor_hook);
    result |= restore_hook(&g_app_notifier_ctor_hook);
    result |= restore_hook(&g_app_usb_connection_hook);
    result |= restore_hook(&g_app_aa_next_hook);
    result |= restore_hook(&g_app_aa_distance_hook);
    result |= restore_hook(&g_app_aa_status_hook);
    result |= restore_hook(&g_app_tbt_hook);
    if (!keep_qcan) result |= restore_hook(&g_app_qcan_raw_hook);
    return result;
}

static void rollback_app_hooks(int keep_qcan) {
    int failed = restore_app_hooks(keep_qcan);
    discard_app_hooks(keep_qcan);
    if (failed)
        syslog(6, "%s", "PGI:writer state=DISABLED reason=ROLLBACK_RETAINED");
}

static int prepare_app_hooks(void) {
    if (!qcan_send_raw_export ||
            prepare_hook(&g_app_qcan_raw_hook, (void *)qcan_send_raw_export,
                         (void *)hook_app_qcan_raw, k_app_qcan_raw_prologue,
                         sizeof(k_app_qcan_raw_prologue))) return -1;
    g_app_original_qcan_raw =
        (app_qcan_raw_fn)g_app_qcan_raw_hook.trampoline;
#define PREPARE_APP(record, address, replacement, expected, original, type) \
    do { \
        if (prepare_hook(&(record), (address), (void *)(replacement), \
                         (expected), sizeof(expected))) return -1; \
        (original) = (type)(record).trampoline; \
    } while (0)
    PREPARE_APP(g_app_tbt_hook, APP_TBT_ADDRESS, hook_app_tbt,
                k_app_tbt_prologue, g_app_original_tbt, app_tbt_fn);
    PREPARE_APP(g_app_usb_connection_hook, APP_USB_CONNECTION_ADDRESS,
                hook_app_usb_connection, k_app_usb_connection_prologue,
                g_app_original_usb_connection, app_usb_connection_fn);
    PREPARE_APP(g_app_aa_status_hook, APP_AA_STATUS_ADDRESS,
                hook_app_aa_status, k_app_aa_status_prologue,
                g_app_original_aa_status, app_aa_status_fn);
    PREPARE_APP(g_app_aa_distance_hook, APP_AA_DISTANCE_ADDRESS,
                hook_app_aa_distance, k_app_aa_guidance_prologue,
                g_app_original_aa_distance, app_aa_distance_fn);
    PREPARE_APP(g_app_aa_next_hook, APP_AA_NEXT_ADDRESS,
                hook_app_aa_next, k_app_aa_guidance_prologue,
                g_app_original_aa_next, app_aa_next_fn);
    PREPARE_APP(g_app_notifier_ctor_hook, APP_NOTIFIER_CTOR_ADDRESS,
                hook_app_notifier_ctor, k_app_notifier_ctor_prologue,
                g_app_original_notifier_ctor, app_notifier_ctor_fn);
    PREPARE_APP(g_app_notifier_dtor_hook, APP_NOTIFIER_DTOR_ADDRESS,
                hook_app_notifier_dtor, k_app_notifier_dtor_prologue,
                g_app_original_notifier_dtor, app_notifier_dtor_fn);
    PREPARE_APP(g_app_owner_event_hook, APP_OWNER_EVENT_ADDRESS,
                hook_app_owner_event, k_app_owner_event_prologue,
                g_app_original_owner_event, app_owner_event_fn);
    PREPARE_APP(g_app_aa_focus_hook, APP_AA_FOCUS_ADDRESS,
                hook_app_aa_focus, k_app_aa_focus_prologue,
                g_app_original_aa_focus, app_aa_focus_fn);
    PREPARE_APP(g_app_aoap_connection_hook, APP_AOAP_CONNECTION_ADDRESS,
                hook_app_aoap_connection, k_app_aoap_connection_prologue,
                g_app_original_aoap_connection, app_aoap_connection_fn);
#undef PREPARE_APP
    return 0;
}

static int commit_app_hooks(void) {
    if (commit_hook(&g_app_notifier_ctor_hook) ||
            commit_hook(&g_app_notifier_dtor_hook) ||
            commit_hook(&g_app_owner_event_hook) ||
            commit_hook(&g_app_aa_focus_hook) ||
            commit_hook(&g_app_aoap_connection_hook) ||
            commit_hook(&g_app_aa_status_hook) ||
            commit_hook(&g_app_aa_distance_hook) ||
            commit_hook(&g_app_aa_next_hook) ||
            commit_hook(&g_app_tbt_hook) ||
            commit_hook(&g_app_usb_connection_hook)) return -1;
    return 0;
}

static int writer_wake_init(void) {
    if (socketpair(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0, g_writer_wake) &&
            socketpair(AF_UNIX, SOCK_DGRAM, 0, g_writer_wake)) return -1;
    (void)fcntl(g_writer_wake[0], F_SETFD, FD_CLOEXEC);
    (void)fcntl(g_writer_wake[1], F_SETFD, FD_CLOEXEC);
    (void)fcntl(g_writer_wake[0], F_SETFL, O_NONBLOCK);
    (void)fcntl(g_writer_wake[1], F_SETFL, O_NONBLOCK);
    return 0;
}

static void writer_wake_close(void) {
    if (g_writer_wake[0] >= 0) (void)close(g_writer_wake[0]);
    if (g_writer_wake[1] >= 0) (void)close(g_writer_wake[1]);
    g_writer_wake[0] = g_writer_wake[1] = -1;
}

static int writer_lease_wake_init(void) {
    if (socketpair(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0, g_lease_wake) &&
            socketpair(AF_UNIX, SOCK_DGRAM, 0, g_lease_wake)) return -1;
    (void)fcntl(g_lease_wake[0], F_SETFD, FD_CLOEXEC);
    (void)fcntl(g_lease_wake[1], F_SETFD, FD_CLOEXEC);
    (void)fcntl(g_lease_wake[0], F_SETFL, O_NONBLOCK);
    (void)fcntl(g_lease_wake[1], F_SETFL, O_NONBLOCK);
    return 0;
}

static void writer_lease_wake_close(void) {
    if (g_lease_wake[0] >= 0) (void)close(g_lease_wake[0]);
    if (g_lease_wake[1] >= 0) (void)close(g_lease_wake[1]);
    g_lease_wake[0] = g_lease_wake[1] = -1;
}

static void writer_note_server_thread_failure(void) {
    syslog(6, "%s", "PGI:server state=DISABLED reason=THREAD");
}

static void discard_navi_hook_if_safe(struct hook_record *record) {
    if (!record->installed) discard_hook(record);
}

static void discard_navi_hooks(void) {
    discard_navi_hook_if_safe(&g_navi_can_send_hook);
    discard_navi_hook_if_safe(&g_navi_sender_dtor_hook);
    discard_navi_hook_if_safe(&g_navi_owner_event_hook);
}

static int restore_navi_hooks(void) {
    int result = 0;
    result |= restore_hook(&g_navi_can_send_hook);
    result |= restore_hook(&g_navi_sender_dtor_hook);
    result |= restore_hook(&g_navi_owner_event_hook);
    return result;
}

static void rollback_navi_hooks(void) {
    int failed;
    g_navi_ready = 0;
    __sync_synchronize();
    failed = restore_navi_hooks();
    /* A hook may already be executing; retain all trampolines after commit. */
    if (failed)
        syslog(6, "%s", "PGI:navi-gate state=DISABLED reason=ROLLBACK_RETAINED");
}

static int prepare_navi_hooks(void) {
    if (prepare_hook(&g_navi_owner_event_hook, NAVI_OWNER_EVENT_ADDRESS,
                     (void *)hook_navi_owner_event,
                     k_navi_owner_event_prologue,
                     sizeof(k_navi_owner_event_prologue))) return -1;
    g_navi_original_owner_event =
        (navi_owner_event_fn)g_navi_owner_event_hook.trampoline;
    if (prepare_hook(&g_navi_sender_dtor_hook, NAVI_SENDER_DTOR_ADDRESS,
                     (void *)hook_navi_sender_dtor,
                     k_navi_sender_dtor_prologue,
                     sizeof(k_navi_sender_dtor_prologue))) return -1;
    g_navi_original_sender_dtor =
        (navi_sender_dtor_fn)g_navi_sender_dtor_hook.trampoline;
    if (prepare_hook(&g_navi_can_send_hook, NAVI_CAN_SEND_ADDRESS,
                     (void *)hook_navi_can_send, k_navi_can_send_prologue,
                     sizeof(k_navi_can_send_prologue))) return -1;
    g_navi_original_can_send =
        (navi_can_send_fn)g_navi_can_send_hook.trampoline;
    return 0;
}

static int commit_navi_hooks(void) {
    if (commit_hook(&g_navi_sender_dtor_hook) ||
            commit_hook(&g_navi_can_send_hook) ||
            commit_hook(&g_navi_owner_event_hook)) return -1;
    return 0;
}

static void init_navi_role(void) {
    if (memcmp(NAVI_BUILD_ID_ADDRESS, k_navi_build_id,
               sizeof(k_navi_build_id))) {
        syslog(6, "%s", "PGI:navi-gate state=DISABLED reason=BUILD_ID");
        return;
    }
    g_navi_server_fd = make_named_server_socket(k_lease_socket_name);
    if (g_navi_server_fd < 0) {
        syslog(6, "%s", "PGI:navi-gate state=DISABLED reason=LEASE_BIND");
        return;
    }
    if (prepare_navi_hooks()) {
        syslog(6, "%s", "PGI:navi-gate state=DISABLED reason=HOOK_ABI");
        discard_navi_hooks();
        (void)close(g_navi_server_fd);
        g_navi_server_fd = -1;
        return;
    }
    if (commit_navi_hooks()) {
        syslog(6, "%s", "PGI:navi-gate state=DISABLED reason=HOOK_COMMIT");
        rollback_navi_hooks();
        (void)close(g_navi_server_fd);
        g_navi_server_fd = -1;
        return;
    }
    if (start_thread(navi_lease_server_worker)) {
        syslog(6, "%s", "PGI:navi-gate state=DISABLED reason=LEASE_THREAD");
        rollback_navi_hooks();
        (void)close(g_navi_server_fd);
        g_navi_server_fd = -1;
        return;
    }
    __sync_synchronize();
    g_navi_ready = 1;
    native_log("PGI:navi-gate state=READY scope=123,115,506");
}

static void init_app_role(void) {
    if (prepare_app_hooks()) {
        syslog(6, "%s", "PGI:writer state=DISABLED reason=HOOK_ABI");
        discard_app_hooks(0);
        return;
    }
    if (writer_wake_init()) {
        syslog(6, "%s", "PGI:writer state=DISABLED reason=TRANSPORT_INIT");
        discard_app_hooks(0);
        writer_wake_close();
        return;
    }
    if (commit_hook(&g_app_qcan_raw_hook)) {
        syslog(6, "%s", "PGI:writer state=DISABLED reason=QCAN_GUARD");
        rollback_app_hooks(0);
        writer_wake_close();
        return;
    }
    if (register_qcan_metatype()) {
        int keep_qcan = g_qcan_metatype_irreversible;
        syslog(6, "%s", "PGI:writer state=DISABLED reason=METATYPE");
        rollback_app_hooks(keep_qcan);
        writer_wake_close();
        return;
    }
    if (commit_app_hooks()) {
        syslog(6, "%s", "PGI:writer state=DISABLED reason=HOOK_COMMIT");
        rollback_app_hooks(1);
        writer_wake_close();
        return;
    }
    blocking_lock(&g_writer.lock);
    g_writer.transport_ready = 1u;
    unlock(&g_writer.lock);
    if (start_thread(writer_worker)) {
        syslog(6, "%s", "PGI:writer state=DISABLED reason=WRITER_THREAD");
        blocking_lock(&g_writer.lock);
        g_writer.transport_ready = 0u;
        unlock(&g_writer.lock);
        rollback_app_hooks(1);
        writer_wake_close();
        return;
    }
    if (writer_lease_wake_init() || start_thread(writer_lease_worker)) {
        syslog(6, "%s", "PGI:lease state=DISABLED reason=THREAD");
        writer_lease_wake_close();
    }
    if (start_thread(app_server_worker)) {
        writer_note_server_thread_failure();
        return;
    }
    __sync_synchronize();
}

CONSTRUCTOR static void projected_guidance_init(void) {
    if (exact_process("/app/bin/iAP2Service")) init_iap_role();
    else if (exact_process("/app/bin/AppProjection")) init_app_role();
    else if (exact_process("/navi/Bin/AppNavi")) init_navi_role();
}

EXPORT const char *pg_build_id(void) { return BUILD_ID; }
