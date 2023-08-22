package com.telink.ble.mesh.model;

import io.objectbox.converter.PropertyConverter;

public class OnlineStateConverter implements PropertyConverter<OnlineState, Integer> {
    @Override
    public OnlineState convertToEntityProperty(Integer databaseValue) {
        if (databaseValue == null) {
            return null;
        }
        for (OnlineState onlineState : OnlineState.values()) {
            if (onlineState.st == databaseValue) {
                return onlineState;
            }
        }
        return OnlineState.OFFLINE;
    }

    @Override
    public Integer convertToDatabaseValue(OnlineState entityProperty) {
        return entityProperty == null ? null : entityProperty.st;
    }
}