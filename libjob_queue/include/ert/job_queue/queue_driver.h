/*
   Copyright (C) 2011  Statoil ASA, Norway.

   The file 'queue_driver.h' is part of ERT - Ensemble based Reservoir Tool.

   ERT is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ERT is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.

   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
   for more details.
 */

#ifndef ERT_QUEUE_DRIVER_H
#define ERT_QUEUE_DRIVER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <ert/util/hash.h>

  typedef enum {
    NULL_DRIVER = 0,
    LSF_DRIVER = 1,
    LOCAL_DRIVER = 2,
    RSH_DRIVER = 3,
    TORQUE_DRIVER = 4
  } job_driver_type;

#define JOB_DRIVER_ENUM_DEFS                                    \
{.value = 0 , .name = "NULL_DRIVER"},                           \
{.value = 1 , .name = "LSF_DRIVER"},                            \
{.value = 2 , .name = "LOCAL_DRIVER"},                          \
{.value = 3 , .name = "RSH_DRIVER"},                            \
{.value = 4 , .name = "TORQUE_DRIVER"}

#define JOB_DRIVER_ENUM_SIZE 5

  /*
    The options supported by the base queue_driver.
   */
#define MAX_RUNNING          "MAX_RUNNING"

/*
                                                                   +---------------------------------+
                                                                   |                                 |
+---------------------------------+                                |  JOB_QUEUE_WAITING              <----------------------------------+
|                                 |                                |                                 <--------------+                   |
|   JOB_QUEUE_NOT_ACTIVE          |                                +---------------+-----------------+              |                   |
|                                 |                                                |                                |                   |
+---------------------------------+                                                |                                |                   |
                                                                                   |                                |                   |
                                                                   +---------------v-----------------+              |                   |
                                                                   |                                 |              |                   |
+---------------------------------+                                |  JOB_QUEUE_SUBMITTED            |              |                   |
|                                 |                                |                                 |              |                   |
|JOB_QUEUE_STATUS_FAILURE         |                                +-------+--------------------+----+              |                   |
|                                 |                                        |                    |                   |                   |
+---------------------------------+                                        |                    |  +----------------+----------------+  |
                                                 +-------------------------v-------+            |  |                                 |  |
                                                 |                                 |            |  | JOB_QUEUE_DO_KILL_NODE_FAILURE  |  |
                                                 | JOB_QUEUE_PENDING               |            |  |                                 |  |
                                                 |                                 |            |  +---------------------^-----------+  |
                                                 +-------------------------+-------+            |                        |              |
                                                                           |                    |                        |              |
                                                                           |    +---------------v-----------------+      |              |
                                                                           |    |                                 |      |              |
                                                                           +----> JOB_QUEUE_RUNNING               +------+              |
                     +----------------------------------------------------------+                                 |                     |
                     |                                                          +---+-------------------+---------+                     |
                     |                                                              |                   |                               |
                     |                                                              |                   |                               |
      +--------------v------------------+    +---------------------------------+    |         +---------v-----------------------+       |
      |                                 |    |                                 |    |         |                                 |       |
      | JOB_QUEUE_DO_KILL               |    | JOB_QUEUE_DONE                  +<---+     +--->  JOB_QUEUE_EXIT                 |       |
      |                                 |    |                                 |          |   |                                 |       |
      +--------------+------------------+    +----------------+----------------+          |   +-----------------+---------------+       |
                     |                                        |                           |                     |                       |
                     |                                        |                           |                     |                       |
                     |                                        |                           |                     |                       |
                     |                       +----------------v----------------+          |   +-----------------v---------------+       |
                     |                       |                                 |          |   |                                 |       |
                     |                       |JOB_QUEUE_RUNNING_DONE_CALLBACK  +----------+   | JOB_QUEUE_RUNNING_EXIT_CALLBACK +-------+
                     |                       |                                 |              |                                 |
                     |                       +----------------+----------------+              +----------------+----------------+
                     |                                        |                                                |
                     |                                        |                                                |
                     |                                        |                                                |
      +--------------v------------------+    +----------------v----------------+              +----------------v----------------+
      |                                 |    |                                 |              |                                 |
      | JOB_QUEUE_IS_KILLED             |    | JOB_QUEUE_SUCCESS               |              |  JOB_QUEUE_FAILED               |
      |                                 |    |                                 |              |                                 |
      +---------------------------------+    +---------------------------------+              +---------------------------------+

*/



/*
  NB: the status count algorithm has a HARD assumption that these
  values are on the 2^N form - without holes in the series.
*/

typedef enum {
    JOB_QUEUE_NOT_ACTIVE            =     1, /* This value is used in external query routines - for jobs which are (currently) not active. */
    JOB_QUEUE_WAITING               =     2, /* A node which is waiting in the internal queue. */
    JOB_QUEUE_SUBMITTED             =     4, /* Internal status: It has has been submitted - the next status update will (should) place it as pending or running. */
    JOB_QUEUE_PENDING               =     8, /* A node which is pending - a status returned by the external system. I.e LSF */
    JOB_QUEUE_RUNNING               =    16, /* The job is running */
    JOB_QUEUE_DONE                  =    32, /* The job is done - but we have not yet checked if the target file is produced */
    JOB_QUEUE_EXIT                  =    64, /* The job has exited - check attempts to determine if we retry or go to complete_fail   */
    JOB_QUEUE_IS_KILLED             =   128, /* The job has been killed, following a  JOB_QUEUE_DO_KILL*/
    JOB_QUEUE_DO_KILL               =   256, /* The the job should be killed, either due to user request, or automated measures - the job can NOT be restarted. */
    JOB_QUEUE_SUCCESS               =   512,
    JOB_QUEUE_RUNNING_DONE_CALLBACK =  1024,
    JOB_QUEUE_RUNNING_EXIT_CALLBACK =  2048,
    JOB_QUEUE_STATUS_FAILURE        =  4096,
    JOB_QUEUE_FAILED                =  8192,
    JOB_QUEUE_DO_KILL_NODE_FAILURE  = 16384
  } job_status_type;

#define JOB_QUEUE_RUNNING_CALLBACK (JOB_QUEUE_RUNNING_DONE_CALLBACK + JOB_QUEUE_RUNNING_EXIT_CALLBACK)

#define JOB_QUEUE_STATUS_ALL (JOB_QUEUE_NOT_ACTIVE + JOB_QUEUE_WAITING + JOB_QUEUE_SUBMITTED + JOB_QUEUE_PENDING + JOB_QUEUE_RUNNING + JOB_QUEUE_DONE + \
                              JOB_QUEUE_EXIT + JOB_QUEUE_IS_KILLED + JOB_QUEUE_DO_KILL + JOB_QUEUE_SUCCESS + JOB_QUEUE_RUNNING_CALLBACK + \
                              JOB_QUEUE_STATUS_FAILURE + JOB_QUEUE_FAILED + JOB_QUEUE_DO_KILL_NODE_FAILURE)

#define JOB_QUEUE_MAX_STATE  15

  /*
    All jobs which are in the status set defined by
    JOB_QUEUE_CAN_RESTART can be restarted based on external
    user-input. It is OK to try to restart a job which is not in this
    state - basically nothing should happen.
   */
#define JOB_QUEUE_CAN_RESTART  (JOB_QUEUE_FAILED + JOB_QUEUE_IS_KILLED  +  JOB_QUEUE_SUCCESS)


  /*
    These are the jobs which can be killed. It is OK to try to kill a
    job which is not in this state, the only thing happening is that the
    function job_queue_kill_simulation() wil return false.
   */
#define JOB_QUEUE_CAN_KILL    (JOB_QUEUE_WAITING + JOB_QUEUE_RUNNING + JOB_QUEUE_PENDING + JOB_QUEUE_SUBMITTED + JOB_QUEUE_DO_KILL + JOB_QUEUE_DO_KILL_NODE_FAILURE)

#define JOB_QUEUE_WAITING_STATUS (JOB_QUEUE_WAITING + JOB_QUEUE_PENDING)

#define JOB_QUEUE_CAN_UPDATE_STATUS (JOB_QUEUE_RUNNING + JOB_QUEUE_PENDING + JOB_QUEUE_SUBMITTED)

#define JOB_QUEUE_COMPLETE_STATUS (JOB_QUEUE_IS_KILLED + JOB_QUEUE_SUCCESS + JOB_QUEUE_FAILED)


  typedef struct queue_driver_struct queue_driver_type;

  typedef void * (submit_job_ftype) (void * data, const char * cmd, int num_cpu, const char * run_path, const char * job_name, int argc, const char ** argv);
  typedef void (blacklist_node_ftype) (void *, void *);
  typedef void (kill_job_ftype) (void *, void *);
  typedef job_status_type(get_status_ftype) (void *, void *);
  typedef void (free_job_ftype) (void *);
  typedef void (free_queue_driver_ftype) (void *);
  typedef bool (set_option_ftype) (void *, const char*, const void *);
  typedef const void * (get_option_ftype) (const void *, const char *);
  typedef bool (has_option_ftype) (const void *, const char *);
  typedef void (init_option_list_ftype) (stringlist_type *);


  queue_driver_type * queue_driver_alloc_RSH(const char * rsh_cmd, const hash_type * rsh_hostlist);
  queue_driver_type * queue_driver_alloc_LSF(const char * queue_name, const char * resource_request, const char * remote_lsf_server);
  queue_driver_type * queue_driver_alloc_TORQUE();
  queue_driver_type * queue_driver_alloc_local();
  queue_driver_type * queue_driver_alloc(job_driver_type type);

  void * queue_driver_submit_job(queue_driver_type * driver, const char * run_cmd, int num_cpu, const char * run_path, const char * job_name, int argc, const char ** argv);
  void queue_driver_free_job(queue_driver_type * driver, void * job_data);
  void queue_driver_blacklist_node(queue_driver_type * driver, void * job_data);
  void queue_driver_kill_job(queue_driver_type * driver, void * job_data);
  job_status_type queue_driver_get_status(queue_driver_type * driver, void * job_data);

  const char * queue_driver_get_name(const queue_driver_type * driver);

  bool queue_driver_set_option(queue_driver_type * driver, const char * option_key, const void * value);
  bool queue_driver_unset_option(queue_driver_type * driver, const char * option_key);
  const void * queue_driver_get_option(queue_driver_type * driver, const char * option_key);
  void queue_driver_init_option_list(queue_driver_type * driver, stringlist_type * option_list);

  void queue_driver_free(queue_driver_type * driver);
  void queue_driver_free__(void * driver);
  const char * queue_driver_type_enum_iget(int index, int * value);

  typedef enum {SUBMIT_OK           = 0 ,
                SUBMIT_JOB_FAIL     = 1 , /* Typically no more attempts. */
                SUBMIT_DRIVER_FAIL  = 2 , /* The driver would not take the job - for whatever reason?? */
                SUBMIT_QUEUE_CLOSED = 3 } /* The queue is currently not accepting more jobs - either (temporarilty)
                                             because of pause or it is going down. */   submit_status_type;

  UTIL_IS_INSTANCE_HEADER( queue_driver );


#ifdef __cplusplus
}
#endif
#endif
