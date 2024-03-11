/********************************************************************************************************
 * @file EventBus.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

package com.telink.ble.mesh.foundation;

import android.os.Handler;
import android.os.Looper;

import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * The code above is a generic implementation of an event bus in Java. An event bus is a mechanism for communication between different components in a software system. It allows components to publish events and subscribe to events they are interested in.
 * <p>
 * The EventBus class has the following properties and methods:
 * <p>
 * <p>
 * - processing:
 * - task:
 * <p>
 * The EventBus class also has the following methods:
 * <p>
 * - removeEventListeners:
 * - dispatchEvent:
 * - processOnThread:
 * - processEvent:
 * - processEventCompleted:
 * <p>
 * The code also includes a nested class DefaultThreadFactory, which is a
 * <p>
 * Please let me know if you have any further questions.
 *
 * @param <T>
 */
public class EventBus<T> {

    /**
     * a constant representing the number of available processors in the system.
     */
    private static final int CPU_COUNT = Runtime.getRuntime().availableProcessors();

    /**
     * a constant representing the core pool size for the thread pool executor.
     */
    private static final int CORE_POOL_SIZE = CPU_COUNT + 1;

    /**
     * a constant representing the maximum pool size for the thread pool executor.
     */
    private static final int MAXIMUM_POOL_SIZE = CPU_COUNT * 2 + 1;

    /**
     * a constant representing the keep alive time for idle threads in the thread pool executor.
     */
    private static final int KEEP_ALIVE = 1;

    /**
     * a blocking queue used to hold the tasks to be executed by the thread pool executor.
     */
    private static final BlockingQueue<Runnable> sPoolWorkQueue =
            new LinkedBlockingQueue<>(128);

    /**
     * a thread factory used to create new threads for the thread pool executor.
     */
    private static final ThreadFactory sThreadFactory = new DefaultThreadFactory();

    /**
     * an executor service that manages the thread pool for executing tasks.
     */
    private static final ExecutorService EXECUTOR_SERVICE = new ThreadPoolExecutor(CORE_POOL_SIZE, MAXIMUM_POOL_SIZE, KEEP_ALIVE,
            TimeUnit.SECONDS, sPoolWorkQueue, sThreadFactory); //Executors.newCachedThreadPool(new DefaultThreadFactory());

    /**
     * a map that holds the event listeners for each event type.
     */
    protected final Map<T, List<EventListener<T>>> mEventListeners = new ConcurrentHashMap<>();

    /**
     * a queue that holds the events to be processed.
     */
    protected final Queue<Event<T>> mEventQueue = new ConcurrentLinkedQueue<>();

    /**
     * a handler associated with the current thread.
     */
    protected final Handler mCurrentThreadHandler = new Handler(Looper.myLooper());

    /**
     * a handler associated with the main thread.
     */
    protected final Handler mMainThreadHandler = new Handler(Looper.getMainLooper());

    /**
     * an object used for synchronization.
     */
    private final Object mLock = new Object();

    /**
     * a boolean flag indicating whether the event bus is currently processing events.
     */
    protected boolean processing = false;

    /**
     * a runnable that represents the task to be executed by the thread pool executor.
     */
    private final Runnable task = new Runnable() {
        @Override
        public void run() {
            processEvent();
        }
    };

    /**
     * adds an event listener for a specific event type.
     */
    public void addEventListener(T eventType, EventListener<T> listener) {

        synchronized (this.mEventListeners) {
            List<EventListener<T>> listeners;

            if (this.mEventListeners.containsKey(eventType)) {
                listeners = this.mEventListeners.get(eventType);
            } else {
                listeners = new CopyOnWriteArrayList<>();
                this.mEventListeners.put(eventType, listeners);
            }

            if (!listeners.contains(listener)) {
                listeners.add(listener);
            }
        }
    }

    /**
     * removes an event listener.
     */
    public void removeEventListener(EventListener<T> listener) {
        synchronized (this.mEventListeners) {
            for (T eventType : this.mEventListeners.keySet()) {
                this.removeEventListener(eventType, listener);
            }
        }
    }

    /**
     * removes an event listener with specified type.
     */
    public void removeEventListener(T eventType, EventListener<T> listener) {
        synchronized (this.mEventListeners) {
            if (this.mEventListeners.containsKey(eventType)) {
                List<EventListener<T>> listeners = this.mEventListeners.get(eventType);
                listeners.remove(listener);
            }
        }
    }

    /**
     * removes all event listeners.
     */
    public void removeEventListeners() {
        synchronized (this.mEventListeners) {
            for (T eventType : this.mEventListeners.keySet()) {
                List<EventListener<T>> listeners = this.mEventListeners.get(eventType);
                listeners.clear();
                this.mEventListeners.remove(eventType);
            }
        }
    }

    /**
     * dispatches an event to be processed.
     *
     * @param event
     */
    public void dispatchEvent(final Event<T> event) {
//        MeshLogger.log("event looper : " + event.getThreadMode());

        this.mEventQueue.add(event);

//        MeshLogger.log("post event : " + event.getType() + "--" + event.getClass().getSimpleName());

        synchronized (this.mLock) {
            if (!this.processing)
                this.processOnThread();
        }
    }

    /**
     * processes an event on a separate thread based on its thread mode.
     */
    private void processOnThread() {

        final Event<T> event;

        synchronized (mEventQueue) {
            event = this.mEventQueue.peek();
            if (event == null)
                return;
        }

        switch (event.getThreadMode()) {
            case Background:
                EXECUTOR_SERVICE.execute(task);
                break;
            case Main:
                mMainThreadHandler.post(task);
                break;
            case Default:
                mCurrentThreadHandler.post(task);
                break;

        }
    }

    /**
     * processes an event by invoking the event listeners.
     */
    private void processEvent() {
//        MeshLogger.log("process on thread : " + Thread.currentThread().getName());

        final Event<T> event;

        synchronized (mEventQueue) {
            event = mEventQueue.poll();
            if (event == null)
                return;
        }

//        MeshLogger.log("process event : " + event.getType() + "--" + event.getClass().getName());

        T eventType = event.getType();
        List<EventListener<T>> listeners = null;

        synchronized (this.mEventListeners) {
            if (this.mEventListeners.containsKey(eventType)) {
                listeners = this.mEventListeners.get(eventType);
            }
        }

        if (listeners != null && !listeners.isEmpty()) {
            synchronized (this.mLock) {
                this.processing = true;
            }

            for (EventListener<T> listener : listeners) {
                if (listener != null)
                    listener.performed(event);
            }
        }

        this.processEventCompleted();
    }

    /**
     * marks the event processing as completed and continues processing any remaining events.
     */
    private void processEventCompleted() {
        synchronized (this.mLock) {
            this.processing = false;
        }

        if (!this.mEventQueue.isEmpty())
            this.processOnThread();
    }

    /**
     * Custom implementation of the ThreadFactory interface.
     * It is responsible for creating new threads for the thread pool executor.
     */
    private static class DefaultThreadFactory implements ThreadFactory {
        private static final AtomicInteger POOL_NUMBER = new AtomicInteger(1);
        private final AtomicInteger threadNumber = new AtomicInteger(1);
        private final ThreadGroup group;
        private final String namePrefix;

        DefaultThreadFactory() {
            SecurityManager s = System.getSecurityManager();
            group = (s != null) ? s.getThreadGroup() :
                    Thread.currentThread().getThreadGroup();
            namePrefix = "pool-" +
                    POOL_NUMBER.getAndIncrement() +
                    "-thread-";
        }

        @Override
        public Thread newThread(final Runnable r) {

            Runnable run = new Runnable() {
                @Override
                public void run() {
                    android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
                    r.run();
                }
            };

            Thread thread = new Thread(group, run,
                    namePrefix + threadNumber.getAndIncrement(),
                    0);
            if (thread.isDaemon())
                thread.setDaemon(false);
            if (thread.getPriority() != Thread.NORM_PRIORITY)
                thread.setPriority(Thread.NORM_PRIORITY);

            return thread;
        }
    }
}
