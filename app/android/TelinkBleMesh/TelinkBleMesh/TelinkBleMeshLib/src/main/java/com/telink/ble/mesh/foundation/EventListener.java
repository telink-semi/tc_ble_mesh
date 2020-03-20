
package com.telink.ble.mesh.foundation;

public interface EventListener<T> {
    void performed(Event<T> event);
}