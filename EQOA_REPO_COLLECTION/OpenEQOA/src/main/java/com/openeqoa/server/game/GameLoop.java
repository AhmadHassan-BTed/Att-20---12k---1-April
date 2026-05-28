package com.openeqoa.server.game;

import com.openeqoa.server.ServerMain;
import lombok.Getter;

/**
 * Class Credit:
 * https://github.com/Wulf/crescent/blob/master/crescent/core/server/ca/live/hk12/crescent/server/CrescentServer.java
 * License:
 * https://github.com/Wulf/crescent/blob/master/LICENSE.txt
 * Modified: Robert Brown & Joseph Rugh
 */
@Getter
public class GameLoop extends Thread {

    // TODO: Looping tasks...

    /* UPDATES PER SECOND */
    private int currentTPS;
    private long variableYieldTime, lastTime;

    public GameLoop() {
        super("GameThread");
    }

    /**
     * Grabs the current ticks per second of the server.
     *
     * @return The current server ticks per second.
     */
    public int getCurrentTPS() {
        return currentTPS;
    }

    @Override
    public void run() {
        int updates = 0;
        long time = 0;
        long nanoSecond = 1000000000; // 1 second -> 1000 ms -> 1000*1,000,000 ns
        long startTime, endTime;
        long numberOfTicksPassed = 0;

        while (ServerMain.getInstance().isRunning()) {
            startTime = System.nanoTime();

            // WARNING: Maintain tick order!
            /* Update Start */

            // TODO: Insert updating task here...

            /* Update End */

            sync(GameConstants.TICKS_PER_SECOND);

            endTime = System.nanoTime();
            updates++;
            numberOfTicksPassed++;

            time += endTime - startTime;
            if (time >= nanoSecond) {
                time -= nanoSecond;
                currentTPS = updates;
                updates = 0;
            }
        }
    }

    /**
     * Author: kappa (On the LWJGL Forums)
     * An accurate sync method that adapts automatically
     * to the system it runs on to provide reliable results.
     *
     * @param tps The desired frame rate, in frames per second.
     */
    private void sync(@SuppressWarnings("SameParameterValue") int tps) {
        if (tps <= 0) return;

        long sleepTime = 1000000000 / tps; // nanoseconds to sleep this frame
        long yieldTime = Math.min(sleepTime, variableYieldTime + sleepTime % (1000 * 1000));
        long overSleep = 0; // time the sync goes over by

        try {
            while (true) {
                long time = System.nanoTime() - lastTime;

                if (time < sleepTime - yieldTime) {
                    Thread.sleep(1);
                } else if (time < sleepTime) {
                    // burn the last few CPU cycles to ensure accuracy
                    Thread.yield();
                } else {
                    overSleep = time - sleepTime;
                    break; // exitServer while loop
                }
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            lastTime = System.nanoTime() - Math.min(overSleep, sleepTime);

            // auto tune the time sync should yield
            if (overSleep > variableYieldTime) {
                // increase by 200 microseconds (1/5 a ms)
                variableYieldTime = Math.min(variableYieldTime + 200 * 1000, sleepTime);
            } else if (overSleep < variableYieldTime - 200 * 1000) {
                // decrease by 2 microseconds
                variableYieldTime = Math.max(variableYieldTime - 2 * 1000, 0);
            }
        }
    }
}
