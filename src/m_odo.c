#include "m_odo.h"

/**
 * @brief Initialize a posebuffer, clearing the poses' contents and setting
 * the discardable pose to `a`. Note that the poses are arranged into an
 * implicit ringbuffer structure.
 *
 * @param pb A pointer to the posebuffer to be initialized.
 */
void MOdo_InitPoseBuffer(posebuf_t* pb) {
    pb->old = &pb->a;
    pb->a = (pose_t){.ts = 0, .x = 0, .y = 0, .th = 0, .next = &pb->b};
    pb->b = (pose_t){.ts = 0, .x = 0, .y = 0, .th = 0, .next = &pb->a};
}

/**
 * @brief Handler to be called when a `SIGXCPU` signal is generated by the
 * scheduler when missing a deadline and `SCHED_FLAG_DL_OVERRUN` is on.
 *
 * @param sig The number of the received signal.
 */
void MOdo_DLMissHandler(int sig) {
    assert(sig != 0);  // Ensure signature is not null (I admit
    // doing this only to remove a compile-time warning)
#if CONFIG_ENABLE_LOGGING
    syslog(LOG_WARNING, "%s", "(M_ODO) Deadline miss detected");
#endif  // CONFIG_ENABLE_LOGGING
    // TODO recovery, what to do when a deadline is missed? At the moment,
    // we're launching the default handler
    (void)signal(SIGXCPU, SIG_DFL);
}

/**
 * @brief The Odometry worker thread. Here's where all the magic happens.
 *
 * @param args A pointer to a memory area representing arguments. By default,
 * `NULL` is passed: this parameter is mostly useful if you have a thread
 * pool, see https://computing.llnl.gov/tutorials/pthreads/.
 *
 * @return In our case, nothing is returned, but having the function signature
 * return `void*` lets the programmer return arbitrary data from the thread,
 * albeit dangerously.
 */
void* MOdo_EntryPoint(void* args) {
    assert(args ==
           NULL);  // Ensure we are not passing arguments (I admit doing
                   // this only to remove a compile-time warning)
    struct sched_attr attr = {.size = sizeof(attr),
                              .sched_flags = 0 | SCHED_FLAG_DL_OVERRUN,
                              .sched_policy = SCHED_DEADLINE,
                              .sched_runtime = ODO_RUNTIME,     // 10ms
                              .sched_period = ODO_PERIOD,       // 20ms
                              .sched_deadline = ODO_DEADLINE};  // 11ms
    (void)signal(SIGXCPU, MOdo_DLMissHandler);  // Register signal handler
    if (sched_setattr(0, &attr, 0)) {
#if CONFIG_PRINT_ERRORS
        perror("MOdo_EntryPoint: sched_setattr");
#endif  // CONFIG_PRINT_ERRORS
        exit(EXIT_FAILURE);
    }
    // BEGIN Worker variables
    arc_t sx = 0;
    arc_t dx = 0;
    pose_t* cur;
    arc_t delta;
    posebuf_t pb;
    double b = USE_BASELINE;
#if CONFIG_CUT_SHORT
    odoStats_t stats = {.targetIterations = CONFIG_MAX_ITERS,
                        .totalIterations = 0,
                        .clockMisses = 0};
#endif  // CONFIG_CUT_SHORT
    struct timespec gts;
    int ottoCharDeviceFd;
    int lastDelta = 0;
    StatusMessage msg = StatusMessage_init_zero;
    int commandSample = 0;
    VelocityCommand cmd = VelocityCommand_init_zero;
    pb_byte_t recvBuf[20], tmpBuf[20];
    int bytesRead = 0, bytesReadTotal = 0, lastBytesRead = 0, bytesMissing;
    VelocityCommand commandBuffer[OTTO_COMMAND_SAMPLES] = {
        {0, 0}, {10, 0}, {10, 0}, {10, 0}, {1, 0},
        {1, 0}, {1, 0},  {1, 0},  {0, 0},  {0, 0}};
    // END Worker variables
    ottoCharDeviceFd = AOtto_Init();
    MOdo_InitPoseBuffer(&pb);
    HTime_InitBase();  // XXX see h_time.h
    for (int i = 0; i < 100; i++) {
        //printf("Spamming...\n");
        AOtto_SerializeWrite(ottoCharDeviceFd, commandBuffer[0]);
    }
    for (;;) {
        // BEGIN Worker code
        bytesMissing = 20 - bytesRead;
        bytesRead = AOtto_Read(ottoCharDeviceFd, bytesMissing, recvBuf);
        bytesReadTotal += bytesRead;
        printf("Read: %i (%i tot)\n", bytesRead, bytesReadTotal);
        if (bytesRead < 20) {
            for (int i = 0; i < bytesRead; i++) {
                tmpBuf[lastBytesRead + i] = recvBuf[i];
                printf("adding byte %2x\n", recvBuf[i]);
            }
            lastBytesRead += bytesRead;
        }
        if (bytesReadTotal == 20) {
            if (lastBytesRead) {
                AOtto_Deserialize(tmpBuf, 20, &msg);
            } else {
                AOtto_Deserialize(recvBuf, 20, &msg);
            }
#if CONFIG_PRINT_MSG_VALUES
            printf("A:%f,L:%f,T:%d,S:%d\n", msg.angular_velocity,
                   msg.linear_velocity, msg.delta_millis, msg.status);
#endif  // CONFIG_PRINT_MSG_VALUES
            bytesReadTotal = 0;
            lastBytesRead = 0;
        }

        // XXX linear_velocity => sx/dx???
        cur = pb.old;  // Select the pose to work on in the current iteration
        pb.old = pb.old->next;  // And mark the other one as disposable
        delta = sx - dx;
        cur->ts = HTime_GetNsDelta(&gts);
#if CONFIG_ENABLE_LOGGING
        if (cur->ts != 0 && pb.old->ts != 0) {
            lastDelta = abs((int)(cur->ts - pb.old->ts - (ODO_PERIOD)));
        }
        if (lastDelta >= ODO_DELTA_WARN_THRESHOLD) {
            syslog(
                LOG_WARNING, "%s%dns",
                "(M_ODO) Abnormal timestamp delta detected between poses: ",
                lastDelta);
        }
#endif  // CONFIG_ENABLE_LOGGING
        // TODO proofread & static test
        if (delta == 0) {  // We are going forward
            cur->x = pb.old->x + cos(pb.old->th) * delta;
            cur->y = pb.old->y + sin(pb.old->th) * delta;
            cur->th = pb.old->th;
        } else {  // We are going left or right
            cur->x =
                pb.old->x -
                (b / 2 + dx * b / delta) * (sin(pb.old->th - delta) / b) -
                sin(pb.old->th);
            cur->y =
                pb.old->y +
                (b / 2 + dx * b / delta) * (cos(pb.old->th - delta) / b) -
                cos(pb.old->th);
            cur->th = pb.old->th - delta / b;
        }
#if CONFIG_CUT_SHORT
        stats.totalIterations++;
        if (stats.totalIterations == stats.targetIterations) {
            printf("HUP!\n");
            break;
        }
#endif
        if (0) {  //(commandSample < OTTO_COMMAND_SAMPLES) {
            cmd = commandBuffer[commandSample];
            AOtto_SerializeWrite(ottoCharDeviceFd, cmd);
            commandSample++;
        }
        // END Worker code
        sched_yield();
    };
    pthread_exit(EXIT_SUCCESS);
    return (NULL);
}
