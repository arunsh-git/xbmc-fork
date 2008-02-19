#pragma once
#include "../DynamicDll.h"

extern "C" {
#include "../lib/libcmyth/cmyth.h"
#include "../lib/libcmyth/mvp_refmem.h"
}

class DllLibCMythInterface
{
public:
  virtual ~DllLibCMythInterface() {}
  virtual cmyth_conn_t     conn_connect_ctrl        (char *server, unsigned short port, unsigned buflen, int tcp_rcvbuf)=0;
  virtual cmyth_conn_t     conn_connect_event       (char *server, unsigned short port, unsigned buflen, int tcp_rcvbuf)=0;
  virtual cmyth_file_t     conn_connect_file        (cmyth_proginfo_t prog, cmyth_conn_t control, unsigned buflen, int tcp_rcvbuf)=0;
  virtual cmyth_recorder_t conn_get_free_recorder   (cmyth_conn_t conn)=0;

  virtual cmyth_event_t    event_get                (cmyth_conn_t conn, char * data, int len)=0;
  virtual int              event_select             (cmyth_conn_t conn, struct timeval *timeout)=0;

  virtual cmyth_proglist_t proglist_get_all_recorded(cmyth_conn_t control)=0;
  virtual cmyth_proginfo_t proglist_get_item        (cmyth_proglist_t pl, int index)=0;
  virtual int              proglist_get_count       (cmyth_proglist_t pl)=0;
  virtual cmyth_channel_t  chanlist_get_item        (cmyth_chanlist_t pl, int index)=0;
  virtual int              chanlist_get_count       (cmyth_chanlist_t pl)=0;

  virtual int              channel_channum          (cmyth_channel_t chan)=0;
  virtual char*            channel_name             (cmyth_channel_t chan)=0;
  virtual char*            channel_icon             (cmyth_channel_t chan)=0;

  virtual cmyth_recorder_t recorder_is_recording    (cmyth_recorder_t conn)=0;
  virtual int              recorder_spawn_livetv    (cmyth_recorder_t rec)=0;
  virtual char*            recorder_get_filename    (cmyth_recorder_t rec)=0;
  virtual cmyth_proginfo_t recorder_get_cur_proginfo(cmyth_recorder_t rec)=0;
  virtual int              recorder_change_channel  (cmyth_recorder_t rec, cmyth_channeldir_t direction)=0;
  virtual int              recorder_pause           (cmyth_recorder_t rec)=0;
  virtual int              recorder_stop_livetv     (cmyth_recorder_t rec)=0;
  virtual int              recorder_set_channel     (cmyth_recorder_t rec, char *channame)=0;

  virtual int              livetv_get_block         (cmyth_recorder_t rec, char *buf, unsigned long len)=0;
  virtual int              livetv_select            (cmyth_recorder_t rec, struct timeval *timeout)=0;
  virtual int              livetv_request_block     (cmyth_recorder_t rec, unsigned long len)=0;
  virtual long long        livetv_seek              (cmyth_recorder_t rec, long long offset, int whence)=0;
  virtual int              livetv_chain_update      (cmyth_recorder_t rec, char * chainid, int tcp_rcvbuf)=0;
  virtual cmyth_recorder_t spawn_live_tv            (cmyth_recorder_t rec, unsigned buflen, int tcp_rcvbuf,
                                                     void (*prog_update_callback)(cmyth_proginfo_t),
                                                     char ** err)=0;

  virtual int                file_get_block         (cmyth_file_t rec, char *buf, unsigned long len)=0;
  virtual int                file_select            (cmyth_file_t rec, struct timeval *timeout)=0;
  virtual int                file_request_block     (cmyth_file_t rec, unsigned long len)=0;
  virtual long long          file_seek              (cmyth_file_t rec, long long offset, int whence)=0;
  virtual unsigned long long file_length            (cmyth_file_t file)=0;
  virtual unsigned long long file_start             (cmyth_file_t file)=0;

  virtual char*             proginfo_pathname       (cmyth_proginfo_t prog)=0;
  virtual char*             proginfo_title          (cmyth_proginfo_t prog)=0;  
  virtual char*             proginfo_description    (cmyth_proginfo_t prog)=0;
  virtual char*             proginfo_chanstr        (cmyth_proginfo_t prog)=0;
  virtual char*             proginfo_channame       (cmyth_proginfo_t prog)=0;
  virtual char*             proginfo_recgroup       (cmyth_proginfo_t prog)=0;
  virtual long long         proginfo_length         (cmyth_proginfo_t prog)=0;
  virtual int               proginfo_length_sec     (cmyth_proginfo_t prog)=0;
  virtual cmyth_timestamp_t proginfo_start          (cmyth_proginfo_t prog)=0;
  virtual cmyth_timestamp_t proginfo_end            (cmyth_proginfo_t prog)=0;
  virtual cmyth_timestamp_t proginfo_rec_start      (cmyth_proginfo_t prog)=0;
  virtual cmyth_timestamp_t proginfo_rec_end        (cmyth_proginfo_t prog)=0;
  virtual cmyth_proginfo_rec_status_t proginfo_rec_status(cmyth_proginfo_t prog)=0;
  virtual cmyth_proginfo_t  proginfo_get_from_basename   (cmyth_conn_t control, const char* basename)=0;

  virtual void             ref_release              (void* ptr)=0;
  virtual void*            ref_hold                 (void* ptr)=0;
  virtual void             dbg_level                (int l)=0;

  virtual time_t           timestamp_to_unixtime    (cmyth_timestamp_t ts)=0;
  virtual cmyth_database_t database_init            (char *host, char *db_name, char *user, char *pass)=0;
  virtual cmyth_chanlist_t mysql_get_chanlist       (cmyth_database_t db)=0;
};

class DllLibCMyth : public DllDynamic, DllLibCMythInterface
{
#ifdef _LINUX
  DECLARE_DLL_WRAPPER(DllLibCMyth, xbmc.so)
#elif __APPLE__
  DECLARE_DLL_WRAPPER(DllLibCMyth, xbmc.so)
#else
  DECLARE_DLL_WRAPPER(DllLibCMyth, Q:\\system\\libcmyth.dll)
#endif
  DEFINE_METHOD4(cmyth_conn_t,        conn_connect_ctrl,        (char *p1, unsigned short p2, unsigned p3, int p4))
  DEFINE_METHOD4(cmyth_conn_t,        conn_connect_event,       (char *p1, unsigned short p2, unsigned p3, int p4))
  DEFINE_METHOD4(cmyth_file_t,        conn_connect_file,        (cmyth_proginfo_t p1, cmyth_conn_t p2, unsigned p3, int p4))
  DEFINE_METHOD1(cmyth_recorder_t,    conn_get_free_recorder,   (cmyth_conn_t p1))

  DEFINE_METHOD3(cmyth_event_t,       event_get,                (cmyth_conn_t p1, char * p2, int p3))
  DEFINE_METHOD2(int,                 event_select,             (cmyth_conn_t p1, struct timeval *p2))

  DEFINE_METHOD1(cmyth_proglist_t,    proglist_get_all_recorded, (cmyth_conn_t p1))
  DEFINE_METHOD2(cmyth_proginfo_t,    proglist_get_item,        (cmyth_proglist_t p1, int p2))
  DEFINE_METHOD1(int,                 proglist_get_count,       (cmyth_proglist_t p1))
  DEFINE_METHOD2(cmyth_channel_t,     chanlist_get_item,        (cmyth_chanlist_t p1, int p2))
  DEFINE_METHOD1(int,                 chanlist_get_count,       (cmyth_chanlist_t p1))
  DEFINE_METHOD1(int,                 channel_channum,          (cmyth_channel_t p1))
  DEFINE_METHOD1(char*,               channel_name,             (cmyth_channel_t p1))
  DEFINE_METHOD1(char*,               channel_icon,             (cmyth_channel_t p1))

  DEFINE_METHOD1(cmyth_recorder_t,    recorder_is_recording,    (cmyth_recorder_t p1))
  DEFINE_METHOD1(int,                 recorder_spawn_livetv,    (cmyth_recorder_t p1))
  DEFINE_METHOD1(char*,               recorder_get_filename,    (cmyth_recorder_t p1))
  DEFINE_METHOD1(cmyth_proginfo_t,    recorder_get_cur_proginfo, (cmyth_recorder_t p1))
  DEFINE_METHOD2(int,                 recorder_change_channel,  (cmyth_recorder_t p1, cmyth_channeldir_t p2))
  DEFINE_METHOD1(int,                 recorder_pause,           (cmyth_recorder_t p1))
  DEFINE_METHOD1(int,                 recorder_stop_livetv,     (cmyth_recorder_t p1))
  DEFINE_METHOD2(int,                 recorder_set_channel,     (cmyth_recorder_t p1, char * p2))

  DEFINE_METHOD3(int,                 livetv_get_block,         (cmyth_recorder_t p1, char *p2, unsigned long p3))
  DEFINE_METHOD2(int,                 livetv_select,            (cmyth_recorder_t p1, struct timeval *p2))
  DEFINE_METHOD2(int,                 livetv_request_block,     (cmyth_recorder_t p1, unsigned long p2))
  DEFINE_METHOD3(long long,           livetv_seek,              (cmyth_recorder_t p1, long long p2, int p3))
  DEFINE_METHOD3(int,                 livetv_chain_update,      (cmyth_recorder_t p1, char * p2, int p3))
  DEFINE_METHOD5(cmyth_recorder_t,    spawn_live_tv,            (cmyth_recorder_t p1, unsigned p2, int p3, void (*p4)(cmyth_proginfo_t), char ** p5))

  DEFINE_METHOD3(int,                 file_get_block,           (cmyth_file_t p1, char *p2, unsigned long p3))
  DEFINE_METHOD2(int,                 file_select,              (cmyth_file_t p1, struct timeval *p2))
  DEFINE_METHOD2(int,                 file_request_block,       (cmyth_file_t p1, unsigned long p2))
  DEFINE_METHOD3(long long,           file_seek,                (cmyth_file_t p1, long long p2, int p3))
  DEFINE_METHOD1(unsigned long long,  file_length,              (cmyth_file_t p1))
  DEFINE_METHOD1(unsigned long long,  file_start,               (cmyth_file_t p1))

  DEFINE_METHOD1(char*,               proginfo_pathname,        (cmyth_proginfo_t p1))
  DEFINE_METHOD1(char*,               proginfo_title,           (cmyth_proginfo_t p1))
  DEFINE_METHOD1(char*,               proginfo_description,     (cmyth_proginfo_t p1))
  DEFINE_METHOD1(char*,               proginfo_chanstr,         (cmyth_proginfo_t p1))
  DEFINE_METHOD1(char*,               proginfo_channame,        (cmyth_proginfo_t p1))
  DEFINE_METHOD1(char*,               proginfo_recgroup,        (cmyth_proginfo_t p1))
  DEFINE_METHOD1(long long,           proginfo_length,          (cmyth_proginfo_t p1))
  DEFINE_METHOD1(int,                 proginfo_length_sec,      (cmyth_proginfo_t p1))
  DEFINE_METHOD1(cmyth_timestamp_t,   proginfo_start,           (cmyth_proginfo_t p1))
  DEFINE_METHOD1(cmyth_timestamp_t,   proginfo_end,             (cmyth_proginfo_t p1))
  DEFINE_METHOD1(cmyth_timestamp_t,   proginfo_rec_start,       (cmyth_proginfo_t p1))
  DEFINE_METHOD1(cmyth_timestamp_t,   proginfo_rec_end,         (cmyth_proginfo_t p1))
  DEFINE_METHOD1(cmyth_proginfo_rec_status_t, proginfo_rec_status, (cmyth_proginfo_t p1))
  DEFINE_METHOD2(cmyth_proginfo_t,    proginfo_get_from_basename,    (cmyth_conn_t p1, const char* p2))

  DEFINE_METHOD1(void,                ref_release,              (void* p1))
  DEFINE_METHOD1(void*,               ref_hold,                 (void* p1))
  DEFINE_METHOD1(void,                dbg_level,                (int p1))

  DEFINE_METHOD1(time_t,              timestamp_to_unixtime,    (cmyth_timestamp_t p1))
  DEFINE_METHOD4(cmyth_database_t,    database_init,            (char *p1, char *p2, char *p3, char *p4))
  DEFINE_METHOD1(cmyth_chanlist_t,    mysql_get_chanlist,       (cmyth_database_t p1))
  BEGIN_METHOD_RESOLVE()
    RESOLVE_METHOD_RENAME(cmyth_conn_connect_ctrl, conn_connect_ctrl)
    RESOLVE_METHOD_RENAME(cmyth_conn_connect_event, conn_connect_event)
    RESOLVE_METHOD_RENAME(cmyth_conn_connect_file, conn_connect_file)
    RESOLVE_METHOD_RENAME(cmyth_conn_get_free_recorder, conn_get_free_recorder)

    RESOLVE_METHOD_RENAME(cmyth_event_get, event_get)
    RESOLVE_METHOD_RENAME(cmyth_event_select, event_select)
    RESOLVE_METHOD_RENAME(cmyth_proglist_get_all_recorded, proglist_get_all_recorded)
    RESOLVE_METHOD_RENAME(cmyth_proglist_get_item, proglist_get_item)
    RESOLVE_METHOD_RENAME(cmyth_proglist_get_count, proglist_get_count)
    RESOLVE_METHOD_RENAME(cmyth_chanlist_get_item, chanlist_get_item)
    RESOLVE_METHOD_RENAME(cmyth_chanlist_get_count, chanlist_get_count)
    RESOLVE_METHOD_RENAME(cmyth_channel_channum, channel_channum)
    RESOLVE_METHOD_RENAME(cmyth_channel_name, channel_name)
    RESOLVE_METHOD_RENAME(cmyth_channel_icon, channel_icon)

    RESOLVE_METHOD_RENAME(cmyth_recorder_is_recording, recorder_is_recording)
    RESOLVE_METHOD_RENAME(cmyth_recorder_spawn_livetv, recorder_spawn_livetv)
    RESOLVE_METHOD_RENAME(cmyth_recorder_get_filename, recorder_get_filename)
    RESOLVE_METHOD_RENAME(cmyth_recorder_get_cur_proginfo, recorder_get_cur_proginfo)
    RESOLVE_METHOD_RENAME(cmyth_recorder_change_channel, recorder_change_channel)
    RESOLVE_METHOD_RENAME(cmyth_recorder_pause, recorder_pause)
    RESOLVE_METHOD_RENAME(cmyth_recorder_stop_livetv, recorder_stop_livetv)
    RESOLVE_METHOD_RENAME(cmyth_recorder_set_channel, recorder_set_channel)


    RESOLVE_METHOD_RENAME(cmyth_livetv_get_block, livetv_get_block)
    RESOLVE_METHOD_RENAME(cmyth_livetv_select, livetv_select)
    RESOLVE_METHOD_RENAME(cmyth_livetv_request_block, livetv_request_block)
    RESOLVE_METHOD_RENAME(cmyth_livetv_seek, livetv_seek)
    RESOLVE_METHOD_RENAME(cmyth_livetv_chain_update, livetv_chain_update)
    RESOLVE_METHOD_RENAME(cmyth_spawn_live_tv, spawn_live_tv)

    RESOLVE_METHOD_RENAME(cmyth_file_get_block, file_get_block)
    RESOLVE_METHOD_RENAME(cmyth_file_select, file_select)
    RESOLVE_METHOD_RENAME(cmyth_file_request_block, file_request_block)
    RESOLVE_METHOD_RENAME(cmyth_file_seek, file_seek)
    RESOLVE_METHOD_RENAME(cmyth_file_length, file_length)
    RESOLVE_METHOD_RENAME(cmyth_file_start, file_start)

    RESOLVE_METHOD_RENAME(cmyth_proginfo_title, proginfo_title)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_description, proginfo_description)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_chanstr, proginfo_chanstr)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_channame, proginfo_channame)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_pathname, proginfo_pathname)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_length, proginfo_length)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_length_sec, proginfo_length_sec)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_start, proginfo_start)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_end, proginfo_end)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_rec_start, proginfo_rec_start)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_rec_end, proginfo_rec_end)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_rec_status, proginfo_rec_status)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_recgroup, proginfo_recgroup)
    RESOLVE_METHOD_RENAME(cmyth_proginfo_get_from_basename, proginfo_get_from_basename)


    RESOLVE_METHOD(ref_release)
    RESOLVE_METHOD(ref_hold)
    RESOLVE_METHOD_RENAME(cmyth_dbg_level, dbg_level)

    RESOLVE_METHOD_RENAME(cmyth_timestamp_to_unixtime, timestamp_to_unixtime)
    RESOLVE_METHOD_RENAME(cmyth_database_init, database_init)
    RESOLVE_METHOD_RENAME(cmyth_mysql_get_chanlist, mysql_get_chanlist)
  END_METHOD_RESOLVE()
};